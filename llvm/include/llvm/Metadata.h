//===-- llvm/Metadata.h - Metadata definitions ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
/// @file
/// This file contains the declarations for metadata subclasses.
/// They represent the different flavors of metadata that live in LLVM.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_METADATA_H
#define LLVM_METADATA_H

#include "llvm/Value.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/ilist_node.h"

namespace llvm {
class Constant;
class Instruction;
class LLVMContext;
class Module;
class MetadataContextImpl;
template <typename T> class SmallVectorImpl;

//===----------------------------------------------------------------------===//
// MetadataBase  - A base class for MDNode, MDString and NamedMDNode.
class MetadataBase : public Value {
protected:
  MetadataBase(const Type *Ty, unsigned scid)
    : Value(Ty, scid) {}

public:

  /// Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const MetadataBase *) { return true; }
  static bool classof(const Value *V) {
    return V->getValueID() == MDStringVal || V->getValueID() == MDNodeVal
      || V->getValueID() == NamedMDNodeVal;
  }
};

//===----------------------------------------------------------------------===//
/// MDString - a single uniqued string.
/// These are used to efficiently contain a byte sequence for metadata.
/// MDString is always unnamd.
class MDString : public MetadataBase {
  MDString(const MDString &);            // DO NOT IMPLEMENT

  StringRef Str;
protected:
  explicit MDString(LLVMContext &C, StringRef S);

public:
  static MDString *get(LLVMContext &Context, StringRef Str);
  static MDString *get(LLVMContext &Context, const char *Str);
  
  StringRef getString() const { return Str; }

  unsigned getLength() const { return (unsigned)Str.size(); }

  typedef StringRef::iterator iterator;
  
  /// begin() - Pointer to the first byte of the string.
  ///
  iterator begin() const { return Str.begin(); }

  /// end() - Pointer to one byte past the end of the string.
  ///
  iterator end() const { return Str.end(); }

  /// Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const MDString *) { return true; }
  static bool classof(const Value *V) {
    return V->getValueID() == MDStringVal;
  }
};

  
class MDNodeElement;
  
//===----------------------------------------------------------------------===//
/// MDNode - a tuple of other values.
/// These contain a list of the values that represent the metadata. 
/// MDNode is always unnamed.
class MDNode : public MetadataBase, public FoldingSetNode {
  MDNode(const MDNode &);                // DO NOT IMPLEMENT
  void operator=(const MDNode &);        // DO NOT IMPLEMENT
  friend class MDNodeElement;

  MDNodeElement *Operands;
  unsigned NumOperands;
  
  // Subclass data enums.
  enum {
    FunctionLocalBit = 1
  };
  
  // Replace each instance of F from the element list of this node with T.
  void replaceElement(MDNodeElement *Op, Value *NewVal);

protected:
  explicit MDNode(LLVMContext &C, Value *const *Vals, unsigned NumVals,
                  bool isFunctionLocal);
public:
  // Constructors and destructors.
  static MDNode *get(LLVMContext &Context, Value *const *Vals, unsigned NumVals,
                     bool isFunctionLocal = false);

  /// ~MDNode - Destroy MDNode.
  ~MDNode();
  
  /// getElement - Return specified element.
  Value *getElement(unsigned i) const;
  
  /// getNumElements - Return number of MDNode elements.
  unsigned getNumElements() const { return NumOperands; }
  
  /// isFunctionLocal - Return whether MDNode is local to a function.
  /// Note: MDNodes are designated as function-local when created, and keep
  ///       that designation even if their operands are modified to no longer
  ///       refer to function-local IR.
  bool isFunctionLocal() const { return SubclassData & FunctionLocalBit; }

  /// Profile - calculate a unique identifier for this MDNode to collapse
  /// duplicates
  void Profile(FoldingSetNodeID &ID) const;

  /// Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const MDNode *) { return true; }
  static bool classof(const Value *V) {
    return V->getValueID() == MDNodeVal;
  }
};

//===----------------------------------------------------------------------===//
/// NamedMDNode - a tuple of other metadata. 
/// NamedMDNode is always named. All NamedMDNode element has a type of metadata.
template<typename ValueSubClass, typename ItemParentClass>
  class SymbolTableListTraits;

class NamedMDNode : public MetadataBase, public ilist_node<NamedMDNode> {
  friend class SymbolTableListTraits<NamedMDNode, Module>;
  friend class LLVMContextImpl;

  NamedMDNode(const NamedMDNode &);      // DO NOT IMPLEMENT

  Module *Parent;
  void *Operands; // SmallVector<TrackingVH<MetadataBase>, 4>

  void setParent(Module *M) { Parent = M; }
protected:
  explicit NamedMDNode(LLVMContext &C, const Twine &N, MetadataBase*const *Vals, 
                       unsigned NumVals, Module *M = 0);
public:
  static NamedMDNode *Create(LLVMContext &C, const Twine &N, 
                             MetadataBase *const *MDs, 
                             unsigned NumMDs, Module *M = 0) {
    return new NamedMDNode(C, N, MDs, NumMDs, M);
  }

  static NamedMDNode *Create(const NamedMDNode *NMD, Module *M = 0);

  /// eraseFromParent - Drop all references and remove the node from parent
  /// module.
  void eraseFromParent();

  /// dropAllReferences - Remove all uses and clear node vector.
  void dropAllReferences();

  /// ~NamedMDNode - Destroy NamedMDNode.
  ~NamedMDNode();

  /// getParent - Get the module that holds this named metadata collection.
  inline Module *getParent() { return Parent; }
  inline const Module *getParent() const { return Parent; }

  /// getElement - Return specified element.
  MetadataBase *getElement(unsigned i) const;
  
  /// getNumElements - Return number of NamedMDNode elements.
  unsigned getNumElements() const;

  /// addElement - Add metadata element.
  void addElement(MetadataBase *M);
  
  /// Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const NamedMDNode *) { return true; }
  static bool classof(const Value *V) {
    return V->getValueID() == NamedMDNodeVal;
  }
};

//===----------------------------------------------------------------------===//
/// MetadataContext - MetadataContext handles uniquing and assignment of IDs for
/// custom metadata types.
///
class MetadataContext {
  MetadataContext(MetadataContext&); // DO NOT IMPLEMENT
  void operator=(MetadataContext&);  // DO NOT IMPLEMENT

  MetadataContextImpl *const pImpl;
  friend class Instruction;
public:
  MetadataContext();
  ~MetadataContext();

  /// getMDKindID - Return a unique non-zero ID for the specified metadata kind.
  unsigned getMDKindID(StringRef Name) const;

  /// isValidName - Return true if Name is a valid custom metadata handler name.
  static bool isValidName(StringRef Name);

  /// copyMD - If metadata is attached with Instruction In1 then attach
  /// the same metadata to In2.
  void copyMD(Instruction *In1, Instruction *In2);

  /// getMDKindNames - Populate client supplied SmallVector with the name for
  /// each custom metadata ID.   ID #0 is not used, so it is filled in as empty.
  void getMDKindNames(SmallVectorImpl<StringRef> &) const;

  /// ValueIsDeleted - This handler is used to update metadata store
  /// when a value is deleted.
  void ValueIsDeleted(const Value *) {}
  void ValueIsDeleted(Instruction *Inst);
  void ValueIsRAUWd(Value *V1, Value *V2);

  /// ValueIsCloned - This handler is used to update metadata store
  /// when In1 is cloned to create In2.
  void ValueIsCloned(const Instruction *In1, Instruction *In2);
};

} // end llvm namespace

#endif
