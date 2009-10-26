module namespace math = "http://www.zorba-xquery.com/modules/math";

(:~
 : Computes the sqrt of the value passed as argument.
 :
 : @param $arg the argument to compute the sqrt of.
 :
 : @returns The sqrt of the $arg value.
 :)
declare function math:sqrt ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:exp ($arg as xs:double) as xs:double external;

(:~
  Natural logarithm.
  To generate logarithms for other bases, use the mathematical relation: log base b of a == natural log (a) / natural log (b).

 :)
declare function math:log ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:sin ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:cos ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:tan ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:asin ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:acos ($arg as xs:double) as xs:double external;

(:~
 :)
declare function math:atan ($arg as xs:double) as xs:double external;


(:~
  Returns the arc tangent of y/x, in radians.
  It uses the signs of both parameters to determine the quadrant of the return value.
  
  @return a value in range -PI/2 ... +PI/2
 :)
declare function math:atan2 ($y as xs:double, $x as xs:double) as xs:double external;

(:~
  Returns the hyperbolic cosine of x.
  If the result it too large, INF is returned.
  
  @param arg must be smaller than 7.104760e+002
 :)
declare function math:cosh ($arg as xs:double) as xs:double external;

(:~
  Inverse hyperbolic cosine.
  
 :)
declare function math:acosh ($arg as xs:double) as xs:double external;

(:~
  Returns the remainder of x/y.
 :)
declare function math:fmod ($x as xs:double, $y as xs:double) as xs:double external;

(:~
  Returns the argument split as mantissa and exponent. 
  The recombining formula is (mantissa * 2^exponent).
  
  @return a sequence of two doubles (mantissa, exponent)
 :)
declare function math:frexp ($arg as xs:double) as xs:double+ external;

(:~
  Computes a real number from the mantissa and exponent.
  The formula is (x * 2^i).
  
  @param x the mantissa
  @param i the exponent
  @return the computed real number
 :)
declare function math:ldexp ($x as xs:double, $i as xs:integer) as xs:double external;

(:~
  The base 10 logarithm.
  To generate logarithms for other bases, use the mathematical relation: log base b of a == natural log (a) / natural log (b).

 :)
declare function math:log10 ($arg as xs:double) as xs:double external;

(:~
  Splits a floating-point value into fractional and integer parts.
  Both the fraction and integer keep the original sign of the value.

  @return a sequence of two doubles (fraction, integer)
 :)
declare function math:modf ($arg as xs:double) as xs:double+ external;

(:~
  Compute x^y.
  If y = 0 then result is 1.
  If x = 0 and y < 0 then result is INF.
  Arguments should not be greater than 264.
 :)
declare function math:pow ($x as xs:double, $y as xs:double) as xs:double external;

(:~
  Calculate hyperbolic sine.
 :)
declare function math:sinh ($arg as xs:double) as xs:double external;

(:~
  Inverse hyperbolic sine of the number.
 :)
declare function math:asinh($arg as xs:double) as xs:double external;

(:~
  Calculate the hyperbolic tangent.
 :)
declare function math:tanh($arg as xs:double) as xs:double external;

(:~
  Calculate the hyperbolic tangent.
  
  @param @arg must be in range -1 ... +1 (exclusive)
 :)
declare function math:atanh($arg as xs:double) as xs:double external;

(:~
  Return the value of PI.
 :)
declare function math:pi() as xs:double external;

(:~
  Checks if the double value is positive or negative infinite.
 :)
declare function math:is_inf($arg as xs:double) as xs:boolean external;

(:~
  Checks if the double value is Not a Number (NaN)
 :)
declare function math:is_nan($arg as xs:double) as xs:boolean external;
