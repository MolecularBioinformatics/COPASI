// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2002 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 *  CTrajectoryProblem class.
 *  This class describes the trajectory problem, i.e., it allows to specify
 *  for example initial conditions and number of steps.
 *
 *  Created for COPASI by Stefan Hoops 2002
 */

#include <limits.h>
#include <cmath>
#include <string>

#include "copasi.h"
#include "CTrajectoryProblem.h"
#include "model/CModel.h"
//#include "model/CState.h"
#include "CopasiDataModel/CDataModel.h"
#include "copasi/core/CRootContainer.h"

/**
 *  Default constructor.
 */
CTrajectoryProblem::CTrajectoryProblem(const CDataContainer * pParent):
  CCopasiProblem(CTaskEnum::Task::timeCourse, pParent),
  mpAutomaticStepSize(NULL),
  mpDuration(NULL),
  mpStepSize(NULL),
  mpStepNumber(NULL),
  mpTimeSeriesRequested(NULL),
  mpOutputStartTime(NULL),
  mpOutputEvent(NULL),
  mpStartInSteadyState(NULL),
  mStepNumberSetLast(true)
{
  initializeParameter();
  initObjects();
  CONSTRUCTOR_TRACE;
}

/**
 *  Copy constructor.
 *  @param "const CTrajectoryProblem &" src
 */
CTrajectoryProblem::CTrajectoryProblem(const CTrajectoryProblem & src,
                                       const CDataContainer * pParent):
  CCopasiProblem(src, pParent),
  mpAutomaticStepSize(NULL),
  mpDuration(NULL),
  mpStepSize(NULL),
  mpStepNumber(NULL),
  mpTimeSeriesRequested(NULL),
  mpOutputStartTime(NULL),
  mpOutputEvent(NULL),
  mpStartInSteadyState(NULL),
  mStepNumberSetLast(src.mStepNumberSetLast)
{
  initializeParameter();
  initObjects();
  CONSTRUCTOR_TRACE;
}

/**
 *  Destructor.
 */
CTrajectoryProblem::~CTrajectoryProblem()
{DESTRUCTOR_TRACE;}

void CTrajectoryProblem::initializeParameter()
{
  mpAutomaticStepSize = assertParameter("AutomaticStepSize", CCopasiParameter::BOOL, (bool) false);
  mpStepNumber = assertParameter("StepNumber", CCopasiParameter::UINT, (unsigned C_INT32) 100);
  mpStepSize = assertParameter("StepSize", CCopasiParameter::DOUBLE, (C_FLOAT64) 0.01);
  mpDuration = assertParameter("Duration", CCopasiParameter::DOUBLE, (C_FLOAT64) 1.0);
  mpTimeSeriesRequested = assertParameter("TimeSeriesRequested", CCopasiParameter::BOOL, (bool) true);
  mpOutputStartTime = assertParameter("OutputStartTime", CCopasiParameter::DOUBLE, (C_FLOAT64) 0.0);
  mpOutputEvent = assertParameter("Output Event", CCopasiParameter::BOOL, (bool) false);
  mpStartInSteadyState = assertParameter("Start in Steady State", CCopasiParameter::BOOL, false);
}

bool CTrajectoryProblem::elevateChildren()
{
  // If we have an old COPASI file "Duration" is not set
  // but we can fix that.
  if (*mpDuration == 1.0) // the default
    setDuration(*mpStepSize * (C_FLOAT64) *mpStepNumber);

  return true;
}

void CTrajectoryProblem::initObjects()
{}

/**
 * Set the number of time steps the trajectory method should integrate.
 * @param "const unsigned C_INT32 &" stepNumber
 */
void CTrajectoryProblem::setStepNumber(const unsigned C_INT32 & stepNumber)
{
  *mpStepNumber = stepNumber;
  mStepNumberSetLast = true;
  sync();

  return;
}

/**
 * Retrieve the number of time steps the trajectory method should integrate.
 * @return "const unsigned C_INT32 &" stepNumber
 */
const unsigned C_INT32 & CTrajectoryProblem::getStepNumber() const
{return *mpStepNumber;}

/**
 * Set the size a integration step the trajectory method should do.
 * @param "const C_FLOAT64 &" stepSize
 */
void CTrajectoryProblem::setStepSize(const C_FLOAT64 & stepSize)
{
  *mpStepSize = stepSize;
  mStepNumberSetLast = false;
  sync();

  return;
}

/**
 * Retrieve the size a integration step the trajectory method should do.
 * @return "const C_FLOAT64 &" stepSize
 */
const C_FLOAT64 & CTrajectoryProblem::getStepSize() const
{return *mpStepSize;}

const bool & CTrajectoryProblem::getAutomaticStepSize() const
{
  return *mpAutomaticStepSize;
}

void CTrajectoryProblem::setAutomaticStepSize(const bool & automaticStepSize)
{
  *mpAutomaticStepSize = automaticStepSize;
}

/**
 * Set the end time.
 * @param "const C_FLOAT64 &" duration
 * @parem bool success
 */
void CTrajectoryProblem::setDuration(const C_FLOAT64 & duration)
{
  *mpDuration = duration;
  sync();

  return;
}

/**
 * Retrieve the end time.
 * @return "const C_FLOAT64 &" duration
 */
const C_FLOAT64 & CTrajectoryProblem::getDuration() const
{return *mpDuration;}

void CTrajectoryProblem::setOutputStartTime(const C_FLOAT64 & startTime)
{
  *mpOutputStartTime = startTime;
}

const C_FLOAT64 & CTrajectoryProblem::getOutputStartTime() const
{return *mpOutputStartTime;}

void CTrajectoryProblem::setTimeSeriesRequested(bool flag)
{
  *mpTimeSeriesRequested = flag;
}

bool CTrajectoryProblem::timeSeriesRequested() const
{return *mpTimeSeriesRequested;}

void CTrajectoryProblem::setOutputEvent(const bool & outputEvent)
{
  *mpOutputEvent = outputEvent;
}

const bool & CTrajectoryProblem::getOutputEvent() const
{return *mpOutputEvent;}

/**
 * Load a trajectory problem
 * @param "CReadConfig &" configBuffer
 */
void CTrajectoryProblem::load(CReadConfig & configBuffer,
                              CReadConfig::Mode C_UNUSED(mode))
{
  if (configBuffer.getVersion() < "4.0")
    {
      configBuffer.getVariable("EndTime", "C_FLOAT64",
                               mpDuration,
                               CReadConfig::LOOP);
      configBuffer.getVariable("Points", "C_INT32",
                               mpStepNumber);
      mStepNumberSetLast = true;

      sync();
    }
}

/**
 * This function synchronizes step size and number
 */
bool CTrajectoryProblem::sync()
{
  bool success = true;

  if (fabs(*mpDuration) < std::numeric_limits< C_FLOAT64 >::min())
    return success;

  C_FLOAT64 Tmp = *mpDuration;
  C_FLOAT64 StepSize = *mpStepSize;
  C_FLOAT64 StepNumber = (C_FLOAT64) * mpStepNumber;

  if (mStepNumberSetLast)
    {
      StepSize = Tmp / (C_FLOAT64) * mpStepNumber;

      /* Assure that the step size is not to small for machine accuracy */
      if (fabs(StepSize) < 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() * fabs(*mpDuration))
        {
          CCopasiMessage(CCopasiMessage::WARNING,
                         MCTrajectoryProblem + 3, StepSize);

          StepSize = 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() * fabs(*mpDuration);
          /* Assure that the step size has the appropriate sign. */
          StepSize = (Tmp < 0.0) ? - fabs(StepSize) : fabs(StepSize);
          StepNumber = fabs(ceil(Tmp / StepSize));
        }
    }
  else
    {
      if (fabs(StepSize) < 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() * fabs(*mpDuration))
        {
          CCopasiMessage(CCopasiMessage::WARNING,
                         MCTrajectoryProblem + 3, StepSize);

          StepSize = 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() * fabs(*mpDuration);

          /* Assure that the step size has the appropriate sign. */
          StepSize = (Tmp < 0.0) ? - fabs(StepSize) : fabs(StepSize);
        }

      StepNumber = fabs(ceil(Tmp / StepSize));

      /* Protect against overflow */
      if ((C_FLOAT64) ULONG_MAX < StepNumber)
        {
          CCopasiMessage(CCopasiMessage::WARNING,
                         MCTrajectoryProblem + 2, StepNumber);

          StepNumber = (C_FLOAT64) ULONG_MAX;
          StepSize = Tmp / StepNumber;
        }

      /* Assure that the step size has the appropriate sign. */
      StepSize = (Tmp < 0.0) ? - fabs(StepSize) : fabs(StepSize);
    }

  *mpStepSize = StepSize;
  *mpStepNumber = (unsigned C_INT32) StepNumber;

  return success;
}

void CTrajectoryProblem::setStartInSteadyState(bool flag)
{
  *mpStartInSteadyState = flag;
}

bool CTrajectoryProblem::getStartInSteadyState() const
{
  if (mpStartInSteadyState)
    return *mpStartInSteadyState;
  else
    return false;
}
