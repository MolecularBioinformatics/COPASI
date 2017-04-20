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

// Copyright (C) 2005 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "DefaultplotDialog.h"

#include "qtUtilities.h"

#include "utilities/CCopasiTask.h"
#include "report/COutputAssistant.h"
#include "listviews.h"

/*
 *  Constructs a DefaultPlotDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
DefaultPlotDialog::DefaultPlotDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
  : QDialog(parent, fl)
{
  setObjectName(QString::fromUtf8(name));
  setModal(modal);
  setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DefaultPlotDialog::~DefaultPlotDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

void DefaultPlotDialog::slotCreate()
{
  if (!mpTask) return;

  CDataModel* pDataModel = ListViews::dataModel(parent());
  assert(pDataModel != NULL);

  CDataObject* result = COutputAssistant::createDefaultOutput(mIndex, mpTask, pDataModel);

  if (result != NULL)
    {

      if (mIndex == 911 // "Plots of Parameter Estimation Results per Experiment";
          || mIndex == 912 // "Plots of Parameter Estimation Results per Dependent Value"
         )
        {
          // don't rename these outputs as multiple ones will have been created
          this->accept();
          return;
        }

      QString current = lineEditTitle->text();
      int count = 0;

      while (!result->setObjectName(TO_UTF8(current)))
        current = QString("%1 %2").arg(lineEditTitle->text()).arg(++count);

      this->accept();
    }
  else
    this->reject();
}

void DefaultPlotDialog::setTask(CCopasiTask * t)
{
  mpTask = t;

  fillList();
}

void DefaultPlotDialog::fillList()
{
  if (mpCheckAll->isChecked())
    mList = COutputAssistant::getListOfDefaultOutputDescriptions(NULL); //this gets the complete list
  else
    mList = COutputAssistant::getListOfDefaultOutputDescriptions(mpTask); //only the items fitting the task

  listBox->clear();

  std::vector<C_INT32>::const_iterator it, itEnd = mList.end();

  QStringList Items;

  for (it = mList.begin(); it != itEnd; ++it)
    {
      QString Name = FROM_UTF8(COutputAssistant::getItemName(*it));

      if (!Name.startsWith("--")) Name = "   " + Name;

      Items.append(Name);
    }

  listBox->insertItems(0, Items);

  listBox->item(0)->setSelected(true);
}

void DefaultPlotDialog::slotSelect()
{
  if (!mpTask) return;

  int i = listBox->currentRow();

  if (i >= 0)
    {
      mIndex = mList[i];
      lineEditTitle->setText(FROM_UTF8(COutputAssistant::getItemName(mIndex)));
      textEdit->setText(FROM_UTF8(COutputAssistant::getItem(mIndex).description));

      createButton->setEnabled(!lineEditTitle->text().startsWith("-- "));
    }
  else //Listbox is empty, so there is no current row (-1)
    {
      mIndex = 0;
      lineEditTitle->setText("");
      textEdit->setText("");

      createButton->setEnabled(false);
    }
}

//virtual
void DefaultPlotDialog::slotToggleAll(bool /*flag*/)
{
  fillList();
}
