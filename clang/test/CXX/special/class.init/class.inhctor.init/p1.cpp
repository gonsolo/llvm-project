// RUN: %clang_cc1 -std=c++11 -verify %s

namespace std_example {
  struct B1 { // expected-note {{declared here}}
    B1(int, ...) {}
  };

  struct B2 {
    B2(double) {}
  };

  int get();

  struct D1 : B1 { // expected-note {{no default constructor}}
    using B1::B1; // inherits B1(int, ...)
    int x;
    int y = get();
  };

  void test() {
    D1 d(2, 3, 4); // OK: B1 is initialized by calling B1(2, 3, 4),
    // then d.x is default-initialized (no initialization is performed),
    // then d.y is initialized by calling get()
    D1 e; // expected-error {{implicitly-deleted}}
  }

  struct D2 : B2 {
    using B2::B2; // expected-error {{cannot use constructor inherited from base class 'B2'; member 'b' of 'std_example::D2' does not have a default constructor}}
    B1 b; // expected-note {{member}}
  };

  D2 f(1.0); // expected-note {{inherited constructor for 'D2' first required here}}

  struct W {
    W(int);
  };
  struct X : virtual W {
    using W::W;
    X() = delete;
  };
  struct Y : X {
    using X::X;
  };
  struct Z : Y, virtual W {
    using Y::Y;
  };
  Z z(0); // OK: initialization of Y does not invoke default constructor of X

  template <class T> struct Log : T {
    using T::T; // inherits all constructors from class T
    ~Log() { /* ... */ }
  };
}

namespace vbase {
  struct V { // expected-note 2{{declared here}}
    V(int);
  };

  struct A : virtual V {
    A() = delete; // expected-note 2{{deleted here}} expected-note {{deleted}}
    using V::V;
  };
  struct B : virtual V {
    B() = delete; // expected-note 2{{deleted here}}
    B(int, int);
    using V::V;
  };
  struct C : B { // expected-note {{deleted default constructor}}
    using B::B; // expected-error {{cannot use constructor inherited from base class 'B'; base class 'vbase::V' of 'vbase::C' does not have a default constructor}}
  };
  struct D : A, C { // expected-note {{deleted default constructor}}
    using A::A;
    using C::C; // expected-error {{cannot use constructor inherited from base class 'C'; base class 'vbase::V' of 'vbase::D' does not have a default constructor}} expected-error {{call to deleted constructor of 'vbase::A'}}
  };

  A a0; // expected-error {{deleted}}
  A a1(0);
  B b0; // expected-error {{deleted}}
  B b1(0);
  B b2(0, 0);
  C c0; // expected-error {{deleted}}
  C c1(0);
  C c2(0, 0); // expected-note {{first required here}}
  D d0; // expected-error {{implicitly-deleted}}
  D d1(0);
  D d2(0, 0); // expected-note {{first required here}}
}

namespace constexpr_init_order {
  struct Param;
  struct A {
    constexpr A(Param);
    int a;
  };

  struct B : A { B(); using A::A; int b = 2; };
  extern const B b;

  struct Param {
    constexpr Param(int c) : n(4 * b.a + b.b + c) {}
    int n;
  };

  constexpr A::A(Param p) : a(p.n) {}

  constexpr B b(1);
  constexpr B c(1);
  static_assert(b.a == 1, "p should be initialized before B() is executed");
  static_assert(c.a == 7, "b not initialzed properly");
}

namespace default_args {
  // We work around a defect in P0136R1 where it would reject reasonable
  // code like the following:
  struct Base {
    Base(int = 0);
  };
  struct Derived : Base {
    using Base::Base;
  };
  Derived d;
  // FIXME: Once a fix is standardized, implement it.
}
