// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2012 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "CQPlotSubwidget.h"
#include "CQPlotEditWidget.h"

#include "curve2dwidget.h"
#include "HistoWidget.h"
#ifdef COPASI_BANDED_GRAPH
#include "BandedGraphWidget.h"
#endif // COPASI_BANDED_GRAPH

#include <copasi/plotUI/CQSpectogramWidget.h>

#include "plotwindow.h"
#include "plot/CPlotSpecification.h"
#include "plot/COutputDefinitionVector.h"
#include "report/CKeyFactory.h"
#include "core/CDataArray.h"
#include "UI/CCopasiPlotSelectionDialog.h"
#include "model/CMetabNameInterface.h"
#include "CopasiDataModel/CDataModel.h"
#include "UI/DataModelGUI.h"
#include "copasi/UI/qtUtilities.h"
#include "copasi/core/CRootContainer.h"

#include "UI/CCopasiSelectionDialog.h"
#include "UI/CQMultipleSelectionDialog.h"
#include <QListWidgetItem>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QMessageBox>
//-----------------------------------------------------------------------------

/*
 *  Constructs a PlotWidget1 as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CQPlotSubwidget::CQPlotSubwidget(QWidget* parent, const char* name, Qt::WindowFlags fl)
  : CopasiWidget(parent, name, fl)
  , mpCurveWidget(NULL)
  , mpHistoWidget(NULL)
#ifdef COPASI_BANDED_GRAPH
  , mpBandedGraphWidget(NULL)
#endif
  , mpSpectogramWidget(NULL)
  , mLastItem(NULL)
{
  setupUi(this);
  mpCurveWidget = new Curve2DWidget(this);
  mpStack->addWidget(mpCurveWidget);
  mpHistoWidget = new HistoWidget(this);
  mpStack->addWidget(mpHistoWidget);
#ifdef COPASI_BANDED_GRAPH
  // this should be implemented in the CQPlotSubwidget.ui file
  // as the button will be appended behind the 'delete' button:
  QToolButton *buttonBandedGraph = new QToolButton(this);
  buttonBandedGraph->setText("New Banded Graph");
  layoutCurves->addWidget(buttonBandedGraph);
  connect(buttonBandedGraph, SIGNAL(clicked()), this, SLOT(addBandedGraphSlot()));
  mpBandedGraphWidget = new BandedGraphWidget(this);
  mpStack->addWidget(mpBandedGraphWidget);
#endif // COPASI_BANDED_GRAPH
  mpSpectogramWidget = new CQSpectogramWidget(this);
  mpStack->addWidget(mpSpectogramWidget);

  auto it = CTaskEnum::TaskName.begin();
  for (; it != CTaskEnum::TaskName.end(); ++it)
  {
    mTaskNames << FROM_UTF8(*it);
  }
}

CPlotItem *CQPlotSubwidget::updateItem(CPlotItem *item)
{
  if (item == NULL || !mpStack->isEnabled()) return NULL;

  QWidget *widget = mpStack->currentWidget();
  CQPlotEditWidget *current = dynamic_cast<CQPlotEditWidget *>(widget);

  if (current != NULL)
    {
      if (!current->SaveToCurveSpec(item, mLastItem))
        {
          return NULL;
        }
    }

  return item;
}

void CQPlotSubwidget::storeChanges()
{
  if (mLastSelection.size() == 0)
    return;

  if (mLastSelection.size() == 1)
    {
      QString oldName = mLastSelection[0]->text();
      CPlotItem *item = mList[oldName];
      updateItem(item);

      if (item == NULL) return;

      QString newName = FROM_UTF8(item->getTitle());

      if (oldName != newName)
        {
          mList.remove(oldName);
          mLastSelection[0]->setText(newName);
          mList.insert(newName, item);
        }

      // assign current
    }
  else
    {
      if (!areOfSameType(mLastSelection) || !mpStack->isEnabled())
        return;

      CPlotItem *common = new CPlotItem("nope");

      if (mpStack->currentWidget() == mpHistoWidget)
        {
          common->setType(CPlotItem::histoItem1d);
        }

#if COPASI_BANDED_GRAPH
      else if (mpStack->currentWidget() == mpBandedGraphWidget)
        {
          common->setType(CPlotItem::bandedGraph);
        }

#endif
      else if (mpStack->currentWidget() == mpSpectogramWidget)
        {
          common->setType(CPlotItem::spectogram);
        }
      else
        {
          common->setType(CPlotItem::curve2d);
        }

      common = updateItem(common);

      if (common == NULL)
        return;

      QList<QListWidgetItem *>::const_iterator it;

      for (it = mLastSelection.begin(); it != mLastSelection.end(); ++it)
        {
          CPlotItem *current = mList[(*it)->text()];

          if (current == NULL)
            continue;

          std::vector<CPlotDataChannelSpec> channels = current->getChannels();
          CPlotItem *newItem = new CPlotItem(*common, NO_PARENT);
          newItem->setType(current->getType());
          newItem->setTitle(current->getTitle());
          newItem->getChannels() = channels;
          newItem->setActivity(common->getActivity());
          mList[(*it)->text()] = newItem;
          delete current;
        }

      pdelete(common);
      // assign multiple
    }
}

//-----------------------------------------------------------------------------
/*
 *  Destroys the object and frees any allocated resources
 */
CQPlotSubwidget::~CQPlotSubwidget()
{}

//-----------------------------------------------------------------------------

//the slot...
void CQPlotSubwidget::addCurveSlot()
{
  if (mType == CPlotItem::plot2d)
    addCurve2D();
}

#ifdef COPASI_BANDED_GRAPH
void CQPlotSubwidget::addBandedGraphSlot()
{
  if (mType == CPlotItem::plot2d)
    addBandedGraph();
}
#endif // COPASI_BANDED_GRAPH

void CQPlotSubwidget::addSpectrumSlot()
{
  if (mType == CPlotItem::plot2d)
    addSpectrum();
}

void CQPlotSubwidget::addHistoSlot()
{
  if (mType == CPlotItem::plot2d)
    addHisto1D();
}

int CQPlotSubwidget::getCurrentIndex()
{
  return mpListPlotItems->currentRow();
}

void CQPlotSubwidget::deleteCurves()
{
  mLastSelection.clear();

  for (int i = mpListPlotItems->count(); i >= 0; --i)
    {
      deleteCurve(i);
    }

  mList.clear();
  mpListPlotItems->clear();
  mLastSelection.clear();
}

int CQPlotSubwidget::getRow(QListWidgetItem *item)
{
  for (int i = 0; i < mpListPlotItems->count(); ++i)
    {
      if (mpListPlotItems->item(i)->text() == item->text())
        return i;
    }

  return -1;
}
void CQPlotSubwidget::deleteCurve(QListWidgetItem *item)
{
  if (item == NULL)
    return;

  delete mList[item->text()];
  mList.remove(item->text());
  mLastSelection.removeOne(item);
  delete mpListPlotItems->takeItem(getRow(item));
}

void CQPlotSubwidget::deleteCurve(int index)
{
  QListWidgetItem *item = mpListPlotItems->item(index);
  deleteCurve(item);
}

void CQPlotSubwidget::setCurrentIndex(int index)
{
  if (index < 0)
    {
      mpListPlotItems->clearSelection();
      return;
    }

  if (mpListPlotItems->count() == 0)
    return;

  if (index < 0 && mpListPlotItems->count() > 0)
    index = 0;

  if (index >= mpListPlotItems->count())
    index = mpListPlotItems->count() - 1;

  mpListPlotItems->setCurrentRow(index, QItemSelectionModel::Select);
}

void CQPlotSubwidget::addPlotItem(CPlotItem *item)
{
  QString title = FROM_UTF8(item->getTitle());
  int count = 0;
  mpListPlotItems->clearSelection();

  while (mList.contains(title))
    {
      title = (FROM_UTF8(item->getTitle()) + " %1").arg(++count);
    }

  item->setTitle(TO_UTF8(title));
  QListWidgetItem *listItem = new QListWidgetItem(FROM_UTF8(item->getTitle()));
  mpListPlotItems->addItem(listItem);
  mList.insert(FROM_UTF8(item->getTitle()), new CPlotItem(*item, NO_PARENT));
  mpListPlotItems->setCurrentRow(mpListPlotItems->count() - 1);
}

CQPlotEditWidget *CQPlotSubwidget::selectControl(CPlotItem::Type type)
{
  switch (type)
    {
#ifdef COPASI_BANDED_GRAPH

      case CPlotItem::bandedGraph:
      {
        mpStack->setCurrentIndex(2);
        return mpBandedGraphWidget;
      }

#endif

      case CPlotItem::histoItem1d:
      {
        mpStack->setCurrentIndex(1);
        return mpHistoWidget;
      }

      case CPlotItem::curve2d:
      {
        mpStack->setCurrentIndex(0);
        return mpCurveWidget;
      }

      case CPlotItem::spectogram:
      {
#ifdef COPASI_BANDED_GRAPH
        mpStack->setCurrentIndex(3);
#else
        mpStack->setCurrentIndex(2);
#endif
        return mpSpectogramWidget;
      }

      default:
        return NULL;
    }
}

void CQPlotSubwidget::selectPlotItem(CPlotItem *item)
{
  CQPlotEditWidget *current = static_cast< CQPlotEditWidget * >(mpStack->currentWidget());

  if (current == NULL) return;

  if (item != NULL)
    {
      current = selectControl(item->getType());
    }

  if (item == NULL)
    {
      mpStack->setEnabled(false);
    }

  current->setModel(mpDataModel->getModel());
  current->LoadFromCurveSpec(item);
  pdelete(mLastItem);

  if (item != NULL)
    mLastItem = new CPlotItem(*item, NO_PARENT);
}

void CQPlotSubwidget::addCurveTab(const std::string &title,
                                  const CPlotDataChannelSpec &x,
                                  const CPlotDataChannelSpec &y)
{
  CPlotItem *item = new CPlotItem(title, NULL, CPlotItem::curve2d);
  item->addChannel(x);
  item->addChannel(y);
  addPlotItem(item);
}

void CQPlotSubwidget::addCurve2D()
{
  CCopasiPlotSelectionDialog *pBrowser = new CCopasiPlotSelectionDialog();
  std::vector< const CDataObject * > vector1;
  std::vector< const CDataObject * > vector2;
  pBrowser->setOutputVectors(&vector1, &vector2);
  assert(mpDataModel != NULL);
  pBrowser->setModel(mpDataModel->getModel(), CQSimpleSelectionTree::NumericValues);

  if (pBrowser->exec() == QDialog::Rejected)
    {
      return;
    }

  //this assumes that the vector is empty if nothing was chosen
  if (vector1.size() == 0 || vector2.size() == 0)
    {
      return;
    }

  std::vector<CCommonName> objects1, objects2;
  size_t i;
  std::vector<CCommonName>::const_iterator sit;
  const CDataArray *pArray;
  // 1. enable user to choose either a cell, an entire row/column, or even the objects themselves, if they are arrays.
  // 2. translate to CNs and remove duplicates
  // x-axis is set for single cell selection
  std::string cn;

  for (i = 0; i < vector1.size(); i++)
    {
      if (vector1[i])  // the object is not empty
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CDataArray * >(vector1[i])))
            {
              // second argument is true as only single cell here is allowed. In this case we
              //can assume that the size of the return vector is 1.
              const CDataObject *pObject = CCopasiSelectionDialog::chooseCellMatrix(pArray, true, true, "X axis: ")[0];

              if (!pObject) continue;

              cn = pObject->getCN();
            }
          else
            cn = vector1[i]->getCN();

          // check whether cn is already on objects1
          for (sit = objects1.begin(); sit != objects1.end(); ++sit)
            {
              if (*sit == cn) break;
            }

          // if not exist, input cn into objects1
          if (sit == objects1.end())
            {
              objects1.push_back(cn);
            }
        }
    }

  for (i = 0; i < vector2.size(); i++)
    {
      if (vector2[i])
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CDataArray * >(vector2[i])))
            {
              // second argument is set false for multi selection
              std::vector<const CDataObject *> vvv = CCopasiSelectionDialog::chooseCellMatrix(pArray, false, true, "Y axis: ");
              std::vector<const CDataObject *>::const_iterator it;

              for (it = vvv.begin(); it != vvv.end(); ++it)
                {
                  if (!*it) continue;

                  cn = (*it)->getCN();

                  //check if the CN already is in the list, if not add it.
                  for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                    if (*sit == cn) break;

                  if (sit == objects2.end())
                    objects2.push_back(cn);
                }
            }
          else
            {
              cn = vector2[i]->getCN();

              //check if the CN already is in the list, if not add it.
              for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                if (*sit == cn) break;

              if (sit == objects2.end())
                objects2.push_back(cn);
            }
        }
    }

  if (objects1.size() == 1)
    {
      for (i = 0; i < objects2.size(); ++i)
        {
          addCurveTab(mpDataModel->getObjectFromCN(objects2[i])->getObjectDisplayName()
                      + "|"
                      + mpDataModel->getObjectFromCN(objects1[0])->getObjectDisplayName(),
                      objects1[0], objects2[i]);
        }
    }
  else if (objects2.size() == 1)
    {
      for (i = 0; i < objects1.size(); ++i)
        {
          addCurveTab(mpDataModel->getObjectFromCN(objects2[0])->getObjectDisplayName()
                      + "|"
                      + mpDataModel->getObjectFromCN(objects1[i])->getObjectDisplayName(),
                      objects1[i], objects2[0]);
        }
    }
  else
    {
      size_t imax;

      if (objects1.size() > objects2.size())
        imax = objects2.size();
      else
        imax = objects1.size();

      for (i = 0; i < imax; ++i)
        {
          addCurveTab(mpDataModel->getObjectFromCN(objects2[i])->getObjectDisplayName()
                      + "|"
                      + mpDataModel->getObjectFromCN(objects1[i])->getObjectDisplayName(),
                      objects1[i], objects2[i]);
        }
    }
}

void CQPlotSubwidget::addSpectrum()
{
  CCopasiPlotSelectionDialog *pBrowser = new CCopasiPlotSelectionDialog();
  std::vector< const CDataObject * > vector1;
  std::vector< const CDataObject * > vector2;
  pBrowser->setOutputVectors(&vector1, &vector2);
  assert(mpDataModel != NULL);
  pBrowser->setModel(mpDataModel->getModel(), CQSimpleSelectionTree::NumericValues);

  if (pBrowser->exec() == QDialog::Rejected)
    {
      return;
    }

  //this assumes that the vector is empty if nothing was chosen
  if (vector1.size() == 0 || vector2.size() == 0)
    {
      return;
    }

  std::vector<CCommonName> objects1, objects2;
  size_t i;
  std::vector<CCommonName>::const_iterator sit;
  const CDataArray *pArray;
  // 1. enable user to choose either a cell, an entire row/column, or even the objects themselves, if they are arrays.
  // 2. translate to CNs and remove duplicates
  // x-axis is set for single cell selection
  std::string cn;

  for (i = 0; i < vector1.size(); i++)
    {
      if (vector1[i])  // the object is not empty
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CDataArray * >(vector1[i])))
            {
              // second argument is true as only single cell here is allowed. In this case we
              //can assume that the size of the return vector is 1.
              const CDataObject *pObject = CCopasiSelectionDialog::chooseCellMatrix(pArray, true, true, "X axis: ")[0];

              if (!pObject) continue;

              cn = pObject->getCN();
            }
          else
            cn = vector1[i]->getCN();

          // check whether cn is already on objects1
          for (sit = objects1.begin(); sit != objects1.end(); ++sit)
            {
              if (*sit == cn) break;
            }

          // if not exist, input cn into objects1
          if (sit == objects1.end())
            {
              objects1.push_back(cn);
            }
        }
    }

  for (i = 0; i < vector2.size(); i++)
    {
      if (vector2[i])
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CDataArray * >(vector2[i])))
            {
              // second argument is set false for multi selection
              std::vector<const CDataObject *> vvv = CCopasiSelectionDialog::chooseCellMatrix(pArray, false, true, "Y axis: ");
              std::vector<const CDataObject *>::const_iterator it;

              for (it = vvv.begin(); it != vvv.end(); ++it)
                {
                  if (!*it) continue;

                  cn = (*it)->getCN();

                  //check if the CN already is in the list, if not add it.
                  for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                    if (*sit == cn) break;

                  if (sit == objects2.end())
                    objects2.push_back(cn);
                }
            }
          else
            {
              cn = vector2[i]->getCN();

              //check if the CN already is in the list, if not add it.
              for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                if (*sit == cn) break;

              if (sit == objects2.end())
                objects2.push_back(cn);
            }
        }
    }

  if (objects1.size() == 1)
    {
      for (i = 0; i < objects2.size(); ++i)
        {
          addSpectrumTab(mpDataModel->getObject(objects2[i])->getObjectDisplayName()
                         + "|"
                         + mpDataModel->getObject(objects1[0])->getObjectDisplayName(),
                         objects1[0], objects2[i]);
        }
    }
  else if (objects2.size() == 1)
    {
      for (i = 0; i < objects1.size(); ++i)
        {
          addSpectrumTab(mpDataModel->getObject(objects2[0])->getObjectDisplayName()
                         + "|"
                         + mpDataModel->getObject(objects1[i])->getObjectDisplayName(),
                         objects1[i], objects2[0]);
        }
    }
  else
    {
      size_t imax;

      if (objects1.size() > objects2.size())
        imax = objects2.size();
      else
        imax = objects1.size();

      for (i = 0; i < imax; ++i)
        {
          addSpectrumTab(mpDataModel->getObject(objects2[i])->getObjectDisplayName()
                         + "|"
                         + mpDataModel->getObject(objects1[i])->getObjectDisplayName(),
                         objects1[i], objects2[i]);
        }
    }
}

#ifdef COPASI_BANDED_GRAPH
void CQPlotSubwidget::addBandedGraphTab(const std::string &title,
                                        const CPlotDataChannelSpec &x,
                                        const CPlotDataChannelSpec &yone,
                                        const CPlotDataChannelSpec &ytwo)
{
  CPlotItem *item = new CPlotItem(title, NULL, CPlotItem::bandedGraph);
  item->addChannel(x);
  item->addChannel(yone);
  item->addChannel(ytwo);
  addPlotItem(item);
}

void CQPlotSubwidget::addBandedGraph()
{
  CCopasiPlotSelectionDialog *pBrowser = new CCopasiPlotSelectionDialog();
  std::vector< const CDataObject * > vector1;
  std::vector< const CDataObject * > vector2;
  pBrowser->setOutputVectors(&vector1, &vector2);
  assert(mpDataModel != NULL);
  pBrowser->setModel(mpDataModel->getModel(), CQSimpleSelectionTree::NumericValues);

  if (pBrowser->exec() == QDialog::Rejected)
    {
      return;
    }

  //this assumes that the vector is empty if nothing was chosen
  if (vector1.size() == 0 || vector2.size() == 0)
    {
      return;
    }

  std::vector<CCommonName> objects1, objects2;
  size_t i;
  std::vector<CCommonName>::const_iterator sit;
  const CDataArray *pArray;
  // 1. enable user to choose either a cell, an entire row/column, or even the objects themselves, if they are arrays.
  // 2. translate to CNs and remove duplicates
  // x-axis is set for single cell selection
  std::string cn;

  for (i = 0; i < vector1.size(); i++)
    {
      if (vector1[i])  // the object is not empty
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CDataArray * >(vector1[i])))
            {
              // second argument is true as only single cell here is allowed. In this case we
              //can assume that the size of the return vector is 1.
              const CDataObject *pObject = CCopasiSelectionDialog::chooseCellMatrix(pArray, true, true, "X axis: ")[0];

              if (!pObject) continue;

              cn = pObject->getCN();
            }
          else
            cn = vector1[i]->getCN();

          // check whether cn is already on objects1
          for (sit = objects1.begin(); sit != objects1.end(); ++sit)
            {
              if (*sit == cn) break;
            }

          // if not exist, input cn into objects1
          if (sit == objects1.end())
            {
              objects1.push_back(cn);
            }
        }
    }

  for (i = 0; i < vector2.size(); i++)
    {
      if (vector2[i])
        {
          // is it an array annotation?
          if ((pArray = dynamic_cast< const CDataArray * >(vector2[i])))
            {
              // second argument is set false for multi selection
              std::vector<const CDataObject *> vvv = CCopasiSelectionDialog::chooseCellMatrix(pArray, false, true, "Y axis: ");
              std::vector<const CDataObject *>::const_iterator it;

              for (it = vvv.begin(); it != vvv.end(); ++it)
                {
                  if (!*it) continue;

                  cn = (*it)->getCN();

                  //check if the CN already is in the list, if not add it.
                  for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                    if (*sit == cn) break;

                  if (sit == objects2.end())
                    objects2.push_back(cn);
                }
            }
          else
            {
              cn = vector2[i]->getCN();

              //check if the CN already is in the list, if not add it.
              for (sit = objects2.begin(); sit != objects2.end(); ++sit)
                if (*sit == cn) break;

              if (sit == objects2.end())
                objects2.push_back(cn);
            }
        }
    }

  if (objects1.size() == 1)
    {
      for (i = 0; i < objects2.size(); ++i)
        {
          addBandedGraphTab(mpDataModel->getObject(objects2[i])->getObjectDisplayName()
                            + "|"
                            + mpDataModel->getObject(objects1[0])->getObjectDisplayName(),
                            objects1[0], objects2[i]);
        }
    }
  else if (objects2.size() == 1)
    {
      for (i = 0; i < objects1.size(); ++i)
        {
          addBandedGraphTab(mpDataModel->getObject(objects2[0])->getObjectDisplayName()
                            + "|"
                            + mpDataModel->getObject(objects1[i])->getObjectDisplayName(),
                            objects1[i], objects2[0]);
        }
    }
  else
    {
      size_t imax;

      if (objects1.size() > objects2.size())
        imax = objects2.size();
      else
        imax = objects1.size();

      for (i = 0; i < imax; ++i)
        {
          addBandedGraphTab(mpDataModel->getObject(objects2[i])->getObjectDisplayName()
                            + "|"
                            + mpDataModel->getObject(objects1[i])->getObjectDisplayName(),
                            objects1[i], objects2[i]);
        }
    }
}
#endif // COPASI_BANDED_GRAPH

void CQPlotSubwidget::addSpectrumTab(const std::string &title,
                                     const CPlotDataChannelSpec &x,
                                     const CPlotDataChannelSpec &yone,
                                     const CPlotDataChannelSpec &ytwo)
{
  CPlotItem *item = new CPlotItem(title, NULL, CPlotItem::spectogram);
  item->addChannel(x);
  item->addChannel(yone);
  item->addChannel(ytwo);
  addPlotItem(item);
}

void CQPlotSubwidget::addHisto1DTab(const std::string &title,
                                    const CPlotDataChannelSpec &x, const C_FLOAT64 &incr)
{
  CPlotItem *item = new CPlotItem(title, NULL, CPlotItem::histoItem1d);
  item->addChannel(x);
  item->setValue("increment", incr);
  addPlotItem(item);
}

void CQPlotSubwidget::addHisto1D()
{
  addHisto1DTab("Histogram", CPlotDataChannelSpec(CCommonName("")), 1.0);
}

void CQPlotSubwidget::createHistograms(std::vector<const CDataObject * >objects, const C_FLOAT64 &incr)
{
  C_INT32 storeTab = getCurrentIndex();
  size_t i;

  for (i = 1; i < objects.size(); ++i)
    {
      if (objects[i])
        addHisto1DTab("Histogram: " + objects[i]->getObjectDisplayName(),
                      CPlotDataChannelSpec(objects[i]->getCN()), incr);

      //         lineEditTitle->setText("Histogram: " + FROM_UTF8(mpObjectX->getObjectDisplayName()));
    }

  setCurrentIndex(storeTab);
}

//-----------------------------------------------------------------------------

void CQPlotSubwidget::removeCurve()
{
  QList<QListWidgetItem *> selection = mpListPlotItems->selectedItems();

  if (selection.size() == 0)
    return;

  if (QMessageBox::question(this, "Delete Curves", QString("Do you really want to delete the %1 selected curve(s)?").arg(selection.size()), QMessageBox::Yes, QMessageBox::No | QMessageBox::Default) == QMessageBox::Yes)
    {
      for (int index = selection.size() - 1; index >= 0; --index)
        {
          deleteCurve(selection.at(index));
        }

      mLastSelection.clear();
    }
}

//-----------------------------------------------------------------------------

void CQPlotSubwidget::commitPlot()
{
  saveToPlotSpec();
  loadFromPlotSpec(dynamic_cast<CPlotSpecification *>(CRootContainer::getKeyFactory()->get(mKey)));
}

//-----------------------------------------------------------------------------

void CQPlotSubwidget::deletePlot()
{
  size_t Index, Size;
  assert(mpDataModel != NULL);

  if (!mpDataModel->getModel())
    return;

  CPlotSpecification *pspec = dynamic_cast< CPlotSpecification * >(CRootContainer::getKeyFactory()->get(mKey));

  if (!pspec) return;

  Index =
    mpDataModel->getPlotDefinitionList()->CDataVector<CPlotSpecification>::getIndex(pspec);
  mpDataModel->getPlotDefinitionList()->removePlotSpec(mKey);
  std::string deletedKey = mKey;
  Size = mpDataModel->getPlotDefinitionList()->size();

  if (Size > 0)
    enter(mpDataModel->getPlotDefinitionList()->operator[](std::min(Index, Size - 1)).CCopasiParameter::getKey());
  else
    enter("");

  //ListViews::
  protectedNotify(ListViews::PLOT, ListViews::DELETE, deletedKey);
}

//-----------------------------------------------------------------------------

void CQPlotSubwidget::copyPlot()
{
  leave();
  CDataModel *pDataModel = mpObject->getObjectDataModel();

  if (pDataModel == NULL) return;

  CPlotSpecification *pPl = new CPlotSpecification(*dynamic_cast<CPlotSpecification *>(CRootContainer::getKeyFactory()->get(mKey)), NO_PARENT);
  std::string baseName = pPl->getObjectName() + "_copy";
  std::string name = baseName;
  int i = 1;

  while (pDataModel->getPlotDefinitionList()->getIndex(name) != C_INVALID_INDEX)
    {
      i++;
      name = baseName + TO_UTF8(QString::number(i));
    }

  pPl->setObjectName(name);
  pDataModel->getPlotDefinitionList()->add(pPl, true);
  std::string key = pPl->CCopasiParameter::getKey();
  protectedNotify(ListViews::PLOT, ListViews::ADD, key);
  enter(key);
  mpListView->switchToOtherWidget(C_INVALID_INDEX, key);
}

//-----------------------------------------------------------------------------

void CQPlotSubwidget::addPlot()
{
  leave();
  CDataModel *pDataModel = mpObject->getObjectDataModel();

  if (pDataModel == NULL) return;

  std::string name = "plot_";
  int i = 0;
  CPlotSpecification *pPl = NULL;
  name += TO_UTF8(QString::number(i));

  while (!(pPl = pDataModel->getPlotDefinitionList()->createPlotSpec(name, CPlotItem::plot2d)))
    {
      i++;
      name = "plot_";
      name += TO_UTF8(QString::number(i));
    }

  std::string key = pPl->CCopasiParameter::getKey();
  protectedNotify(ListViews::PLOT, ListViews::ADD, key);
  enter(key);
  mpListView->switchToOtherWidget(C_INVALID_INDEX, key);
}

//-----------------------------------------------------------------------------

void CQPlotSubwidget::resetPlot()
{
  loadFromPlotSpec(dynamic_cast<CPlotSpecification *>(CRootContainer::getKeyFactory()->get(mKey)));
}

#include <QInputDialog>

void CQPlotSubwidget::selectTaskTypes()
{
  CQMultipleSelectionDialog* dlg = new CQMultipleSelectionDialog(this);
  dlg->setWindowTitle("Select Tasks");
  dlg->setMinimumHeight(400);
  dlg->setSelectionList(mTaskNames);
  
  QStringList currentSelection;
  if (!mTaskTypes.empty())
  {
    std::istringstream ss(mTaskTypes);
    std::string token;
    while (std::getline(ss, token, ',')) {

      while (token[0] == ' ') // remove leading spaces
        token.erase(0, 1);

      currentSelection << FROM_UTF8(token);
    }

  }

  dlg->setCurrentSelection(currentSelection);
  if (dlg->exec() != QDialog::Accepted)
    return;

  const QStringList& selection = dlg->getSelection();
  
  std::stringstream str; 
  if (!selection.empty())
  {
    auto it = selection.begin();
    str << TO_UTF8(*it++);
    for (; it != selection.end(); ++it)
    {
      str << ", ";
      str << TO_UTF8(*it);
    }
  }

  mTaskTypes = str.str();

  chkTaskTypes->setChecked(mTaskTypes.empty());
  txtTaskTypes->setText(FROM_UTF8(mTaskTypes));

}

void CQPlotSubwidget::allTaskTypesClicked()
{
  if (!mTaskTypes.empty())
  {
    mTaskTypes.clear();
    txtTaskTypes->clear();
  }
  else
  {
    selectTaskTypes();
  }
}

//-----------------------------------------------------------------------------

bool CQPlotSubwidget::loadFromPlotSpec(const CPlotSpecification *pspec)
{
  if (!pspec) return false;

  mLastSelection.clear();
  //title
  titleLineEdit->setText(pspec->getTitle().c_str());
  //active?
  activeCheckBox->setChecked(pspec->isActive());
  //type
  mType = pspec->getType();

  mTaskTypes = pspec->getTaskTypes();

  txtTaskTypes->setText(FROM_UTF8(mTaskTypes));
  chkTaskTypes->setChecked(mTaskTypes.empty());

  switch (mType)
    {
#ifdef COPASI_BANDED_GRAPH

      case CPlotItem::bandedGraph:
#endif // COPASI_BANDED_GRAPH
      case CPlotItem::spectogram:
      case CPlotItem::plot2d:
        checkLogX->setChecked(pspec->isLogX());
        checkLogY->setChecked(pspec->isLogY());
        break;

      default:
        fatalError();
        break;
    }

  //clear tabWidget
  deleteCurves();
  mpListPlotItems->clearSelection();
  //reconstruct tabWidget from curve specs
  CDataVector<CPlotItem>::const_iterator it = pspec->getItems().begin();
  CDataVector<CPlotItem>::const_iterator end = pspec->getItems().end();
  QStringList PlotItems;

  for (; it != end; ++it)
    {
      QString title = FROM_UTF8(it->getTitle());
      PlotItems.append(title);
      CPlotItem *pItem = new CPlotItem(*it, NO_PARENT);
      // The copy has the same parent as the original, i.e., it has been added to the plot specification.
      const_cast< CPlotSpecification * >(pspec)->getItems().remove(pItem);
      mList.insert(title, pItem);
    }

  mpListPlotItems->addItems(PlotItems);

  if (pspec->getItems().size() > 0)
    {
      mpListPlotItems->setCurrentRow(0, QItemSelectionModel::Select);
    }
  else
    {
      // We need to clear the current items display
      selectPlotItem(NULL);
    }

  return true; //TODO really check
}

bool CQPlotSubwidget::saveToPlotSpec()
{
  CPlotSpecification *pspec = dynamic_cast< CPlotSpecification * >(mpObject);

  if (!pspec) return true;

  pspec->cleanup();

  //title
  if (pspec->getTitle() != TO_UTF8(titleLineEdit->text()))
    {
      pspec->setTitle(TO_UTF8(titleLineEdit->text()));
      protectedNotify(ListViews::PLOT, ListViews::RENAME, mKey);
    }

  //active?
  pspec->setActive(activeCheckBox->isChecked());
  //scales
  pspec->setLogX(checkLogX->isChecked());
  pspec->setLogY(checkLogY->isChecked());
  // task types
  pspec->setTaskTypes(mTaskTypes);
  
  //curves
  CPlotItem *item;
  storeChanges();
  QMap<QString, CPlotItem *>::iterator it;

  for (it = mList.begin(); it != mList.end(); ++it)
    {
      CPlotItem *currentItem = (*it);

      if (currentItem == NULL) continue;

      item = new CPlotItem(*currentItem, NO_PARENT);
      pspec->getItems().add(item, true);
    }

  // :TODO Bug 322: This should only be called when actual changes have been saved.
  // However we do not check whether the scan item are changed we delete all
  // and create them new.
  if (true)
    {
      if (mpDataModel != NULL)
        {
          mpDataModel->changed();
        }

      // mChanged = false;
    }

  return true;
}

//-----------------------------------------------------------------------------

//TODO:  save a copy!

bool CQPlotSubwidget::enterProtected()
{
  CPlotSpecification *pspec = dynamic_cast< CPlotSpecification * >(mpObject);

  if (!pspec)
    {
      mpListView->switchToOtherWidget(42, "");
      return false;
    }

  return loadFromPlotSpec(pspec);
}
bool CQPlotSubwidget::areOfSameType(QList<QListWidgetItem *> &items)
{
  if (items.size() <= 1) return true;

  QList<CPlotItem::Type> listOfUniqueTypes;
  QList<QListWidgetItem *>::const_iterator it = items.begin();

  while (it != items.end())
    {
      QString currentText = (*it)->text();
      CPlotItem *item = mList[currentText];

      if (!listOfUniqueTypes.contains(item->getType()))
        listOfUniqueTypes.append(item->getType());

      ++it;
    }

  return listOfUniqueTypes.size() == 1;
}

void CQPlotSubwidget::itemSelectionChanged()
{
  storeChanges();
  QList<QListWidgetItem *> current = mpListPlotItems->selectedItems();

  if (current.size() == 0)
    {
      mpStack->setEnabled(false);
    }
  else if (current.size() == 1)
    {
      mpStack->setEnabled(true);
      selectPlotItem(mList[current[0]->text()]);
      (static_cast<CQPlotEditWidget *>(mpStack->currentWidget()))->setMultipleEditMode(false);
    }
  else
    {
      if (!areOfSameType(current))
        {
          mpStack->setEnabled(false);
        }
      else
        {
          mpStack->setEnabled(true);
          selectPlotItem(mList[current[0]->text()]);
          (static_cast<CQPlotEditWidget *>(mpStack->currentWidget()))->setMultipleEditMode(true);
        }
    }

  mLastSelection = current;
}

//-----------------------------------------------------------------------------

bool CQPlotSubwidget::update(ListViews::ObjectType objectType, ListViews::Action action, const std::string &key)
{
  if (mIgnoreUpdates || isHidden()) return true;

  switch (objectType)
    {
        //TODO: check list:
      case ListViews::MODEL:
        switch (action)
          {
            case ListViews::DELETE:
              mpObject = NULL;
              mKey = "";
              return enterProtected();
              break;

            default:
              break;
          }

        break;

      case ListViews::PLOT:
        if (key == mKey)
          {
            switch (action)
              {
                case ListViews::DELETE:
                  mpObject = NULL;
                  mKey = "";
                  return enterProtected();
                  break;

                case ListViews::CHANGE:
                  return enterProtected();
                  break;

                default:
                  break;
              }
          }

        break;

      default:
        break;
    }

  return true;
}

//-----------------------------------------------------------------------------

bool CQPlotSubwidget::leave()
{
  return saveToPlotSpec();
}
