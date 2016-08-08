//===-- esan_hashtable.h ----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of EfficiencySanitizer, a family of performance tuners.
//
// Generic resizing hashtable.
//===----------------------------------------------------------------------===//

#include "sanitizer_common/sanitizer_allocator_internal.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_mutex.h"
#include <stddef.h>

namespace __esan {

//===----------------------------------------------------------------------===//
// Default hash and comparison functions
//===----------------------------------------------------------------------===//

template <typename T> struct DefaultHash {
  size_t operator()(const T &Key) const {
    return (size_t)Key;
  }
};

template <typename T> struct DefaultEqual {
  bool operator()(const T &Key1, const T &Key2) const {
    return Key1 == Key2;
  }
};

//===----------------------------------------------------------------------===//
// HashTable declaration
//===----------------------------------------------------------------------===//

// A simple resizing and mutex-locked hashtable.
//
// If the default hash functor is used, KeyTy must have an operator size_t().
// If the default comparison functor is used, KeyTy must have an operator ==.
//
// By default all operations are internally-synchronized with a mutex, with no
// synchronization for payloads once hashtable functions return.  If
// ExternalLock is set to true, the caller should call the lock() and unlock()
// routines around all hashtable operations and subsequent manipulation of
// payloads.
template <typename KeyTy, typename DataTy, bool ExternalLock = false,
          typename HashFuncTy = DefaultHash<KeyTy>,
          typename EqualFuncTy = DefaultEqual<KeyTy> >
class HashTable {
public:
  // InitialCapacity must be a power of 2.
  // ResizeFactor must be between 1 and 99 and indicates the
  // maximum percentage full that the table should ever be.
  HashTable(u32 InitialCapacity = 2048, u32 ResizeFactor = 70);
  ~HashTable();
  bool lookup(const KeyTy &Key, DataTy &Payload); // Const except for Mutex.
  bool add(const KeyTy &Key, const DataTy &Payload);
  bool remove(const KeyTy &Key);
  u32 size(); // Const except for Mutex.
  // If the table is internally-synchronized, this lock must not be held
  // while a hashtable function is called as it will deadlock: the lock
  // is not recursive.  This is meant for use with externally-synchronized
  // tables.
  void lock();
  void unlock();

private:
  void resize();

  struct HashEntry {
    KeyTy Key;
    DataTy Payload;
    HashEntry *Next;
  };

  HashEntry **Table;
  u32 Capacity;
  u32 Entries;
  const u32 ResizeFactor;
  BlockingMutex Mutex;
  const HashFuncTy HashFunc;
  const EqualFuncTy EqualFunc;
};

//===----------------------------------------------------------------------===//
// Hashtable implementation
//===----------------------------------------------------------------------===//

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::HashTable(
    u32 InitialCapacity, u32 ResizeFactor)
    : Capacity(InitialCapacity), Entries(0), ResizeFactor(ResizeFactor),
      HashFunc(HashFuncTy()), EqualFunc(EqualFuncTy()) {
  CHECK(IsPowerOfTwo(Capacity));
  CHECK(ResizeFactor >= 1 && ResizeFactor <= 99);
  Table = (HashEntry **)InternalAlloc(Capacity * sizeof(HashEntry *));
  internal_memset(Table, 0, Capacity * sizeof(HashEntry *));
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::~HashTable() {
  for (u32 i = 0; i < Capacity; ++i) {
    HashEntry *Entry = Table[i];
    while (Entry != nullptr) {
      HashEntry *Next = Entry->Next;
      Entry->Payload.~DataTy();
      InternalFree(Entry);
      Entry = Next;
    }
  }
  InternalFree(Table);
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
u32 HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::size() {
  u32 Res;
  if (!ExternalLock)
    Mutex.Lock();
  Res = Entries;
  if (!ExternalLock)
    Mutex.Unlock();
  return Res;
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
bool HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::lookup(
    const KeyTy &Key, DataTy &Payload) {
  if (!ExternalLock)
    Mutex.Lock();
  bool Found = false;
  size_t Hash = HashFunc(Key) % Capacity;
  HashEntry *Entry = Table[Hash];
  for (; Entry != nullptr; Entry = Entry->Next) {
    if (EqualFunc(Entry->Key, Key)) {
      Payload = Entry->Payload;
      Found = true;
      break;
    }
  }
  if (!ExternalLock)
    Mutex.Unlock();
  return Found;
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
void HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::resize() {
  if (!ExternalLock)
    Mutex.CheckLocked();
  size_t OldCapacity = Capacity;
  HashEntry **OldTable = Table;
  Capacity *= 2;
  Table = (HashEntry **)InternalAlloc(Capacity * sizeof(HashEntry *));
  internal_memset(Table, 0, Capacity * sizeof(HashEntry *));
  // Re-hash
  for (u32 i = 0; i < OldCapacity; ++i) {
    HashEntry *OldEntry = OldTable[i];
    while (OldEntry != nullptr) {
      HashEntry *Next = OldEntry->Next;
      size_t Hash = HashFunc(OldEntry->Key) % Capacity;
      OldEntry->Next = Table[Hash];
      Table[Hash] = OldEntry;
      OldEntry = Next;
    }
  }
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
bool HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::add(
    const KeyTy &Key, const DataTy &Payload) {
  if (!ExternalLock)
    Mutex.Lock();
  bool Exists = false;
  size_t Hash = HashFunc(Key) % Capacity;
  HashEntry *Entry = Table[Hash];
  for (; Entry != nullptr; Entry = Entry->Next) {
    if (EqualFunc(Entry->Key, Key)) {
      Exists = true;
      break;
    }
  }
  if (!Exists) {
    Entries++;
    if (Entries * 100 >= Capacity * ResizeFactor) {
      resize();
      Hash = HashFunc(Key) % Capacity;
    }
    HashEntry *Add = (HashEntry *)InternalAlloc(sizeof(*Add));
    Add->Key = Key;
    Add->Payload = Payload;
    Add->Next = Table[Hash];
    Table[Hash] = Add;
  }
  if (!ExternalLock)
    Mutex.Unlock();
  return !Exists;
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
bool HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::remove(
    const KeyTy &Key) {
  if (!ExternalLock)
    Mutex.Lock();
  bool Found = false;
  size_t Hash = HashFunc(Key) % Capacity;
  HashEntry *Entry = Table[Hash];
  HashEntry *Prev = nullptr;
  for (; Entry != nullptr; Prev = Entry, Entry = Entry->Next) {
    if (EqualFunc(Entry->Key, Key)) {
      Found = true;
      Entries--;
      if (Prev == nullptr)
        Table[Hash] = Entry->Next;
      else
        Prev->Next = Entry->Next;
      Entry->Payload.~DataTy();
      InternalFree(Entry);
      break;
    }
  }
  if (!ExternalLock)
    Mutex.Unlock();
  return Found;
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
void HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::lock() {
  Mutex.Lock();
}

template <typename KeyTy, typename DataTy, bool ExternalLock,
          typename HashFuncTy, typename EqualFuncTy>
void HashTable<KeyTy, DataTy, ExternalLock, HashFuncTy, EqualFuncTy>::unlock() {
  Mutex.Unlock();
}

} // namespace __esan
