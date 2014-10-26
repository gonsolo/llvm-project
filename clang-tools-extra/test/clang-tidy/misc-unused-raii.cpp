// RUN: $(dirname %s)/check_clang_tidy.sh %s misc-unused-raii %t
// REQUIRES: shell

struct Foo {
  Foo();
  Foo(int);
  Foo(int, int);
  ~Foo();
};

struct Bar {
  Bar();
  Foo f;
};

template <typename T>
void qux() {
  T(42);
}

template <typename T>
struct TFoo {
  TFoo(T);
  ~TFoo();
};

Foo f();

struct Ctor {
  Ctor(int);
  Ctor() {
    Ctor(0); // TODO: warn here.
  }
};

void test() {
  Foo(42);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: object destroyed immediately after creation; did you mean to name the object?
// CHECK-FIXES: Foo give_me_a_name(42);
  Foo(23, 42);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: object destroyed immediately after creation; did you mean to name the object?
// CHECK-FIXES: Foo give_me_a_name(23, 42);
  Foo();
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: object destroyed immediately after creation; did you mean to name the object?
// CHECK-FIXES: Foo give_me_a_name;
  TFoo<int>(23);
// CHECK-MESSAGES: :[[@LINE-1]]:3: warning: object destroyed immediately after creation; did you mean to name the object?
// CHECK-FIXES: TFoo<int> give_me_a_name(23);

  Bar();
  f();
  qux<Foo>();

#define M Foo();
  M

  {
    Foo();
  }
  Foo();
}
