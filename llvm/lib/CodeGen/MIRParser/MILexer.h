//===- MILexer.h - Lexer for machine instructions -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the function that lexes the machine instruction source
// string.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_CODEGEN_MIRPARSER_MILEXER_H
#define LLVM_LIB_CODEGEN_MIRPARSER_MILEXER_H

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/STLExtras.h"
#include <functional>

namespace llvm {

class Twine;

/// A token produced by the machine instruction lexer.
struct MIToken {
  enum TokenKind {
    // Markers
    Eof,
    Error,

    // Tokens with no info.
    comma,
    equal,
    underscore,
    colon,
    coloncolon,
    exclaim,
    lparen,
    rparen,
    plus,
    minus,

    // Keywords
    kw_implicit,
    kw_implicit_define,
    kw_dead,
    kw_killed,
    kw_undef,
    kw_early_clobber,
    kw_debug_use,
    kw_frame_setup,
    kw_debug_location,
    kw_cfi_offset,
    kw_cfi_def_cfa_register,
    kw_cfi_def_cfa_offset,
    kw_cfi_def_cfa,
    kw_blockaddress,
    kw_target_index,
    kw_half,
    kw_float,
    kw_double,
    kw_x86_fp80,
    kw_fp128,
    kw_ppc_fp128,
    kw_target_flags,
    kw_volatile,
    kw_non_temporal,
    kw_invariant,

    // Identifier tokens
    Identifier,
    IntegerType,
    NamedRegister,
    MachineBasicBlock,
    StackObject,
    FixedStackObject,
    NamedGlobalValue,
    GlobalValue,
    ExternalSymbol,

    // Other tokens
    IntegerLiteral,
    FloatingPointLiteral,
    VirtualRegister,
    ConstantPoolItem,
    JumpTableIndex,
    NamedIRBlock,
    IRBlock,
    NamedIRValue,
  };

private:
  TokenKind Kind;
  unsigned StringOffset;
  bool HasStringValue;
  StringRef Range;
  std::string StringValue;
  APSInt IntVal;

public:
  MIToken(TokenKind Kind, StringRef Range, unsigned StringOffset = 0)
      : Kind(Kind), StringOffset(StringOffset), HasStringValue(false),
        Range(Range) {}

  MIToken(TokenKind Kind, StringRef Range, std::string StringValue,
          unsigned StringOffset = 0)
      : Kind(Kind), StringOffset(StringOffset), HasStringValue(true),
        Range(Range), StringValue(std::move(StringValue)) {}

  MIToken(TokenKind Kind, StringRef Range, const APSInt &IntVal,
          unsigned StringOffset = 0)
      : Kind(Kind), StringOffset(StringOffset), HasStringValue(false),
        Range(Range), IntVal(IntVal) {}

  TokenKind kind() const { return Kind; }

  bool isError() const { return Kind == Error; }

  bool isRegister() const {
    return Kind == NamedRegister || Kind == underscore ||
           Kind == VirtualRegister;
  }

  bool isRegisterFlag() const {
    return Kind == kw_implicit || Kind == kw_implicit_define ||
           Kind == kw_dead || Kind == kw_killed || Kind == kw_undef ||
           Kind == kw_early_clobber || Kind == kw_debug_use;
  }

  bool isMemoryOperandFlag() const {
    return Kind == kw_volatile || Kind == kw_non_temporal ||
           Kind == kw_invariant;
  }

  bool is(TokenKind K) const { return Kind == K; }

  bool isNot(TokenKind K) const { return Kind != K; }

  StringRef::iterator location() const { return Range.begin(); }

  /// Return the token's raw string value.
  ///
  /// If the string value is quoted, this method returns that quoted string as
  /// it is, without unescaping the string value.
  StringRef rawStringValue() const { return Range.drop_front(StringOffset); }

  /// Return the token's string value.
  StringRef stringValue() const {
    return HasStringValue ? StringRef(StringValue)
                          : Range.drop_front(StringOffset);
  }

  const APSInt &integerValue() const { return IntVal; }

  bool hasIntegerValue() const {
    return Kind == IntegerLiteral || Kind == MachineBasicBlock ||
           Kind == StackObject || Kind == FixedStackObject ||
           Kind == GlobalValue || Kind == VirtualRegister ||
           Kind == ConstantPoolItem || Kind == JumpTableIndex ||
           Kind == IRBlock;
  }
};

/// Consume a single machine instruction token in the given source and return
/// the remaining source string.
StringRef lexMIToken(
    StringRef Source, MIToken &Token,
    function_ref<void(StringRef::iterator, const Twine &)> ErrorCallback);

} // end namespace llvm

#endif
