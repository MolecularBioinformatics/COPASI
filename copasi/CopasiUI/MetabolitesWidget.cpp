/***********************************************************************
 **  $ CopasiUI/MetabolitesWidget.cpp                 
 **  $ Author  : Mudita Singhal
 **
 ** This file is used to create the GUI FrontPage for the  information
 ** obtained from the data model about the Metabolites----It is 
 ** the First level of Metabolites.
 ************************************************************************/
#include "MetabolitesWidget.h"

#include <qlayout.h>
#include <qwidget.h>
#include <qpushbutton.h>

#include "MyTable.h"
#include "model/CModel.h"
#include "model/CMetab.h"
#include "model/CCompartment.h"
#include "listviews.h"
#include "report/CKeyFactory.h"

/**
 *  Constructs a Widget for the Metabolites subsection of the tree for 
 *  displaying the Metabolites in model 'model'.
 *  This widget is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'. 
 *  @param model The CModel class which contains the metabolites 
 *  to be displayed.
 *  @param parent The widget which this widget is a child of.
 *  @param name The object name is a text that can be used to identify 
 *  this QObject. It's particularly useful in conjunction with the Qt Designer.
 *  You can find an object by name (and type) using child(), and more than one 
 *  using queryList(). 
 *  @param flags Flags for this widget. Redfer Qt::WidgetFlags of Qt documentation 
 *  for more information about these flags.
 */
MetabolitesWidget::MetabolitesWidget(QWidget *parent, const char * name, WFlags f)
    : CopasiWidget(parent, name, f)
{
  binitialized = true;

  table = new MyTable(0, 5, this, "tblMetabolites");
  table->setNumRows(-1);
  QVBoxLayout *vBoxLayout = new QVBoxLayout(this, 0);
  vBoxLayout->addWidget(table);

  QHeader *tableHeader = table->horizontalHeader();
  tableHeader->setLabel(0, "Name");
  tableHeader->setLabel(1, "Concentration");
  tableHeader->setLabel(2, "Number");
  tableHeader->setLabel(3, "Status");
  tableHeader->setLabel(4, "Compartment");

  btnOK = new QPushButton("&OK", this);
  btnCancel = new QPushButton("&Cancel", this);

  QHBoxLayout *hBoxLayout = new QHBoxLayout(vBoxLayout, 0);

  //To match the Table left Vertical Header Column Width.
  hBoxLayout->addSpacing(32);

  hBoxLayout->addSpacing(50);
  hBoxLayout->addWidget(btnOK);
  hBoxLayout->addSpacing(5);
  hBoxLayout->addWidget(btnCancel);
  hBoxLayout->addSpacing(50);

  table->sortColumn (0, true, true);
  table->setSorting (true);
  table->setFocusPolicy(QWidget::WheelFocus);

  // signals and slots connections
  connect(table, SIGNAL(doubleClicked(int, int, int, const QPoint &)),
          this, SLOT(slotTableCurrentChanged(int, int, int, const QPoint &)));
  connect(table, SIGNAL(selectionChanged ()),
          this, SLOT(slotTableSelectionChanged ()));
  connect(btnOK, SIGNAL(clicked ()),
          this, SLOT(slotBtnOKClicked()));
  connect(btnCancel, SIGNAL(clicked ()),
          this, SLOT(slotBtnCancelClicked()));
}

void MetabolitesWidget::fillTable()
{
  const CMetab *obj;
  const CCopasiVector < CMetab > & objects = dataModel->getModel()->getMetabolites();
  C_INT32 j, jmax = objects.size();
  table->setNumRows(jmax);
  mKeys.resize(jmax);

  for (j = 0; j < jmax; ++j)
    {
      obj = objects[j];
      table->setText(j, 0, obj->getName().c_str());
      table->setText(j, 1, QString::number(obj->getConcentration()));
      table->setText(j, 2, QString::number(obj->getNumberDbl()));
      table->setText(j, 3, CMetab::StatusName[obj->getStatus()].c_str());
      table->setText(j, 4, obj->getCompartment()->getName().c_str());

      mKeys[j] = obj->getKey();
    }
  table->setText(jmax, 1, "");
  table->setText(jmax, 2, "");
  table->setText(jmax, 3, "");
  table->setText(jmax, 4, "");
}

//**************************************************************************

void MetabolitesWidget::createNewObject()
{}

void MetabolitesWidget::slotTableCurrentChanged(int row,
    int C_UNUSED(col),
    int C_UNUSED(m) ,
    const QPoint & C_UNUSED(n))
{
  if (row >= table->numRows() || row < 0) return;

  if (row == table->numRows() - 1)
    {
      //TODO: create a new Object
    }

  pListView->switchToOtherWidget(mKeys[row]);
}

void MetabolitesWidget::slotTableSelectionChanged()
{
  if (!table->hasFocus()) table->setFocus();
}

void MetabolitesWidget::slotBtnOKClicked()
{}

void MetabolitesWidget::slotBtnCancelClicked()
{}

void MetabolitesWidget::tableValueChanged(int C_UNUSED(row),
    int C_UNUSED(col))
{}

bool MetabolitesWidget::update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key)
{
  switch (objectType)
    {
    case ListViews::MODEL:
    case ListViews::STATE:
    case ListViews::METABOLITE:
    case ListViews::COMPARTMENT:
      fillTable();
      break;

    default:
      break;
    }
  return true;
}

bool MetabolitesWidget::leave()
{
  //does nothing.
  return true;
}

bool MetabolitesWidget::enter(const std::string & key)
{
  //does nothing.
  return true;
}

//*******************************************************************************************

void MetabolitesWidget::resizeEvent(QResizeEvent * re)
{
  if (isVisible())
    {
      //      repaint_table();
      if (binitialized)
        {
          int newWidth = re->size().width();
          newWidth -= 35; //Accounting for the left (vertical) header width.
          float weight0 = 4.0, weight1 = 3.0, weight2 = 3.0, weight3 = 3.0 , weight4 = 3.0;
          float weightSum = weight0 + weight1 + weight2 + weight3 + weight4;
          int w0, w1, w2, w3 , w4;
          w0 = newWidth * (weight0 / weightSum);
          w1 = newWidth * (weight1 / weightSum);
          w2 = newWidth * (weight2 / weightSum);
          w3 = newWidth * (weight3 / weightSum);
          w4 = newWidth - w0 - w1 - w2 - w3;
          table->setColumnWidth(0, w0);
          table->setColumnWidth(1, w1);
          table->setColumnWidth(2, w2);
          table->setColumnWidth(3, w3);
          table->setColumnWidth(4, w4);
          binitialized = false;
        }
      else
        {
          table->DisableColWidthUpdate();
          int newWidth = re->size().width();
          int i;

          int totalWidth = 0;
          for (i = 0; i < table->numCols(); i++)
            totalWidth += table->columnWidth(i);

          int minTotalWidth = 0;
          for (i = 0; i < table->numCols(); i++)
            minTotalWidth += table->minColWidth[i];

          //Zoom in
          if (newWidth > re->oldSize().width())
            {
              if (newWidth > totalWidth) // can do expansion
                {
                  if (totalWidth < re->oldSize().width())
                    for (i = 0; i < table->numCols(); i++) // Do expansion
                      table->setColumnWidth(i, newWidth*table->columnWidth(i) / re->oldSize().width());
                  else
                    for (i = 0; i < table->numCols(); i++) // Do expansion
                      table->setColumnWidth(i, float(newWidth)*float(table->columnWidth(i)) / float(totalWidth));
                }
              else
                for (i = 0; i < table->numCols(); i++) // Do not expand
                  table->setColumnWidth(i, table->columnWidth(i));

              table->EnableColWidthUpdate();
              return;
            }
          //Zoom out
          //calculate total Width
          if (newWidth >= totalWidth)    //will not decrease any column width
            {
              for (i = 0; i < table->numCols(); i++)
                table->setColumnWidth(i, table->columnWidth(i));
              table->EnableColWidthUpdate();
              return;
            }
          //will decrease only those larger than the minimum width
          //Less than the user specified total width
          if (newWidth <= minTotalWidth)
            {
              for (i = 0; i < table->numCols(); i++)
                table->setColumnWidth(i, table->minColWidth[i]);
              table->EnableColWidthUpdate();
              return;
            }
          //Bigger than the user specified total width
          for (i = 0; i < table->numCols(); i++) // Do Expansion
            table->setColumnWidth(i, (newWidth - minTotalWidth)*(table->columnWidth(i) - table->minColWidth[i]) / (totalWidth - minTotalWidth) + table->minColWidth[i]);
          table->EnableColWidthUpdate();
          return;
        }
    }
  CopasiWidget::resizeEvent(re);
}
