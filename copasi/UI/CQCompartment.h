// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQCompartment.h,v $
//   $Revision: 1.13 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2009/08/07 14:12:33 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#ifndef CQCOMPARTMENT_H
#define CQCOMPARTMENT_H

#include <qvariant.h>

#include "ui_CQCompartment.h"

class CQExpressionWidget;
class CCompartment;
class CExpression;

class CQCompartment : public CopasiWidget, public Ui::CQCompartment
{
  Q_OBJECT

public:
  CQCompartment(QWidget* parent = 0, const char* name = 0);
  ~CQCompartment();

  virtual bool leave();
  virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key);

protected:
  virtual bool enterProtected();

protected slots:
  virtual void languageChange();

private:
  std::vector< int > mItemToType;
  CCompartment * mpCompartment;
  bool mChanged;
  bool mExpressionValid;
  bool mInitialExpressionValid;

  void init();
  void load();
  void save();
  void destroy();
  void loadMetaboliteTable();

private slots:
  void slotBtnCommit();
  void slotBtnRevert();
  void slotBtnNew();
  void slotBtnDelete();
  void slotTypeChanged(int type);
  void slotInitialTypeChanged(bool useInitialAssignment);
  void slotNameLostFocus();
  void slotExpressionValid(bool valid);
  void slotInitialExpressionValid(bool valid);
  void slotSwitchToSpecies(int row, int column);
};

#endif // CQCOMPARTMENT_H
