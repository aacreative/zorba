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

#include <string>
#include <iostream>

#include <zorba/config.h>

#ifdef ZORBA_HAVE_EXECINFO_H
#include <cstdlib>
#include <execinfo.h>
#endif

#include "errors.h"
#include "error_manager.h"

#ifdef WIN32
#include "StackWalker.h"
#endif // WIN32

using namespace std;

namespace zorba {

void print_stack_trace( ostream &o ) {
#ifdef ZORBA_HAVE_EXECINFO_H
  int const BUF_SIZE = 25;
  void *buf[ BUF_SIZE ];
  int const size = backtrace( buf, BUF_SIZE );
  if ( char **const symbols = backtrace_symbols( buf, size ) ) {
    for ( int i = 0; i < size; ++i )
      o << symbols[i] << endl;
    free( symbols );
  } else {
    o << "allocation of backtrace symbols failed" << endl;
  }
#endif
#ifdef WIN32
  StackWalker sw;
  sw.ShowCallstack();
#endif // WIN32
}

void assertion_failed( char const *condition, char const *file, int line ) {
  print_stack_trace( cerr );

  ostringstream oss;
  oss << file << ':' << line;

  throw error::ErrorManager::createException(
    XQP0005_SYSTEM_ASSERT_FAILED, condition, oss.str(), file, line
  );
}

} // namespace zorba
/* vim:set et sw=2 ts=2: */