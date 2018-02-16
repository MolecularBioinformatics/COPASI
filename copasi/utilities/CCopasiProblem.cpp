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
 *  CCopasiProblem class.
 *  This class is used to describe a task in COPASI. This class is
 *  intended to be used as the parent class for all tasks within COPASI.
 *
 *  Created for COPASI by Stefan Hoops 2003
 */

#include "copasi.h"

#include "CCopasiProblem.h"

#include "copasi/model/CMetab.h"
#include "copasi/model/CModel.h"
#include "copasi/CopasiDataModel/CDataModel.h"
#include "copasi/core/CDataVector.h"
#include "copasi/math/CMathContainer.h"

CCopasiProblem::CCopasiProblem():
  CCopasiParameterGroup("NoName", NULL, "Problem"),
  mType(CTaskEnum::Task::UnsetTask),
  mpContainer(NULL),
  mpCallBack(NULL),
  mpReport(NULL)
{}

CCopasiProblem::CCopasiProblem(const CTaskEnum::Task & type,
                               const CDataContainer * pParent):
  CCopasiParameterGroup(CTaskEnum::TaskName[type], pParent, "Problem"),
  mType(type),
  mpContainer(NULL),
  mpCallBack(NULL),
  mpReport(NULL)
{
  const CCopasiTask * pTask = dynamic_cast< const CCopasiTask * >(pParent);

  if (pTask != NULL)
    {
      setMathContainer(pTask->getMathContainer());
    }
}

CCopasiProblem::CCopasiProblem(const CCopasiProblem & src,
                               const CDataContainer * pParent):
  CCopasiParameterGroup(src, pParent),
  mType(src.mType),
  mpContainer(src.mpContainer),
  mpCallBack(src.mpCallBack),
  mpReport(src.mpReport)
{
  const CCopasiTask * pTask = dynamic_cast< const CCopasiTask * >(pParent);

  if (pTask != NULL)
    {
      setMathContainer(pTask->getMathContainer());
    }
}

CCopasiProblem::~CCopasiProblem() {}

const CTaskEnum::Task & CCopasiProblem::getType() const {return mType;}

void CCopasiProblem::setMathContainer(CMathContainer * pContainer)
{
  if (pContainer != mpContainer)
    {
      mpContainer = pContainer;
      signalMathContainerChanged();
    }
}

// virtual
void CCopasiProblem::signalMathContainerChanged()
{}

CMathContainer * CCopasiProblem::getMathContainer() const
{
  return mpContainer;
}

bool CCopasiProblem::setCallBack(CProcessReport * pCallBack)
{
  mpCallBack = pCallBack;
  return true;
}

CProcessReport * CCopasiProblem::getCallBack() const
{
  return mpCallBack;
}

void CCopasiProblem::clearCallBack()
{
  setCallBack(NULL);
}

// probably for optimization only

bool CCopasiProblem::initialize() {return true;}

bool CCopasiProblem::restore(const bool & /* updateModel */) {return true;}

void CCopasiProblem::print(std::ostream * ostream) const
{*ostream << *this;}

void CCopasiProblem::printResult(std::ostream * ostream) const
{*ostream << "Not implemented.";}

std::ostream &operator<<(std::ostream &os, const CCopasiProblem & o)
{
  os << "Problem Description:" << std::endl;

  CCopasiParameterGroup::elements::const_iterator it = o.beginIndex();
  CCopasiParameterGroup::elements::const_iterator end = o.endIndex();

  for (; it != end; ++it)
    {
      (*it)->print(&os);
      os << std::endl;
    }

  return os;
}

bool
CCopasiProblem::setModel(CModel* model)
{
  if (model == NULL) return false;

  setMathContainer(&model->getMathContainer());
  return true;
}
