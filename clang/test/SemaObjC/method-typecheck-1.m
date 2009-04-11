// RUN: clang-cc -fsyntax-only -verify %s

@interface A
- (void) setMoo: (int) x;	//  expected-note {{previous definition is here}}
- (int) setMoo1: (int) x;	//  expected-note {{previous definition is here}}
- (int) setOk : (int) x : (double) d;
@end

@implementation A 
-(void) setMoo: (float) x {}	//  expected-warning {{conflicting parameter types in implementation of 'setMoo:': 'int' vs 'float'}}
- (char) setMoo1: (int) x {}	//  expected-warning {{conflicting return type in implementation of 'setMoo1:': 'int' vs 'char'}}
- (int) setOk : (int) x : (double) d {}
@end



@interface C
+ (void) cMoo: (int) x;	//  expected-note 2 {{previous definition is here}}
@end

@implementation C 
+(float) cMoo:   // expected-warning {{conflicting return type in implementation of 'cMoo:': 'void' vs 'float'}}
   (float) x {}	//  expected-warning {{conflicting parameter types in implementation of 'cMoo:': 'int' vs 'float'}}
@end


@interface A(CAT)
- (void) setCat: (int) x;	// expected-note 2 {{previous definition is here}}
+ (void) cCat: (int) x;	//  expected-note {{previous definition is here}}
@end

@implementation A(CAT) 
-(float) setCat:  // expected-warning {{conflicting return type in implementation of 'setCat:': 'void' vs 'float'}}
(float) x {}	//  expected-warning {{conflicting parameter types in implementation of 'setCat:': 'int' vs 'float'}}
+ (int) cCat: (int) x {}	//  expected-warning {{conflicting return type in implementation of 'cCat:': 'void' vs 'int'}}
@end

