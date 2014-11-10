// Copyright (C) 2014 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * RemoveGlobalQuantityRowsCommand.cpp
 *
 *  Created on: 11 Sep 2014
 *      Author: dada
 */

#include "report/CCopasiRootContainer.h"
#include "model/CModelValue.h"
#include "model/CReactionInterface.h"
#include "model/CModel.h"
#include "CQGlobalQuantityDM.h"
#include "function/CFunctionDB.h"

#include "UndoGlobalQuantityData.h"
#include "UndoReactionData.h"
#include "RemoveGlobalQuantityRowsCommand.h"

RemoveGlobalQuantityRowsCommand::RemoveGlobalQuantityRowsCommand(QModelIndexList rows, CQGlobalQuantityDM * pGlobalQuantityDM, const QModelIndex&)
{
  mpGlobalQuantityDM = pGlobalQuantityDM;
  mRows = rows;
  mFirstTime = true;

  assert(CCopasiRootContainer::getDatamodelList()->size() > 0);
  CCopasiDataModel* pDataModel = (*CCopasiRootContainer::getDatamodelList())[0];
  assert(pDataModel != NULL);
  CModel * pModel = pDataModel->getModel();

  assert(pModel != NULL);

  QModelIndexList::const_iterator i;

  for (i = rows.begin(); i != rows.end(); ++i)
    {
      UndoGlobalQuantityData *data = new UndoGlobalQuantityData();

      if (!pGlobalQuantityDM->isDefaultRow(*i) && pModel->getModelValues()[(*i).row()])
        {
          data->setName(pModel->getModelValues()[(*i).row()]->getObjectName());
          data->setInitialValue(pModel->getModelValues()[(*i).row()]->getInitialValue());
          data->setStatus(pModel->getModelValues()[(*i).row()]->getStatus());

          setDependentObjects(pModel->getModelValues()[(*i).row()]->getDeletedObjects());
          data->setReactionDependencyObjects(getReactionData());

          mpGlobalQuantityData.append(data);
        }
    }

  this->setText(removeGlobalQuantityRowsText());
}

void RemoveGlobalQuantityRowsCommand::redo()
{
  if (mFirstTime)
    {
      mpGlobalQuantityDM->removeGlobalQuantityRows(mRows, QModelIndex());
      mFirstTime = false;
    }
  else
    {
      mpGlobalQuantityDM->deleteGlobalQuantityRows(mpGlobalQuantityData);
    }
}

void RemoveGlobalQuantityRowsCommand::undo()
{
  mpGlobalQuantityDM->insertGlobalQuantityRows(mpGlobalQuantityData);
}

QString RemoveGlobalQuantityRowsCommand::removeGlobalQuantityRowsText() const
{
  return QObject::tr(": Removed Global Quantity");
}

RemoveGlobalQuantityRowsCommand::~RemoveGlobalQuantityRowsCommand()
{
  // TODO Auto-generated destructor stub
}
