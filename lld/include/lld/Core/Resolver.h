//===- Core/Resolver.h - Resolves Atom References -------------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_CORE_RESOLVER_H
#define LLD_CORE_RESOLVER_H

#include "lld/Core/File.h"
#include "lld/Core/SharedLibraryFile.h"
#include "lld/Core/SymbolTable.h"

#include "llvm/ADT/DenseSet.h"

#include <set>
#include <vector>

namespace lld {

class Atom;
class LinkingContext;

/// \brief The Resolver is responsible for merging all input object files
/// and producing a merged graph.
class Resolver {
public:
  enum ResolverState {
    StateNoChange = 0,              // The default resolver state
    StateNewDefinedAtoms = 1,       // New defined atoms were added
    StateNewUndefinedAtoms = 2,     // New undefined atoms were added
    StateNewSharedLibraryAtoms = 4, // New shared library atoms were added
    StateNewAbsoluteAtoms = 8       // New absolute atoms were added
  };

  Resolver(LinkingContext &context)
      : _context(context), _symbolTable(context), _result(new MergedFile()),
        _haveLLVMObjs(false), _addToFinalSection(false) {}

  virtual ~Resolver() {}

  // InputFiles::Handler methods
  virtual void doDefinedAtom(const DefinedAtom&);
  virtual void doUndefinedAtom(const UndefinedAtom&);
  virtual void doSharedLibraryAtom(const SharedLibraryAtom &);
  virtual void doAbsoluteAtom(const AbsoluteAtom &);

  // Handle files, this adds atoms from the current file thats
  // being processed by the resolver
  virtual void handleFile(const File &);

  // Handle an archive library file.
  virtual void handleArchiveFile(const File &);

  // Handle a shared library file.
  virtual void handleSharedLibrary(const File &);

  /// @brief do work of merging and resolving and return list
  bool resolve();

  std::unique_ptr<MutableFile> resultFile() { return std::move(_result); }

private:
  typedef std::function<void(StringRef, bool)> UndefCallback;

  /// \brief Add section group/.gnu.linkonce if it does not exist previously.
  bool maybeAddSectionGroupOrGnuLinkOnce(const DefinedAtom &atom);

  /// \brief The main function that iterates over the files to resolve
  bool resolveUndefines();
  void updateReferences();
  void deadStripOptimize();
  bool checkUndefines(bool isFinal);
  void removeCoalescedAwayAtoms();
  void checkDylibSymbolCollisions();
  void linkTimeOptimize();
  void tweakAtoms();
  void forEachUndefines(UndefCallback callback, bool searchForOverrides);

  void markLive(const Atom &atom);
  void addAtoms(const std::vector<const DefinedAtom *>&);

  class MergedFile : public MutableFile {
  public:
    MergedFile() : MutableFile("<linker-internal>") {}

    const atom_collection<DefinedAtom> &defined() const override {
      return _definedAtoms;
    }
    const atom_collection<UndefinedAtom>& undefined() const override {
      return _undefinedAtoms;
    }
    const atom_collection<SharedLibraryAtom>& sharedLibrary() const override {
      return _sharedLibraryAtoms;
    }
    const atom_collection<AbsoluteAtom>& absolute() const override {
      return _absoluteAtoms;
    }

    void addAtoms(std::vector<const Atom*>& atoms);

    void addAtom(const Atom& atom) override;
    DefinedAtomRange definedAtoms() override;

  private:
    atom_collection_vector<DefinedAtom>         _definedAtoms;
    atom_collection_vector<UndefinedAtom>       _undefinedAtoms;
    atom_collection_vector<SharedLibraryAtom>   _sharedLibraryAtoms;
    atom_collection_vector<AbsoluteAtom>        _absoluteAtoms;
  };

  LinkingContext &_context;
  SymbolTable _symbolTable;
  std::vector<const Atom *>     _atoms;
  std::set<const Atom *>        _deadStripRoots;
  std::vector<const Atom *>     _atomsWithUnresolvedReferences;
  llvm::DenseSet<const Atom *>  _liveAtoms;
  std::unique_ptr<MergedFile> _result;
  bool                          _haveLLVMObjs;
  bool _addToFinalSection;
};

} // namespace lld

#endif // LLD_CORE_RESOLVER_H
