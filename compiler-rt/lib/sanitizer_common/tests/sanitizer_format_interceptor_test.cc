//===-- sanitizer_format_interceptor_test.cc ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Tests for *scanf interceptors implementation in sanitizer_common.
//
//===----------------------------------------------------------------------===//
#include <vector>

#include "interception/interception.h"
#include "sanitizer_test_utils.h"
#include "sanitizer_common/sanitizer_libc.h"
#include "sanitizer_common/sanitizer_common.h"
#include "gtest/gtest.h"

using namespace __sanitizer;

#define COMMON_INTERCEPTOR_READ_WRITE_RANGE(ctx, ptr, size)                    \
  do {                                                                         \
    ((std::vector<unsigned> *)ctx)->push_back(size);                           \
    ptr = ptr;                                                                 \
  } while (0)

#define COMMON_INTERCEPTOR_READ_RANGE(ctx, ptr, size)                          \
  COMMON_INTERCEPTOR_READ_WRITE_RANGE(ctx, ptr, size)

#define COMMON_INTERCEPTOR_WRITE_RANGE(ctx, ptr, size)                         \
  COMMON_INTERCEPTOR_READ_WRITE_RANGE(ctx, ptr, size)

#define SANITIZER_INTERCEPT_PRINTF 1
#include "sanitizer_common/sanitizer_common_interceptors_format.inc"

static const unsigned I = sizeof(int);
static const unsigned L = sizeof(long);
static const unsigned LL = sizeof(long long);
static const unsigned S = sizeof(short);
static const unsigned C = sizeof(char);
static const unsigned D = sizeof(double);
static const unsigned LD = sizeof(long double);
static const unsigned F = sizeof(float);
static const unsigned P = sizeof(char *);

static void verifyFormatResults(const char *format, unsigned n,
                                const std::vector<unsigned> &computed_sizes,
                                va_list expected_sizes) {
  // "+ 1" because of format string
  ASSERT_EQ(n + 1,
            computed_sizes.size()) << "Unexpected number of format arguments: '"
                                   << format << "'";
  for (unsigned i = 0; i < n; ++i)
    EXPECT_EQ(va_arg(expected_sizes, unsigned), computed_sizes[i + 1])
        << "Unexpect write size for argument " << i << ", format string '"
        << format << "'";
}

static const char test_buf[] = "Test string.";
static const size_t test_buf_size = sizeof(test_buf);

static const unsigned SCANF_ARGS_MAX = 16;

static void testScanf3(void *ctx, int result, bool allowGnuMalloc,
                       const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  scanf_common(ctx, result, allowGnuMalloc, format, ap);
  va_end(ap);
}

static void testScanf2(const char *format, int scanf_result,
                       bool allowGnuMalloc, unsigned n,
                       va_list expected_sizes) {
  std::vector<unsigned> scanf_sizes;
  // 16 args should be enough.
  testScanf3((void *)&scanf_sizes, scanf_result, allowGnuMalloc, format,
             test_buf, test_buf, test_buf, test_buf, test_buf, test_buf,
             test_buf, test_buf, test_buf, test_buf, test_buf, test_buf,
             test_buf, test_buf, test_buf, test_buf);
  verifyFormatResults(format, n, scanf_sizes, expected_sizes);
}

static void testScanf(const char *format, unsigned n, ...) {
  va_list ap;
  va_start(ap, n);
  testScanf2(format, SCANF_ARGS_MAX, /* allowGnuMalloc */ true, n, ap);
  va_end(ap);
}

static void testScanfPartial(const char *format, int scanf_result, unsigned n,
                             ...) {
  va_list ap;
  va_start(ap, n);
  testScanf2(format, scanf_result, /* allowGnuMalloc */ true,  n, ap);
  va_end(ap);
}

static void testScanfNoGnuMalloc(const char *format, unsigned n, ...) {
  va_list ap;
  va_start(ap, n);
  testScanf2(format, SCANF_ARGS_MAX, /* allowGnuMalloc */ false, n, ap);
  va_end(ap);
}

TEST(SanitizerCommonInterceptors, Scanf) {
  testScanf("%d", 1, I);
  testScanf("%d%d%d", 3, I, I, I);
  testScanf("ab%u%dc", 2, I, I);
  testScanf("%ld", 1, L);
  testScanf("%llu", 1, LL);
  testScanf("a %hd%hhx", 2, S, C);
  testScanf("%c", 1, C);

  testScanf("%%", 0);
  testScanf("a%%", 0);
  testScanf("a%%b", 0);
  testScanf("a%%%%b", 0);
  testScanf("a%%b%%", 0);
  testScanf("a%%%%%%b", 0);
  testScanf("a%%%%%b", 0);
  testScanf("a%%%%%f", 1, F);
  testScanf("a%%%lxb", 1, L);
  testScanf("a%lf%%%lxb", 2, D, L);
  testScanf("%nf", 1, I);

  testScanf("%10s", 1, 11);
  testScanf("%10c", 1, 10);
  testScanf("%%10s", 0);
  testScanf("%*10s", 0);
  testScanf("%*d", 0);

  testScanf("%4d%8f%c", 3, I, F, C);
  testScanf("%s%d", 2, test_buf_size, I);
  testScanf("%[abc]", 1, test_buf_size);
  testScanf("%4[bcdef]", 1, 5);
  testScanf("%[]]", 1, test_buf_size);
  testScanf("%8[^]%d0-9-]%c", 2, 9, C);

  testScanf("%*[^:]%n:%d:%1[ ]%n", 4, I, I, 2, I);

  testScanf("%*d%u", 1, I);

  testScanf("%c%d", 2, C, I);
  testScanf("%A%lf", 2, F, D);

  testScanf("%ms %Lf", 2, P, LD);
  testScanf("s%Las", 1, LD);
  testScanf("%ar", 1, F);

  // In the cases with std::min below the format spec can be interpreted as
  // either floating-something, or (GNU extension) callee-allocated string.
  // Our conservative implementation reports one of the two possibilities with
  // the least store range.
  testScanf("%a[", 0);
  testScanf("%a[]", 0);
  testScanf("%a[]]", 1, std::min(F, P));
  testScanf("%a[abc]", 1, std::min(F, P));
  testScanf("%a[^abc]", 1, std::min(F, P));
  testScanf("%a[ab%c] %d", 0);
  testScanf("%a[^ab%c] %d", 0);
  testScanf("%as", 1, std::min(F, P));
  testScanf("%aS", 1, std::min(F, P));
  testScanf("%a13S", 1, std::min(F, P));
  testScanf("%alS", 1, std::min(F, P));

  testScanfNoGnuMalloc("s%Las", 1, LD);
  testScanfNoGnuMalloc("%ar", 1, F);
  testScanfNoGnuMalloc("%a[", 1, F);
  testScanfNoGnuMalloc("%a[]", 1, F);
  testScanfNoGnuMalloc("%a[]]", 1, F);
  testScanfNoGnuMalloc("%a[abc]", 1, F);
  testScanfNoGnuMalloc("%a[^abc]", 1, F);
  testScanfNoGnuMalloc("%a[ab%c] %d", 3, F, C, I);
  testScanfNoGnuMalloc("%a[^ab%c] %d", 3, F, C, I);
  testScanfNoGnuMalloc("%as", 1, F);
  testScanfNoGnuMalloc("%aS", 1, F);
  testScanfNoGnuMalloc("%a13S", 1, F);
  testScanfNoGnuMalloc("%alS", 1, F);

  testScanf("%5$d", 0);
  testScanf("%md", 0);
  testScanf("%m10s", 0);

  testScanfPartial("%d%d%d%d //1\n", 1, 1, I);
  testScanfPartial("%d%d%d%d //2\n", 2, 2, I, I);
  testScanfPartial("%d%d%d%d //3\n", 3, 3, I, I, I);
  testScanfPartial("%d%d%d%d //4\n", 4, 4, I, I, I, I);

  testScanfPartial("%d%n%n%d //1\n", 1, 3, I, I, I);
  testScanfPartial("%d%n%n%d //2\n", 2, 4, I, I, I, I);

  testScanfPartial("%d%n%n%d %s %s", 3, 5, I, I, I, I, test_buf_size);
  testScanfPartial("%d%n%n%d %s %s", 4, 6, I, I, I, I, test_buf_size,
                   test_buf_size);
}

static void testPrintf3(void *ctx, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  printf_common(ctx, format, ap);
  va_end(ap);
}

static void testPrintf2(const char *format, unsigned n,
                       va_list expected_sizes) {
  std::vector<unsigned> printf_sizes;
  // 16 args should be enough.
  testPrintf3((void *)&printf_sizes, format,
             test_buf, test_buf, test_buf, test_buf, test_buf, test_buf,
             test_buf, test_buf, test_buf, test_buf, test_buf, test_buf,
             test_buf, test_buf, test_buf, test_buf);
  verifyFormatResults(format, n, printf_sizes, expected_sizes);
}

static void testPrintf(const char *format, unsigned n, ...) {
  va_list ap;
  va_start(ap, n);
  testPrintf2(format, n, ap);
  va_end(ap);
}

TEST(SanitizerCommonInterceptors, Printf) {
  // Only test functionality which differs from scanf

  // Indexed arguments
  testPrintf("%5$d", 0);
  testPrintf("%.*5$d", 0);

  // errno
  testPrintf("%0-m", 0);

  // Dynamic width
  testPrintf("%*n", 1, I);
  testPrintf("%*.10n", 1, I);

  // Precision
  testPrintf("%10.10n", 1, I);
  testPrintf("%.3s", 1, 3);
  testPrintf("%.20s", 1, test_buf_size);

  // Dynamic precision
  testPrintf("%.*n", 1, I);
  testPrintf("%10.*n", 1, I);

  // Dynamic precision for strings is not implemented yet.
  testPrintf("%.*s", 1, 0);
}
