// Copyright (C) 2014 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// This class defines the kinds of SI units COPASI's units will be
// based on, as well as associated symbols (UTF strings), for display.

#ifndef CBASEUNIT_H
#define CBASEUNIT_H

// SI units based on http://physics.nist.gov/cuu/Units/units.html

#include <string>

class CBaseUnit
{

public:
  enum Kind
  {
    // prime numbers to make reliable to quickly calculate if CUnit is dimensionless
    dimensionless = 0,
    meter,
    gram,
    second,
    ampere,
    kelvin,
    item,  // mole can be derived with the current value of Avagadro's Number
    candela
  };

  static const char * Name[];

  static const std::string getSymbol(Kind kind);

  static const Kind fromSymbol(const std::string & symbol);

  //http://physics.nist.gov/cuu/Units/prefixes.html
  enum Scale
  {
    // SCALE    [afpnumcdhkMGTP]
    atto = -18,
    femto = -15,
    pico = -12,
    nano = -9,
    micro = -6,
    milli = -3,
    centi = -2,
    deci = -1,
    zero = 0,
    hecto = 2,
    kilo = 3,
    mega = 6,
    giga = 9,
    tera = 12,
    peta = 15
  };

  static const Scale scaleFromPrefix(const std::string & prefix);
};

#endif // CBASEUNIT_H
