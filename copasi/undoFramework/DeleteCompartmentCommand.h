// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2014 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * DeleteCompartmentCommand.h
 *
 *  Created on: 16 Sep 2014
 *      Author: dada
 */

#ifndef DELETECOMPARTMENTCOMMAND_H_
#define DELETECOMPARTMENTCOMMAND_H_

#include "CCopasiUndoCommand.h"

class UndoCompartmentData;
class CQCompartment;

class DeleteCompartmentCommand: public CCopasiUndoCommand
{
public:
  DeleteCompartmentCommand(CQCompartment *pCompartment);
  virtual ~DeleteCompartmentCommand();

  void redo();
  void undo();

private:
  UndoCompartmentData *mpCompartmentData;
  CQCompartment* mpCompartment;
};

#endif /* DELETECOMPARTMENTCOMMAND_H_ */
