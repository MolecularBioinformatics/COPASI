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

/********************************************************
 Author: Liang Xu
 Version : 1.xx  <first>
 Description:
 Date: 04/03
 Comment : CopasiWidget: Provide base class for all widget: for resizable/minimize
 Contact: Please contact lixu1@vt.edu.
 *********************************************************/

#ifndef COPASI_WIDGET_H
#define COPASI_WIDGET_H

#include <QWidget>

#include "copasi/UI/listviews.h"

class DataModelGUI;
class QResizeEvent;
class ListViews;
class CDataModel;
class CQBaseDataModel;
class CDataObject;
class QUndoStack;

class CopasiWidget : public QWidget
{
  Q_OBJECT

public:
  CopasiWidget(QWidget *parent = 0, const char *name = 0, Qt::WindowFlags = 0);
  virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string &key);
  virtual bool leave();

  /**
   * leaves the widget and enters it again
   */
  virtual void refresh();

  bool enter(const std::string &key);
  virtual void setFramework(int framework);
  bool getIgnoreUpdates();
  void setIgnoreUpdates(bool v);
  CDataModel *getDataModel() const;

  void setUndoStack(QUndoStack *undoStack);
  QUndoStack *getUndoStack();

  /**
   * @return the current key of the widget
   */
  const std::string& getKey() const;

  /**
   * @return the pointer to the current object
   */
  const CDataObject* getObject() const;

  /**
   * This method returns the data model from all the overview widgets.
   * @return data model of this widget (or NULL if it does not have one)
   */
  virtual CQBaseDataModel* getCqDataModel();

protected:
  virtual bool enterProtected();

  QUndoStack *mpUndoStack;

  ListViews *mpListView;
  std::string mKey;
  CDataObject *mpObject;
  CDataModel *mpDataModel;

  bool mIgnoreUpdates;
  int mFramework;

protected slots:
  virtual bool protectedNotify(ListViews::ObjectType objectType, ListViews::Action action, const std::string &key = "");
};

#endif // !defined(COPASI_WIDGET_H)
