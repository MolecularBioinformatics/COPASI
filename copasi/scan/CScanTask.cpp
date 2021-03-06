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

// Copyright (C) 2003 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 * CScanTask class.
 *
 * This class implements a scan task which is comprised of a
 * of a problem and a method.
 *
 */
#include "copasi.h"
#include "CScanTask.h"
#include "CScanProblem.h"
#include "CScanMethod.h"
#include "utilities/CReadConfig.h"
#include "report/CKeyFactory.h"
#include "report/CReport.h"

#include "math/CMathContainer.h"
#include "optimization/COptProblem.h"

#include "trajectory/CTrajectoryTask.h"
#include "trajectory/CTrajectoryProblem.h"
#include "steadystate/CSteadyStateTask.h"
#include "steadystate/CSteadyStateProblem.h"
#include "lna/CLNAProblem.h"
#include "lna/CLNATask.h"
#include "output/COutputHandler.h"
#include "utilities/CProcessReport.h"
#include "CopasiDataModel/CDataModel.h"
#include "copasi/core/CRootContainer.h"
#include "crosssection/CCrossSectionTask.h"

CScanTask::CScanTask(const CDataContainer * pParent,
                     const CTaskEnum::Task & type):
  CCopasiTask(pParent, type),
  mProgress(0),
  mhProgress(C_INVALID_INDEX),
  mpSubtask(NULL),
  mOutputInSubtask(false),
  mUseInitialValues(true)
{
  mpProblem = new CScanProblem(this);
  mpMethod = createMethod(CTaskEnum::Method::scanMethod);
  static_cast< CScanMethod * >(mpMethod)->setProblem(static_cast< CScanProblem * >(mpProblem));
}

CScanTask::CScanTask(const CScanTask & src,
                     const CDataContainer * pParent):
  CCopasiTask(src, pParent),
  mProgress(0),
  mhProgress(C_INVALID_INDEX),
  mpSubtask(NULL),
  mOutputInSubtask(false),
  mUseInitialValues(true)
{
  mpProblem = new CScanProblem(*(CScanProblem *) src.mpProblem, this);
  mpMethod = createMethod(CTaskEnum::Method::scanMethod);
  static_cast< CScanMethod * >(mpMethod)->setProblem(static_cast< CScanProblem * >(mpProblem));
}

CScanTask::~CScanTask()
{cleanup();}

void CScanTask::cleanup()
{}

bool CScanTask::initialize(const OutputFlag & of,
                           COutputHandler * pOutputHandler,
                           std::ostream * pOstream)
{
  assert(mpProblem && mpMethod);

  bool success = mpMethod->isValidProblem(mpProblem);

  if ((of & REPORT) &&
      pOutputHandler != NULL)
    {
      if (mReport.open(getObjectDataModel(), pOstream))
        pOutputHandler->addInterface(&mReport);
      else
        CCopasiMessage(CCopasiMessage::COMMANDLINE, MCCopasiTask + 5, getObjectName().c_str());
    }

  success &= initSubtask(of, pOutputHandler, mReport.getStream());
  success &= CCopasiTask::initialize(of, pOutputHandler, mReport.getStream());

  return success;
}

void CScanTask::load(CReadConfig & C_UNUSED(configBuffer))
{}

bool CScanTask::process(const bool & useInitialValues)
{
  if (!mpProblem) fatalError();

  if (!mpMethod) fatalError();

  CScanProblem * pProblem = dynamic_cast<CScanProblem *>(mpProblem);

  if (!pProblem) fatalError();

  CScanMethod * pMethod = dynamic_cast<CScanMethod *>(mpMethod);

  if (!pMethod) fatalError();

  bool success = true;

  CCrossSectionTask* task = dynamic_cast<CCrossSectionTask*>(mpSubtask);

  if (task != NULL)
    task->createEvent();

  if (useInitialValues)
    {
      mpContainer->applyInitialValues();
    }

  //TODO: reports

  // initialize the method (parsing the ScanItems)
  pMethod->setProblem(pProblem);

  if (!pMethod->init()) return false;

  // init progress bar
  mProgress = 0;

  if (mpCallBack)
    {
      mpCallBack->setName("performing parameter scan...");

      unsigned C_INT32 totalSteps = (unsigned C_INT32) pMethod->getTotalNumberOfSteps();
      mhProgress = mpCallBack->addItem("Number of Steps",
                                       mProgress,
                                       &totalSteps);

      if (mpSubtask)
        mpSubtask->setCallBack(mpCallBack);
    }

  // init output handler (plotting)
  output(COutputInterface::BEFORE);

  //calling the scanner, output is done in the callback
  if (!pMethod->scan()) success = false;

  if (task != NULL)
    task->removeEvent();

  //finishing progress bar and output
  //if (mpCallBack) mpCallBack->finish();
  //if (mpOutputHandler) mpOutputHandler->finish();
  output(COutputInterface::AFTER);

  if (mpSubtask)
    mpSubtask->setCallBack(NULL);

  return success;
}

// virtual
const CTaskEnum::Method * CScanTask::getValidMethods() const
{
  static const CTaskEnum::Method ValidMethods[] =
  {
    CTaskEnum::Method::scanMethod,
    CTaskEnum::Method::UnsetMethod
  };

  return ValidMethods;
}

bool CScanTask::processCallback()
{
  bool success = mpSubtask->process(mUseInitialValues);

  //do output
  if (success && !mOutputInSubtask)
    output(COutputInterface::DURING);

  if (mpSubtask->isUpdateModel())
    {
      COptProblem* problem = dynamic_cast<COptProblem*>(mpSubtask->getProblem());

      if (problem != NULL)
        {
          problem->restore(true);
        }
    }

  //do progress bar
  ++mProgress;

  if (mpCallBack) return mpCallBack->progressItem(mhProgress);

  return true;
}

bool CScanTask::outputSeparatorCallback(bool isLast)
{
  if ((!isLast) || mOutputInSubtask)
    separate(COutputInterface::DURING);

  return true;
}

bool CScanTask::initSubtask(const OutputFlag & /* of */,
                            COutputHandler * pOutputHandler,
                            std::ostream * pOstream)
{
  if (!mpProblem) fatalError();

  CScanProblem * pProblem = dynamic_cast<CScanProblem *>(mpProblem);

  if (!pProblem) fatalError();

  //get the parameters from the problem
  CTaskEnum::Task type = (CTaskEnum::Task) pProblem->getValue< unsigned C_INT32 >("Subtask");
  CDataModel* pDataModel = getObjectDataModel();
  assert(pDataModel != NULL);

  switch (type)
    {
      case CTaskEnum::Task::steadyState:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Steady-State"));
        break;

      case CTaskEnum::Task::timeCourse:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Time-Course"));
        break;

      case CTaskEnum::Task::mca:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Metabolic Control Analysis"));
        break;

      case CTaskEnum::Task::lyap:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Lyapunov Exponents"));
        break;

      case CTaskEnum::Task::optimization:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Optimization"));
        break;

      case CTaskEnum::Task::parameterFitting:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Parameter Estimation"));
        break;

      case CTaskEnum::Task::sens:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Sensitivities"));
        break;

      case CTaskEnum::Task::lna:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Linear Noise Approximation"));
        break;

      case CTaskEnum::Task::tssAnalysis :
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[](CTaskEnum::TaskName[CTaskEnum::Task::tssAnalysis]));
        break;

      case CTaskEnum::Task::crosssection:
        mpSubtask = dynamic_cast<CCopasiTask*>
                    (&pDataModel->getTaskList()->operator[]("Cross Section"));
        break;

      default:
        mpSubtask = NULL;
    }

  mOutputInSubtask = pProblem->getValue< bool >("Output in subtask");
  mUseInitialValues = !pProblem->getContinueFromCurrentState();

  if (!mpSubtask) return false;

  mpSubtask->setMathContainer(mpContainer); //TODO
  mpSubtask->setCallBack(NULL);

  if (mOutputInSubtask)
    return mpSubtask->initialize(OUTPUT, pOutputHandler, pOstream);
  else
    return mpSubtask->initialize(NO_OUTPUT, pOutputHandler, pOstream);

  return true;
}

void CScanTask::fixBuild81()
{
  CScanProblem * pProblem = dynamic_cast< CScanProblem * >(mpProblem);

  if (pProblem == NULL) return;

  pProblem->fixBuild81();

  return;
}
