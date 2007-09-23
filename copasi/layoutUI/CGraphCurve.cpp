// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/layoutUI/CGraphCurve.cpp,v $
//   $Revision: 1.3 $
//   $Name:  $
//   $Author: urost $
//   $Date: 2007/09/23 10:27:56 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "copasi.h"

#include "CGraphCurve.h"

CGraphCurve::CGraphCurve()
    : CLCurve()
{
  mHasArrow = false;
}

CGraphCurve::CGraphCurve(const CLCurve & c)
    : CLCurve(c)
{
  mHasArrow = false;
}

CGraphCurve::CGraphCurve(const CGraphCurve & c)
    : CLCurve(c)
{
  mHasArrow = c.mHasArrow;
  mArrow = c.mArrow;
}

void CGraphCurve::scale (const double & scaleFactor)
{
  unsigned int i; // scale all segments
  for (i = 0;i < mCurveSegments.size();i++)
    {
      mCurveSegments[i].scale(scaleFactor);
    }

  if (mHasArrow)
    mArrow.scale(scaleFactor);
}
