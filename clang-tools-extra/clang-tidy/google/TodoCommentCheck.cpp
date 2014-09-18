//===--- TodoCommentCheck.cpp - clang-tidy --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TodoCommentCheck.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"

namespace clang {
namespace tidy {
namespace readability {

namespace {
class TodoCommentHandler : public CommentHandler {
public:
  explicit TodoCommentHandler(TodoCommentCheck &Check)
      : Check(Check), TodoMatch("^// *TODO(\\(.*\\))?:?( )?(.*)$") {}

  bool HandleComment(Preprocessor &PP, SourceRange Range) override {
    StringRef Text =
        Lexer::getSourceText(CharSourceRange::getCharRange(Range),
                             PP.getSourceManager(), PP.getLangOpts());

    SmallVector<StringRef, 4> Matches;
    if (!TodoMatch.match(Text, &Matches))
      return false;

    StringRef Username = Matches[1];
    StringRef Comment = Matches[3];

    if (!Username.empty())
      return false;

    // If the username is missing put in the current user's name. Not ideal but
    // works for running tidy locally.
    // FIXME: Can we get this from a more reliable source?
    const char *User = std::getenv("USER");
    if (!User)
      User = "unknown";
    std::string NewText = ("// TODO(" + Twine(User) + "): " + Comment).str();

    Check.diag(Range.getBegin(), "missing username/bug in TODO")
        << FixItHint::CreateReplacement(CharSourceRange::getCharRange(Range),
                                        NewText);
    return false;
  }

private:
  TodoCommentCheck &Check;
  llvm::Regex TodoMatch;
};
} // namespace

void TodoCommentCheck::registerPPCallbacks(CompilerInstance &Compiler) {
  Compiler.getPreprocessor().addCommentHandler(new TodoCommentHandler(*this));
}

} // namespace readability
} // namespace tidy
} // namespace clang
