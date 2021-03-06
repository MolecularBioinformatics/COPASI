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

#include "CQCompartmentsWidget.h"

#include <QHeaderView>
#include <QClipboard>
#include <QKeyEvent>

#include "qtUtilities.h"
#include "copasi.h"
#include "CQMessageBox.h"

#include "model/CModel.h"
#include "CopasiDataModel/CDataModel.h"
#include "copasi/core/CRootContainer.h"

#include "copasiui3window.h"

/*
 *  Constructs a CQCompartmentsWidget which is a child of 'parent', with the
 *  name 'name'.'
 */
CQCompartmentsWidget::CQCompartmentsWidget(QWidget *parent, const char *name)
  : CopasiWidget(parent, name)
{
  setupUi(this);
  //Create Source Data Model.
  mpCompartmentDM = new CQCompartmentDM(this);
  mpCompartmentDM->setDataModel(mpDataModel);
  //Create the Proxy Model for sorting/filtering and set its properties.
  mpProxyModel = new CQSortFilterProxyModel();
  mpProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
  mpProxyModel->setFilterKeyColumn(-1);
  //Setting values for Types comboBox
  mpTypeDelegate = new CQIndexComboDelegate(this, mpCompartmentDM->getTypes());
  mpTblCompartments->setItemDelegateForColumn(COL_TYPE_COMPARTMENTS, mpTypeDelegate);
#if QT_VERSION >= 0x050000
  mpTblCompartments->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
  mpTblCompartments->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
  mpTblCompartments->verticalHeader()->hide();
  mpTblCompartments->sortByColumn(COL_ROW_NUMBER, Qt::AscendingOrder);
  // Connect the table widget
  connect(mpCompartmentDM, SIGNAL(notifyGUI(ListViews::ObjectType, ListViews::Action, const std::string)),
          this, SLOT(protectedNotify(ListViews::ObjectType, ListViews::Action, const std::string)));
  connect(mpCompartmentDM, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
  connect(mpLEFilter, SIGNAL(textChanged(const QString &)),
          this, SLOT(slotFilterChanged()));
  CopasiUI3Window   *pWindow = dynamic_cast<CopasiUI3Window * >(parent->parent());
  mpCompartmentDM->setUndoStack(pWindow->getUndoStack());
}

/*
 *  Destroys the object and frees any allocated resources
 */
CQCompartmentsWidget::~CQCompartmentsWidget()
{
  pdelete(mpTypeDelegate);
  pdelete(mpProxyModel);
  pdelete(mpCompartmentDM);
  // no need to delete child widgets, Qt does it all for us
}

void CQCompartmentsWidget::slotBtnNewClicked()
{
  mpCompartmentDM->insertRow(mpCompartmentDM->rowCount() - 1, QModelIndex());
  updateDeleteBtns();
}

void CQCompartmentsWidget::slotBtnDeleteClicked()
{
  if (mpTblCompartments->hasFocus())
    {deleteSelectedCompartments();}

  updateDeleteBtns();
}

void CQCompartmentsWidget::deleteSelectedCompartments()
{
  const QItemSelectionModel *pSelectionModel = mpTblCompartments->selectionModel();
  QModelIndexList mappedSelRows;
  size_t i, imax = mpCompartmentDM->rowCount();

  for (i = 0; i < imax; i++)
    {
      if (pSelectionModel->isRowSelected((int) i, QModelIndex()))
        {
          mappedSelRows.append(mpProxyModel->mapToSource(mpProxyModel->index((int) i, 0)));
        }
    }

  if (mappedSelRows.empty())
    {return;}

  mpCompartmentDM->removeRows(mappedSelRows);
}

void CQCompartmentsWidget::slotBtnClearClicked()
{
  int ret = CQMessageBox::question(this, tr("Confirm Delete"), "Delete all Compartments?",
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (ret == QMessageBox::Yes)
    {
      mpCompartmentDM->clear();
    }

  updateDeleteBtns();
}

bool CQCompartmentsWidget::update(ListViews::ObjectType objectType, ListViews::Action C_UNUSED(action), const std::string &C_UNUSED(key))
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

bool CQCompartmentsWidget::leave()
{
  return true;
}

CQBaseDataModel * CQCompartmentsWidget::getCqDataModel()
{
  return mpCompartmentDM;
}

bool CQCompartmentsWidget::enterProtected()
{
  if (mpTblCompartments->selectionModel() != NULL)
    {
      disconnect(mpTblCompartments->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                 this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &)));
    }

  mpCompartmentDM->setDataModel(mpDataModel);
  mpProxyModel->setSourceModel(mpCompartmentDM);
  mpTblCompartments->setModel(NULL);
  mpTblCompartments->setModel(mpProxyModel);
  mpTblCompartments->resizeColumnsToContents();
  connect(mpTblCompartments->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(slotSelectionChanged(const QItemSelection &, const QItemSelection &)));
  updateDeleteBtns();
  return true;
}

void CQCompartmentsWidget::updateDeleteBtns()
{
  bool selected = false;
  QModelIndexList selRows = mpTblCompartments->selectionModel()->selectedRows();

  if (selRows.size() == 0)
    selected = false;
  else
    {
      if (selRows.size() == 1)
        {
          if (mpCompartmentDM->isDefaultRow(mpProxyModel->mapToSource(selRows[0])))
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

void CQCompartmentsWidget::slotSelectionChanged(const QItemSelection &C_UNUSED(selected),
    const QItemSelection &C_UNUSED(deselected))
{
  updateDeleteBtns();
}

void CQCompartmentsWidget::dataChanged(const QModelIndex &C_UNUSED(topLeft),
                                       const QModelIndex &C_UNUSED(bottomRight))
{
  mpTblCompartments->resizeColumnsToContents();
  updateDeleteBtns();
}

void CQCompartmentsWidget::slotDoubleClicked(const QModelIndex proxyIndex)
{
  QModelIndex index = mpProxyModel->mapToSource(proxyIndex);

  if (index.row() < 0)
    return;

  if (mpCompartmentDM->isDefaultRow(index))
    {
      slotBtnNewClicked();
    }

  assert(mpDataModel != NULL); // Is this necessary?
  CModel *pModel = mpDataModel->getModel();

  if (pModel == NULL)
    return;

  std::string key = pModel->getCompartments()[index.row()].getKey();

  if (CRootContainer::getKeyFactory()->get(key))
    mpListView->switchToOtherWidget(C_INVALID_INDEX, key);
}

void CQCompartmentsWidget::keyPressEvent(QKeyEvent *ev)
{
  if (ev->key() == Qt::Key_Delete)
    slotBtnDeleteClicked();
  else if (ev->key() == Qt::Key_C && (ev->modifiers() & Qt::ControlModifier))
    {
      QModelIndexList selRows = mpTblCompartments->selectionModel()->selectedRows(0);

      if (selRows.empty())
        {return;}

      QString str;
      QModelIndexList::const_iterator i;

      for (i = selRows.begin(); i != selRows.end(); ++i)
        {
          for (int x = 0; x < mpCompartmentDM->columnCount(); ++x)
            {
              if (!mpTblCompartments->isColumnHidden(x))
                {
                  if (!str.isEmpty())
                    str += "\t";

                  str += mpCompartmentDM->index(mpProxyModel->mapToSource(*i).row(), x).data().toString();
                }
            }

          str += "\n";
        }

      QApplication::clipboard()->setText(str);
    }
}

void CQCompartmentsWidget::slotFilterChanged()
{
  QRegExp regExp(mpLEFilter->text() + "|New Compartment", Qt::CaseInsensitive, QRegExp::RegExp);
  mpProxyModel->setFilterRegExp(regExp);
}
