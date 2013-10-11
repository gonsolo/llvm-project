//===-- RuntimeDyldELF.h - Run-time dynamic linker for MC-JIT ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// ELF support for MC-JIT runtime dynamic linker.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_RUNTIME_DYLD_ELF_H
#define LLVM_RUNTIME_DYLD_ELF_H

#include "RuntimeDyldImpl.h"
#include "llvm/ADT/DenseMap.h"

using namespace llvm;

namespace llvm {

namespace {
  // Helper for extensive error checking in debug builds.
  error_code Check(error_code Err) {
    if (Err) {
      report_fatal_error(Err.message());
    }
    return Err;
  }
} // end anonymous namespace

class RuntimeDyldELF : public RuntimeDyldImpl {
  void resolveRelocation(const SectionEntry &Section,
                         uint64_t Offset,
                         uint64_t Value,
                         uint32_t Type,
                         int64_t Addend,
                         uint64_t SymOffset=0);

  void resolveX86_64Relocation(const SectionEntry &Section,
                               uint64_t Offset,
                               uint64_t Value,
                               uint32_t Type,
                               int64_t  Addend,
                               uint64_t SymOffset);

  void resolveX86Relocation(const SectionEntry &Section,
                            uint64_t Offset,
                            uint32_t Value,
                            uint32_t Type,
                            int32_t Addend);

  void resolveAArch64Relocation(const SectionEntry &Section,
                                uint64_t Offset,
                                uint64_t Value,
                                uint32_t Type,
                                int64_t Addend);

  void resolveARMRelocation(const SectionEntry &Section,
                            uint64_t Offset,
                            uint32_t Value,
                            uint32_t Type,
                            int32_t Addend);

  void resolveMIPSRelocation(const SectionEntry &Section,
                             uint64_t Offset,
                             uint32_t Value,
                             uint32_t Type,
                             int32_t Addend);

  void resolvePPC64Relocation(const SectionEntry &Section,
                              uint64_t Offset,
                              uint64_t Value,
                              uint32_t Type,
                              int64_t Addend);

  void resolveSystemZRelocation(const SectionEntry &Section,
                                uint64_t Offset,
                                uint64_t Value,
                                uint32_t Type,
                                int64_t Addend);

  uint64_t findPPC64TOC() const;
  void findOPDEntrySection(ObjectImage &Obj,
                           ObjSectionToIDMap &LocalSections,
                           RelocationValueRef &Rel);

  uint64_t findGOTEntry(uint64_t LoadAddr, uint64_t Offset);
  size_t getGOTEntrySize();

  virtual void updateGOTEntries(StringRef Name, uint64_t Addr);

  // Relocation entries for symbols whose position-independant offset is
  // updated in a global offset table.
  typedef SmallVector<RelocationValueRef, 2> GOTRelocations;
  GOTRelocations GOTEntries; // List of entries requiring finalization.
  SmallVector<std::pair<SID, GOTRelocations>, 8> GOTs; // Allocated tables.

  // When a module is loaded we save the SectionID of the EH frame section
  // in a table until we receive a request to register all unregistered
  // EH frame sections with the memory manager.
  SmallVector<SID, 2> UnregisteredEHFrameSections;

public:
  RuntimeDyldELF(RTDyldMemoryManager *mm) : RuntimeDyldImpl(mm)
                                          {}

  virtual void resolveRelocation(const RelocationEntry &RE, uint64_t Value);
  virtual void processRelocationRef(unsigned SectionID,
                                    RelocationRef RelI,
                                    ObjectImage &Obj,
                                    ObjSectionToIDMap &ObjSectionToID,
                                    const SymbolTableMap &Symbols,
                                    StubMap &Stubs);
  virtual bool isCompatibleFormat(const ObjectBuffer *Buffer) const;
  virtual ObjectImage *createObjectImage(ObjectBuffer *InputBuffer);
  virtual void registerEHFrames();
  virtual void finalizeLoad(ObjSectionToIDMap &SectionMap);
  virtual ~RuntimeDyldELF();
};

} // end namespace llvm

#endif
