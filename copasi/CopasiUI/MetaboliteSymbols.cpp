/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/CopasiUI/Attic/MetaboliteSymbols.cpp,v $
   $Revision: 1.19 $
   $Name:  $
   $Author: shoops $ 
   $Date: 2003/10/30 17:57:33 $
   End CVS Header */

/*******************************************************************
 **  $ CopasiUI/MetaboliteSymbols.cpp                 
 **  $ Author  : Mudita Singhal
 **
 ** This file is used to create the GUI FrontPage for the 
 ** information obtained from the data model about the 
 ** Metabolite Symbols
 ********************************************************************/
#include <qlayout.h>
#include <qwidget.h>
#include <qmessagebox.h>
#include "MetaboliteSymbols.h"
#include "mathmodel/CMathModel.h"
#include "mathmodel/CMathVariable.h"
#include "mathmodel/CMathConstant.h"
#include "model/CCompartment.h"
#include "listviews.h"

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
MetaboliteSymbols::MetaboliteSymbols(QWidget *parent, const char * name, WFlags f)
    : CopasiWidget(parent, name, f)
{
  mModel = NULL;
  table = new MyTable(this, "tblMetaboliteSymbols");
  table->setNumCols(7);
  table->setNumRows(-1);
  QVBoxLayout *vBoxLayout = new QVBoxLayout(this, 0);
  vBoxLayout->addWidget(table);

  QHeader *tableHeader = table->horizontalHeader();
  tableHeader->setLabel(0, "Symbol");
  tableHeader->setLabel(1, "Metabolite");
  tableHeader->setLabel(2, "Compartment");
  tableHeader->setLabel(3, "Initial Concentration");
  tableHeader->setLabel(4, "Initial Particle No.");
  tableHeader->setLabel(5, "Concentration");
  tableHeader->setLabel(6, "Particle No.");

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
  table->setProtected(true);

  // signals and slots connections
  connect(table, SIGNAL(selectionChanged ()), this, SLOT(slotTableSelectionChanged ()));
  connect(btnOK, SIGNAL(clicked ()), this, SLOT(slotBtnOKClicked()));
  connect(btnCancel, SIGNAL(clicked ()), this, SLOT(slotBtnCancelClicked()));
}

void MetaboliteSymbols::loadMetaboliteSymbols(CMathModel *model)
{
  if (model != NULL)
    {
      int i;
      mModel = model;
      //Emptying the table
      int numberOfRows = table->numRows();
      for (i = 0; i < numberOfRows; i++)
        {
          table->removeRow(0);
        }

      std::map< std::string, CMathVariableMetab * > metabList = mModel->getMetabList();
      std::map<std::string, CMathVariableMetab * >::iterator it;
      CMathVariableMetab * variableMetab;

      table->setNumRows(metabList.size());
      int index = 0;
      for (it = metabList.begin(); it != metabList.end();++it)
        {
          variableMetab = it->second;

          table->setText(index, 0, it->first.c_str());
          table->setText(index, 1, variableMetab->getObject()->getName().c_str());
          table->setText(index, 2, variableMetab->getCompartment().getName().c_str());
          table->setText(index, 3, QString::number(variableMetab->getInitialConcentration()));
          table->setText(index, 4, QString::number(variableMetab->getInitialParticleNumber()));
          table->setText(index, 5, QString::number(variableMetab->getConcentration()));
          table->setText(index, 6, QString::number(variableMetab->getParticleNumber()));
          index++;
        }

      /*
           unsigned C_INT32 k= mModel->getIntMetab();
           CCopasiVectorN< CMetab > metabolite(mModel->getMetabolitesX());
           C_INT32 noOfMetaboliteRows = metabolite.size();
           table->setNumRows(k);
           const CMetab *metab;
        for (i = 0; i < k; i++)
             {
               
               metab = metabolite[i];
               table->setText(i, 0, metab->getName().c_str());
         table->setText(i, 2, QString::number(metab->getInitialConcentration()));
               table->setText(i, 3, QString::number(metab->getInitialNumberDbl()));
         table->setText(i, 4, QString::number(metab->getConcentration()));
               table->setText(i, 5, QString::number(metab->getNumberDbl()));
         const CCompartment *Compartment=metab->getCompartment();
               table->setText(i, 6, Compartment->getName().c_str());
       }*/
    }
}

void MetaboliteSymbols::slotTableSelectionChanged()
{
  if (!table->hasFocus())
    {
      table->setFocus();
    }
}

/***********ListViews::showMessage(QString caption,QString text)------------------------>
 **
 ** Parameters:- 1. QString :- The Title that needs to be displayed in message box
 **              2. QString :_ The Text that needs to be displayed in the message box
 ** Returns  :-  void(Nothing)
 ** Description:- This method is used to show the message box on the screen
 ****************************************************************************************/

void MetaboliteSymbols::slotBtnOKClicked()
{
  //QMessageBox::information(this, "Metabolites Widget", "Do you really want to commit changes");
}

void MetaboliteSymbols::slotBtnCancelClicked()
{
  //QMessageBox::information(this, "Metabolites Widget", "Do you really want to cancel changes");
}

void MetaboliteSymbols::resizeEvent(QResizeEvent * re)
{
  if (isVisible())
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
    }
  CopasiWidget::resizeEvent(re);
}
