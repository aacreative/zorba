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

#ifndef ZORBA_INTERNAL_ERR_H
#define ZORBA_INTERNAL_ERR_H

#include <string>
#include <vector>

#include <zorba/err.h>

#include "ztd.h"

namespace zorba {
namespace internal{
  namespace err{
  class location;
}
}
namespace serialization{
  class Archiver;
  void operator&(zorba::serialization::Archiver &ar, zorba::internal::err::location &obj);
}
namespace internal {
namespace err {

///////////////////////////////////////////////////////////////////////////////

/**
 * A %location hold the file location of an error.
 */
class ZORBA_DLL_PUBLIC location {
public:
  /**
   * The line-number type.
   */
  typedef unsigned line_type;

  /**
   * The column-number type.
   */
  typedef unsigned short column_type;

  /**
   * A empty instance for convenience.
   */
  static location const empty;

  /**
   * Constructs a default (empty) %location.
   */
  location() : line_( 0 ), column_( 0 ) {
  }

  /**
   * Constructs a %location.
   *
   * @param file The name of the file where the error occurred.
   * @param line The line number of the file where the error occurred.
   * @param column The column number, if any, of the file where the error
   * occurred.
   */
  location( char const *file, line_type line, column_type column = 0 ) :
    file_( file ), line_( line ), column_( column )
  {
  }

  /**
   * Constructs a %location.
   *
   * @tparam StringType The string type for \a file.
   * @param file The name of the file where the error occurred.
   * @param line The line number of the file where the error occurred.
   * @param column The column number, if any, of the file where the error
   * occurred.
   */
  template<class StringType>
  location( StringType const &file, line_type line, column_type column = 0 ) :
    file_( file.c_str() ), line_( line ), column_( column )
  {
  }

  /**
   * Gets the file name, if any.
   *
   * @return Returns the file name or the empty string if unset.
   */
  char const* file() const {
    return file_.c_str();
  }

  /**
   * Gets the line number, if any.
   *
   * @return Returns the line number or 0 if unset.
   */
  line_type line() const {
    return line_;
  }

  /**
   * Gets the column number, if any.
   *
   * @return Returns the column number or 0 if unset.
   */
  column_type column() const {
    return column_;
  }

  /**
   * Conversion to \c bool for testing whether this %location has been set.
   *
   * @return Returns \c true only if this %location has been set.
   */
  operator bool() const {
    return !!line_;
  }

  /**
   * Checks whether this %location has not been set.
   *
   * @return Returns \c true only if this %location has not been set.
   */
  bool operator!() const {
    return !line_;
  }

  /**
   * Sets the %location information.
   *
   * @param file The name of the file where the error occurred.
   * @param line The line number of the file where the error occurred.
   * @param column The column number, if any, of the file where the error
   * occurred.
   */
  void set( char const *file, line_type line, column_type column = 0 ) {
    file_ = file;
    line_ = line;
    column_ = column;
  }

private:
  std::string file_;
  line_type line_;
  column_type column_;

private:
  //for plan serialization
  friend void zorba::serialization::operator&(zorba::serialization::Archiver &ar, location &obj);
};

/**
 * \internal
 * Makes an empty location.
 *
 * @return Returns said location.
 */
inline location make_location() {
  return location();
}

/**
 * \internal
 * Makes a location.
 *
 * @param file The name of the file where the error occurred.
 * @param line The line number of the file where the error occurred.
 * @param column The column number, if any, of the file where the error
 * occurred.
 */
inline location make_location( char const *file, location::line_type line,
                               location::column_type column = 0 ) {
  return location( file, line, column );
}

/**
 * \internal
 * Makes a location.
 *
 * @tparam StringType The string type for \a file.
 * @param file The name of the file where the error occurred.
 * @param line The line number of the file where the error occurred.
 * @param column The column number, if any, of the file where the error
 * occurred.
 */
template<class StringType> inline
location make_location( StringType const &file, location::line_type line,
                        location::column_type column = 0 ) {
  return location( file, line, column );
}

///////////////////////////////////////////////////////////////////////////////

/**
 * \internal
 * A %parameters holds the parameters for an error message.
 */
class ZORBA_DLL_PUBLIC parameters {
  typedef std::vector<std::string> params_type;
public:
  typedef params_type::value_type value_type;
  typedef params_type::size_type size_type;

  /**
   * A empty instance for convenience.
   */
  static parameters const empty;

  /**
   * Constructs a %parameters object having \a n parameters.
   *
   * @param n The number of parameters in the range [0,9].
   */
  explicit parameters( unsigned n = 0 );

  /**
   * Gets the i'th parameter value.
   * Parameter numbers start at 1.
   *
   * @param i The parameter to get.
   * @return Returns said parameter value.
   */
  value_type& operator[]( size_type i ) {
    return params_[ i - 1 ];
  }

  /**
   * Gets the i'th parameter value.
   * Parameter numbers start at 1.
   *
   * @param i The parameter to get.
   * @return Returns said parameter value.
   */
  value_type const& operator[]( size_type i ) const {
    return params_[ i - 1 ];
  }

  /**
   * Substitutes substrings of the given string.  There are two forms:
   *
   * - <code>$</code><em>i</em>
   * - <code>${</code><em>chars i chars</em><code>}</code>
   *
   * where <em>i</em> is an integer in the range <code>[1,9]</code>
   * and <em>chars</em> are any characters except <code>[1-9}]</code>.
   *
   * The second form elides the addition characacters if the value of the
   * <em>ith</em> parameter is empty.  For example, <code>${"1"}</code> will
   * substitute the value of the 1st parameter quoted if non-empty; if empty,
   * the entire substitution set of characters (everything from the
   * <code>$</code> to the <code>}</code>) will be elided.
   *
   * @param s The string to perform the substitutions on.
   */
  void substitute( value_type *s ) const;

private:
  params_type params_;

  value_type lookup_param( size_type i ) const;
};

/**
 * Makes a parameters object with no parameters.
 *
 * @return Returns said parameters object.
 */
inline parameters make_parameters() {
  return parameters( 0 );
}

/**
 * Makes a parameters object with 1 parameter.
 *
 * @tparam T1 The type of the parameter.
 * @param p1 The parameter.
 * @return Returns said parameters object.
 */
template<typename T1>
inline parameters make_parameters( T1 const &p1 ) {
  parameters p( 1 );
  ztd::to_string( p1, &p[1] );
  return p;
}

/**
 * Makes a parameters object with 2 parameters.
 *
 * @tparam T1 The type of the first parameter.
 * @tparam T2 The type of the second parameter.
 * @param p1 The first parameter.
 * @param p2 The second parameter.
 * @return Returns said parameters object.
 */
template<typename T1,typename T2>
inline parameters make_parameters( T1 const &p1, T2 const &p2 ) {
  parameters p( 2 );
  ztd::to_string( p1, &p[1] );
  ztd::to_string( p2, &p[2] );
  return p;
}

/**
 * Makes a parameters object with 3 parameters.
 *
 * @tparam T1 The type of the first parameter.
 * @tparam T2 The type of the second parameter.
 * @tparam T3 The type of the third parameter.
 * @param p1 The first parameter.
 * @param p2 The second parameter.
 * @param p3 The third parameter.
 * @return Returns said parameters object.
 */
template<typename T1,typename T2,typename T3>
inline parameters make_parameters( T1 const &p1, T2 const &p2, T3 const &p3 ) {
  parameters p( 3 );
  ztd::to_string( p1, &p[1] );
  ztd::to_string( p2, &p[2] );
  ztd::to_string( p3, &p[3] );
  return p;
}

/**
 * Makes a parameters object with 4 parameters.
 *
 * @tparam T1 The type of the first parameter.
 * @tparam T2 The type of the second parameter.
 * @tparam T3 The type of the third parameter.
 * @tparam T4 The type of the fourth parameter.
 * @param p1 The first parameter.
 * @param p2 The second parameter.
 * @param p3 The third parameter.
 * @param p4 The fourth parameter.
 * @return Returns said parameters object.
 */
template<typename T1,typename T2,typename T3,typename T4>
inline parameters make_parameters( T1 const &p1, T2 const &p2, T3 const &p3,
                                   T4 const &p4 ) {
  parameters p( 4 );
  ztd::to_string( p1, &p[1] );
  ztd::to_string( p2, &p[2] );
  ztd::to_string( p3, &p[3] );
  ztd::to_string( p4, &p[4] );
  return p;
}

/**
 * Makes a parameters object with 5 parameters.
 *
 * @tparam T1 The type of the first parameter.
 * @tparam T2 The type of the second parameter.
 * @tparam T3 The type of the third parameter.
 * @tparam T4 The type of the fourth parameter.
 * @tparam T5 The type of the fifth parameter.
 * @param p1 The first parameter.
 * @param p2 The second parameter.
 * @param p3 The third parameter.
 * @param p4 The fourth parameter.
 * @param p5 The fifth parameter.
 * @return Returns said parameters object.
 */
template<typename T1,typename T2,typename T3,typename T4,typename T5>
inline parameters make_parameters( T1 const &p1, T2 const &p2, T3 const &p3,
                                   T4 const &p4, T5 const &p5 ) {
  parameters p( 5 );
  ztd::to_string( p1, &p[1] );
  ztd::to_string( p2, &p[2] );
  ztd::to_string( p3, &p[3] );
  ztd::to_string( p4, &p[4] );
  ztd::to_string( p4, &p[5] );
  return p;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace err
} // namespace internal
} // namespace zorba
#endif /* ZORBA_INTERNAL_ERR_H */
/* vim:set et sw=2 ts=2: */
