//===-- MipsastISel.cpp - Mips FastISel implementation
//---------------------===//

#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "llvm/CodeGen/FastISel.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "MipsRegisterInfo.h"
#include "MipsISelLowering.h"
#include "MipsMachineFunction.h"
#include "MipsSubtarget.h"
#include "MipsTargetMachine.h"

using namespace llvm;

namespace {

// All possible address modes.
typedef struct Address {
  enum { RegBase, FrameIndexBase } BaseType;

  union {
    unsigned Reg;
    int FI;
  } Base;

  int64_t Offset;

  // Innocuous defaults for our address.
  Address() : BaseType(RegBase), Offset(0) { Base.Reg = 0; }
} Address;

class MipsFastISel final : public FastISel {

  /// Subtarget - Keep a pointer to the MipsSubtarget around so that we can
  /// make the right decision when generating code for different targets.
  Module &M;
  const TargetMachine &TM;
  const TargetInstrInfo &TII;
  const TargetLowering &TLI;
  const MipsSubtarget *Subtarget;
  MipsFunctionInfo *MFI;

  // Convenience variables to avoid some queries.
  LLVMContext *Context;

  bool TargetSupported;
  bool UnsupportedFPMode;

public:
  explicit MipsFastISel(FunctionLoweringInfo &funcInfo,
                        const TargetLibraryInfo *libInfo)
      : FastISel(funcInfo, libInfo),
        M(const_cast<Module &>(*funcInfo.Fn->getParent())),
        TM(funcInfo.MF->getTarget()),
        TII(*TM.getSubtargetImpl()->getInstrInfo()),
        TLI(*TM.getSubtargetImpl()->getTargetLowering()),
        Subtarget(&TM.getSubtarget<MipsSubtarget>()) {
    MFI = funcInfo.MF->getInfo<MipsFunctionInfo>();
    Context = &funcInfo.Fn->getContext();
    TargetSupported = ((Subtarget->getRelocationModel() == Reloc::PIC_) &&
                       ((Subtarget->hasMips32r2() || Subtarget->hasMips32()) &&
                        (Subtarget->isABI_O32())));
    UnsupportedFPMode = Subtarget->isFP64bit();
  }

  bool fastSelectInstruction(const Instruction *I) override;
  unsigned fastMaterializeConstant(const Constant *C) override;

  bool ComputeAddress(const Value *Obj, Address &Addr);

private:
  bool EmitLoad(MVT VT, unsigned &ResultReg, Address &Addr,
                unsigned Alignment = 0);
  bool EmitStore(MVT VT, unsigned SrcReg, Address &Addr,
                 unsigned Alignment = 0);
  bool SelectLoad(const Instruction *I);
  bool SelectRet(const Instruction *I);
  bool SelectStore(const Instruction *I);
  bool SelectIntExt(const Instruction *I);
  bool SelectTrunc(const Instruction *I);
  bool SelectFPExt(const Instruction *I);
  bool SelectFPTrunc(const Instruction *I);
  bool SelectFPToI(const Instruction *I, bool IsSigned);
  bool SelectCmp(const Instruction *I);

  bool isTypeLegal(Type *Ty, MVT &VT);
  bool isLoadTypeLegal(Type *Ty, MVT &VT);

  unsigned getRegEnsuringSimpleIntegerWidening(const Value *, bool IsUnsigned);

  unsigned MaterializeFP(const ConstantFP *CFP, MVT VT);
  unsigned MaterializeGV(const GlobalValue *GV, MVT VT);
  unsigned MaterializeInt(const Constant *C, MVT VT);
  unsigned Materialize32BitInt(int64_t Imm, const TargetRegisterClass *RC);

  bool EmitIntExt(MVT SrcVT, unsigned SrcReg, MVT DestVT, unsigned DestReg,
                  bool IsZExt);

  bool EmitIntZExt(MVT SrcVT, unsigned SrcReg, MVT DestVT, unsigned DestReg);

  bool EmitIntSExt(MVT SrcVT, unsigned SrcReg, MVT DestVT, unsigned DestReg);
  bool EmitIntSExt32r1(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                       unsigned DestReg);
  bool EmitIntSExt32r2(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                       unsigned DestReg);
  // for some reason, this default is not generated by tablegen
  // so we explicitly generate it here.
  //
  unsigned fastEmitInst_riir(uint64_t inst, const TargetRegisterClass *RC,
                             unsigned Op0, bool Op0IsKill, uint64_t imm1,
                             uint64_t imm2, unsigned Op3, bool Op3IsKill) {
    return 0;
  }

  MachineInstrBuilder EmitInst(unsigned Opc) {
    return BuildMI(*FuncInfo.MBB, FuncInfo.InsertPt, DbgLoc, TII.get(Opc));
  }

  MachineInstrBuilder EmitInst(unsigned Opc, unsigned DstReg) {
    return BuildMI(*FuncInfo.MBB, FuncInfo.InsertPt, DbgLoc, TII.get(Opc),
                   DstReg);
  }

  MachineInstrBuilder EmitInstStore(unsigned Opc, unsigned SrcReg,
                                    unsigned MemReg, int64_t MemOffset) {
    return EmitInst(Opc).addReg(SrcReg).addReg(MemReg).addImm(MemOffset);
  }

  MachineInstrBuilder EmitInstLoad(unsigned Opc, unsigned DstReg,
                                   unsigned MemReg, int64_t MemOffset) {
    return EmitInst(Opc, DstReg).addReg(MemReg).addImm(MemOffset);
  }

#include "MipsGenFastISel.inc"
};

bool MipsFastISel::isTypeLegal(Type *Ty, MVT &VT) {
  EVT evt = TLI.getValueType(Ty, true);
  // Only handle simple types.
  if (evt == MVT::Other || !evt.isSimple())
    return false;
  VT = evt.getSimpleVT();

  // Handle all legal types, i.e. a register that will directly hold this
  // value.
  return TLI.isTypeLegal(VT);
}

bool MipsFastISel::isLoadTypeLegal(Type *Ty, MVT &VT) {
  if (isTypeLegal(Ty, VT))
    return true;
  // We will extend this in a later patch:
  //   If this is a type than can be sign or zero-extended to a basic operation
  //   go ahead and accept it now.
  if (VT == MVT::i8 || VT == MVT::i16)
    return true;
  return false;
}

bool MipsFastISel::ComputeAddress(const Value *Obj, Address &Addr) {
  // This construct looks a big awkward but it is how other ports handle this
  // and as this function is more fully completed, these cases which
  // return false will have additional code in them.
  //
  if (isa<Instruction>(Obj))
    return false;
  else if (isa<ConstantExpr>(Obj))
    return false;
  Addr.Base.Reg = getRegForValue(Obj);
  return Addr.Base.Reg != 0;
}

unsigned MipsFastISel::getRegEnsuringSimpleIntegerWidening(const Value *V,
                                                           bool IsUnsigned) {
  unsigned VReg = getRegForValue(V);
  if (VReg == 0)
    return 0;
  MVT VMVT = TLI.getValueType(V->getType(), true).getSimpleVT();
  if ((VMVT == MVT::i8) || (VMVT == MVT::i16)) {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    if (!EmitIntExt(VMVT, VReg, MVT::i32, TempReg, IsUnsigned))
      return 0;
    VReg = TempReg;
  }
  return VReg;
}

bool MipsFastISel::EmitLoad(MVT VT, unsigned &ResultReg, Address &Addr,
                            unsigned Alignment) {
  //
  // more cases will be handled here in following patches.
  //
  unsigned Opc;
  switch (VT.SimpleTy) {
  case MVT::i32: {
    ResultReg = createResultReg(&Mips::GPR32RegClass);
    Opc = Mips::LW;
    break;
  }
  case MVT::i16: {
    ResultReg = createResultReg(&Mips::GPR32RegClass);
    Opc = Mips::LHu;
    break;
  }
  case MVT::i8: {
    ResultReg = createResultReg(&Mips::GPR32RegClass);
    Opc = Mips::LBu;
    break;
  }
  case MVT::f32: {
    if (UnsupportedFPMode)
      return false;
    ResultReg = createResultReg(&Mips::FGR32RegClass);
    Opc = Mips::LWC1;
    break;
  }
  case MVT::f64: {
    if (UnsupportedFPMode)
      return false;
    ResultReg = createResultReg(&Mips::AFGR64RegClass);
    Opc = Mips::LDC1;
    break;
  }
  default:
    return false;
  }
  EmitInstLoad(Opc, ResultReg, Addr.Base.Reg, Addr.Offset);
  return true;
}

// Materialize a constant into a register, and return the register
// number (or zero if we failed to handle it).
unsigned MipsFastISel::fastMaterializeConstant(const Constant *C) {
  EVT CEVT = TLI.getValueType(C->getType(), true);

  // Only handle simple types.
  if (!CEVT.isSimple())
    return 0;
  MVT VT = CEVT.getSimpleVT();

  if (const ConstantFP *CFP = dyn_cast<ConstantFP>(C))
    return (UnsupportedFPMode) ? 0 : MaterializeFP(CFP, VT);
  else if (const GlobalValue *GV = dyn_cast<GlobalValue>(C))
    return MaterializeGV(GV, VT);
  else if (isa<ConstantInt>(C))
    return MaterializeInt(C, VT);

  return 0;
}

bool MipsFastISel::EmitStore(MVT VT, unsigned SrcReg, Address &Addr,
                             unsigned Alignment) {
  //
  // more cases will be handled here in following patches.
  //
  unsigned Opc;
  switch (VT.SimpleTy) {
  case MVT::i8:
    Opc = Mips::SB;
    break;
  case MVT::i16:
    Opc = Mips::SH;
    break;
  case MVT::i32:
    Opc = Mips::SW;
    break;
  case MVT::f32:
    if (UnsupportedFPMode)
      return false;
    Opc = Mips::SWC1;
    break;
  case MVT::f64:
    if (UnsupportedFPMode)
      return false;
    Opc = Mips::SDC1;
    break;
  default:
    return false;
  }
  EmitInstStore(Opc, SrcReg, Addr.Base.Reg, Addr.Offset);
  return true;
}

bool MipsFastISel::EmitIntSExt32r1(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                                   unsigned DestReg) {
  unsigned ShiftAmt;
  switch (SrcVT.SimpleTy) {
  default:
    return false;
  case MVT::i8:
    ShiftAmt = 24;
    break;
  case MVT::i16:
    ShiftAmt = 16;
    break;
  }
  unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
  EmitInst(Mips::SLL, TempReg).addReg(SrcReg).addImm(ShiftAmt);
  EmitInst(Mips::SRA, DestReg).addReg(TempReg).addImm(ShiftAmt);
  return true;
}

bool MipsFastISel::EmitIntSExt32r2(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                                   unsigned DestReg) {
  switch (SrcVT.SimpleTy) {
  default:
    return false;
  case MVT::i8:
    EmitInst(Mips::SEB, DestReg).addReg(SrcReg);
    break;
  case MVT::i16:
    EmitInst(Mips::SEH, DestReg).addReg(SrcReg);
    break;
  }
  return true;
}

bool MipsFastISel::EmitIntExt(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                              unsigned DestReg, bool IsZExt) {
  if (IsZExt)
    return EmitIntZExt(SrcVT, SrcReg, DestVT, DestReg);
  return EmitIntSExt(SrcVT, SrcReg, DestVT, DestReg);
}

bool MipsFastISel::EmitIntSExt(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                               unsigned DestReg) {
  if ((DestVT != MVT::i32) && (DestVT != MVT::i16))
    return false;
  if (Subtarget->hasMips32r2())
    return EmitIntSExt32r2(SrcVT, SrcReg, DestVT, DestReg);
  return EmitIntSExt32r1(SrcVT, SrcReg, DestVT, DestReg);
}

bool MipsFastISel::EmitIntZExt(MVT SrcVT, unsigned SrcReg, MVT DestVT,
                               unsigned DestReg) {
  switch (SrcVT.SimpleTy) {
  default:
    return false;
  case MVT::i1:
    EmitInst(Mips::ANDi, DestReg).addReg(SrcReg).addImm(1);
    break;
  case MVT::i8:
    EmitInst(Mips::ANDi, DestReg).addReg(SrcReg).addImm(0xff);
    break;
  case MVT::i16:
    EmitInst(Mips::ANDi, DestReg).addReg(SrcReg).addImm(0xffff);
    break;
  }
  return true;
}

bool MipsFastISel::SelectLoad(const Instruction *I) {
  // Atomic loads need special handling.
  if (cast<LoadInst>(I)->isAtomic())
    return false;

  // Verify we have a legal type before going any further.
  MVT VT;
  if (!isLoadTypeLegal(I->getType(), VT))
    return false;

  // See if we can handle this address.
  Address Addr;
  if (!ComputeAddress(I->getOperand(0), Addr))
    return false;

  unsigned ResultReg;
  if (!EmitLoad(VT, ResultReg, Addr, cast<LoadInst>(I)->getAlignment()))
    return false;
  updateValueMap(I, ResultReg);
  return true;
}

bool MipsFastISel::SelectStore(const Instruction *I) {
  Value *Op0 = I->getOperand(0);
  unsigned SrcReg = 0;

  // Atomic stores need special handling.
  if (cast<StoreInst>(I)->isAtomic())
    return false;

  // Verify we have a legal type before going any further.
  MVT VT;
  if (!isLoadTypeLegal(I->getOperand(0)->getType(), VT))
    return false;

  // Get the value to be stored into a register.
  SrcReg = getRegForValue(Op0);
  if (SrcReg == 0)
    return false;

  // See if we can handle this address.
  Address Addr;
  if (!ComputeAddress(I->getOperand(1), Addr))
    return false;

  if (!EmitStore(VT, SrcReg, Addr, cast<StoreInst>(I)->getAlignment()))
    return false;
  return true;
}

bool MipsFastISel::SelectRet(const Instruction *I) {
  const ReturnInst *Ret = cast<ReturnInst>(I);

  if (!FuncInfo.CanLowerReturn)
    return false;
  if (Ret->getNumOperands() > 0) {
    return false;
  }
  EmitInst(Mips::RetRA);
  return true;
}

// Attempt to fast-select a floating-point extend instruction.
bool MipsFastISel::SelectFPExt(const Instruction *I) {
  if (UnsupportedFPMode)
    return false;
  Value *Src = I->getOperand(0);
  EVT SrcVT = TLI.getValueType(Src->getType(), true);
  EVT DestVT = TLI.getValueType(I->getType(), true);

  if (SrcVT != MVT::f32 || DestVT != MVT::f64)
    return false;

  unsigned SrcReg =
      getRegForValue(Src); // his must be a 32 bit floating point register class
                           // maybe we should handle this differently
  if (!SrcReg)
    return false;

  unsigned DestReg = createResultReg(&Mips::AFGR64RegClass);
  EmitInst(Mips::CVT_D32_S, DestReg).addReg(SrcReg);
  updateValueMap(I, DestReg);
  return true;
}

// Attempt to fast-select a floating-point truncate instruction.
bool MipsFastISel::SelectFPTrunc(const Instruction *I) {
  if (UnsupportedFPMode)
    return false;
  Value *Src = I->getOperand(0);
  EVT SrcVT = TLI.getValueType(Src->getType(), true);
  EVT DestVT = TLI.getValueType(I->getType(), true);

  if (SrcVT != MVT::f64 || DestVT != MVT::f32)
    return false;

  unsigned SrcReg = getRegForValue(Src);
  if (!SrcReg)
    return false;

  unsigned DestReg = createResultReg(&Mips::FGR32RegClass);
  if (!DestReg)
    return false;

  EmitInst(Mips::CVT_S_D32, DestReg).addReg(SrcReg);
  updateValueMap(I, DestReg);
  return true;
}

bool MipsFastISel::SelectIntExt(const Instruction *I) {
  Type *DestTy = I->getType();
  Value *Src = I->getOperand(0);
  Type *SrcTy = Src->getType();

  bool isZExt = isa<ZExtInst>(I);
  unsigned SrcReg = getRegForValue(Src);
  if (!SrcReg)
    return false;

  EVT SrcEVT, DestEVT;
  SrcEVT = TLI.getValueType(SrcTy, true);
  DestEVT = TLI.getValueType(DestTy, true);
  if (!SrcEVT.isSimple())
    return false;
  if (!DestEVT.isSimple())
    return false;

  MVT SrcVT = SrcEVT.getSimpleVT();
  MVT DestVT = DestEVT.getSimpleVT();
  unsigned ResultReg = createResultReg(&Mips::GPR32RegClass);

  if (!EmitIntExt(SrcVT, SrcReg, DestVT, ResultReg, isZExt))
    return false;
  updateValueMap(I, ResultReg);
  return true;
}

bool MipsFastISel::SelectTrunc(const Instruction *I) {
  // The high bits for a type smaller than the register size are assumed to be
  // undefined.
  Value *Op = I->getOperand(0);

  EVT SrcVT, DestVT;
  SrcVT = TLI.getValueType(Op->getType(), true);
  DestVT = TLI.getValueType(I->getType(), true);

  if (SrcVT != MVT::i32 && SrcVT != MVT::i16 && SrcVT != MVT::i8)
    return false;
  if (DestVT != MVT::i16 && DestVT != MVT::i8 && DestVT != MVT::i1)
    return false;

  unsigned SrcReg = getRegForValue(Op);
  if (!SrcReg)
    return false;

  // Because the high bits are undefined, a truncate doesn't generate
  // any code.
  updateValueMap(I, SrcReg);
  return true;
}

// Attempt to fast-select a floating-point-to-integer conversion.
bool MipsFastISel::SelectFPToI(const Instruction *I, bool IsSigned) {
  if (UnsupportedFPMode)
    return false;
  MVT DstVT, SrcVT;
  if (!IsSigned)
    return false; // We don't handle this case yet. There is no native
                  // instruction for this but it can be synthesized.
  Type *DstTy = I->getType();
  if (!isTypeLegal(DstTy, DstVT))
    return false;

  if (DstVT != MVT::i32)
    return false;

  Value *Src = I->getOperand(0);
  Type *SrcTy = Src->getType();
  if (!isTypeLegal(SrcTy, SrcVT))
    return false;

  if (SrcVT != MVT::f32 && SrcVT != MVT::f64)
    return false;

  unsigned SrcReg = getRegForValue(Src);
  if (SrcReg == 0)
    return false;

  // Determine the opcode for the conversion, which takes place
  // entirely within FPRs.
  unsigned DestReg = createResultReg(&Mips::GPR32RegClass);
  unsigned TempReg = createResultReg(&Mips::FGR32RegClass);
  unsigned Opc;

  if (SrcVT == MVT::f32)
    Opc = Mips::TRUNC_W_S;
  else
    Opc = Mips::TRUNC_W_D32;

  // Generate the convert.
  EmitInst(Opc, TempReg).addReg(SrcReg);

  EmitInst(Mips::MFC1, DestReg).addReg(TempReg);

  updateValueMap(I, DestReg);
  return true;
}

//
// Because of how SelectCmp is called with fast-isel, you can
// end up with redundant "andi" instructions after the sequences emitted below.
// We should try and solve this issue in the future.
//
bool MipsFastISel::SelectCmp(const Instruction *I) {
  const CmpInst *CI = cast<CmpInst>(I);
  bool IsUnsigned = CI->isUnsigned();
  const Value *Left = I->getOperand(0), *Right = I->getOperand(1);

  unsigned LeftReg = getRegEnsuringSimpleIntegerWidening(Left, IsUnsigned);
  if (LeftReg == 0)
    return false;
  unsigned RightReg = getRegEnsuringSimpleIntegerWidening(Right, IsUnsigned);
  if (RightReg == 0)
    return false;
  unsigned ResultReg = createResultReg(&Mips::GPR32RegClass);
  CmpInst::Predicate P = CI->getPredicate();
  switch (P) {
  default:
    return false;
  case CmpInst::ICMP_EQ: {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::XOR, TempReg).addReg(LeftReg).addReg(RightReg);
    EmitInst(Mips::SLTiu, ResultReg).addReg(TempReg).addImm(1);
    break;
  }
  case CmpInst::ICMP_NE: {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::XOR, TempReg).addReg(LeftReg).addReg(RightReg);
    EmitInst(Mips::SLTu, ResultReg).addReg(Mips::ZERO).addReg(TempReg);
    break;
  }
  case CmpInst::ICMP_UGT: {
    EmitInst(Mips::SLTu, ResultReg).addReg(RightReg).addReg(LeftReg);
    break;
  }
  case CmpInst::ICMP_ULT: {
    EmitInst(Mips::SLTu, ResultReg).addReg(LeftReg).addReg(RightReg);
    break;
  }
  case CmpInst::ICMP_UGE: {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::SLTu, TempReg).addReg(LeftReg).addReg(RightReg);
    EmitInst(Mips::XORi, ResultReg).addReg(TempReg).addImm(1);
    break;
  }
  case CmpInst::ICMP_ULE: {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::SLTu, TempReg).addReg(RightReg).addReg(LeftReg);
    EmitInst(Mips::XORi, ResultReg).addReg(TempReg).addImm(1);
    break;
  }
  case CmpInst::ICMP_SGT: {
    EmitInst(Mips::SLT, ResultReg).addReg(RightReg).addReg(LeftReg);
    break;
  }
  case CmpInst::ICMP_SLT: {
    EmitInst(Mips::SLT, ResultReg).addReg(LeftReg).addReg(RightReg);
    break;
  }
  case CmpInst::ICMP_SGE: {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::SLT, TempReg).addReg(LeftReg).addReg(RightReg);
    EmitInst(Mips::XORi, ResultReg).addReg(TempReg).addImm(1);
    break;
  }
  case CmpInst::ICMP_SLE: {
    unsigned TempReg = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::SLT, TempReg).addReg(RightReg).addReg(LeftReg);
    EmitInst(Mips::XORi, ResultReg).addReg(TempReg).addImm(1);
    break;
  }
  case CmpInst::FCMP_OEQ:
  case CmpInst::FCMP_UNE:
  case CmpInst::FCMP_OLT:
  case CmpInst::FCMP_OLE:
  case CmpInst::FCMP_OGT:
  case CmpInst::FCMP_OGE: {
    if (UnsupportedFPMode)
      return false;
    bool IsFloat = Left->getType()->isFloatTy();
    bool IsDouble = Left->getType()->isDoubleTy();
    if (!IsFloat && !IsDouble)
      return false;
    unsigned Opc, CondMovOpc;
    switch (P) {
    case CmpInst::FCMP_OEQ:
      Opc = IsFloat ? Mips::C_EQ_S : Mips::C_EQ_D32;
      CondMovOpc = Mips::MOVT_I;
      break;
    case CmpInst::FCMP_UNE:
      Opc = IsFloat ? Mips::C_EQ_S : Mips::C_EQ_D32;
      CondMovOpc = Mips::MOVF_I;
      break;
    case CmpInst::FCMP_OLT:
      Opc = IsFloat ? Mips::C_OLT_S : Mips::C_OLT_D32;
      CondMovOpc = Mips::MOVT_I;
      break;
    case CmpInst::FCMP_OLE:
      Opc = IsFloat ? Mips::C_OLE_S : Mips::C_OLE_D32;
      CondMovOpc = Mips::MOVT_I;
      break;
    case CmpInst::FCMP_OGT:
      Opc = IsFloat ? Mips::C_ULE_S : Mips::C_ULE_D32;
      CondMovOpc = Mips::MOVF_I;
      break;
    case CmpInst::FCMP_OGE:
      Opc = IsFloat ? Mips::C_ULT_S : Mips::C_ULT_D32;
      CondMovOpc = Mips::MOVF_I;
      break;
    default:
      llvm_unreachable("Only switching of a subset of CCs.");
    }
    unsigned RegWithZero = createResultReg(&Mips::GPR32RegClass);
    unsigned RegWithOne = createResultReg(&Mips::GPR32RegClass);
    EmitInst(Mips::ADDiu, RegWithZero).addReg(Mips::ZERO).addImm(0);
    EmitInst(Mips::ADDiu, RegWithOne).addReg(Mips::ZERO).addImm(1);
    EmitInst(Opc).addReg(LeftReg).addReg(RightReg).addReg(
        Mips::FCC0, RegState::ImplicitDefine);
    MachineInstrBuilder MI = EmitInst(CondMovOpc, ResultReg)
                                 .addReg(RegWithOne)
                                 .addReg(Mips::FCC0)
                                 .addReg(RegWithZero, RegState::Implicit);
    MI->tieOperands(0, 3);
    break;
  }
  }
  updateValueMap(I, ResultReg);
  return true;
}

bool MipsFastISel::fastSelectInstruction(const Instruction *I) {
  if (!TargetSupported)
    return false;
  switch (I->getOpcode()) {
  default:
    break;
  case Instruction::Load:
    return SelectLoad(I);
  case Instruction::Store:
    return SelectStore(I);
  case Instruction::Ret:
    return SelectRet(I);
  case Instruction::Trunc:
    return SelectTrunc(I);
  case Instruction::ZExt:
  case Instruction::SExt:
    return SelectIntExt(I);
  case Instruction::FPTrunc:
    return SelectFPTrunc(I);
  case Instruction::FPExt:
    return SelectFPExt(I);
  case Instruction::FPToSI:
    return SelectFPToI(I, /*isSigned*/ true);
  case Instruction::FPToUI:
    return SelectFPToI(I, /*isSigned*/ false);
  case Instruction::ICmp:
  case Instruction::FCmp:
    return SelectCmp(I);
  }
  return false;
}

unsigned MipsFastISel::MaterializeFP(const ConstantFP *CFP, MVT VT) {
  if (UnsupportedFPMode)
    return 0;
  int64_t Imm = CFP->getValueAPF().bitcastToAPInt().getZExtValue();
  if (VT == MVT::f32) {
    const TargetRegisterClass *RC = &Mips::FGR32RegClass;
    unsigned DestReg = createResultReg(RC);
    unsigned TempReg = Materialize32BitInt(Imm, &Mips::GPR32RegClass);
    EmitInst(Mips::MTC1, DestReg).addReg(TempReg);
    return DestReg;
  } else if (VT == MVT::f64) {
    const TargetRegisterClass *RC = &Mips::AFGR64RegClass;
    unsigned DestReg = createResultReg(RC);
    unsigned TempReg1 = Materialize32BitInt(Imm >> 32, &Mips::GPR32RegClass);
    unsigned TempReg2 =
        Materialize32BitInt(Imm & 0xFFFFFFFF, &Mips::GPR32RegClass);
    EmitInst(Mips::BuildPairF64, DestReg).addReg(TempReg2).addReg(TempReg1);
    return DestReg;
  }
  return 0;
}

unsigned MipsFastISel::MaterializeGV(const GlobalValue *GV, MVT VT) {
  // For now 32-bit only.
  if (VT != MVT::i32)
    return 0;
  const TargetRegisterClass *RC = &Mips::GPR32RegClass;
  unsigned DestReg = createResultReg(RC);
  const GlobalVariable *GVar = dyn_cast<GlobalVariable>(GV);
  bool IsThreadLocal = GVar && GVar->isThreadLocal();
  // TLS not supported at this time.
  if (IsThreadLocal)
    return 0;
  EmitInst(Mips::LW, DestReg)
      .addReg(MFI->getGlobalBaseReg())
      .addGlobalAddress(GV, 0, MipsII::MO_GOT);
  if ((GV->hasInternalLinkage() ||
       (GV->hasLocalLinkage() && !isa<Function>(GV)))) {
    unsigned TempReg = createResultReg(RC);
    EmitInst(Mips::ADDiu, TempReg)
        .addReg(DestReg)
        .addGlobalAddress(GV, 0, MipsII::MO_ABS_LO);
    DestReg = TempReg;
  }
  return DestReg;
}

unsigned MipsFastISel::MaterializeInt(const Constant *C, MVT VT) {
  if (VT != MVT::i32 && VT != MVT::i16 && VT != MVT::i8 && VT != MVT::i1)
    return 0;
  const TargetRegisterClass *RC = &Mips::GPR32RegClass;
  const ConstantInt *CI = cast<ConstantInt>(C);
  int64_t Imm;
  if ((VT != MVT::i1) && CI->isNegative())
    Imm = CI->getSExtValue();
  else
    Imm = CI->getZExtValue();
  return Materialize32BitInt(Imm, RC);
}

unsigned MipsFastISel::Materialize32BitInt(int64_t Imm,
                                           const TargetRegisterClass *RC) {
  unsigned ResultReg = createResultReg(RC);

  if (isInt<16>(Imm)) {
    unsigned Opc = Mips::ADDiu;
    EmitInst(Opc, ResultReg).addReg(Mips::ZERO).addImm(Imm);
    return ResultReg;
  } else if (isUInt<16>(Imm)) {
    EmitInst(Mips::ORi, ResultReg).addReg(Mips::ZERO).addImm(Imm);
    return ResultReg;
  }
  unsigned Lo = Imm & 0xFFFF;
  unsigned Hi = (Imm >> 16) & 0xFFFF;
  if (Lo) {
    // Both Lo and Hi have nonzero bits.
    unsigned TmpReg = createResultReg(RC);
    EmitInst(Mips::LUi, TmpReg).addImm(Hi);
    EmitInst(Mips::ORi, ResultReg).addReg(TmpReg).addImm(Lo);
  } else {
    EmitInst(Mips::LUi, ResultReg).addImm(Hi);
  }
  return ResultReg;
}
}

namespace llvm {
FastISel *Mips::createFastISel(FunctionLoweringInfo &funcInfo,
                               const TargetLibraryInfo *libInfo) {
  return new MipsFastISel(funcInfo, libInfo);
}
}
