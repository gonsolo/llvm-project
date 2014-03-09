//==- X86ATTInstPrinter.h - Convert X86 MCInst to assembly syntax -*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an X86 MCInst to AT&T style .s file syntax.
//
//===----------------------------------------------------------------------===//

#ifndef X86_ATT_INST_PRINTER_H
#define X86_ATT_INST_PRINTER_H

#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

class MCOperand;
  
class X86ATTInstPrinter : public MCInstPrinter {
public:
  X86ATTInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                    const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI) {}

  void printRegName(raw_ostream &OS, unsigned RegNo) const override;
  void printInst(const MCInst *MI, raw_ostream &OS, StringRef Annot) override;

  // Autogenerated by tblgen, returns true if we successfully printed an
  // alias.
  bool printAliasInstr(const MCInst *MI, raw_ostream &OS);

  // Autogenerated by tblgen.
  void printInstruction(const MCInst *MI, raw_ostream &OS);
  static const char *getRegisterName(unsigned RegNo);

  void printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &OS);
  void printMemReference(const MCInst *MI, unsigned Op, raw_ostream &OS);
  void printSSECC(const MCInst *MI, unsigned Op, raw_ostream &OS);
  void printAVXCC(const MCInst *MI, unsigned Op, raw_ostream &OS);
  void printPCRelImm(const MCInst *MI, unsigned OpNo, raw_ostream &OS);
  void printSrcIdx(const MCInst *MI, unsigned OpNo, raw_ostream &OS);
  void printDstIdx(const MCInst *MI, unsigned OpNo, raw_ostream &OS);
  void printMemOffset(const MCInst *MI, unsigned OpNo, raw_ostream &OS);
  void printRoundingControl(const MCInst *MI, unsigned Op, raw_ostream &OS);

  void printopaquemem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  
  void printi8mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printi16mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printi32mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printi64mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printi128mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printi256mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printi512mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printf32mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printf64mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printf80mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printf128mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printf256mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }
  void printf512mem(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemReference(MI, OpNo, O);
  }

  void printSrcIdx8(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printSrcIdx(MI, OpNo, O);
  }
  void printSrcIdx16(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printSrcIdx(MI, OpNo, O);
  }
  void printSrcIdx32(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printSrcIdx(MI, OpNo, O);
  }
  void printSrcIdx64(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printSrcIdx(MI, OpNo, O);
  }
  void printDstIdx8(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printDstIdx(MI, OpNo, O);
  }
  void printDstIdx16(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printDstIdx(MI, OpNo, O);
  }
  void printDstIdx32(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printDstIdx(MI, OpNo, O);
  }
  void printDstIdx64(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printDstIdx(MI, OpNo, O);
  }
  void printMemOffs8(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemOffset(MI, OpNo, O);
  }
  void printMemOffs16(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemOffset(MI, OpNo, O);
  }
  void printMemOffs32(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemOffset(MI, OpNo, O);
  }
  void printMemOffs64(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
    printMemOffset(MI, OpNo, O);
  }
};
  
}

#endif
