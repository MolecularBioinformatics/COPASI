// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2014 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * UndoCompartmentData.cpp
 *
 *  Created on: 15 Sep 2014
 *      Author: dada
 */

#include <QtCore/QList>

#include "copasi/copasi.h"

#include "copasi/model/CModel.h"
#include "copasi/model/CMetab.h"
#include "copasi/model/CCompartment.h"
#include "copasi/core/CRootContainer.h"
#include "copasi/function/CExpression.h"

#include "CCopasiUndoCommand.h"
#include "UndoData.h"
#include "UndoDependentData.h"
#include "UndoCompartmentData.h"

UndoCompartmentData::UndoCompartmentData()
  : UndoData()
  , mInitialValue()
  , mInitialExpression()
  , mExpression()
  , mStatus()
{
}

UndoCompartmentData::UndoCompartmentData(const CCompartment *compartment,
    bool trackDependencies /*= true*/)
  : UndoData(compartment->getKey(), compartment->getObjectName())
  , mInitialValue(compartment->getInitialValue())
  , mInitialExpression(compartment->getInitialExpression())
  , mExpression(compartment->getExpression())
  , mStatus(compartment->getStatus())
{
  if (trackDependencies)
    mpData->initializeFrom(compartment);
}

UndoCompartmentData::~UndoCompartmentData()
{
}

CDataObject *
UndoCompartmentData::createObjectIn(CModel *pModel)
{
  if (pModel == NULL) return NULL;

  CCompartment *pCompartment = pModel->createCompartment(mName);

  if (pCompartment == NULL)
    return NULL;

  createDependentObjects(pModel);

  mKey = pCompartment->getKey();

  return pCompartment;
}

CDataObject *
UndoCompartmentData::restoreObjectIn(CModel *pModel)
{
  CDataObject  *pCompartment = createObjectIn(pModel);

  if (pCompartment == NULL)
    return NULL;

  fillObject(pModel);
  fillDependentObjects(pModel);

  return pCompartment;
}

void UndoCompartmentData::fillObject(CModel *)
{
  CCompartment* pCompartment = dynamic_cast<CCompartment*>(
                                 CRootContainer::getKeyFactory()->get(mKey));

  if (pCompartment == NULL) return;

  pCompartment->setInitialValue(getInitialValue());
  pCompartment->setStatus(getStatus());

  if (getStatus() == CModelEntity::Status::ODE || getStatus() == CModelEntity::Status::ASSIGNMENT)
    {
      pCompartment->setExpression(getExpression());
    }

  // set initial expression
  if (getStatus() != CModelEntity::Status::ASSIGNMENT)
    {
      pCompartment->setInitialExpression(getInitialExpression());
    }
}

CModelEntity::Status
UndoCompartmentData::getStatus() const
{
  return mStatus;
}

void
UndoCompartmentData::setStatus(CModelEntity::Status status)
{
  mStatus = status;
}

double
UndoCompartmentData::getInitialValue() const
{
  return mInitialValue;
}

void
UndoCompartmentData::setInitialValue(double initialValue)
{
  mInitialValue = initialValue;
}

const std::string&
UndoCompartmentData::getInitialExpression() const
{
  return mInitialExpression;
}

void UndoCompartmentData::setInitialExpression(const std::string &initialExpression)
{
  mInitialExpression = initialExpression;
}

const std::string& UndoCompartmentData::getExpression() const
{
  return mExpression;
}

void UndoCompartmentData::setExpression(const std::string &expression)
{
  mExpression = expression;
}
