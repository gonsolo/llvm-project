//===- unittest/Format/FormatTestProto.cpp --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "format-test"

#include "FormatTestUtils.h"
#include "clang/Format/Format.h"
#include "llvm/Support/Debug.h"
#include "gtest/gtest.h"

namespace clang {
namespace format {

class FormatTestProto : public ::testing::Test {
protected:
  static std::string format(llvm::StringRef Code, unsigned Offset,
                            unsigned Length, const FormatStyle &Style) {
    DEBUG(llvm::errs() << "---\n");
    DEBUG(llvm::errs() << Code << "\n\n");
    std::vector<tooling::Range> Ranges(1, tooling::Range(Offset, Length));
    tooling::Replacements Replaces = reformat(Style, Code, Ranges);
    std::string Result = applyAllReplacements(Code, Replaces);
    EXPECT_NE("", Result);
    DEBUG(llvm::errs() << "\n" << Result << "\n\n");
    return Result;
  }

  static std::string format(llvm::StringRef Code) {
    FormatStyle Style = getGoogleStyle(FormatStyle::LK_Proto);
    Style.ColumnLimit = 60; // To make writing tests easier.
    return format(Code, 0, Code.size(), Style);
  }

  static void verifyFormat(llvm::StringRef Code) {
    EXPECT_EQ(Code.str(), format(test::messUp(Code)));
  }
};

TEST_F(FormatTestProto, FormatsMessages) {
  verifyFormat("message SomeMessage {\n"
               "  required int32 field1 = 1;\n"
               "}");
  verifyFormat("message SomeMessage {\n"
               "  required int32 field1 = 1;\n"
               "  optional string field2 = 2 [default = \"2\"]\n"
               "}");

  verifyFormat("message SomeMessage {\n"
               "  optional really.really.long.and.qualified.type.aaaaaaa\n"
               "      fiiiiiiiiiiiiiiiiiiiiiiiiield = 1;\n"
               "  optional\n"
               "      really.really.long.and.qualified.type.aaaaaaa.aaaaaaaa\n"
               "          another_fiiiiiiiiiiiiiiiiiiiiield = 2;\n"
               "}");
}

TEST_F(FormatTestProto, FormatsEnums) {
  verifyFormat("enum Type {\n"
               "  UNKNOWN = 0;\n"
               "  TYPE_A = 1;\n"
               "  TYPE_B = 2;\n"
               "};");
}

TEST_F(FormatTestProto, UnderstandsReturns) {
  verifyFormat("rpc Search(SearchRequest) returns (SearchResponse);");
}

TEST_F(FormatTestProto, MessageFieldAttributes) {
  verifyFormat("optional string test = 1 [default = \"test\"];");
  verifyFormat("optional bool a = 1 [default = true, deprecated = true];");
  verifyFormat("optional LongMessageType long_proto_field = 1\n"
               "    [default = REALLY_REALLY_LONG_CONSTANT_VALUE,\n"
               "     deprecated = true];");
  verifyFormat("optional LongMessageType long_proto_field = 1\n"
               "    [default = REALLY_REALLY_LONG_CONSTANT_VALUE];");
  verifyFormat("repeated double value = 1\n"
               "    [(aaaaaaa.aaaaaaaaa) = {aaaaaaaaaaaaaaaaa : AAAAAAAA}];");
  verifyFormat("repeated double value = 1\n"
               "    [(aaaaaaa.aaaaaaaaa) = {aaaaaaaaaaaaaaaa : AAAAAAAAAA,\n"
               "                            bbbbbbbbbbbbbbbb : BBBBBBBBBB}];");
}

TEST_F(FormatTestProto, FormatsOptions) {
  verifyFormat("option java_package = \"my.test.package\";");
  verifyFormat("option (my_custom_option) = \"abc\";");
}

} // end namespace tooling
} // end namespace clang
