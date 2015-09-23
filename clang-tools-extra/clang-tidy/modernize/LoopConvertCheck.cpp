//===--- LoopConvertCheck.cpp - clang-tidy---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "LoopConvertCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace llvm;

namespace clang {
namespace tidy {
namespace modernize {

const char LoopNameArray[] = "forLoopArray";
const char LoopNameIterator[] = "forLoopIterator";
const char LoopNamePseudoArray[] = "forLoopPseudoArray";
const char ConditionBoundName[] = "conditionBound";
const char ConditionVarName[] = "conditionVar";
const char IncrementVarName[] = "incrementVar";
const char InitVarName[] = "initVar";
const char BeginCallName[] = "beginCall";
const char EndCallName[] = "endCall";
const char ConditionEndVarName[] = "conditionEndVar";
const char EndVarName[] = "endVar";
const char DerefByValueResultName[] = "derefByValueResult";
const char DerefByRefResultName[] = "derefByRefResult";

// shared matchers
static const TypeMatcher AnyType = anything();

static const StatementMatcher IntegerComparisonMatcher =
    expr(ignoringParenImpCasts(
        declRefExpr(to(varDecl(hasType(isInteger())).bind(ConditionVarName)))));

static const DeclarationMatcher InitToZeroMatcher =
    varDecl(hasInitializer(ignoringParenImpCasts(integerLiteral(equals(0)))))
        .bind(InitVarName);

static const StatementMatcher IncrementVarMatcher =
    declRefExpr(to(varDecl(hasType(isInteger())).bind(IncrementVarName)));

/// \brief The matcher for loops over arrays.
///
/// In this general example, assuming 'j' and 'k' are of integral type:
/// \code
///   for (int i = 0; j < 3 + 2; ++k) { ... }
/// \endcode
/// The following string identifiers are bound to these parts of the AST:
///   ConditionVarName: 'j' (as a VarDecl)
///   ConditionBoundName: '3 + 2' (as an Expr)
///   InitVarName: 'i' (as a VarDecl)
///   IncrementVarName: 'k' (as a VarDecl)
///   LoopName: The entire for loop (as a ForStmt)
///
/// Client code will need to make sure that:
///   - The three index variables identified by the matcher are the same
///     VarDecl.
///   - The index variable is only used as an array index.
///   - All arrays indexed by the loop are the same.
StatementMatcher makeArrayLoopMatcher() {
  StatementMatcher ArrayBoundMatcher =
      expr(hasType(isInteger())).bind(ConditionBoundName);

  return forStmt(
             unless(isInTemplateInstantiation()),
             hasLoopInit(declStmt(hasSingleDecl(InitToZeroMatcher))),
             hasCondition(anyOf(
                 binaryOperator(hasOperatorName("<"),
                                hasLHS(IntegerComparisonMatcher),
                                hasRHS(ArrayBoundMatcher)),
                 binaryOperator(hasOperatorName(">"), hasLHS(ArrayBoundMatcher),
                                hasRHS(IntegerComparisonMatcher)))),
             hasIncrement(unaryOperator(hasOperatorName("++"),
                                        hasUnaryOperand(IncrementVarMatcher))))
      .bind(LoopNameArray);
}

/// \brief The matcher used for iterator-based for loops.
///
/// This matcher is more flexible than array-based loops. It will match
/// catch loops of the following textual forms (regardless of whether the
/// iterator type is actually a pointer type or a class type):
///
/// Assuming f, g, and h are of type containerType::iterator,
/// \code
///   for (containerType::iterator it = container.begin(),
///        e = createIterator(); f != g; ++h) { ... }
///   for (containerType::iterator it = container.begin();
///        f != anotherContainer.end(); ++h) { ... }
/// \endcode
/// The following string identifiers are bound to the parts of the AST:
///   InitVarName: 'it' (as a VarDecl)
///   ConditionVarName: 'f' (as a VarDecl)
///   LoopName: The entire for loop (as a ForStmt)
///   In the first example only:
///     EndVarName: 'e' (as a VarDecl)
///     ConditionEndVarName: 'g' (as a VarDecl)
///   In the second example only:
///     EndCallName: 'container.end()' (as a CXXMemberCallExpr)
///
/// Client code will need to make sure that:
///   - The iterator variables 'it', 'f', and 'h' are the same.
///   - The two containers on which 'begin' and 'end' are called are the same.
///   - If the end iterator variable 'g' is defined, it is the same as 'f'.
StatementMatcher makeIteratorLoopMatcher() {
  StatementMatcher BeginCallMatcher =
      cxxMemberCallExpr(argumentCountIs(0),
                        callee(cxxMethodDecl(hasName("begin"))))
          .bind(BeginCallName);

  DeclarationMatcher InitDeclMatcher =
      varDecl(hasInitializer(anyOf(ignoringParenImpCasts(BeginCallMatcher),
                                   materializeTemporaryExpr(
                                       ignoringParenImpCasts(BeginCallMatcher)),
                                   hasDescendant(BeginCallMatcher))))
          .bind(InitVarName);

  DeclarationMatcher EndDeclMatcher =
      varDecl(hasInitializer(anything())).bind(EndVarName);

  StatementMatcher EndCallMatcher = cxxMemberCallExpr(
      argumentCountIs(0), callee(cxxMethodDecl(hasName("end"))));

  StatementMatcher IteratorBoundMatcher =
      expr(anyOf(ignoringParenImpCasts(
                     declRefExpr(to(varDecl().bind(ConditionEndVarName)))),
                 ignoringParenImpCasts(expr(EndCallMatcher).bind(EndCallName)),
                 materializeTemporaryExpr(ignoringParenImpCasts(
                     expr(EndCallMatcher).bind(EndCallName)))));

  StatementMatcher IteratorComparisonMatcher = expr(
      ignoringParenImpCasts(declRefExpr(to(varDecl().bind(ConditionVarName)))));

  StatementMatcher OverloadedNEQMatcher =
      cxxOperatorCallExpr(hasOverloadedOperatorName("!="), argumentCountIs(2),
                          hasArgument(0, IteratorComparisonMatcher),
                          hasArgument(1, IteratorBoundMatcher));

  // This matcher tests that a declaration is a CXXRecordDecl that has an
  // overloaded operator*(). If the operator*() returns by value instead of by
  // reference then the return type is tagged with DerefByValueResultName.
  internal::Matcher<VarDecl> TestDerefReturnsByValue =
      hasType(cxxRecordDecl(hasMethod(allOf(
          hasOverloadedOperatorName("*"),
          anyOf(
              // Tag the return type if it's by value.
              returns(qualType(unless(hasCanonicalType(referenceType())))
                          .bind(DerefByValueResultName)),
              returns(
                  // Skip loops where the iterator's operator* returns an
                  // rvalue reference. This is just weird.
                  qualType(unless(hasCanonicalType(rValueReferenceType())))
                      .bind(DerefByRefResultName)))))));

  return forStmt(
             unless(isInTemplateInstantiation()),
             hasLoopInit(anyOf(declStmt(declCountIs(2),
                                        containsDeclaration(0, InitDeclMatcher),
                                        containsDeclaration(1, EndDeclMatcher)),
                               declStmt(hasSingleDecl(InitDeclMatcher)))),
             hasCondition(
                 anyOf(binaryOperator(hasOperatorName("!="),
                                      hasLHS(IteratorComparisonMatcher),
                                      hasRHS(IteratorBoundMatcher)),
                       binaryOperator(hasOperatorName("!="),
                                      hasLHS(IteratorBoundMatcher),
                                      hasRHS(IteratorComparisonMatcher)),
                       OverloadedNEQMatcher)),
             hasIncrement(anyOf(
                 unaryOperator(hasOperatorName("++"),
                               hasUnaryOperand(declRefExpr(
                                   to(varDecl(hasType(pointsTo(AnyType)))
                                          .bind(IncrementVarName))))),
                 cxxOperatorCallExpr(
                     hasOverloadedOperatorName("++"),
                     hasArgument(
                         0, declRefExpr(to(varDecl(TestDerefReturnsByValue)
                                               .bind(IncrementVarName))))))))
      .bind(LoopNameIterator);
}

/// \brief The matcher used for array-like containers (pseudoarrays).
///
/// This matcher is more flexible than array-based loops. It will match
/// loops of the following textual forms (regardless of whether the
/// iterator type is actually a pointer type or a class type):
///
/// Assuming f, g, and h are of type containerType::iterator,
/// \code
///   for (int i = 0, j = container.size(); f < g; ++h) { ... }
///   for (int i = 0; f < container.size(); ++h) { ... }
/// \endcode
/// The following string identifiers are bound to the parts of the AST:
///   InitVarName: 'i' (as a VarDecl)
///   ConditionVarName: 'f' (as a VarDecl)
///   LoopName: The entire for loop (as a ForStmt)
///   In the first example only:
///     EndVarName: 'j' (as a VarDecl)
///     ConditionEndVarName: 'g' (as a VarDecl)
///   In the second example only:
///     EndCallName: 'container.size()' (as a CXXMemberCallExpr)
///
/// Client code will need to make sure that:
///   - The index variables 'i', 'f', and 'h' are the same.
///   - The containers on which 'size()' is called is the container indexed.
///   - The index variable is only used in overloaded operator[] or
///     container.at().
///   - If the end iterator variable 'g' is defined, it is the same as 'j'.
///   - The container's iterators would not be invalidated during the loop.
StatementMatcher makePseudoArrayLoopMatcher() {
  // Test that the incoming type has a record declaration that has methods
  // called 'begin' and 'end'. If the incoming type is const, then make sure
  // these methods are also marked const.
  //
  // FIXME: To be completely thorough this matcher should also ensure the
  // return type of begin/end is an iterator that dereferences to the same as
  // what operator[] or at() returns. Such a test isn't likely to fail except
  // for pathological cases.
  //
  // FIXME: Also, a record doesn't necessarily need begin() and end(). Free
  // functions called begin() and end() taking the container as an argument
  // are also allowed.
  TypeMatcher RecordWithBeginEnd = qualType(
      anyOf(qualType(isConstQualified(),
                     hasDeclaration(cxxRecordDecl(
                         hasMethod(cxxMethodDecl(hasName("begin"), isConst())),
                         hasMethod(cxxMethodDecl(hasName("end"),
                                                 isConst())))) // hasDeclaration
                     ),                                        // qualType
            qualType(unless(isConstQualified()),
                     hasDeclaration(
                         cxxRecordDecl(hasMethod(hasName("begin")),
                                       hasMethod(hasName("end"))))) // qualType
            ));

  StatementMatcher SizeCallMatcher = cxxMemberCallExpr(
      argumentCountIs(0),
      callee(cxxMethodDecl(anyOf(hasName("size"), hasName("length")))),
      on(anyOf(hasType(pointsTo(RecordWithBeginEnd)),
               hasType(RecordWithBeginEnd))));

  StatementMatcher EndInitMatcher =
      expr(anyOf(ignoringParenImpCasts(expr(SizeCallMatcher).bind(EndCallName)),
                 explicitCastExpr(hasSourceExpression(ignoringParenImpCasts(
                     expr(SizeCallMatcher).bind(EndCallName))))));

  DeclarationMatcher EndDeclMatcher =
      varDecl(hasInitializer(EndInitMatcher)).bind(EndVarName);

  StatementMatcher IndexBoundMatcher =
      expr(anyOf(ignoringParenImpCasts(declRefExpr(to(
                     varDecl(hasType(isInteger())).bind(ConditionEndVarName)))),
                 EndInitMatcher));

  return forStmt(
             unless(isInTemplateInstantiation()),
             hasLoopInit(
                 anyOf(declStmt(declCountIs(2),
                                containsDeclaration(0, InitToZeroMatcher),
                                containsDeclaration(1, EndDeclMatcher)),
                       declStmt(hasSingleDecl(InitToZeroMatcher)))),
             hasCondition(anyOf(
                 binaryOperator(hasOperatorName("<"),
                                hasLHS(IntegerComparisonMatcher),
                                hasRHS(IndexBoundMatcher)),
                 binaryOperator(hasOperatorName(">"), hasLHS(IndexBoundMatcher),
                                hasRHS(IntegerComparisonMatcher)))),
             hasIncrement(unaryOperator(hasOperatorName("++"),
                                        hasUnaryOperand(IncrementVarMatcher))))
      .bind(LoopNamePseudoArray);
}

/// \brief Determine whether Init appears to be an initializing an iterator.
///
/// If it is, returns the object whose begin() or end() method is called, and
/// the output parameter isArrow is set to indicate whether the initialization
/// is called via . or ->.
static const Expr *getContainerFromBeginEndCall(const Expr *Init, bool IsBegin,
                                                bool *IsArrow) {
  // FIXME: Maybe allow declaration/initialization outside of the for loop.
  const auto *TheCall =
      dyn_cast_or_null<CXXMemberCallExpr>(digThroughConstructors(Init));
  if (!TheCall || TheCall->getNumArgs() != 0)
    return nullptr;

  const auto *Member = dyn_cast<MemberExpr>(TheCall->getCallee());
  if (!Member)
    return nullptr;
  StringRef Name = Member->getMemberDecl()->getName();
  StringRef TargetName = IsBegin ? "begin" : "end";
  if (Name != TargetName)
    return nullptr;

  const Expr *SourceExpr = Member->getBase();
  if (!SourceExpr)
    return nullptr;

  *IsArrow = Member->isArrow();
  return SourceExpr;
}

/// \brief Determines the container whose begin() and end() functions are called
/// for an iterator-based loop.
///
/// BeginExpr must be a member call to a function named "begin()", and EndExpr
/// must be a member.
static const Expr *findContainer(ASTContext *Context, const Expr *BeginExpr,
                                 const Expr *EndExpr,
                                 bool *ContainerNeedsDereference) {
  // Now that we know the loop variable and test expression, make sure they are
  // valid.
  bool BeginIsArrow = false;
  bool EndIsArrow = false;
  const Expr *BeginContainerExpr =
      getContainerFromBeginEndCall(BeginExpr, /*IsBegin=*/true, &BeginIsArrow);
  if (!BeginContainerExpr)
    return nullptr;

  const Expr *EndContainerExpr =
      getContainerFromBeginEndCall(EndExpr, /*IsBegin=*/false, &EndIsArrow);
  // Disallow loops that try evil things like this (note the dot and arrow):
  //  for (IteratorType It = Obj.begin(), E = Obj->end(); It != E; ++It) { }
  if (!EndContainerExpr || BeginIsArrow != EndIsArrow ||
      !areSameExpr(Context, EndContainerExpr, BeginContainerExpr))
    return nullptr;

  *ContainerNeedsDereference = BeginIsArrow;
  return BeginContainerExpr;
}

/// \brief Obtain the original source code text from a SourceRange.
static StringRef getStringFromRange(SourceManager &SourceMgr,
                                    const LangOptions &LangOpts,
                                    SourceRange Range) {
  if (SourceMgr.getFileID(Range.getBegin()) !=
      SourceMgr.getFileID(Range.getEnd())) {
    return StringRef(); // Empty string.
  }

  return Lexer::getSourceText(CharSourceRange(Range, true), SourceMgr,
                              LangOpts);
}

/// \brief If the given expression is actually a DeclRefExpr, find and return
/// the underlying VarDecl; otherwise, return NULL.
static const VarDecl *getReferencedVariable(const Expr *E) {
  if (const DeclRefExpr *DRE = getDeclRef(E))
    return dyn_cast<VarDecl>(DRE->getDecl());
  return nullptr;
}

/// \brief Returns true when the given expression is a member expression
/// whose base is `this` (implicitly or not).
static bool isDirectMemberExpr(const Expr *E) {
  if (const auto *Member = dyn_cast<MemberExpr>(E->IgnoreParenImpCasts()))
    return isa<CXXThisExpr>(Member->getBase()->IgnoreParenImpCasts());
  return false;
}

/// \brief Returns true when it can be guaranteed that the elements of the
/// container are not being modified.
static bool usagesAreConst(const UsageResult &Usages) {
  // FIXME: Make this function more generic.
  return Usages.empty();
}

/// \brief Returns true if the elements of the container are never accessed
/// by reference.
static bool usagesReturnRValues(const UsageResult &Usages) {
  for (const auto &U : Usages) {
    if (U.Expression && !U.Expression->isRValue())
      return false;
  }
  return true;
}

/// \brief Returns true if the container is const-qualified.
static bool containerIsConst(const Expr *ContainerExpr, bool Dereference) {
  if (const auto *VDec = getReferencedVariable(ContainerExpr)) {
    QualType CType = VDec->getType();
    if (Dereference) {
      if (!CType->isPointerType())
        return false;
      CType = CType->getPointeeType();
    }
    // If VDec is a reference to a container, Dereference is false,
    // but we still need to check the const-ness of the underlying container
    // type.
    if (const auto &RT = CType->getAs<ReferenceType>()) {
      CType = RT->getPointeeType();
    }
    return CType.isConstQualified();
  }
  return false;
}

LoopConvertCheck::RangeDescriptor::RangeDescriptor()
    : ContainerNeedsDereference(false), DerefByConstRef(false),
      DerefByValue(false), IsTriviallyCopyable(false) {}

LoopConvertCheck::LoopConvertCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context), TUInfo(new TUTrackingInfo),
      MinConfidence(StringSwitch<Confidence::Level>(
                        Options.get("MinConfidence", "reasonable"))
                        .Case("safe", Confidence::CL_Safe)
                        .Case("risky", Confidence::CL_Risky)
                        .Default(Confidence::CL_Reasonable)) {}

void LoopConvertCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  SmallVector<std::string, 3> Confs{"risky", "reasonable", "safe"};
  Options.store(Opts, "MinConfidence", Confs[static_cast<int>(MinConfidence)]);
}

void LoopConvertCheck::registerMatchers(MatchFinder *Finder) {
  // Only register the matchers for C++. Because this checker is used for
  // modernization, it is reasonable to run it on any C++ standard with the
  // assumption the user is trying to modernize their codebase.
  if (!getLangOpts().CPlusPlus)
    return;

  Finder->addMatcher(makeArrayLoopMatcher(), this);
  Finder->addMatcher(makeIteratorLoopMatcher(), this);
  Finder->addMatcher(makePseudoArrayLoopMatcher(), this);
}

/// \brief Computes the changes needed to convert a given for loop, and
/// applies them.
void LoopConvertCheck::doConversion(
    ASTContext *Context, const VarDecl *IndexVar, const VarDecl *MaybeContainer,
    const UsageResult &Usages, const DeclStmt *AliasDecl, bool AliasUseRequired,
    bool AliasFromForInit, const ForStmt *Loop, RangeDescriptor Descriptor) {
  auto Diag = diag(Loop->getForLoc(), "use range-based for loop instead");

  std::string VarName;
  bool VarNameFromAlias = (Usages.size() == 1) && AliasDecl;
  bool AliasVarIsRef = false;

  if (VarNameFromAlias) {
    const auto *AliasVar = cast<VarDecl>(AliasDecl->getSingleDecl());
    VarName = AliasVar->getName().str();
    AliasVarIsRef = AliasVar->getType()->isReferenceType();

    // We keep along the entire DeclStmt to keep the correct range here.
    const SourceRange &ReplaceRange = AliasDecl->getSourceRange();

    std::string ReplacementText;
    if (AliasUseRequired) {
      ReplacementText = VarName;
    } else if (AliasFromForInit) {
      // FIXME: Clang includes the location of the ';' but only for DeclStmt's
      // in a for loop's init clause. Need to put this ';' back while removing
      // the declaration of the alias variable. This is probably a bug.
      ReplacementText = ";";
    }

    Diag << FixItHint::CreateReplacement(
        CharSourceRange::getTokenRange(ReplaceRange), ReplacementText);
    // No further replacements are made to the loop, since the iterator or index
    // was used exactly once - in the initialization of AliasVar.
  } else {
    VariableNamer Namer(&TUInfo->getGeneratedDecls(),
                        &TUInfo->getParentFinder().getStmtToParentStmtMap(),
                        Loop, IndexVar, MaybeContainer, Context);
    VarName = Namer.createIndexName();
    // First, replace all usages of the array subscript expression with our new
    // variable.
    for (const auto &Usage : Usages) {
      std::string ReplaceText;
      if (Usage.Expression) {
        // If this is an access to a member through the arrow operator, after
        // the replacement it must be accessed through the '.' operator.
        ReplaceText = Usage.Kind == Usage::UK_MemberThroughArrow ? VarName + "."
                                                                 : VarName;
      } else {
        // The Usage expression is only null in case of lambda captures (which
        // are VarDecl). If the index is captured by value, add '&' to capture
        // by reference instead.
        ReplaceText =
            Usage.Kind == Usage::UK_CaptureByCopy ? "&" + VarName : VarName;
      }
      TUInfo->getReplacedVars().insert(std::make_pair(Loop, IndexVar));
      Diag << FixItHint::CreateReplacement(
          CharSourceRange::getTokenRange(Usage.Range), ReplaceText);
    }
  }

  // Now, we need to construct the new range expression.
  SourceRange ParenRange(Loop->getLParenLoc(), Loop->getRParenLoc());

  QualType AutoType = Context->getAutoDeductType();

  // If the new variable name is from the aliased variable, then the reference
  // type for the new variable should only be used if the aliased variable was
  // declared as a reference.
  if (!VarNameFromAlias || AliasVarIsRef) {
    if (Descriptor.DerefByConstRef) {
      AutoType =
          Context->getLValueReferenceType(Context->getConstType(AutoType));
    } else if (Descriptor.DerefByValue) {
      if (!Descriptor.IsTriviallyCopyable)
        AutoType = Context->getRValueReferenceType(AutoType);
    } else {
      AutoType = Context->getLValueReferenceType(AutoType);
    }
  }

  StringRef MaybeDereference = Descriptor.ContainerNeedsDereference ? "*" : "";
  std::string TypeString = AutoType.getAsString();
  std::string Range = ("(" + TypeString + " " + VarName + " : " +
                       MaybeDereference + Descriptor.ContainerString + ")")
                          .str();
  Diag << FixItHint::CreateReplacement(
      CharSourceRange::getTokenRange(ParenRange), Range);
  TUInfo->getGeneratedDecls().insert(make_pair(Loop, VarName));
}

/// \brief Returns a string which refers to the container iterated over.
StringRef LoopConvertCheck::getContainerString(ASTContext *Context,
                                               const ForStmt *Loop,
                                               const Expr *ContainerExpr) {
  StringRef ContainerString;
  if (isa<CXXThisExpr>(ContainerExpr->IgnoreParenImpCasts())) {
    ContainerString = "this";
  } else {
    ContainerString =
        getStringFromRange(Context->getSourceManager(), Context->getLangOpts(),
                           ContainerExpr->getSourceRange());
  }

  return ContainerString;
}

/// \brief Determines what kind of 'auto' must be used after converting a for
/// loop that iterates over an array or pseudoarray.
void LoopConvertCheck::getArrayLoopQualifiers(ASTContext *Context,
                                              const BoundNodes &Nodes,
                                              const Expr *ContainerExpr,
                                              const UsageResult &Usages,
                                              RangeDescriptor &Descriptor) {
  // On arrays and pseudoarrays, we must figure out the qualifiers from the
  // usages.
  if (usagesAreConst(Usages)) {
    Descriptor.DerefByConstRef = true;
  } else {
    if (usagesReturnRValues(Usages)) {
      // If the index usages (dereference, subscript, at, ...) return rvalues,
      // then we should not use a reference, because we need to keep the code
      // correct if it mutates the returned objects.
      Descriptor.DerefByValue = true;
      // Try to find the type of the elements on the container, to check if
      // they are trivially copyable.
      for (const Usage &U : Usages) {
        if (!U.Expression || U.Expression->getType().isNull())
          continue;
        QualType Type = U.Expression->getType().getCanonicalType();
        if (U.Kind == Usage::UK_MemberThroughArrow) {
          if (!Type->isPointerType())
            continue;
          Type = Type->getPointeeType();
        }
        Descriptor.IsTriviallyCopyable = Type.isTriviallyCopyableType(*Context);
        break;
      }
    } else if (containerIsConst(ContainerExpr,
                                Descriptor.ContainerNeedsDereference)) {
      // The usages are lvalue references, we add a 'const' if the container
      // is 'const'. This will always be the case with const arrays (unless
      // usagesAreConst() returned true).
      Descriptor.DerefByConstRef = true;
    }
  }
}

/// \brief Determines what kind of 'auto' must be used after converting an
/// iterator based for loop.
void LoopConvertCheck::getIteratorLoopQualifiers(ASTContext *Context,
                                                 const BoundNodes &Nodes,
                                                 RangeDescriptor &Descriptor) {
  // The matchers for iterator loops provide bound nodes to obtain this
  // information.
  const auto *InitVar = Nodes.getDeclAs<VarDecl>(InitVarName);
  QualType CanonicalInitVarType = InitVar->getType().getCanonicalType();
  const auto *DerefByValueType =
      Nodes.getNodeAs<QualType>(DerefByValueResultName);
  Descriptor.DerefByValue = DerefByValueType;

  if (Descriptor.DerefByValue) {
    // If the dereference operator returns by value then test for the
    // canonical const qualification of the init variable type.
    Descriptor.DerefByConstRef = CanonicalInitVarType.isConstQualified();
    Descriptor.IsTriviallyCopyable =
        DerefByValueType->isTriviallyCopyableType(*Context);
  } else {
    if (const auto *DerefType =
            Nodes.getNodeAs<QualType>(DerefByRefResultName)) {
      // A node will only be bound with DerefByRefResultName if we're dealing
      // with a user-defined iterator type. Test the const qualification of
      // the reference type.
      Descriptor.DerefByConstRef = (*DerefType)
                                       ->getAs<ReferenceType>()
                                       ->getPointeeType()
                                       .isConstQualified();
    } else {
      // By nature of the matcher this case is triggered only for built-in
      // iterator types (i.e. pointers).
      assert(isa<PointerType>(CanonicalInitVarType) &&
             "Non-class iterator type is not a pointer type");

      // We test for const qualification of the pointed-at type.
      Descriptor.DerefByConstRef =
          CanonicalInitVarType->getPointeeType().isConstQualified();
    }
  }
}

/// \brief Determines the parameters needed to build the range replacement.
void LoopConvertCheck::determineRangeDescriptor(
    ASTContext *Context, const BoundNodes &Nodes, const ForStmt *Loop,
    LoopFixerKind FixerKind, const Expr *ContainerExpr,
    const UsageResult &Usages, RangeDescriptor &Descriptor) {
  Descriptor.ContainerString = getContainerString(Context, Loop, ContainerExpr);

  if (FixerKind == LFK_Iterator)
    getIteratorLoopQualifiers(Context, Nodes, Descriptor);
  else
    getArrayLoopQualifiers(Context, Nodes, ContainerExpr, Usages, Descriptor);
}

/// \brief Check some of the conditions that must be met for the loop to be
/// convertible.
bool LoopConvertCheck::isConvertible(ASTContext *Context,
                                     const ast_matchers::BoundNodes &Nodes,
                                     const ForStmt *Loop,
                                     LoopFixerKind FixerKind) {
  // If we already modified the range of this for loop, don't do any further
  // updates on this iteration.
  if (TUInfo->getReplacedVars().count(Loop))
    return false;

  // Check that we have exactly one index variable and at most one end variable.
  const auto *LoopVar = Nodes.getDeclAs<VarDecl>(IncrementVarName);
  const auto *CondVar = Nodes.getDeclAs<VarDecl>(ConditionVarName);
  const auto *InitVar = Nodes.getDeclAs<VarDecl>(InitVarName);
  if (!areSameVariable(LoopVar, CondVar) || !areSameVariable(LoopVar, InitVar))
    return false;
  const auto *EndVar = Nodes.getDeclAs<VarDecl>(EndVarName);
  const auto *ConditionEndVar = Nodes.getDeclAs<VarDecl>(ConditionEndVarName);
  if (EndVar && !areSameVariable(EndVar, ConditionEndVar))
    return false;

  // FIXME: Try to put most of this logic inside a matcher.
  if (FixerKind == LFK_Iterator) {
    QualType InitVarType = InitVar->getType();
    QualType CanonicalInitVarType = InitVarType.getCanonicalType();

    const auto *BeginCall = Nodes.getNodeAs<CXXMemberCallExpr>(BeginCallName);
    assert(BeginCall && "Bad Callback. No begin call expression");
    QualType CanonicalBeginType =
        BeginCall->getMethodDecl()->getReturnType().getCanonicalType();
    if (CanonicalBeginType->isPointerType() &&
        CanonicalInitVarType->isPointerType()) {
      // If the initializer and the variable are both pointers check if the
      // un-qualified pointee types match, otherwise we don't use auto.
      if (!Context->hasSameUnqualifiedType(
              CanonicalBeginType->getPointeeType(),
              CanonicalInitVarType->getPointeeType()))
        return false;
    } else if (!Context->hasSameType(CanonicalInitVarType,
                                     CanonicalBeginType)) {
      // Check for qualified types to avoid conversions from non-const to const
      // iterator types.
      return false;
    }
  } else if (FixerKind == LFK_PseudoArray) {
    // This call is required to obtain the container.
    const auto *EndCall = Nodes.getStmtAs<CXXMemberCallExpr>(EndCallName);
    if (!EndCall || !dyn_cast<MemberExpr>(EndCall->getCallee()))
      return false;
  }
  return true;
}

void LoopConvertCheck::check(const MatchFinder::MatchResult &Result) {
  const BoundNodes &Nodes = Result.Nodes;
  Confidence ConfidenceLevel(Confidence::CL_Safe);
  ASTContext *Context = Result.Context;

  const ForStmt *Loop;
  LoopFixerKind FixerKind;
  RangeDescriptor Descriptor;

  if ((Loop = Nodes.getStmtAs<ForStmt>(LoopNameArray))) {
    FixerKind = LFK_Array;
  } else if ((Loop = Nodes.getStmtAs<ForStmt>(LoopNameIterator))) {
    FixerKind = LFK_Iterator;
  } else {
    Loop = Nodes.getStmtAs<ForStmt>(LoopNamePseudoArray);
    assert(Loop && "Bad Callback. No for statement");
    FixerKind = LFK_PseudoArray;
  }

  if (!isConvertible(Context, Nodes, Loop, FixerKind))
    return;

  const auto *LoopVar = Nodes.getDeclAs<VarDecl>(IncrementVarName);
  const auto *EndVar = Nodes.getDeclAs<VarDecl>(EndVarName);

  // If the loop calls end()/size() after each iteration, lower our confidence
  // level.
  if (FixerKind != LFK_Array && !EndVar)
    ConfidenceLevel.lowerTo(Confidence::CL_Reasonable);

  // If the end comparison isn't a variable, we can try to work with the
  // expression the loop variable is being tested against instead.
  const auto *EndCall = Nodes.getStmtAs<CXXMemberCallExpr>(EndCallName);
  const auto *BoundExpr = Nodes.getStmtAs<Expr>(ConditionBoundName);

  // Find container expression of iterators and pseudoarrays, and determine if
  // this expression needs to be dereferenced to obtain the container.
  // With array loops, the container is often discovered during the
  // ForLoopIndexUseVisitor traversal.
  const Expr *ContainerExpr = nullptr;
  if (FixerKind == LFK_Iterator) {
    ContainerExpr = findContainer(Context, LoopVar->getInit(),
                                  EndVar ? EndVar->getInit() : EndCall,
                                  &Descriptor.ContainerNeedsDereference);
  } else if (FixerKind == LFK_PseudoArray) {
    ContainerExpr = EndCall->getImplicitObjectArgument();
    Descriptor.ContainerNeedsDereference =
        dyn_cast<MemberExpr>(EndCall->getCallee())->isArrow();
  }

  // We must know the container or an array length bound.
  if (!ContainerExpr && !BoundExpr)
    return;

  ForLoopIndexUseVisitor Finder(Context, LoopVar, EndVar, ContainerExpr,
                                BoundExpr,
                                Descriptor.ContainerNeedsDereference);

  // Find expressions and variables on which the container depends.
  if (ContainerExpr) {
    ComponentFinderASTVisitor ComponentFinder;
    ComponentFinder.findExprComponents(ContainerExpr->IgnoreParenImpCasts());
    Finder.addComponents(ComponentFinder.getComponents());
  }

  // Find usages of the loop index. If they are not used in a convertible way,
  // stop here.
  if (!Finder.findAndVerifyUsages(Loop->getBody()))
    return;
  ConfidenceLevel.lowerTo(Finder.getConfidenceLevel());

  // Obtain the container expression, if we don't have it yet.
  if (FixerKind == LFK_Array) {
    ContainerExpr = Finder.getContainerIndexed()->IgnoreParenImpCasts();

    // Very few loops are over expressions that generate arrays rather than
    // array variables. Consider loops over arrays that aren't just represented
    // by a variable to be risky conversions.
    if (!getReferencedVariable(ContainerExpr) &&
        !isDirectMemberExpr(ContainerExpr))
      ConfidenceLevel.lowerTo(Confidence::CL_Risky);
  }

  // Find out which qualifiers we have to use in the loop range.
  const UsageResult &Usages = Finder.getUsages();
  determineRangeDescriptor(Context, Nodes, Loop, FixerKind, ContainerExpr,
                           Usages, Descriptor);

  // Ensure that we do not try to move an expression dependent on a local
  // variable declared inside the loop outside of it.
  // FIXME: Determine when the external dependency isn't an expression converted
  // by another loop.
  TUInfo->getParentFinder().gatherAncestors(Context->getTranslationUnitDecl());
  DependencyFinderASTVisitor DependencyFinder(
      &TUInfo->getParentFinder().getStmtToParentStmtMap(),
      &TUInfo->getParentFinder().getDeclToParentStmtMap(),
      &TUInfo->getReplacedVars(), Loop);

  if (DependencyFinder.dependsOnInsideVariable(ContainerExpr) ||
      Descriptor.ContainerString.empty() || Usages.empty() ||
      ConfidenceLevel.getLevel() < MinConfidence)
    return;

  doConversion(Context, LoopVar, getReferencedVariable(ContainerExpr), Usages,
               Finder.getAliasDecl(), Finder.aliasUseRequired(),
               Finder.aliasFromForInit(), Loop, Descriptor);
}

} // namespace modernize
} // namespace tidy
} // namespace clang
