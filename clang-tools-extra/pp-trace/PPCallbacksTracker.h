//===--- PPCallbacksTracker.h - Preprocessor tracking -*- C++ -*---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
///
/// \file
/// \brief Classes and definitions for preprocessor tracking.
///
/// The core definition is the PPCallbacksTracker class, derived from Clang's
/// PPCallbacks class from the Lex library, which overrides all the callbacks
/// and collects information about each callback call, saving it in a
/// data structure built up of CallbackCall and Argument objects, which
/// record the preprocessor callback name and arguments in high-level string
/// form for later inspection.
///
//===--------------------------------------------------------------------===//

#ifndef PPTRACE_PPCALLBACKSTRACKER_H
#define PPTRACE_PPCALLBACKSTRACKER_H

#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"

/// \brief This class represents one callback function argument by name
///   and value.
class Argument {
public:
  Argument(llvm::StringRef Name, llvm::StringRef Value)
      : Name(Name), Value(Value) {}
  Argument() {}

  std::string Name;
  std::string Value;
};

/// \brief This class represents one callback call by name and an array
///   of arguments.
class CallbackCall {
public:
  CallbackCall(llvm::StringRef Name) : Name(Name) {}
  CallbackCall() {}

  std::string Name;
  std::vector<Argument> Arguments;
};

/// \brief This class overrides the PPCallbacks class for tracking preprocessor
///   activity by means of its callback functions.
///
/// This object is given a vector for storing the trace information, built up
/// of CallbackCall and subordinate Argument objects for representing the
/// callback calls and their arguments.  It's a reference so the vector can
/// exist beyond the lifetime of this object, because it's deleted by the
/// preprocessor automatically in its destructor.
///
/// This class supports a mechanism for inhibiting trace output for
/// specific callbacks by name, for the purpose of eliminating output for
/// callbacks of no interest that might clutter the output.
///
/// Following the constructor and destructor function declarations, the
/// overidden callback functions are defined.  The remaining functions are
/// helpers for recording the trace data, to reduce the coupling between it
/// and the recorded data structure.
class PPCallbacksTracker : public clang::PPCallbacks {
public:
  /// \brief Note that all of the arguments are references, and owned
  /// by the caller.
  /// \param Ignore - Set of names of callbacks to ignore.
  /// \param CallbackCalls - Trace buffer.
  /// \param PP - The preprocessor.  Needed for getting some argument strings.
  PPCallbacksTracker(llvm::SmallSet<std::string, 4> &Ignore,
                     std::vector<CallbackCall> &CallbackCalls,
                     clang::Preprocessor &PP);

  virtual ~PPCallbacksTracker();

  // Overidden callback functions.

  void FileChanged(clang::SourceLocation Loc,
                   clang::PPCallbacks::FileChangeReason Reason,
                   clang::SrcMgr::CharacteristicKind FileType,
                   clang::FileID PrevFID = clang::FileID()) LLVM_OVERRIDE;
  void FileSkipped(const clang::FileEntry &ParentFile,
                   const clang::Token &FilenameTok,
                   clang::SrcMgr::CharacteristicKind FileType) LLVM_OVERRIDE;
  bool FileNotFound(llvm::StringRef FileName,
                    llvm::SmallVectorImpl<char> &RecoveryPath) LLVM_OVERRIDE;
  void InclusionDirective(clang::SourceLocation HashLoc,
                          const clang::Token &IncludeTok,
                          llvm::StringRef FileName, bool IsAngled,
                          clang::CharSourceRange FilenameRange,
                          const clang::FileEntry *File,
                          llvm::StringRef SearchPath,
                          llvm::StringRef RelativePath,
                          const clang::Module *Imported) LLVM_OVERRIDE;
  void moduleImport(clang::SourceLocation ImportLoc, clang::ModuleIdPath Path,
                    const clang::Module *Imported) LLVM_OVERRIDE;
  void EndOfMainFile() LLVM_OVERRIDE;
  void Ident(clang::SourceLocation Loc, const std::string &str) LLVM_OVERRIDE;
  void PragmaDirective(clang::SourceLocation Loc,
                       clang::PragmaIntroducerKind Introducer) LLVM_OVERRIDE;
  void PragmaComment(clang::SourceLocation Loc,
                     const clang::IdentifierInfo *Kind,
                     const std::string &Str) LLVM_OVERRIDE;
  void PragmaDetectMismatch(clang::SourceLocation Loc, const std::string &Name,
                            const std::string &Value) LLVM_OVERRIDE;
  void PragmaDebug(clang::SourceLocation Loc,
                   llvm::StringRef DebugType) LLVM_OVERRIDE;
  void PragmaMessage(clang::SourceLocation Loc, llvm::StringRef Namespace,
                     clang::PPCallbacks::PragmaMessageKind Kind,
                     llvm::StringRef Str) LLVM_OVERRIDE;
  void PragmaDiagnosticPush(clang::SourceLocation Loc,
                            llvm::StringRef Namespace) LLVM_OVERRIDE;
  void PragmaDiagnosticPop(clang::SourceLocation Loc,
                           llvm::StringRef Namespace) LLVM_OVERRIDE;
  void PragmaDiagnostic(clang::SourceLocation Loc, llvm::StringRef Namespace,
                        clang::diag::Mapping mapping,
                        llvm::StringRef Str) LLVM_OVERRIDE;
  void PragmaOpenCLExtension(clang::SourceLocation NameLoc,
                             const clang::IdentifierInfo *Name,
                             clang::SourceLocation StateLoc,
                             unsigned State) LLVM_OVERRIDE;
  void PragmaWarning(clang::SourceLocation Loc, llvm::StringRef WarningSpec,
                     llvm::ArrayRef<int> Ids) LLVM_OVERRIDE;
  void PragmaWarningPush(clang::SourceLocation Loc, int Level) LLVM_OVERRIDE;
  void PragmaWarningPop(clang::SourceLocation Loc) LLVM_OVERRIDE;
  void MacroExpands(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD, clang::SourceRange Range,
                    const clang::MacroArgs *Args) LLVM_OVERRIDE;
  void MacroDefined(const clang::Token &MacroNameTok,
                    const clang::MacroDirective *MD) LLVM_OVERRIDE;
  void MacroUndefined(const clang::Token &MacroNameTok,
                      const clang::MacroDirective *MD) LLVM_OVERRIDE;
  void Defined(const clang::Token &MacroNameTok,
               const clang::MacroDirective *MD,
               clang::SourceRange Range) LLVM_OVERRIDE;
  void SourceRangeSkipped(clang::SourceRange Range) LLVM_OVERRIDE;
  void If(clang::SourceLocation Loc, clang::SourceRange ConditionRange,
          bool ConditionValue) LLVM_OVERRIDE;
  void Elif(clang::SourceLocation Loc, clang::SourceRange ConditionRange,
            bool ConditionValue, clang::SourceLocation IfLoc) LLVM_OVERRIDE;
  void Ifdef(clang::SourceLocation Loc, const clang::Token &MacroNameTok,
             const clang::MacroDirective *MD) LLVM_OVERRIDE;
  void Ifndef(clang::SourceLocation Loc, const clang::Token &MacroNameTok,
              const clang::MacroDirective *MD) LLVM_OVERRIDE;
  void Else(clang::SourceLocation Loc,
            clang::SourceLocation IfLoc) LLVM_OVERRIDE;
  void Endif(clang::SourceLocation Loc,
             clang::SourceLocation IfLoc) LLVM_OVERRIDE;

  // Helper functions.

  /// \brief Start a new callback.
  void beginCallback(const char *Name);

  /// \brief Append a string to the top trace item.
  void append(const char *Str);

  /// \brief Append a bool argument to the top trace item.
  void appendArgument(const char *Name, bool Value);

  /// \brief Append an int argument to the top trace item.
  void appendArgument(const char *Name, int Value);

  /// \brief Append a string argument to the top trace item.
  void appendArgument(const char *Name, const char *Value);

  /// \brief Append a string reference object argument to the top trace item.
  void appendArgument(const char *Name, llvm::StringRef Value);

  /// \brief Append a string object argument to the top trace item.
  void appendArgument(const char *Name, const std::string &Value);

  /// \brief Append a token argument to the top trace item.
  void appendArgument(const char *Name, const clang::Token &Value);

  /// \brief Append an enum argument to the top trace item.
  void appendArgument(const char *Name, int Value, const char *Strings[]);

  /// \brief Append a FileID argument to the top trace item.
  void appendArgument(const char *Name, clang::FileID Value);

  /// \brief Append a FileEntry argument to the top trace item.
  void appendArgument(const char *Name, const clang::FileEntry *Value);

  /// \brief Append a SourceLocation argument to the top trace item.
  void appendArgument(const char *Name, clang::SourceLocation Value);

  /// \brief Append a SourceRange argument to the top trace item.
  void appendArgument(const char *Name, clang::SourceRange Value);

  /// \brief Append a CharSourceRange argument to the top trace item.
  void appendArgument(const char *Name, clang::CharSourceRange Value);

  /// \brief Append a ModuleIdPath argument to the top trace item.
  void appendArgument(const char *Name, clang::ModuleIdPath Value);

  /// \brief Append an IdentifierInfo argument to the top trace item.
  void appendArgument(const char *Name, const clang::IdentifierInfo *Value);

  /// \brief Append a MacroDirective argument to the top trace item.
  void appendArgument(const char *Name, const clang::MacroDirective *Value);

  /// \brief Append a MacroArgs argument to the top trace item.
  void appendArgument(const char *Name, const clang::MacroArgs *Value);

  /// \brief Append a Module argument to the top trace item.
  void appendArgument(const char *Name, const clang::Module *Value);

  /// \brief Append a double-quoted argument to the top trace item.
  void appendQuotedArgument(const char *Name, const std::string &Value);

  /// \brief Append a double-quoted file path argument to the top trace item.
  void appendFilePathArgument(const char *Name, llvm::StringRef Value);

  /// \brief Get the raw source string of the range.
  llvm::StringRef getSourceString(clang::CharSourceRange Range);

  /// \brief Callback trace information.
  /// We use a reference so the trace will be preserved for the caller
  /// after this object is destructed.
  std::vector<CallbackCall> &CallbackCalls;

  /// \brief Names of callbacks to ignore.
  llvm::SmallSet<std::string, 4> &Ignore;

  /// \brief Inhibit trace while this is set.
  bool DisableTrace;

  clang::Preprocessor &PP;
};

#endif // PPTRACE_PPCALLBACKSTRACKER_H
