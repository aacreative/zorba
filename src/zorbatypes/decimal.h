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
#ifndef ZORBA_DECIMAL_H
#define ZORBA_DECIMAL_H

#include <zorba/config.h>
#include "common/common.h"

#ifndef ZORBA_NO_BIGNUMBERS
#include "zorbatypes/m_apm.h"
#else
#include <math.h>
#endif
#include "zorbatypes/xqpstring.h"
#include "zorbatypes/zorbatypes_decl.h"
#include "zorbaserialization/class_serializer.h"


namespace zorba {

#ifdef ZORBA_NO_BIGNUMBERS
typedef double    MAPM;
#endif

// exported for testing only
class ZORBA_DLL_PUBLIC  Decimal  : public ::zorba::serialization::SerializeBaseClass
{
  friend class Integer;

  template <typename Type>
    friend class FloatImpl;

  friend class NumConversions;

public:
  static Decimal& zero();

  static bool parseString(const char*, Decimal&);

  static bool parseNativeDouble(double val, Decimal&);

  static bool parseFloat(const Float& val, Decimal&);

  static bool parseDouble(const Double& val, Decimal&);

  static Decimal parseInteger(const Integer& val);

  static Decimal parseLongLong(int64_t val);

  static Decimal parseULongLong(uint64_t val);

  static Decimal parseInt(int32_t val);

  static Decimal parseUInt(uint32_t val);

#ifndef ZORBA_NO_BIGNUMBERS
  static MAPM round(const MAPM& aValue, const MAPM& aPrecision);
  static MAPM roundHalfToEven(const MAPM& aValue, const MAPM& aPrecision);
#else
  static MAPM round(MAPM aValue, int aPrecision);
  static MAPM roundHalfToEven(MAPM aValue, int aPrecision);
#endif

private:
  static xqpStringStore_t decimalToString(
        MAPM,
        int precision = ZORBA_FLOAT_POINT_PRECISION);

  static void reduceFloatingPointString(char* str);

private:
  MAPM theDecimal;

public:
  SERIALIZABLE_CLASS(Decimal)
  SERIALIZABLE_CLASS_CONSTRUCTOR(Decimal)
  void serialize(::zorba::serialization::Archiver& ar);

public:
  Decimal() : theDecimal(0) { }

  Decimal(const Decimal& aDecimal) 
    :
    ::zorba::serialization::SerializeBaseClass(), theDecimal(aDecimal.theDecimal) 
  {
  }
  
  Decimal(const MAPM& val) : theDecimal(val) { }

  virtual ~Decimal() {}

  Decimal& operator=(const Decimal&);

  Decimal operator+(const Integer&) const;
  Decimal operator+(const Decimal&) const;

  Decimal& operator+=(const Integer&);
  Decimal& operator+=(const Decimal&);

  Decimal operator-(const Integer&) const;
  Decimal operator-(const Decimal&) const;

  Decimal& operator-=(const Integer&);
  Decimal& operator-=(const Decimal&);

  Decimal operator*(const Integer&) const;
  Decimal operator*(const Decimal&) const;

  Decimal& operator*=(const Integer&);
  Decimal& operator*=(const Decimal&);

  Decimal operator/(const Integer&) const;
  Decimal operator/(const Decimal&) const;

  Decimal& operator/=(const Integer&);
  Decimal& operator/=(const Decimal&);

  Decimal operator%(const Integer&) const;
  Decimal operator%(const Decimal&) const;

  Decimal& operator%=(const Integer&);
  Decimal& operator%=(const Decimal&);

  Decimal operator-() const; 

#ifndef ZORBA_NO_BIGNUMBERS
  Decimal floor() const { return Decimal(theDecimal.floor()); }
  Decimal ceil() const { return Decimal(theDecimal.ceil()); }
#else
  Decimal floor() const { return Decimal(::floor(theDecimal)); }
  Decimal ceil() const { return Decimal(::ceil(theDecimal)); }
#endif

  Decimal round() const;

  Decimal round(Integer aPrecision) const; 

  Decimal roundHalfToEven(Integer aPrecision) const;

  Decimal sqrt() const;

  long compare(const Decimal& other) const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return theDecimal.compare(other.theDecimal);
#else
    return (theDecimal < other.theDecimal ?
            -1 :
            (theDecimal == other.theDecimal ? 0 : 1)); 
#endif
  }

  bool operator==(const Decimal& aDecimal) const 
  {
    return theDecimal == aDecimal.theDecimal;
  }

  bool operator!=(const Decimal& aDecimal) const 
  {
    return theDecimal != aDecimal.theDecimal;
  }

  bool operator<(const Decimal& aDecimal) const 
  {
    return theDecimal < aDecimal.theDecimal;
  }

  bool operator<=(const Decimal& aDecimal) const 
  {
    return theDecimal <= aDecimal.theDecimal;
  }

  bool operator>(const Decimal& aDecimal) const 
  {
    return theDecimal > aDecimal.theDecimal; 
  }

  bool operator>=(const Decimal& aDecimal) const 
  {
    return theDecimal >= aDecimal.theDecimal;
  }

  bool operator==(const Integer&) const;

  bool operator!=(const Integer&) const;

  bool operator<(const Integer&) const;

  bool operator<=(const Integer&) const;

  bool operator>(const Integer&) const;

  bool operator>=(const Integer&) const;

  uint32_t hash() const;

  xqpStringStore_t toString() const;

  xqpStringStore_t toIntegerString() const;

  bool isInteger() const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return theDecimal.is_integer() != 0;
#else
    return false;
#endif
  }

  bool isULong() const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return (theDecimal.is_integer() &&
            theDecimal.sign() >= 0 &&
            theDecimal < MAPM::getMaxUInt64());
#else
    return false;
#endif
  }

  bool isLong() const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return (theDecimal.is_integer() && 
            theDecimal < MAPM::getMaxInt64() &&
            theDecimal > MAPM::getMinInt64());
#else
    return false;
#endif
  }

  bool isUInt() const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return (theDecimal.is_integer() &&
            theDecimal.sign() >= 0 &&
            theDecimal < MAPM::getMaxUInt32());
#else
    return false;
#endif
  }

  bool isInt() const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return (theDecimal.is_integer() && 
            theDecimal < MAPM::getMaxInt32() &&
            theDecimal > MAPM::getMinInt32());
#else
    return false;
#endif
  }

  bool isNegative() const
  {
#ifndef ZORBA_NO_BIGNUMBERS
    return theDecimal.sign() < 0;
#else
    return theDecimal < 0;
#endif
  }

  int getValueAsInt() const;
};

  
 std::ostream& operator<<(std::ostream& os, const Decimal&);

} // namespace zorba

#endif // ZORBA_DECIMAL_H

/*
 * Local variables:
 * mode: c++
 * End:
 */
