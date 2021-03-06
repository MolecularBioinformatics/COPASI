// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2014 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * InsertGlobalQuantityRowsCommand.cpp
 *
 *  Created on: 11 Sep 2014
 *      Author: dada
 */

#include "CopasiDataModel/CDataModel.h"
#include "copasi/core/CRootContainer.h"
#include "model/CModelValue.h"
#include "model/CModel.h"
#include "CQGlobalQuantityDM.h"

#include "InsertGlobalQuantityRowsCommand.h"
#include "UndoGlobalQuantityData.h"

#include "InsertGlobalQuantityRowsCommand.h"

InsertGlobalQuantityRowsCommand::InsertGlobalQuantityRowsCommand(int position, int rows, CQGlobalQuantityDM *pGlobalQuantityDM)
  : CCopasiUndoCommand("Global Quantity", GLOBALQUANTITY_INSERT)
  , mpGlobalQuantityDM(pGlobalQuantityDM)
  , mRows(rows)
  , mPosition(position)
  , mIndex()
  , mpGlobalQuantityData(NULL)
  , mValue()
{
  setText(QObject::tr(": Inserted new global quantity"));
}

InsertGlobalQuantityRowsCommand::InsertGlobalQuantityRowsCommand(int position, int rows, CQGlobalQuantityDM *pGlobalQuantityDM, const QModelIndex& index, const QVariant& value)
  : CCopasiUndoCommand("Global Quantity", GLOBALQUANTITY_INSERT)
  , mpGlobalQuantityDM(pGlobalQuantityDM)
  , mRows(rows)
  , mPosition(position)
  , mIndex(index)
  , mpGlobalQuantityData(NULL)
  , mValue(value)
{
  setText(QObject::tr(": Inserted new global quantity"));
}

void InsertGlobalQuantityRowsCommand::redo()
{
  if (mpGlobalQuantityData == NULL)
    {
      mpGlobalQuantityDM->insertNewGlobalQuantityRow(mPosition, mRows, mIndex, mValue);

      assert(mpGlobalQuantityDM->getDataModel() != NULL);
      CModel * pModel = mpGlobalQuantityDM->getDataModel()->getModel();
      assert(pModel != NULL);

      //create always adds element at the end
      CModelValue *pGlobalQuantity = &pModel->getModelValues()[pModel->getModelValues().size() - 1];
      mpGlobalQuantityData = new UndoGlobalQuantityData(pGlobalQuantity);
    }
  else
    {
      mpGlobalQuantityDM->addGlobalQuantityRow(mpGlobalQuantityData);
    }

  setUndoState(true);
  setAction("Add to list");
  setName(mpGlobalQuantityData->getName());
}

void InsertGlobalQuantityRowsCommand::undo()
{
  mpGlobalQuantityDM->deleteGlobalQuantityRow(mpGlobalQuantityData);
  setUndoState(false);
  setAction("Remove from list");
}

InsertGlobalQuantityRowsCommand::~InsertGlobalQuantityRowsCommand()
{
  pdelete(mpGlobalQuantityData);
}
