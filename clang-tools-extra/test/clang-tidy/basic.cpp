// RUN: clang-tidy %s -checks='-*,llvm-namespace-comment' -- | FileCheck %s

namespace i {
}
// CHECK: warning: namespace not terminated with a closing comment [llvm-namespace-comment]
