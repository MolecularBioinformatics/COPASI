// Copyright (C) 2014 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "copasi.h"

#include "CUnitParser.h"

CUnitParserBase::CUnitParserBase():
  mPosition(0),
  mpUnit(NULL)
{}

CUnitParserBase::~CUnitParserBase()
{pdelete(mpUnit)}

const std::set< CUnitComponent > & CUnitParserBase::getComponents() const
{
  if (mpUnit != NULL)
    {
      return mpUnit->getComponents();
    }

  static std::set< CUnitComponent > Components;
  return Components;
}

void CUnitParserBase::correctErrorPosition()
{return;}

int CUnitParser::yywrap()
{return 1;}
