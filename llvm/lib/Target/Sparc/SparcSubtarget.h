//===-- SparcSubtarget.h - Define Subtarget for the SPARC -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the SPARC specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef SPARC_SUBTARGET_H
#define SPARC_SUBTARGET_H

#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "SparcGenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class SparcSubtarget : public SparcGenSubtargetInfo {
  virtual void anchor();
  bool IsV9;
  bool V8DeprecatedInsts;
  bool IsVIS;
  bool Is64Bit;
  bool HasHardQuad;

public:
  SparcSubtarget(const std::string &TT, const std::string &CPU,
                 const std::string &FS, bool is64bit);

  bool isV9() const { return IsV9; }
  bool isVIS() const { return IsVIS; }
  bool useDeprecatedV8Instructions() const { return V8DeprecatedInsts; }
  bool hasHardQuad() const { return HasHardQuad; }

  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  bool is64Bit() const { return Is64Bit; }

  /// The 64-bit ABI uses biased stack and frame pointers, so the stack frame
  /// of the current function is the area from [%sp+BIAS] to [%fp+BIAS].
  int64_t getStackPointerBias() const {
    return is64Bit() ? 2047 : 0;
  }

  /// Given a actual stack size as determined by FrameInfo, this function
  /// returns adjusted framesize which includes space for register window
  /// spills and arguments.
  int getAdjustedFrameSize(int stackSize) const;

};

} // end namespace llvm

#endif
