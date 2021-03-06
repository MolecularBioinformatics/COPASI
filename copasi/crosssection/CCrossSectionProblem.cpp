// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "copasi/copasi.h"

#include "CCrossSectionProblem.h"

#include "CopasiDataModel/CDataModel.h"

CCrossSectionProblem::CCrossSectionProblem(const CDataContainer * pParent):
  CTrajectoryProblem(pParent),
  mpFlagLimitCrossings(NULL),
  mpCrossingsLimit(NULL),
  mpFlagLimitConvergence(NULL),
  mpConvergenceTolerance(NULL),
  mpFlagLimitOutConvergence(NULL),
  mpConvergenceOutTolerance(NULL),
  mpFlagLimitOutCrossings(NULL),
  mpOutCrossingsLimit(NULL),
  mpFlagLimitOutTime(NULL),
  mSingleObjectCN(NULL),
  mpFlagPositiveDirection(NULL),
  mpThreshold(NULL)
{
  initializeParameter();
  initObjects();
  CONSTRUCTOR_TRACE;
}

CCrossSectionProblem::CCrossSectionProblem(const CCrossSectionProblem & src,
    const CDataContainer * pParent):
  CTrajectoryProblem(src, pParent),
  mpFlagLimitCrossings(NULL),
  mpCrossingsLimit(NULL),
  mpFlagLimitConvergence(NULL),
  mpConvergenceTolerance(NULL),
  mpFlagLimitOutConvergence(NULL),
  mpConvergenceOutTolerance(NULL),
  mpFlagLimitOutCrossings(NULL),
  mpOutCrossingsLimit(NULL),
  mpFlagLimitOutTime(NULL),
  mSingleObjectCN(NULL),
  mpFlagPositiveDirection(NULL),
  mpThreshold(NULL)
{
  initializeParameter();
  initObjects();
  CONSTRUCTOR_TRACE;
}

void CCrossSectionProblem::initializeParameter()
{
  mpFlagLimitCrossings = assertParameter("LimitCrossings", CCopasiParameter::BOOL, false);
  mpCrossingsLimit = assertParameter("NumCrossingsLimit", CCopasiParameter::UINT, (unsigned C_INT32)0);
  mpFlagLimitOutTime = assertParameter("LimitOutTime", CCopasiParameter::BOOL, false);
  mpFlagLimitOutCrossings = assertParameter("LimitOutCrossings", CCopasiParameter::BOOL, false);
  mpFlagPositiveDirection = assertParameter("PositiveDirection", CCopasiParameter::BOOL, true);
  mpOutCrossingsLimit = assertParameter("NumOutCrossingsLimit", CCopasiParameter::UINT, (unsigned C_INT32)0);
  mpFlagLimitConvergence = assertParameter("LimitUntilConvergence", CCopasiParameter::BOOL, false);
  mpConvergenceTolerance = assertParameter("ConvergenceTolerance", CCopasiParameter::DOUBLE, (C_FLOAT64)1E-6);
  mpThreshold = assertParameter("Threshold", CCopasiParameter::DOUBLE, (C_FLOAT64)0);
  mpFlagLimitOutConvergence = assertParameter("DelayOutputUntilConvergence", CCopasiParameter::BOOL, false);
  mpConvergenceOutTolerance = assertParameter("OutputConvergenceTolerance", CCopasiParameter::DOUBLE, (C_FLOAT64)1E-6);
  mpTriggerExpression = assertParameter("TriggerExpression", CCopasiParameter::EXPRESSION, std::string(""));
  mSingleObjectCN = assertParameter("SingleVariable", CCopasiParameter::CN, CCommonName(""));
  setOutputEvent(false);
}

void CCrossSectionProblem::initObjects()
{
  //here we should create things like object references to results of the calculation
}

/**
 *  Destructor.
 */
CCrossSectionProblem::~CCrossSectionProblem()
{DESTRUCTOR_TRACE;}

std::ostream &operator<<(std::ostream &os, const CCrossSectionProblem & o)
{
  os << "Cross Section Problem description: Not implemented yet." << std::endl;
  const CDataModel* pDataModel = o.getObjectDataModel();
  assert(pDataModel != NULL);

  return os;
}

void CCrossSectionProblem::print(std::ostream * ostream) const
{*ostream << *this;}

bool CCrossSectionProblem::getFlagLimitCrossings() const
{return *mpFlagLimitCrossings;}

const unsigned C_INT32 & CCrossSectionProblem::getCrossingsLimit() const
{return *mpCrossingsLimit;}

bool CCrossSectionProblem::getFlagLimitOutCrossings() const
{return *mpFlagLimitOutCrossings;}

const unsigned C_INT32 & CCrossSectionProblem::getOutCrossingsLimit() const
{return *mpOutCrossingsLimit;}

bool CCrossSectionProblem::getFlagLimitOutTime() const
{return *mpFlagLimitOutTime;}

const C_FLOAT64 & CCrossSectionProblem::getTimeLimit() const
{return *mpDuration;}

bool CCrossSectionProblem::isPositiveDirection() const
{
  return *mpFlagPositiveDirection;
}
void CCrossSectionProblem::setPositiveDirection(bool isPositive)
{
  *mpFlagPositiveDirection = isPositive;
}

void CCrossSectionProblem::setFlagLimitCrossings(bool flagLimitCrossing)
{*mpFlagLimitCrossings = flagLimitCrossing;}

void CCrossSectionProblem::setCrossingsLimit(const unsigned C_INT32 &crossingLimit)
{*mpCrossingsLimit = crossingLimit;}

void CCrossSectionProblem::setFlagLimitOutCrossings(bool flagLimitCrossing)
{*mpFlagLimitOutCrossings = flagLimitCrossing;}

void CCrossSectionProblem::setOutCrossingsLimit(const unsigned C_INT32 &crossingLimit)
{*mpOutCrossingsLimit = crossingLimit;}

void CCrossSectionProblem::setFlagLimitOutTime(bool flagLimitTime)
{*mpFlagLimitOutTime = flagLimitTime;}

void CCrossSectionProblem::setTimeLimit(const C_FLOAT64 &timeLimit)
{*mpDuration = timeLimit;}

/**
 * return the variable
 */
const std::string& CCrossSectionProblem::getSingleObjectCN() const
{
  return *mSingleObjectCN;
}

void CCrossSectionProblem::setSingleObjectCN(const CDataObject* pObject)
{
  if (pObject == NULL) return;

  setSingleObjectCN(pObject->getCN());
}

const C_FLOAT64& CCrossSectionProblem::getThreshold() const
{
  return *mpThreshold;
}
void CCrossSectionProblem::setThreshold(const C_FLOAT64 &threshold)
{
  *mpThreshold = threshold;
}

/*
 * set the variable
 */
void CCrossSectionProblem::setSingleObjectCN(const std::string& cn)
{
  *mSingleObjectCN = cn;
}

bool CCrossSectionProblem::getFlagLimitConvergence() const
{
  return *mpFlagLimitConvergence;
}

const C_FLOAT64 &CCrossSectionProblem::getConvergenceTolerance() const
{
  return *mpConvergenceTolerance;
}

void CCrossSectionProblem::setFlagLimitConvergence(bool flagLimitConvergence)
{
  *mpFlagLimitConvergence = flagLimitConvergence;
}

void CCrossSectionProblem::setConvergenceTolerance(const C_FLOAT64 &convergenceTolerance)
{
  *mpConvergenceTolerance = convergenceTolerance;
}

bool CCrossSectionProblem::getFlagLimitOutConvergence() const
{
  return *mpFlagLimitOutConvergence;
}

const C_FLOAT64 &CCrossSectionProblem::getConvergenceOutTolerance() const
{
  return *mpConvergenceOutTolerance;
}

void CCrossSectionProblem::setFlagLimitOutConvergence(bool flagLimitConvergence)
{
  *mpFlagLimitOutConvergence = flagLimitConvergence;
}

void CCrossSectionProblem::setConvergenceOutTolerance(const C_FLOAT64 &convergenceTolerance)
{
  *mpConvergenceOutTolerance = convergenceTolerance;
}
