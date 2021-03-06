// RUN: %check_clang_tidy %s readability-delete-null-pointer %t

#define NULL 0

void f() {
  int *p = 0;

  // #1
  if (p) { // #2
    delete p;
  } // #3
  // CHECK-MESSAGES: :[[@LINE-3]]:3: warning: 'if' statement is unnecessary; deleting null pointer has no effect [readability-delete-null-pointer]

  // CHECK-FIXES: {{^  }}// #1
  // CHECK-FIXES-NEXT: {{^  }}// #2
  // CHECK-FIXES-NEXT: delete p;
  // CHECK-FIXES-NEXT: {{^  }}// #3

  int *p2 = new int[3];
  // #4
  if (p2) // #5
    delete[] p2;
  // CHECK-MESSAGES: :[[@LINE-2]]:3: warning: 'if' statement is unnecessary;

  // CHECK-FIXES: // #4
  // CHECK-FIXES-NEXT: {{^  }}// #5
  // CHECK-FIXES-NEXT: delete[] p2;

  int *p3 = 0;
  if (NULL != p3) {
    // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: 'if' statement is unnecessary;
    delete p3;
  }
  // CHECK-FIXES-NOT: if (NULL != p3) {
  // CHECK-FIXES: delete p3;

  int *p4 = nullptr;
  if (p4 != nullptr) {
    // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: 'if' statement is unnecessary;
    delete p4;
  }
  // CHECK-FIXES-NOT: if (p4 != nullptr) {
  // CHECK-FIXES: delete p4;

  char *c;
  if (c != 0) {
    // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: 'if' statement is unnecessary;
    delete c;
  }
  // CHECK-FIXES-NOT: if (c != 0) {
  // CHECK-FIXES: delete c;

  char *c2;
  if (c2) {
    // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: 'if' statement is unnecessary;
    // CHECK-FIXES: } else {
    // CHECK-FIXES: c2 = c;
    delete c2;
  } else {
    c2 = c;
  }
}

void g() {
  int *p5, *p6;
  if (p5)
    delete p6;

  if (p5 && p6)
    delete p5;

  if (p6) {
    int x = 5;
    delete p6;
  }
}
