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

#ifndef SCANWIDGET_H
#define SCANWIDGET_H

#include "copasi.h"
#include "TaskWidget.h"

#include <QGridLayout>
#include <QHBoxLayout>

class QPushButton;
class QComboBox;

class CScanContainerWidget;

class ScanWidget : public TaskWidget
{
  Q_OBJECT

public:
  ScanWidget(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = 0);
  ~ScanWidget();

  virtual bool runTask();

protected:

  virtual bool loadTask();

  virtual bool saveTask();

  //dummy, should not be used in this widget
  virtual CCopasiMethod * createMethod(const CTaskEnum::Method & /*type*/);

  CScanContainerWidget* scrollview;

  QPushButton* buttonNewItem;
  QComboBox* comboType;

  QGridLayout* ScanWidgetLayout;

protected slots:
  bool slotAddItem();
};

#endif
