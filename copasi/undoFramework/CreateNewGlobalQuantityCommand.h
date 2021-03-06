// Copyright (C) 2014 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * CreateNewGlobalQuantityCommand.h
 *
 *  Created on: 11 Sep 2014
 *      Author: dada
 */

#ifndef CREATENEWGLOBALQUANTITYCOMMAND_H_
#define CREATENEWGLOBALQUANTITYCOMMAND_H_

#include "CCopasiUndoCommand.h"

class CQModelValue;
class UndoGlobalQuantityData;

class CreateNewGlobalQuantityCommand: public CCopasiUndoCommand
{
public:
  CreateNewGlobalQuantityCommand(CQModelValue *pModelValue);
  virtual ~CreateNewGlobalQuantityCommand();

  void redo();
  void undo();

private:
  UndoGlobalQuantityData *mpGlobalQuantityData;
  CQModelValue* mpModelValue;
};

#endif /* CREATENEWGLOBALQUANTITYCOMMAND_H_ */
