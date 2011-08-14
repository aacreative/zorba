/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <iomanip>

#include "util/unique_ptr.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

static int failures;

static bool assert_true( char const *expr, int line, bool result ) {
  if ( !result ) {
    cout << "FAILED, line " << line << ": " << expr << endl;
    ++failures;
  }
  return result;
}

#define ASSERT_TRUE( EXPR ) assert_true( #EXPR, __LINE__, EXPR )

///////////////////////////////////////////////////////////////////////////////

template<class C>
struct instances {
  static int count;
  instances()  { ++count; }
  ~instances() { --count; }
};

template<class C> int instances<C>::count = 0;

#define COUNT(C) instances<C>::count

struct A : instances<A> {
  virtual ~A() { }
};

struct B : A, instances<B> {
};

struct D {
  void destroy() {
    delete this;
  }
};

template<typename T>
struct destroy_deleter {
  void operator()( T *p ) {
    p->destroy();
  }
};

///////////////////////////////////////////////////////////////////////////////

using namespace zorba;

static void test_basic() {
  {
    ztd::unique_ptr<A> a( new A );
    ASSERT_TRUE( COUNT(A) == 1 );
  }
  ASSERT_TRUE( COUNT(A) == 0 );
}

static void test_assignment() {
  ztd::unique_ptr<A> a1( new A );
  ztd::unique_ptr<A> a2;
  a2 = ztd::move( a1 );
  ASSERT_TRUE( COUNT(A) == 1 );
  ASSERT_TRUE( !a1 );
  ASSERT_TRUE( a2 );
}

static void test_copy_constructor() {
  ztd::unique_ptr<A> a1( new A );
  ASSERT_TRUE( COUNT(A) == 1 );
  ztd::unique_ptr<A> a2( ztd::move( a1 ) );
  ASSERT_TRUE( COUNT(A) == 1 );
  ASSERT_TRUE( !a1 );
  ASSERT_TRUE( a2 );
}

static void test_deleter_reference() {
  destroy_deleter<D> dd;
  ztd::unique_ptr<D,destroy_deleter<D>&> d1( new D, dd );
  ztd::unique_ptr<D,destroy_deleter<D>&> d2( new D, dd );
  ASSERT_TRUE( &d1.get_deleter() == &d2.get_deleter() );
}

static void test_derived() {
  {
    ztd::unique_ptr<A> a1( new B );
    ASSERT_TRUE( COUNT(A) == 1 );
    ASSERT_TRUE( COUNT(B) == 1 );
  }
  ASSERT_TRUE( COUNT(A) == 0 );
  ASSERT_TRUE( COUNT(B) == 0 );
  {
    ztd::unique_ptr<A> a1( new A );
    ztd::unique_ptr<B> b1( new B );
    ASSERT_TRUE( COUNT(A) == 2 );
    ASSERT_TRUE( COUNT(B) == 1 );
    a1 = b1;
    ASSERT_TRUE( COUNT(A) == 1 );
    ASSERT_TRUE( COUNT(B) == 1 );
    ASSERT_TRUE( !b1 );
    ASSERT_TRUE( a1 );
  }
}

template<typename T>
void f( ztd::unique_ptr<T> p ) {
  ASSERT_TRUE( p );
  ASSERT_TRUE( COUNT(A) == 1 );
}

static void test_function_arg() {
  ztd::unique_ptr<A> a1( new A );
  ASSERT_TRUE( COUNT(A) == 1 );
  f( ztd::move( a1 ) );
  ASSERT_TRUE( COUNT(A) == 0 );
  ASSERT_TRUE( !a1 );
}

static void test_null_assignment() {
  ztd::unique_ptr<A> a1( new A );
  ASSERT_TRUE( COUNT(A) == 1 );
  a1 = 0;
  ASSERT_TRUE( COUNT(A) == 0 );
  ASSERT_TRUE( !a1 );
}

static void test_release() {
  A *p = 0;
  {
    ztd::unique_ptr<A> a1( new A );
    ASSERT_TRUE( COUNT(A) == 1 );
    p = a1.release();
    ASSERT_TRUE( COUNT(A) == 1 );
    ASSERT_TRUE( !a1 );
    ASSERT_TRUE( p );
  }
  ASSERT_TRUE( COUNT(A) == 1 );
  delete p;
  ASSERT_TRUE( COUNT(A) == 0 );
}

static void test_sizeof() {
  ztd::unique_ptr<A> a1;
  ASSERT_TRUE( sizeof( a1 ) == sizeof( void* ) );
  ztd::unique_ptr<D,destroy_deleter<D> > d1;
  ASSERT_TRUE( sizeof( d1 ) == sizeof( void* ) );
}

static void test_swap() {
  A *p1 = new A;
  A *p2 = new A;
  ztd::unique_ptr<A> a1( p1 );
  ztd::unique_ptr<A> a2( p2 );
  ASSERT_TRUE( a1.get() == p1 );
  ASSERT_TRUE( a2.get() == p2 );
  a1.swap( a2 );
  ASSERT_TRUE( COUNT(A) == 2 );
  ASSERT_TRUE( a1.get() == p2 );
  ASSERT_TRUE( a2.get() == p1 );
  swap( a1, a2 );
  ASSERT_TRUE( COUNT(A) == 2 );
  ASSERT_TRUE( a1.get() == p1 );
  ASSERT_TRUE( a2.get() == p2 );
}

///////////////////////////////////////////////////////////////////////////////

int unique_ptr( int, char*[] ) {
  test_basic();
  test_assignment();
  test_copy_constructor();
  test_deleter_reference();
  test_derived();
  test_function_arg();
  test_null_assignment();
  test_release();
  test_sizeof();
  test_swap();

  cout << failures << " test(s) failed\n";
  return failures ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////

/* vim:set et sw=2 ts=2: */
