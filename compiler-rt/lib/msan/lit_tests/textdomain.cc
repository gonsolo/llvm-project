// RUN: %clangxx_msan -m64 -O0 -g %s -o %t && %t

#include <libintl.h>
#include <stdio.h>

int main() {
  const char *td = textdomain("abcd");
  if (td[0] == 0) {
    printf("Try read"); 
  }
  return 0;
}
