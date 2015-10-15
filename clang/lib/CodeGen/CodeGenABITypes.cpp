//==--- CodeGenABITypes.cpp - Convert Clang types to LLVM types for ABI ----==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// CodeGenABITypes is a simple interface for getting LLVM types for
// the parameters and the return value of a function given the Clang
// types.
//
// The class is implemented as a public wrapper around the private
// CodeGenTypes class in lib/CodeGen.
//
//===----------------------------------------------------------------------===//

#include "clang/CodeGen/CodeGenABITypes.h"
#include "CodeGenModule.h"
#include "clang/CodeGen/CGFunctionInfo.h"
#include "clang/Frontend/CodeGenOptions.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/PreprocessorOptions.h"

using namespace clang;
using namespace CodeGen;

CodeGenABITypes::CodeGenABITypes(ASTContext &C, llvm::Module &M,
                                 CoverageSourceInfo *CoverageInfo)
    : CGO(new CodeGenOptions), HSO(new HeaderSearchOptions),
      PPO(new PreprocessorOptions),
      CGM(new CodeGen::CodeGenModule(C, *HSO, *PPO, *CGO, M, C.getDiagnostics(),
                                     CoverageInfo)) {}

// Explicitly out-of-line because ~CodeGenModule() is private but
// CodeGenABITypes.h is part of clang's API.
CodeGenABITypes::~CodeGenABITypes() = default;
