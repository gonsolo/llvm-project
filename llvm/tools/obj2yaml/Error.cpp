//===- Error.cpp - system_error extensions for obj2yaml ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Error.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class _obj2yaml_error_category : public error_category {
public:
  const char *name() const override;
  std::string message(int ev) const override;
  error_condition default_error_condition(int ev) const override;
};
} // namespace

const char *_obj2yaml_error_category::name() const { return "obj2yaml"; }

std::string _obj2yaml_error_category::message(int ev) const {
  switch (ev) {
  case obj2yaml_error::success:
    return "Success";
  case obj2yaml_error::file_not_found:
    return "No such file.";
  case obj2yaml_error::unrecognized_file_format:
    return "Unrecognized file type.";
  case obj2yaml_error::unsupported_obj_file_format:
    return "Unsupported object file format.";
  default:
    llvm_unreachable("An enumerator of obj2yaml_error does not have a message "
                     "defined.");
  }
}

error_condition
_obj2yaml_error_category::default_error_condition(int ev) const {
  if (ev == obj2yaml_error::success)
    return error_condition();
  return errc::invalid_argument;
}

namespace llvm {
const error_category &obj2yaml_category() {
  static _obj2yaml_error_category o;
  return o;
}
} // namespace llvm
