// Copyright (C) 2014 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

/*
 * DeleteReactionCommand.h
 *
 *  Created on: 4 Jul 2014
 *      Author: dada
 */

#ifndef DELETEREACTIONCOMMAND_H_
#define DELETEREACTIONCOMMAND_H_

#include "CCopasiUndoCommand.h"

class CReaction;
class ReactionsWidget1;
class UndoReactionData;

class DeleteReactionCommand: public CCopasiUndoCommand
{
public:
  DeleteReactionCommand(ReactionsWidget1 *pReactionWidget);
  virtual ~DeleteReactionCommand();

  void redo();
  void undo();

private:
  std::string mReaObjectName;
  CReactionInterface *mpRi;
  CReaction *mpReaction;
  UndoReactionData *mpReactionData;
  ReactionsWidget1* mpReactionWidget;
};

#endif /* DELETEREACTIONCOMMAND_H_ */
