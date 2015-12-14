//===-- PPCSubtarget.h - Define Subtarget for the PPC ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the PowerPC specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_POWERPC_PPCSUBTARGET_H
#define LLVM_LIB_TARGET_POWERPC_PPCSUBTARGET_H

#include "PPCFrameLowering.h"
#include "PPCISelLowering.h"
#include "PPCInstrInfo.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/Target/TargetSelectionDAGInfo.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "PPCGenSubtargetInfo.inc"

// GCC #defines PPC on Linux but we use it as our namespace name
#undef PPC

namespace llvm {
class StringRef;

namespace PPC {
  // -m directive values.
  enum {
    DIR_NONE,
    DIR_32,
    DIR_440,
    DIR_601,
    DIR_602,
    DIR_603,
    DIR_7400,
    DIR_750,
    DIR_970,
    DIR_A2,
    DIR_E500mc,
    DIR_E5500,
    DIR_PWR3,
    DIR_PWR4,
    DIR_PWR5,
    DIR_PWR5X,
    DIR_PWR6,
    DIR_PWR6X,
    DIR_PWR7,
    DIR_PWR8,
    DIR_64
  };
}

class GlobalValue;
class TargetMachine;

class PPCSubtarget : public PPCGenSubtargetInfo {
protected:
  /// TargetTriple - What processor and OS we're targeting.
  Triple TargetTriple;

  /// stackAlignment - The minimum alignment known to hold of the stack frame on
  /// entry to the function and which must be maintained by every function.
  unsigned StackAlignment;

  /// Selected instruction itineraries (one entry per itinerary class.)
  InstrItineraryData InstrItins;

  /// Which cpu directive was used.
  unsigned DarwinDirective;

  /// Used by the ISel to turn in optimizations for POWER4-derived architectures
  bool HasMFOCRF;
  bool Has64BitSupport;
  bool Use64BitRegs;
  bool UseCRBits;
  bool UseSoftFloat;
  bool IsPPC64;
  bool HasAltivec;
  bool HasSPE;
  bool HasQPX;
  bool HasVSX;
  bool HasP8Vector;
  bool HasP8Altivec;
  bool HasP8Crypto;
  bool HasFCPSGN;
  bool HasFSQRT;
  bool HasFRE, HasFRES, HasFRSQRTE, HasFRSQRTES;
  bool HasRecipPrec;
  bool HasSTFIWX;
  bool HasLFIWAX;
  bool HasFPRND;
  bool HasFPCVT;
  bool HasISEL;
  bool HasPOPCNTD;
  bool HasBPERMD;
  bool HasExtDiv;
  bool HasCMPB;
  bool HasLDBRX;
  bool IsBookE;
  bool HasOnlyMSYNC;
  bool IsE500;
  bool IsPPC4xx;
  bool IsPPC6xx;
  bool FeatureMFTB;
  bool DeprecatedDST;
  bool HasLazyResolverStubs;
  bool IsLittleEndian;
  bool HasICBT;
  bool HasInvariantFunctionDescriptors;
  bool HasPartwordAtomics;
  bool HasDirectMove;
  bool HasHTM;
  bool HasFusion;

  /// When targeting QPX running a stock PPC64 Linux kernel where the stack
  /// alignment has not been changed, we need to keep the 16-byte alignment
  /// of the stack.
  bool IsQPXStackUnaligned;

  const PPCTargetMachine &TM;
  PPCFrameLowering FrameLowering;
  PPCInstrInfo InstrInfo;
  PPCTargetLowering TLInfo;
  TargetSelectionDAGInfo TSInfo;

public:
  /// This constructor initializes the data members to match that
  /// of the specified triple.
  ///
  PPCSubtarget(const Triple &TT, const std::string &CPU, const std::string &FS,
               const PPCTargetMachine &TM);

  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  /// getStackAlignment - Returns the minimum alignment known to hold of the
  /// stack frame on entry to the function and which must be maintained by every
  /// function for this subtarget.
  unsigned getStackAlignment() const { return StackAlignment; }

  /// getDarwinDirective - Returns the -m directive specified for the cpu.
  ///
  unsigned getDarwinDirective() const { return DarwinDirective; }

  /// getInstrItins - Return the instruction itineraries based on subtarget
  /// selection.
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }

  const PPCFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const PPCInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const PPCTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const TargetSelectionDAGInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  const PPCRegisterInfo *getRegisterInfo() const override {
    return &getInstrInfo()->getRegisterInfo();
  }
  const PPCTargetMachine &getTargetMachine() const { return TM; }

  /// initializeSubtargetDependencies - Initializes using a CPU and feature string
  /// so that we can use initializer lists for subtarget initialization.
  PPCSubtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS);

private:
  void initializeEnvironment();
  void initSubtargetFeatures(StringRef CPU, StringRef FS);

public:
  /// isPPC64 - Return true if we are generating code for 64-bit pointer mode.
  ///
  bool isPPC64() const;

  /// has64BitSupport - Return true if the selected CPU supports 64-bit
  /// instructions, regardless of whether we are in 32-bit or 64-bit mode.
  bool has64BitSupport() const { return Has64BitSupport; }
  // useSoftFloat - Return true if soft-float option is turned on.
  bool useSoftFloat() const { return UseSoftFloat; }

  /// use64BitRegs - Return true if in 64-bit mode or if we should use 64-bit
  /// registers in 32-bit mode when possible.  This can only true if
  /// has64BitSupport() returns true.
  bool use64BitRegs() const { return Use64BitRegs; }

  /// useCRBits - Return true if we should store and manipulate i1 values in
  /// the individual condition register bits.
  bool useCRBits() const { return UseCRBits; }

  /// hasLazyResolverStub - Return true if accesses to the specified global have
  /// to go through a dyld lazy resolution stub.  This means that an extra load
  /// is required to get the address of the global.
  bool hasLazyResolverStub(const GlobalValue *GV) const;

  // isLittleEndian - True if generating little-endian code
  bool isLittleEndian() const { return IsLittleEndian; }

  // Specific obvious features.
  bool hasFCPSGN() const { return HasFCPSGN; }
  bool hasFSQRT() const { return HasFSQRT; }
  bool hasFRE() const { return HasFRE; }
  bool hasFRES() const { return HasFRES; }
  bool hasFRSQRTE() const { return HasFRSQRTE; }
  bool hasFRSQRTES() const { return HasFRSQRTES; }
  bool hasRecipPrec() const { return HasRecipPrec; }
  bool hasSTFIWX() const { return HasSTFIWX; }
  bool hasLFIWAX() const { return HasLFIWAX; }
  bool hasFPRND() const { return HasFPRND; }
  bool hasFPCVT() const { return HasFPCVT; }
  bool hasAltivec() const { return HasAltivec; }
  bool hasSPE() const { return HasSPE; }
  bool hasQPX() const { return HasQPX; }
  bool hasVSX() const { return HasVSX; }
  bool hasP8Vector() const { return HasP8Vector; }
  bool hasP8Altivec() const { return HasP8Altivec; }
  bool hasP8Crypto() const { return HasP8Crypto; }
  bool hasMFOCRF() const { return HasMFOCRF; }
  bool hasISEL() const { return HasISEL; }
  bool hasPOPCNTD() const { return HasPOPCNTD; }
  bool hasBPERMD() const { return HasBPERMD; }
  bool hasExtDiv() const { return HasExtDiv; }
  bool hasCMPB() const { return HasCMPB; }
  bool hasLDBRX() const { return HasLDBRX; }
  bool isBookE() const { return IsBookE; }
  bool hasOnlyMSYNC() const { return HasOnlyMSYNC; }
  bool isPPC4xx() const { return IsPPC4xx; }
  bool isPPC6xx() const { return IsPPC6xx; }
  bool isE500() const { return IsE500; }
  bool isFeatureMFTB() const { return FeatureMFTB; }
  bool isDeprecatedDST() const { return DeprecatedDST; }
  bool hasICBT() const { return HasICBT; }
  bool hasInvariantFunctionDescriptors() const {
    return HasInvariantFunctionDescriptors;
  }
  bool hasPartwordAtomics() const { return HasPartwordAtomics; }
  bool hasDirectMove() const { return HasDirectMove; }

  bool isQPXStackUnaligned() const { return IsQPXStackUnaligned; }
  unsigned getPlatformStackAlignment() const {
    if ((hasQPX() || isBGQ()) && !isQPXStackUnaligned())
      return 32;

    return 16;
  }
  bool hasHTM() const { return HasHTM; }
  bool hasFusion() const { return HasFusion; }

  const Triple &getTargetTriple() const { return TargetTriple; }

  /// isDarwin - True if this is any darwin platform.
  bool isDarwin() const { return TargetTriple.isMacOSX(); }
  /// isBGQ - True if this is a BG/Q platform.
  bool isBGQ() const { return TargetTriple.getVendor() == Triple::BGQ; }

  bool isTargetELF() const { return TargetTriple.isOSBinFormatELF(); }
  bool isTargetMachO() const { return TargetTriple.isOSBinFormatMachO(); }

  bool isDarwinABI() const { return isTargetMachO() || isDarwin(); }
  bool isSVR4ABI() const { return !isDarwinABI(); }
  bool isELFv2ABI() const;

  bool enableEarlyIfConversion() const override { return hasISEL(); }

  // Scheduling customization.
  bool enableMachineScheduler() const override;
  // This overrides the PostRAScheduler bit in the SchedModel for each CPU.
  bool enablePostRAScheduler() const override;
  AntiDepBreakMode getAntiDepBreakMode() const override;
  void getCriticalPathRCs(RegClassVector &CriticalPathRCs) const override;

  void overrideSchedPolicy(MachineSchedPolicy &Policy,
                           MachineInstr *begin,
                           MachineInstr *end,
                           unsigned NumRegionInstrs) const override;
  bool useAA() const override;

  bool enableSubRegLiveness() const override;

  /// classifyGlobalReference - Classify a global variable reference for the
  /// current subtarget accourding to how we should reference it.
  unsigned char classifyGlobalReference(const GlobalValue *GV) const;
};
} // End llvm namespace

#endif
