//===- CodeGenInstruction.cpp - CodeGen Instruction Class Wrapper ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the CodeGenInstruction class.
//
//===----------------------------------------------------------------------===//

#include "CodeGenInstruction.h"
#include "CodeGenTarget.h"
#include "Error.h"
#include "Record.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/STLExtras.h"
#include <set>
using namespace llvm;

//===----------------------------------------------------------------------===//
// CGIOperandList Implementation
//===----------------------------------------------------------------------===//

CGIOperandList::CGIOperandList(Record *R) : TheDef(R) {
  isPredicable = false;
  hasOptionalDef = false;
  isVariadic = false;

  const DagInit *OutDI = R->getValueAsDag("OutOperandList");

  if (const DefInit *Init = dynamic_cast<const DefInit*>(OutDI->getOperator())) {
    if (Init->getDef()->getName() != "outs")
      throw R->getName() + ": invalid def name for output list: use 'outs'";
  } else
    throw R->getName() + ": invalid output list: use 'outs'";

  NumDefs = OutDI->getNumArgs();

  const DagInit *InDI = R->getValueAsDag("InOperandList");
  if (const DefInit *Init = dynamic_cast<const DefInit*>(InDI->getOperator())) {
    if (Init->getDef()->getName() != "ins")
      throw R->getName() + ": invalid def name for input list: use 'ins'";
  } else
    throw R->getName() + ": invalid input list: use 'ins'";

  unsigned MIOperandNo = 0;
  std::set<std::string> OperandNames;
  for (unsigned i = 0, e = InDI->getNumArgs()+OutDI->getNumArgs(); i != e; ++i){
    const Init *ArgInit;
    std::string ArgName;
    if (i < NumDefs) {
      ArgInit = OutDI->getArg(i);
      ArgName = OutDI->getArgName(i);
    } else {
      ArgInit = InDI->getArg(i-NumDefs);
      ArgName = InDI->getArgName(i-NumDefs);
    }

    const DefInit *Arg = dynamic_cast<const DefInit*>(ArgInit);
    if (!Arg)
      throw "Illegal operand for the '" + R->getName() + "' instruction!";

    Record *Rec = Arg->getDef();
    std::string PrintMethod = "printOperand";
    std::string EncoderMethod;
    std::string OperandType = "OPERAND_UNKNOWN";
    unsigned NumOps = 1;
    const DagInit *MIOpInfo = 0;
    if (Rec->isSubClassOf("RegisterOperand")) {
      PrintMethod = Rec->getValueAsString("PrintMethod");
    } else if (Rec->isSubClassOf("Operand")) {
      PrintMethod = Rec->getValueAsString("PrintMethod");
      OperandType = Rec->getValueAsString("OperandType");
      // If there is an explicit encoder method, use it.
      EncoderMethod = Rec->getValueAsString("EncoderMethod");
      MIOpInfo = Rec->getValueAsDag("MIOperandInfo");

      // Verify that MIOpInfo has an 'ops' root value.
      if (!dynamic_cast<const DefInit*>(MIOpInfo->getOperator()) ||
          dynamic_cast<const DefInit*>(MIOpInfo->getOperator())
          ->getDef()->getName() != "ops")
        throw "Bad value for MIOperandInfo in operand '" + Rec->getName() +
        "'\n";

      // If we have MIOpInfo, then we have #operands equal to number of entries
      // in MIOperandInfo.
      if (unsigned NumArgs = MIOpInfo->getNumArgs())
        NumOps = NumArgs;

      if (Rec->isSubClassOf("PredicateOperand"))
        isPredicable = true;
      else if (Rec->isSubClassOf("OptionalDefOperand"))
        hasOptionalDef = true;
    } else if (Rec->getName() == "variable_ops") {
      isVariadic = true;
      continue;
    } else if (Rec->isSubClassOf("RegisterClass")) {
      OperandType = "OPERAND_REGISTER";
    } else if (!Rec->isSubClassOf("PointerLikeRegClass") &&
               Rec->getName() != "unknown")
      throw "Unknown operand class '" + Rec->getName() +
      "' in '" + R->getName() + "' instruction!";

    // Check that the operand has a name and that it's unique.
    if (ArgName.empty())
      throw "In instruction '" + R->getName() + "', operand #" + utostr(i) +
      " has no name!";
    if (!OperandNames.insert(ArgName).second)
      throw "In instruction '" + R->getName() + "', operand #" + utostr(i) +
      " has the same name as a previous operand!";

    OperandList.push_back(OperandInfo(Rec, ArgName, PrintMethod, EncoderMethod,
                                      OperandType, MIOperandNo, NumOps,
                                      MIOpInfo));
    MIOperandNo += NumOps;
  }


  // Make sure the constraints list for each operand is large enough to hold
  // constraint info, even if none is present.
  for (unsigned i = 0, e = OperandList.size(); i != e; ++i)
    OperandList[i].Constraints.resize(OperandList[i].MINumOperands);
}


/// getOperandNamed - Return the index of the operand with the specified
/// non-empty name.  If the instruction does not have an operand with the
/// specified name, throw an exception.
///
unsigned CGIOperandList::getOperandNamed(StringRef Name) const {
  unsigned OpIdx;
  if (hasOperandNamed(Name, OpIdx)) return OpIdx;
  throw "'" + TheDef->getName() + "' does not have an operand named '$" +
    Name.str() + "'!";
}

/// hasOperandNamed - Query whether the instruction has an operand of the
/// given name. If so, return true and set OpIdx to the index of the
/// operand. Otherwise, return false.
bool CGIOperandList::hasOperandNamed(StringRef Name, unsigned &OpIdx) const {
  assert(!Name.empty() && "Cannot search for operand with no name!");
  for (unsigned i = 0, e = OperandList.size(); i != e; ++i)
    if (OperandList[i].Name == Name) {
      OpIdx = i;
      return true;
    }
  return false;
}

std::pair<unsigned,unsigned>
CGIOperandList::ParseOperandName(const std::string &Op, bool AllowWholeOp) {
  if (Op.empty() || Op[0] != '$')
    throw TheDef->getName() + ": Illegal operand name: '" + Op + "'";

  std::string OpName = Op.substr(1);
  std::string SubOpName;

  // Check to see if this is $foo.bar.
  std::string::size_type DotIdx = OpName.find_first_of(".");
  if (DotIdx != std::string::npos) {
    SubOpName = OpName.substr(DotIdx+1);
    if (SubOpName.empty())
      throw TheDef->getName() + ": illegal empty suboperand name in '" +Op +"'";
    OpName = OpName.substr(0, DotIdx);
  }

  unsigned OpIdx = getOperandNamed(OpName);

  if (SubOpName.empty()) {  // If no suboperand name was specified:
    // If one was needed, throw.
    if (OperandList[OpIdx].MINumOperands > 1 && !AllowWholeOp &&
        SubOpName.empty())
      throw TheDef->getName() + ": Illegal to refer to"
      " whole operand part of complex operand '" + Op + "'";

    // Otherwise, return the operand.
    return std::make_pair(OpIdx, 0U);
  }

  // Find the suboperand number involved.
  const DagInit *MIOpInfo = OperandList[OpIdx].MIOperandInfo;
  if (MIOpInfo == 0)
    throw TheDef->getName() + ": unknown suboperand name in '" + Op + "'";

  // Find the operand with the right name.
  for (unsigned i = 0, e = MIOpInfo->getNumArgs(); i != e; ++i)
    if (MIOpInfo->getArgName(i) == SubOpName)
      return std::make_pair(OpIdx, i);

  // Otherwise, didn't find it!
  throw TheDef->getName() + ": unknown suboperand name in '" + Op + "'";
}

static void ParseConstraint(const std::string &CStr, CGIOperandList &Ops) {
  // EARLY_CLOBBER: @early $reg
  std::string::size_type wpos = CStr.find_first_of(" \t");
  std::string::size_type start = CStr.find_first_not_of(" \t");
  std::string Tok = CStr.substr(start, wpos - start);
  if (Tok == "@earlyclobber") {
    std::string Name = CStr.substr(wpos+1);
    wpos = Name.find_first_not_of(" \t");
    if (wpos == std::string::npos)
      throw "Illegal format for @earlyclobber constraint: '" + CStr + "'";
    Name = Name.substr(wpos);
    std::pair<unsigned,unsigned> Op = Ops.ParseOperandName(Name, false);

    // Build the string for the operand
    if (!Ops[Op.first].Constraints[Op.second].isNone())
      throw "Operand '" + Name + "' cannot have multiple constraints!";
    Ops[Op.first].Constraints[Op.second] =
    CGIOperandList::ConstraintInfo::getEarlyClobber();
    return;
  }

  // Only other constraint is "TIED_TO" for now.
  std::string::size_type pos = CStr.find_first_of('=');
  assert(pos != std::string::npos && "Unrecognized constraint");
  start = CStr.find_first_not_of(" \t");
  std::string Name = CStr.substr(start, pos - start);

  // TIED_TO: $src1 = $dst
  wpos = Name.find_first_of(" \t");
  if (wpos == std::string::npos)
    throw "Illegal format for tied-to constraint: '" + CStr + "'";
  std::string DestOpName = Name.substr(0, wpos);
  std::pair<unsigned,unsigned> DestOp = Ops.ParseOperandName(DestOpName, false);

  Name = CStr.substr(pos+1);
  wpos = Name.find_first_not_of(" \t");
  if (wpos == std::string::npos)
    throw "Illegal format for tied-to constraint: '" + CStr + "'";

  std::pair<unsigned,unsigned> SrcOp =
  Ops.ParseOperandName(Name.substr(wpos), false);
  if (SrcOp > DestOp)
    throw "Illegal tied-to operand constraint '" + CStr + "'";


  unsigned FlatOpNo = Ops.getFlattenedOperandNumber(SrcOp);

  if (!Ops[DestOp.first].Constraints[DestOp.second].isNone())
    throw "Operand '" + DestOpName + "' cannot have multiple constraints!";
  Ops[DestOp.first].Constraints[DestOp.second] =
  CGIOperandList::ConstraintInfo::getTied(FlatOpNo);
}

static void ParseConstraints(const std::string &CStr, CGIOperandList &Ops) {
  if (CStr.empty()) return;

  const std::string delims(",");
  std::string::size_type bidx, eidx;

  bidx = CStr.find_first_not_of(delims);
  while (bidx != std::string::npos) {
    eidx = CStr.find_first_of(delims, bidx);
    if (eidx == std::string::npos)
      eidx = CStr.length();

    ParseConstraint(CStr.substr(bidx, eidx - bidx), Ops);
    bidx = CStr.find_first_not_of(delims, eidx);
  }
}

void CGIOperandList::ProcessDisableEncoding(std::string DisableEncoding) {
  while (1) {
    std::pair<StringRef, StringRef> P = getToken(DisableEncoding, " ,\t");
    std::string OpName = P.first;
    DisableEncoding = P.second;
    if (OpName.empty()) break;

    // Figure out which operand this is.
    std::pair<unsigned,unsigned> Op = ParseOperandName(OpName, false);

    // Mark the operand as not-to-be encoded.
    if (Op.second >= OperandList[Op.first].DoNotEncode.size())
      OperandList[Op.first].DoNotEncode.resize(Op.second+1);
    OperandList[Op.first].DoNotEncode[Op.second] = true;
  }

}

//===----------------------------------------------------------------------===//
// CodeGenInstruction Implementation
//===----------------------------------------------------------------------===//

CodeGenInstruction::CodeGenInstruction(Record *R) : TheDef(R), Operands(R) {
  Namespace = R->getValueAsString("Namespace");
  AsmString = R->getValueAsString("AsmString");

  isReturn     = R->getValueAsBit("isReturn");
  isBranch     = R->getValueAsBit("isBranch");
  isIndirectBranch = R->getValueAsBit("isIndirectBranch");
  isCompare    = R->getValueAsBit("isCompare");
  isMoveImm    = R->getValueAsBit("isMoveImm");
  isBitcast    = R->getValueAsBit("isBitcast");
  isBarrier    = R->getValueAsBit("isBarrier");
  isCall       = R->getValueAsBit("isCall");
  canFoldAsLoad = R->getValueAsBit("canFoldAsLoad");
  mayLoad      = R->getValueAsBit("mayLoad");
  mayStore     = R->getValueAsBit("mayStore");
  isPredicable = Operands.isPredicable || R->getValueAsBit("isPredicable");
  isConvertibleToThreeAddress = R->getValueAsBit("isConvertibleToThreeAddress");
  isCommutable = R->getValueAsBit("isCommutable");
  isTerminator = R->getValueAsBit("isTerminator");
  isReMaterializable = R->getValueAsBit("isReMaterializable");
  hasDelaySlot = R->getValueAsBit("hasDelaySlot");
  usesCustomInserter = R->getValueAsBit("usesCustomInserter");
  hasCtrlDep   = R->getValueAsBit("hasCtrlDep");
  isNotDuplicable = R->getValueAsBit("isNotDuplicable");
  hasSideEffects = R->getValueAsBit("hasSideEffects");
  neverHasSideEffects = R->getValueAsBit("neverHasSideEffects");
  isAsCheapAsAMove = R->getValueAsBit("isAsCheapAsAMove");
  hasExtraSrcRegAllocReq = R->getValueAsBit("hasExtraSrcRegAllocReq");
  hasExtraDefRegAllocReq = R->getValueAsBit("hasExtraDefRegAllocReq");
  isCodeGenOnly = R->getValueAsBit("isCodeGenOnly");
  isPseudo = R->getValueAsBit("isPseudo");
  ImplicitDefs = R->getValueAsListOfDefs("Defs");
  ImplicitUses = R->getValueAsListOfDefs("Uses");

  if (neverHasSideEffects + hasSideEffects > 1)
    throw R->getName() + ": multiple conflicting side-effect flags set!";

  // Parse Constraints.
  ParseConstraints(R->getValueAsString("Constraints"), Operands);

  // Parse the DisableEncoding field.
  Operands.ProcessDisableEncoding(R->getValueAsString("DisableEncoding"));
}

/// HasOneImplicitDefWithKnownVT - If the instruction has at least one
/// implicit def and it has a known VT, return the VT, otherwise return
/// MVT::Other.
MVT::SimpleValueType CodeGenInstruction::
HasOneImplicitDefWithKnownVT(const CodeGenTarget &TargetInfo) const {
  if (ImplicitDefs.empty()) return MVT::Other;

  // Check to see if the first implicit def has a resolvable type.
  Record *FirstImplicitDef = ImplicitDefs[0];
  assert(FirstImplicitDef->isSubClassOf("Register"));
  const std::vector<MVT::SimpleValueType> &RegVTs =
    TargetInfo.getRegisterVTs(FirstImplicitDef);
  if (RegVTs.size() == 1)
    return RegVTs[0];
  return MVT::Other;
}


/// FlattenAsmStringVariants - Flatten the specified AsmString to only
/// include text from the specified variant, returning the new string.
std::string CodeGenInstruction::
FlattenAsmStringVariants(StringRef Cur, unsigned Variant) {
  std::string Res = "";

  for (;;) {
    // Find the start of the next variant string.
    size_t VariantsStart = 0;
    for (size_t e = Cur.size(); VariantsStart != e; ++VariantsStart)
      if (Cur[VariantsStart] == '{' &&
          (VariantsStart == 0 || (Cur[VariantsStart-1] != '$' &&
                                  Cur[VariantsStart-1] != '\\')))
        break;

    // Add the prefix to the result.
    Res += Cur.slice(0, VariantsStart);
    if (VariantsStart == Cur.size())
      break;

    ++VariantsStart; // Skip the '{'.

    // Scan to the end of the variants string.
    size_t VariantsEnd = VariantsStart;
    unsigned NestedBraces = 1;
    for (size_t e = Cur.size(); VariantsEnd != e; ++VariantsEnd) {
      if (Cur[VariantsEnd] == '}' && Cur[VariantsEnd-1] != '\\') {
        if (--NestedBraces == 0)
          break;
      } else if (Cur[VariantsEnd] == '{')
        ++NestedBraces;
    }

    // Select the Nth variant (or empty).
    StringRef Selection = Cur.slice(VariantsStart, VariantsEnd);
    for (unsigned i = 0; i != Variant; ++i)
      Selection = Selection.split('|').second;
    Res += Selection.split('|').first;

    assert(VariantsEnd != Cur.size() &&
           "Unterminated variants in assembly string!");
    Cur = Cur.substr(VariantsEnd + 1);
  }

  return Res;
}


//===----------------------------------------------------------------------===//
/// CodeGenInstAlias Implementation
//===----------------------------------------------------------------------===//

/// tryAliasOpMatch - This is a helper function for the CodeGenInstAlias
/// constructor.  It checks if an argument in an InstAlias pattern matches
/// the corresponding operand of the instruction.  It returns true on a
/// successful match, with ResOp set to the result operand to be used.
bool CodeGenInstAlias::tryAliasOpMatch(const DagInit *Result, unsigned AliasOpNo,
                                       Record *InstOpRec, bool hasSubOps,
                                       SMLoc Loc, CodeGenTarget &T,
                                       ResultOperand &ResOp) {
  const Init *Arg = Result->getArg(AliasOpNo);
  const DefInit *ADI = dynamic_cast<const DefInit*>(Arg);

  if (ADI && ADI->getDef() == InstOpRec) {
    // If the operand is a record, it must have a name, and the record type
    // must match up with the instruction's argument type.
    if (Result->getArgName(AliasOpNo).empty())
      throw TGError(Loc, "result argument #" + utostr(AliasOpNo) +
                    " must have a name!");
    ResOp = ResultOperand(Result->getArgName(AliasOpNo), ADI->getDef());
    return true;
  }

  // Handle explicit registers.
  if (ADI && ADI->getDef()->isSubClassOf("Register")) {
    if (InstOpRec->isSubClassOf("RegisterOperand"))
      InstOpRec = InstOpRec->getValueAsDef("RegClass");

    if (!InstOpRec->isSubClassOf("RegisterClass"))
      return false;

    if (!T.getRegisterClass(InstOpRec)
        .contains(T.getRegBank().getReg(ADI->getDef())))
      throw TGError(Loc, "fixed register " +ADI->getDef()->getName()
                    + " is not a member of the " + InstOpRec->getName() +
                    " register class!");

    if (!Result->getArgName(AliasOpNo).empty())
      throw TGError(Loc, "result fixed register argument must "
                    "not have a name!");

    ResOp = ResultOperand(ADI->getDef());
    return true;
  }

  // Handle "zero_reg" for optional def operands.
  if (ADI && ADI->getDef()->getName() == "zero_reg") {

    // Check if this is an optional def.
    if (!InstOpRec->isSubClassOf("OptionalDefOperand"))
      throw TGError(Loc, "reg0 used for result that is not an "
                    "OptionalDefOperand!");

    ResOp = ResultOperand(static_cast<Record*>(0));
    return true;
  }

  if (const IntInit *II = dynamic_cast<const IntInit*>(Arg)) {
    if (hasSubOps || !InstOpRec->isSubClassOf("Operand"))
      return false;
    // Integer arguments can't have names.
    if (!Result->getArgName(AliasOpNo).empty())
      throw TGError(Loc, "result argument #" + utostr(AliasOpNo) +
                    " must not have a name!");
    ResOp = ResultOperand(II->getValue());
    return true;
  }

  return false;
}

CodeGenInstAlias::CodeGenInstAlias(Record *R, CodeGenTarget &T) : TheDef(R) {
  AsmString = R->getValueAsString("AsmString");
  Result = R->getValueAsDag("ResultInst");

  // Verify that the root of the result is an instruction.
  const DefInit *DI = dynamic_cast<const DefInit*>(Result->getOperator());
  if (DI == 0 || !DI->getDef()->isSubClassOf("Instruction"))
    throw TGError(R->getLoc(), "result of inst alias should be an instruction");

  ResultInst = &T.getInstruction(DI->getDef());

  // NameClass - If argument names are repeated, we need to verify they have
  // the same class.
  StringMap<Record*> NameClass;
  for (unsigned i = 0, e = Result->getNumArgs(); i != e; ++i) {
    const DefInit *ADI = dynamic_cast<const DefInit*>(Result->getArg(i));
    if (!ADI || Result->getArgName(i).empty())
      continue;
    // Verify we don't have something like: (someinst GR16:$foo, GR32:$foo)
    // $foo can exist multiple times in the result list, but it must have the
    // same type.
    Record *&Entry = NameClass[Result->getArgName(i)];
    if (Entry && Entry != ADI->getDef())
      throw TGError(R->getLoc(), "result value $" + Result->getArgName(i) +
                    " is both " + Entry->getName() + " and " +
                    ADI->getDef()->getName() + "!");
    Entry = ADI->getDef();
  }

  // Decode and validate the arguments of the result.
  unsigned AliasOpNo = 0;
  for (unsigned i = 0, e = ResultInst->Operands.size(); i != e; ++i) {

    // Tied registers don't have an entry in the result dag.
    if (ResultInst->Operands[i].getTiedRegister() != -1)
      continue;

    if (AliasOpNo >= Result->getNumArgs())
      throw TGError(R->getLoc(), "not enough arguments for instruction!");

    Record *InstOpRec = ResultInst->Operands[i].Rec;
    unsigned NumSubOps = ResultInst->Operands[i].MINumOperands;
    ResultOperand ResOp(static_cast<int64_t>(0));
    if (tryAliasOpMatch(Result, AliasOpNo, InstOpRec, (NumSubOps > 1),
                        R->getLoc(), T, ResOp)) {
      ResultOperands.push_back(ResOp);
      ResultInstOperandIndex.push_back(std::make_pair(i, -1));
      ++AliasOpNo;
      continue;
    }

    // If the argument did not match the instruction operand, and the operand
    // is composed of multiple suboperands, try matching the suboperands.
    if (NumSubOps > 1) {
      const DagInit *MIOI = ResultInst->Operands[i].MIOperandInfo;
      for (unsigned SubOp = 0; SubOp != NumSubOps; ++SubOp) {
        if (AliasOpNo >= Result->getNumArgs())
          throw TGError(R->getLoc(), "not enough arguments for instruction!");
        Record *SubRec = dynamic_cast<const DefInit*>(MIOI->getArg(SubOp))->getDef();
        if (tryAliasOpMatch(Result, AliasOpNo, SubRec, false,
                            R->getLoc(), T, ResOp)) {
          ResultOperands.push_back(ResOp);
          ResultInstOperandIndex.push_back(std::make_pair(i, SubOp));
          ++AliasOpNo;
        } else {
          throw TGError(R->getLoc(), "result argument #" + utostr(AliasOpNo) +
                        " does not match instruction operand class " +
                        (SubOp == 0 ? InstOpRec->getName() :SubRec->getName()));
        }
      }
      continue;
    }
    throw TGError(R->getLoc(), "result argument #" + utostr(AliasOpNo) +
                  " does not match instruction operand class " +
                  InstOpRec->getName());
  }

  if (AliasOpNo != Result->getNumArgs())
    throw TGError(R->getLoc(), "too many operands for instruction!");
}
