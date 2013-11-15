//===- lib/ReaderWriter/PECOFF/GroupedSectionsPass.h-----------------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file \brief This pass adds layout-{before,after} references to atoms in
/// grouped sections, so that they will appear in the correct order in the
/// output.
///
/// In COFF, sections will be merged into one section by the linker if their
/// names are the same after discarding the "$" character and all characters
/// follow it from their names. The characters following the "$" character
/// determines the merge order. Assume there's an object file containing four
/// data sections in the following order.
///
///   - .data$2
///   - .data$3
///   - .data$1
///   - .data
///
/// In this case, the resulting binary should have ".data" section with the
/// contents of ".data", ".data$1", ".data$2" and ".data$3" in that order.
///
/// In lld, this contraint is modeled by the atom model using
/// layout-{before,after} references. Atoms in the same (unmerged-)section have
/// already been connected with layout-{before,after} edges each other when the
/// control reaches to this pass. We pick the head atom from each section that
/// needs to merged, and connects them with layout-{before,after} edges in the
/// right order, so that they'll be sorted correctly in the layout pass.
///
//===----------------------------------------------------------------------===//

#ifndef LLD_READER_WRITER_PE_COFF_GROUPED_SECTIONS_PASS_H
#define LLD_READER_WRITER_PE_COFF_GROUPED_SECTIONS_PASS_H

#include "Atoms.h"
#include "lld/Core/Pass.h"
#include "llvm/Support/Debug.h"

#include <algorithm>
#include <map>

using lld::coff::COFFBaseDefinedAtom;
using lld::coff::COFFDefinedAtom;

namespace lld {
namespace pecoff {

namespace {
bool compareByFileOrdinal(const DefinedAtom *a, const DefinedAtom *b) {
  return a->file().ordinal() > b->file().ordinal();
}
}

class GroupedSectionsPass : public lld::Pass {
public:
  GroupedSectionsPass() {}

  virtual void perform(std::unique_ptr<MutableFile> &mergedFile) {
    std::map<StringRef, std::vector<COFFDefinedAtom *> > sectionToHeadAtoms(
        filterHeadAtoms(*mergedFile));
    std::vector<std::vector<COFFDefinedAtom *>> groupedAtomsList(
        groupBySectionName(sectionToHeadAtoms));
    for (auto &groupedAtoms : groupedAtomsList)
      connectAtoms(groupedAtoms);
  }

private:
  typedef std::map<StringRef, std::vector<COFFDefinedAtom *>> SectionToAtomsT;

  /// Returns the list of atoms that appeared at the beginning of sections.
  SectionToAtomsT filterHeadAtoms(MutableFile &mutableFile) const {
    SectionToAtomsT result;
    for (const DefinedAtom *atom : mutableFile.defined()) {
      auto *coffAtom = dyn_cast<COFFDefinedAtom>((COFFBaseDefinedAtom *)atom);
      if (coffAtom && coffAtom->ordinal() == 0)
        result[coffAtom->getSectionName()].push_back(coffAtom);
    }
    return result;
  }

  /// Group atoms that needs to be merged. Returned atoms are sorted by section
  /// name and file ordinal.
  std::vector<std::vector<COFFDefinedAtom *>>
  groupBySectionName(SectionToAtomsT sectionToHeadAtoms) const {
    SectionToAtomsT res;
    // Note that the atoms are already sorted by section name because std::map
    // is a sorted map.
    for (auto &i : sectionToHeadAtoms) {
      StringRef sectionName = i.first;
      std::vector<COFFDefinedAtom*> &atoms = i.second;
      // Sections with the same name could exist if they are from different
      // files. If that's the case, the sections needs to be sorted in the same
      // order as they appeared in the command line.
      std::stable_sort(atoms.begin(), atoms.end(), compareByFileOrdinal);
      for (COFFDefinedAtom *atom : atoms) {
        StringRef baseName = sectionName.split('$').first;
        res[baseName].push_back(atom);
      }
    }
    std::vector<std::vector<COFFDefinedAtom *>> vec;
    for (auto &i : res)
      vec.push_back(std::move(i.second));
    return vec;
  }

  /// For each pair of atoms in the given vector, add a layout edge from the
  /// follow-on tail of the first atom to the second atom. As a result, the
  /// atoms in the vectors will be output as the same order as in the vector.
  void connectAtoms(std::vector<COFFDefinedAtom *> heads) {
    if (heads.empty())
      return;
    COFFDefinedAtom *tail = getTail(heads[0]);
    for (auto i = heads.begin() + 1, e = heads.end(); i != e; ++i) {
      COFFDefinedAtom *head = *i;
      connectWithLayoutEdge(tail, head);
      tail = getTail(head);
    }
  }

  /// Follows the follow-on chain and returns the last atom.
  COFFDefinedAtom *getTail(COFFDefinedAtom *atom) {
    for (;;) {
      COFFDefinedAtom *next = nullptr;
      for (const Reference *r : *atom) {
	if (r->kind() != lld::Reference::kindLayoutAfter)
	  continue;
	next = (COFFDefinedAtom *)(r->target());
	break;
      }
      if (!next)
	return atom;
      atom = next;
    }
  }
};

} // namespace pecoff
} // namespace lld

#endif
