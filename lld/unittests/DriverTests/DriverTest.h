//===- lld/unittest/DriverTest.h ------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stdarg.h>

#include "gtest/gtest.h"

#include "lld/Driver/Driver.h"
#include "lld/Driver/LinkerInput.h"

#include "llvm/Support/raw_ostream.h"

namespace {

using namespace llvm;
using namespace lld;

template<typename D, typename T>
class ParserTest : public testing::Test {
protected:

  virtual const LinkingContext *linkingContext() = 0;

  std::string &errorMessage() { return  _errorMessage; }

  // Convenience method for getting number of input files.
  int inputFileCount() { return linkingContext()->inputGraph().numFiles(); }

  // Convenience method for getting i'th input files name.
  std::string inputFile(unsigned index) {
    const InputElement &inputElement = linkingContext()->inputGraph()[index];
    if (inputElement.kind() == InputElement::Kind::File)
      return *(llvm::dyn_cast<FileNode>(&inputElement))
                  ->path(*linkingContext());
    llvm_unreachable("not handling other types of input files");
  }

  // For unit tests to call driver with various command lines.
  bool parse(const char *args, ...) {
    // Construct command line options from varargs.
    std::vector<const char *> vec;
    vec.push_back(args);
    va_list ap;
    va_start(ap, args);
    while (const char *arg = va_arg(ap, const char *))
      vec.push_back(arg);
    va_end(ap);

    // Call the parser.
    raw_string_ostream os(_errorMessage);
    return D::parse(vec.size(), &vec[0], _context, os);
  }

  T _context;
  std::string _errorMessage;
};

}
