// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "CQGlobalQuantitiesWidget.h"

#include <QHeaderView>
#include <QClipboard>
#include <QKeyEvent>

#include "copasi.h"

#include "qtUtilities.h"
#include "CQMessageBox.h"

#include "model/CModel.h"
#include "CopasiDataModel/CDataModel.h"
#include "copasi/core/CRootContainer.h"

#include "copasiui3window.h"

/*
 *  Constructs a CQGlobalQuantitiesWidget which is a child of 'parent', with the
 *  name 'name'.'
 */
CQGlobalQuantitiesWidget::CQGlobalQuantitiesWidget(QWidget *parent, const char *name)
  : CopasiWidget(parent, name)
{
  setupUi(this);
  //Create Source Data Model.
  mpGlobalQuantityDM = new CQGlobalQuantityDM(this);
  //Create the Proxy Model for sorting/filtering and set its properties.
  mpProxyModel = new CQSortFilterProxyModel();
  mpProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
  mpProxyModel->setFilterKeyColumn(-1);
  //Setting values for Types comboBox
  mpTypeDelegate = new CQIndexComboDelegate(this, mpGlobalQuantityDM->getTypes());
  mpTblGlobalQuantities->setItemDelegateForColumn(COL_TYPE_GQ, mpTypeDelegate);
#if QT_VERSION >= 0x050000
  mpTblGlobalQuantities->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
  mpTblGlobalQuantities->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
  mpTblGlobalQuantities->verticalHeader()->hide();
  mpTblGlobalQuantities->sortByColumn(COL_ROW_NUMBER, Qt::AscendingOrder);
  // Connect the table widget
  connect(mpGlobalQuantityDM, SIGNAL(notifyGUI(ListViews::ObjectType, ListViews::Action, const std::string)),
          this, SLOT(protectedNotify(ListViews::ObjectType, ListViews::Action, const std::string)));
  connect(mpGlobalQuantityDM, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
  connect(mpLEFilter, SIGNAL(textChanged(const QString &)),
          this, SLOT(slotFilterChanged()));
  CopasiUI3Window   *pWindow = dynamic_cast<CopasiUI3Window * >(parent->parent());
  mpGlobalQuantityDM->setUndoStack(pWindow->getUndoStack());
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQGlobalQuantitiesWidget::~CQGlobalQuantitiesWidget()
{
  pdelete(mpTypeDelegate);
  pdelete(mpProxyModel);
  pdelete(mpGlobalQuantityDM);
  // no need to delete child widgets, Qt does it all for us
}

void CQGlobalQuantitiesWidget::slotBtnNewClicked()
{
  mpGlobalQuantityDM->insertRow(mpGlobalQuantityDM->rowCount() - 1, QModelIndex());
  updateDeleteBtns();
}

void CQGlobalQuantitiesWidget::slotBtnDeleteClicked()
{
  if (mpTblGlobalQuantities->hasFocus())
    {deleteSelectedGlobalQuantities();}

  updateDeleteBtns();
}

void CQGlobalQuantitiesWidget::deleteSelectedGlobalQuantities()
{
  const QItemSelectionModel *pSelectionModel = mpTblGlobalQuantities->selectionModel();
  QModelIndexList mappedSelRows;
  size_t i, imax = mpGlobalQuantityDM->rowCount();

  for (i = 0; i < imax; i++)
    {
      if (pSelectionModel->isRowSelected((int) i, QModelIndex()))
        {
          mappedSelRows.append(mpProxyModel->mapToSource(mpProxyModel->index((int) i, 0)));
        }
    }

  if (mappedSelRows.empty())
    {return;}

  mpGlobalQuantityDM->removeRows(mappedSelRows);
}

void CQGlobalQuantitiesWidget::slotBtnClearClicked()
{
  int ret = CQMessageBox::question(this, tr("Confirm Delete"), "Delete all Quantities?",
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (ret == QMessageBox::Yes)
    {
      mpGlobalQuantityDM->clear();
    }

  updateDeleteBtns();
}

bool CQGlobalQuantitiesWidget::update(ListViews::ObjectType objectType, ListViews::Action C_UNUSED(action), const std::string &C_UNUSED(key))
{
  if (mIgnoreUpdates || !isVisible())
    {
      return true;
    }

  if (objectType == ListViews::MODEL)
    {
      enterProtected();
    }

  return true;
}

bool CQGlobalQuantitiesWidget::leave()
{
  return true;
}

CQBaseDataModel * CQGlobalQuantitiesWidget::getCqDataModel()
{
  return mpGlobalQuantityDM;
}

bool CQGlobalQuantitiesWidget::enterProtected()
{
  if (mpTblGlobalQuantities->selectionModel() != NULL)
    {
      disconnect(mpTblGlobalQuantities->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                 this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &)));
    }

  mpGlobalQuantityDM->setDataModel(mpDataModel);
  mpProxyModel->setSourceModel(mpGlobalQuantityDM);
  //Set Model for the TableView
  mpTblGlobalQuantities->setModel(NULL);
  mpTblGlobalQuantities->setModel(mpProxyModel);
  connect(mpTblGlobalQuantities->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &)));
  updateDeleteBtns();
  mpTblGlobalQuantities->resizeColumnsToContents();
  return true;
}

void CQGlobalQuantitiesWidget::updateDeleteBtns()
{
  bool selected = false;
  QModelIndexList selRows = mpTblGlobalQuantities->selectionModel()->selectedRows();

  if (selRows.size() == 0)
    selected = false;
  else
    {
      if (selRows.size() == 1)
        {
          if (mpGlobalQuantityDM->isDefaultRow(mpProxyModel->mapToSource(selRows[0])))
            selected = false;
          else
            selected = true;
        }
      else
        selected = true;
    }

  mpBtnDelete->setEnabled(selected);

  if (mpProxyModel->rowCount() - 1)
    mpBtnClear->setEnabled(true);
  else
    mpBtnClear->setEnabled(false);
}

void CQGlobalQuantitiesWidget::slotSelectionChanged(const QItemSelection &C_UNUSED(selected),
    const QItemSelection &C_UNUSED(deselected))
{
  updateDeleteBtns();
}

void CQGlobalQuantitiesWidget::dataChanged(const QModelIndex &C_UNUSED(topLeft),
    const QModelIndex &C_UNUSED(bottomRight))
{
  mpTblGlobalQuantities->resizeColumnsToContents();
  updateDeleteBtns();
}

void CQGlobalQuantitiesWidget::slotDoubleClicked(const QModelIndex proxyIndex)
{
  QModelIndex index = mpProxyModel->mapToSource(proxyIndex);

  if (index.row() < 0)
    return;

  if (mpGlobalQuantityDM->isDefaultRow(index))
    {
      slotBtnNewClicked();
    }

  assert(mpDataModel != NULL);
  CModel *pModel = mpDataModel->getModel();

  if (pModel == NULL)
    return;

  std::string key = pModel->getModelValues()[index.row()].getKey();

  if (CRootContainer::getKeyFactory()->get(key))
    mpListView->switchToOtherWidget(C_INVALID_INDEX, key);
}

void CQGlobalQuantitiesWidget::keyPressEvent(QKeyEvent *ev)
{
  if (ev->key() == Qt::Key_Delete)
    slotBtnDeleteClicked();
  else if (ev->key() == Qt::Key_C && (ev->modifiers() & Qt::ControlModifier))
    {
      QModelIndexList selRows = mpTblGlobalQuantities->selectionModel()->selectedRows(0);

      if (selRows.empty())
        {return;}

      QString str;
      QModelIndexList::const_iterator i;

      for (i = selRows.begin(); i != selRows.end(); ++i)
        {
          for (int x = 0; x < mpGlobalQuantityDM->columnCount(); ++x)
            {
              if (!mpTblGlobalQuantities->isColumnHidden(x))
                {
                  if (!str.isEmpty())
                    str += "\t";

                  str += mpGlobalQuantityDM->index(mpProxyModel->mapToSource(*i).row(), x).data().toString();
                }
            }

          str += "\n";
        }

      QApplication::clipboard()->setText(str);
    }
}

void CQGlobalQuantitiesWidget::slotFilterChanged()
{
  QRegExp regExp(mpLEFilter->text() + "|New Quantity", Qt::CaseInsensitive, QRegExp::RegExp);
  mpProxyModel->setFilterRegExp(regExp);
}
