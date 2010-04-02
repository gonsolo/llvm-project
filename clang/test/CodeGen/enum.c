// RUN: %clang_cc1 -triple i386-unknown-unknown %s -O3 -emit-llvm -o - | grep 'ret i32 6'
// RUN: %clang_cc1 -triple i386-unknown-unknown -x c++ %s -O3 -emit-llvm -o - | grep 'ret i32 7'

static enum { foo, bar = 1U } z;

int main (void)
{
  int r = 0;

  if (bar - 2 < 0)
    r += 4;
  if (foo - 1 < 0)
    r += 2;
  if (z - 1 < 0)
    r++;

  return r;
}

