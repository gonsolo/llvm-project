//===- ReaderWriter/ReaderArchive.h - Archive Library Reader ------------===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//

#ifndef LLD_READER_ARCHIVE_H
#define LLD_READER_ARCHIVE_H

#include "lld/Core/ArchiveLibraryFile.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/system_error.h"
#include "llvm/Object/Archive.h"
#include "lld/Core/File.h"
#include "lld/Core/LLVM.h"
#include "lld/ReaderWriter/Reader.h"
#include "lld/ReaderWriter/ReaderArchive.h"
#include <memory>
#include <bits/unique_ptr.h>
#include <vector>

namespace lld
{
///
/// The ReaderOptionsArchive encapsulates the options used by the ReaderArchive.
/// The option objects are the only way to control the behaviour of Readers.
///
class ReaderOptionsArchive
{
public:
  ReaderOptionsArchive(bool is_force_load=false): _isForceLoad(is_force_load),
                                                  _reader(nullptr)
  { }
  
  bool isForceLoad() const {
    return _isForceLoad;
  }
  
  Reader *reader() const {
    return _reader;
  }

  void setReader(Reader *r) {
    _reader = r;
  }
  
private:
  bool _isForceLoad;
  Reader *_reader;
};

// ReaderArchive is a class for reading archive libraries
class ReaderArchive final
{
public:
  ReaderArchive(ReaderOptionsArchive &options) : _options(options),
                                                 _archive()
  { }

  // Returns a vector of Files that are contained in the archive file 
  // pointed to by the Memorybuffer
  virtual error_code parseFile(std::unique_ptr<llvm::MemoryBuffer> mb,
                               std::vector<std::unique_ptr<File>> &result);

  virtual ~ReaderArchive() { }

private:
  ReaderOptionsArchive &_options;
  std::unique_ptr<llvm::object::Archive> _archive;
};

} // namespace lld

#endif // LLD_READER_ARCHIVE_H
