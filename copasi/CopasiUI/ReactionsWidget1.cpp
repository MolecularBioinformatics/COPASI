/*********************************************************************
 **  $ CopasiUI/ReactionsWidget1.cpp                 
 **  $ Author  : Mudita Singhal
 **
 ** This file is used to create the GUI FrontPage for the  information 
 ** obtained from the data model about the Recations----It is Basically 
 ** the Second level of Reactions.
 **********************************************************************/

#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include "copasi.h"
#include "utilities/CCopasiVector.h"
#include "ReactionsWidget1.h"
#include "listviews.h"
#include "model/model.h"
#include "function/CFunctionDB.h"
#include "function/CFunctionParameters.h"
#include "function/CFunctionParameter.h"
#include "utilities/CGlobals.h"

/*
 *  Constructs a ReactionsWidget which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 */

ReactionsWidget1::ReactionsWidget1(QWidget *parent, const char * name, WFlags f)
    : CopasiWidget(parent, name, f)

{
  if (!name)
    setName("ReactionsWidget1");
  resize(568, 479);
  setCaption(trUtf8("ReactionsWidget1"));
  ReactionsWidget1Layout = new QGridLayout(this, 1, 1, 11, 6, "ReactionsWidget1Layout");

  TextLabel4 = new QLabel(this, "TextLabel4");
  TextLabel4->setText(trUtf8("Name"));

  ReactionsWidget1Layout->addWidget(TextLabel4, 0, 0);

  TextLabel7 = new QLabel(this, "TextLabel7");
  TextLabel7->setText(trUtf8("Symbol Definition"));

  ReactionsWidget1Layout->addWidget(TextLabel7, 8, 0);

  Layout1 = new QHBoxLayout(0, 0, 6, "Layout1");

  commitChanges = new QPushButton(this, "commitChanges");
  commitChanges->setText(trUtf8("&Commit"));
  Layout1->addWidget(commitChanges);

  cancelChanges = new QPushButton(this, "cancelChanges");
  cancelChanges->setText(trUtf8("&Cancel"));
  Layout1->addWidget(cancelChanges);

  newReaction = new QPushButton(this, "newReaction");
  newReaction->setText(trUtf8("&New"));
  Layout1->addWidget(newReaction);

  deleteReaction = new QPushButton(this, "deleteReaction");
  deleteReaction->setText(trUtf8("&Delete"));
  Layout1->addWidget(deleteReaction);

  ReactionsWidget1Layout->addMultiCellLayout(Layout1, 12, 12, 0, 3);

  TextLabel8 = new QLabel(this, "TextLabel8");
  TextLabel8->setText(trUtf8("Flux"));

  ReactionsWidget1Layout->addWidget(TextLabel8, 10, 0);

  Line2 = new QFrame(this, "Line2");
  Line2->setFrameShape(QFrame::HLine);
  Line2->setFrameShadow(QFrame::Sunken);
  Line2->setFrameShape(QFrame::HLine);

  ReactionsWidget1Layout->addMultiCellWidget(Line2, 6, 7, 0, 3);

  Line1 = new QFrame(this, "Line1");
  Line1->setFrameShape(QFrame::HLine);
  Line1->setFrameShadow(QFrame::Sunken);
  Line1->setFrameShape(QFrame::HLine);

  ReactionsWidget1Layout->addMultiCellWidget(Line1, 11, 11, 0, 3);

  Line3 = new QFrame(this, "Line3");
  Line3->setFrameShape(QFrame::HLine);
  Line3->setFrameShadow(QFrame::Sunken);
  Line3->setFrameShape(QFrame::HLine);

  ReactionsWidget1Layout->addMultiCellWidget(Line3, 1, 1, 0, 3);

  TextLabel6 = new QLabel(this, "TextLabel6");
  TextLabel6->setText(trUtf8("Kinetics"));

  ReactionsWidget1Layout->addWidget(TextLabel6, 4, 0);

  LineEdit1 = new QLineEdit(this, "LineEdit1");

  ReactionsWidget1Layout->addMultiCellWidget(LineEdit1, 0, 0, 1, 3);

  LineEdit2 = new MyLineEdit(this, "LineEdit2");
  LineEdit2->setFrameShape(QLineEdit::LineEditPanel);
  LineEdit2->setFrameShadow(QLineEdit::Sunken);

  ReactionsWidget1Layout->addMultiCellWidget(LineEdit2, 2, 2, 1, 3);

  ComboBox1 = new QComboBox(FALSE, this, "ComboBox1");

  ReactionsWidget1Layout->addMultiCellWidget(ComboBox1, 4, 4, 1, 3);

  Line4 = new QFrame(this, "Line4");
  Line4->setFrameShape(QFrame::HLine);
  Line4->setFrameShadow(QFrame::Sunken);
  Line4->setFrameShape(QFrame::HLine);

  ReactionsWidget1Layout->addMultiCellWidget(Line4, 3, 3, 0, 3);

  table = new QTable(this, "table");
  table->setNumCols(table->numCols() + 1); table->horizontalHeader()->setLabel(table->numCols() - 1, trUtf8("Value"));
  table->setNumRows(0);
  table->setNumCols(1);

  ReactionsWidget1Layout->addMultiCellWidget(table, 7, 9, 1, 3);

  LineEdit3 = new QLineEdit(this, "LineEdit3");
  LineEdit3->setEnabled(FALSE);

  ReactionsWidget1Layout->addMultiCellWidget(LineEdit3, 10, 10, 1, 3);
  QSpacerItem* spacer = new QSpacerItem(80, 101, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ReactionsWidget1Layout->addItem(spacer, 9, 0);

  TextLabel5 = new QLabel(this, "TextLabel5");
  TextLabel5->setText(trUtf8("Chemical Reaction"));

  ReactionsWidget1Layout->addWidget(TextLabel5, 2, 0);

  newKinetics = new QPushButton(this, "newKinetics");
  newKinetics->setText(trUtf8("&New Kinetics"));

  ReactionsWidget1Layout->addWidget(newKinetics, 5, 1);
  QSpacerItem* spacer_2 = new QSpacerItem(111, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);
  ReactionsWidget1Layout->addItem(spacer_2, 5, 2);

  CheckBox = new QCheckBox(this, "CheckBox");
  CheckBox->setText(trUtf8("Reversible"));

  ReactionsWidget1Layout->addWidget(CheckBox, 5, 3);

  connect(commitChanges, SIGNAL(clicked()), this, SLOT(slotBtnOKClicked()));
  connect(cancelChanges, SIGNAL(clicked()), this, SLOT(slotBtnCancelClicked()));
  connect(this, SIGNAL(signal_emitted(const QString &)), (ListViews*)parent, SLOT(slotReactionTableChanged(const QString &)));
  connect(CheckBox, SIGNAL(clicked()), this, SLOT(slotCheckBoxClicked()));
  connect(ComboBox1, SIGNAL(activated(const QString &)), this, SLOT(slotComboBoxSelectionChanged(const QString &)));
  connect(LineEdit2, SIGNAL(edited()), this, SLOT(slotLineEditChanged()));
  connect(newReaction, SIGNAL(clicked()), this, SLOT(slotBtnNewClicked()));
  connect(this, SIGNAL(new_reaction()), (ListViews*)parent, SLOT(slotNewReaction()));

  connect(this, SIGNAL(leaf(CModel*)), (ListViews*)parent, SLOT(loadReactionsNodes(CModel*)));
  connect(this, SIGNAL(updated()), (ListViews*)parent, SLOT(dataModelUpdated()));
}

ReactionsWidget1::~ReactionsWidget1()
{}

/*This function is used to connect this class to the listviews
    class to basically choose the right widget to display   */
int ReactionsWidget1::isName(QString setValue)
{
  if (mModel == NULL)
    {
      return 0;
    }

  if (mModel->getReactions().getIndex((std::string)setValue.latin1()) != C_INVALID_INDEX)
    {
      loadName(setValue);
      name = setValue;
      return 1;
    }
  else
    return 0;
}

/*This function is to load the model for the reactions*/
void ReactionsWidget1::loadReactions(CModel *model)
{
  if (model != NULL)
    {
      mModel = model;
    }
}

/* This function loads the reactions widget when its name is
   clicked in the tree   */
void ReactionsWidget1::loadName(QString setValue)
{
  if (mModel == NULL)
    {
      return;
    }

  name = setValue;
  CReaction *reactn;
  CCopasiVectorNS < CReaction > & reactions = mModel->getReactions();
  reactn = reactions[(std::string)setValue.latin1()];
  TriLogic reversible;
  if (reactn->isReversible() == false)
    reversible = TriFalse;
  else
    reversible = TriTrue;

  const CCopasiVectorN < CFunction > & Functions =
    Copasi->pFunctionDB->suitableFunctions(reactn->getChemEq().getSubstrates().size(),
                                           reactn->getChemEq().getSubstrates().size(),
                                           reversible);
  //const CFunction *function;

  const CChemEq * chemEq;

  ComboBox1->clear();

  LineEdit1->setText(reactn->getName().c_str());
  //  Reaction1_Name = new QString(reactn->getName().c_str());

  chemEq = & reactn->getChemEq();
  LineEdit2->setText(chemEq->getChemicalEquationConverted().c_str());

  LineEdit3->setText(QString::number(reactn->getFlux()));

  //function = &reactn->getFunction();  // function seems not to be used afterwards
  //function1 = &reactn->getFunction(); // function1 seems not to be used either
  //ComboBox1->insertItem(function->getName().c_str(), m);

  QStringList comboEntries;
  QString comboEntry;
  unsigned int temp2;

  for (temp2 = 0; temp2 < Functions.size(); temp2++)
    {
      const CFunction *function = Functions[temp2];
      comboEntry = function->getName().c_str();
      comboEntries.push_front(comboEntry);
    }

  ComboBox1->insertStringList(comboEntries, -1);

  CheckBox->setChecked(false);

  if (reactn->isReversible() == true)
    {
      CheckBox->setChecked(true);
    }

  table->setNumCols(1);
  table->horizontalHeader()->setLabel(0, "Value");
  table->setColumnWidth (0, 200);

  if (&reactn->getFunction())
    {
      comboEntry = reactn->getFunction().getName().c_str();
      ComboBox1->setCurrentText(comboEntry);
      slotComboBoxSelectionChanged(comboEntry);
    }
  //  slotComboBoxSelectionChanged(reactn->getFunction().getName().c_str());
  //emit sideySignal();
}

/*This slot is activated when the cancel button is clicked.It basically cancels any changes that
  are made.It does this by emiiting a signal whcih si connected to the listviews and it reloads 
  the widget with the initial values.*/
void ReactionsWidget1::slotBtnCancelClicked()
{
  //QMessageBox::information(this, "Reactions Widget", "Do you really want to cancel changes");
  emit signal_emitted(name);
}

/*This slot is connected to the commit changes button.There is a difference between commit
  changes and save changes using the icon on the toolbar, but I am not sure  what each one 
  does and what is the difference between them.Have to ask Dr Hoops about it.*/
void ReactionsWidget1::slotBtnOKClicked()
{
  /*This code is to save the changes in the reaction*/
  CReaction *pReaction = mModel->getReactions()[name.latin1()];

  if (pReaction->getName() != LineEdit1->text().latin1())
    if (pReaction->setName(LineEdit1->text().latin1()))
      name = LineEdit1->text();

  CChemEq ChemEq;
  ChemEq.setChemicalEquation(LineEdit2->text().latin1());

  if (ChemEq.getChemicalEquationConverted() !=
      pReaction->getChemEq().getChemicalEquationConverted())
    {
      slotLineEditChanged();
      //        pReaction->setChemEq(LineEdit2->text().latin1());
      //        pReaction->compileChemEq(mModel->getCompartments());
    }

  if (!&pReaction->getFunction())
    pReaction->setFunction(ComboBox1->currentText().latin1());
  else if (pReaction->getFunction().getName() != ComboBox1->currentText().latin1())
    pReaction->setFunction(ComboBox1->currentText().latin1());

  CCopasiVector< CReaction::CId2Metab > & Substrates =
    pReaction->getId2Substrates();
  Substrates.cleanup();
  CCopasiVector< CReaction::CId2Metab > & Products =
    pReaction->getId2Products();
  Products.cleanup();
  CCopasiVector< CReaction::CId2Metab > & Modifiers =
    pReaction->getId2Modifiers();
  Modifiers.cleanup();
  CCopasiVector< CReaction::CId2Param > & Parameters =
    pReaction->getId2Parameters();
  Parameters.cleanup();

  const CFunctionParameters & Variables =
    pReaction->getFunction().getParameters();

  unsigned C_INT32 i, imax = std::min(Variables.size(), (unsigned C_INT32) table->numRows());
  unsigned C_INT32 j, jmax;
  unsigned C_INT32 l;
  CReaction::CId2Metab Metabolite;
  CReaction::CId2Param Parameter;
  const CCopasiVectorN< CMetab > & MetaboliteList = mModel->getMetabolites();

  std::string ThisUsage;
  for (i = 0, l = 0; i < imax; i++, l++)
    {
      ThisUsage = Variables[i]->getUsage();
      if (ThisUsage == "PARAMETER")
        {
          Parameter.setIdentifierName(table->verticalHeader()->label(l).latin1());
          Parameter.setValue(table->text(l, 0).toDouble());
          Parameters.add(Parameter);
        }
      else
        {
          if (Variables[i]->getType() < CFunctionParameter::VINT32)
            {
              Metabolite.setIdentifierName(table->verticalHeader()->label(l).latin1());
              Metabolite.setMetaboliteName(table->text(l, 0).latin1());
              Metabolite.setCompartmentName(MetaboliteList[table->text(l, 0).latin1()]->getCompartment()->getName());

              if (ThisUsage == "SUBSTRATE")
                Substrates.add(Metabolite);
              else if (ThisUsage == "PRODUCT")
                Products.add(Metabolite);
              else if (ThisUsage == "MODIFIER")
                Modifiers.add(Metabolite);
            }
          else
            {
              if (ThisUsage == "SUBSTRATE")
                for (j = 0, jmax = 0; j < pReaction->getChemEq().getSubstrates().size(); j++)
                  jmax += pReaction->getChemEq().getSubstrates()[j]->getMultiplicity();
              else if (ThisUsage == "PRODUCT")
                for (j = 0, jmax = 0; j < pReaction->getChemEq().getProducts().size(); j++)
                  jmax += pReaction->getChemEq().getProducts()[j]->getMultiplicity();

              for (j = 0; j < jmax; j++)
                {
                  Metabolite.setIdentifierName(table->verticalHeader()->label(l).latin1());
                  Metabolite.setMetaboliteName(table->text(l, 0).latin1());
                  Metabolite.setCompartmentName(MetaboliteList[table->text(l, 0).latin1()]->getCompartment()->getName());

                  if (ThisUsage == "SUBSTRATE")
                    Substrates.add(Metabolite);
                  else if (ThisUsage == "PRODUCT")
                    Products.add(Metabolite);

                  l++;
                }
              l--;
            }
        }
    }

  pReaction->compile(mModel->getCompartments());

  emit updated();
  emit leaf(mModel);
  emit signal_emitted(name);
  //   CChemEq * chem;
  //   chem = & reactn1->getChemEq();
  //   chem->setChemicalEquation(chemical_reaction->latin1());
  //   reactn1->setChemEq(chemical_reaction->latin1());
  //   reactn1->save(*Rtn);
  //   Copasi->Model->save(*Rtn);
}

/*This slot is activated when the check box is clicked.It needs to have functionality to
  make and update changes in the "Chemical Reaction" Text box and the "Kinetics" Combobox.*/

void ReactionsWidget1::slotCheckBoxClicked()
{
  CCopasiVectorNS < CReaction > & reactions = mModel->getReactions();
  reactn1 = reactions[(std::string)name.latin1()];
  const CChemEq * chemEq1;

  chemEq1 = & reactn1->getChemEq();
  std::string chemEq2 = chemEq1->getChemicalEquationConverted();
  QString chemical_reaction = chemEq2.c_str();
  TriLogic reversible;
  if (reactn1->isReversible())
    {
      reversible = TriTrue;
    }
  else
    {
      reversible = TriFalse;
    }

  if (!chemical_reaction.length())
    return;
  if (CheckBox->isChecked() == false && reactn1->isReversible() == true)
    {
      int i = chemical_reaction.find ("=", 0, true);
      chemical_reaction = chemical_reaction.replace(i, 1, "->");
      reversible = TriFalse;
    }
  else if (CheckBox->isChecked() == true && reactn1->isReversible() == false)
    {
      int i = chemical_reaction.find ("->", 0, true);
      chemical_reaction = chemical_reaction.replace(i, 2, "=");
      reversible = TriTrue;
    }
  if (chemical_reaction.length())
    {
      const std::string chemEq3 = chemical_reaction.latin1();
      //chemEq1->setChemicalEquation(chemEq3); //is this really necessary?
      reactn1->setChemEq(chemEq3);
    }

  LineEdit2->setText(chemEq1->getChemicalEquationConverted().c_str());

  ComboBox1->clear();
  const CCopasiVectorN < CFunction > & Functions =
    Copasi->pFunctionDB->suitableFunctions(reactn1->getChemEq().getSubstrates().size(),
                                           reactn1->getChemEq().getSubstrates().size(),
                                           reversible);
  QStringList comboEntries;
  QString comboEntry;
  unsigned int temp2;

  for (temp2 = 0; temp2 < Functions.size(); temp2++)
    {
      const CFunction *function = Functions[temp2];
      comboEntry = function->getName().c_str();
      comboEntries.push_back(comboEntry);
    }

  ComboBox1->insertStringList(comboEntries, -1);
  QString comboValue = ComboBox1->currentText();
  slotComboBoxSelectionChanged(comboValue);
  //QMessageBox::information(this, "Reactions Widget", "You need to change the Chemical Equation and a select a new Kinetics type");
}

void ReactionsWidget1::slotComboBoxSelectionChanged(const QString & p2)
{
  const std::string p1 = p2.latin1();
  const CFunction * pFunction = Copasi->pFunctionDB->findLoadFunction(p1);
  const CFunctionParameters & functionParameters = pFunction->getParameters();

  unsigned C_INT32 i, imax;
  unsigned C_INT32 j, jmax;

  CChemEq ChemicalEquation;
  ChemicalEquation.setChemicalEquation(LineEdit2->text().latin1());
  const CCopasiVector< CChemEqElement > * pElementList;
  const std::string * pName;

  /* build list of substrates */
  QStringList SubstrateNames;
  std::vector< const std::string * > Substrates;
  pElementList = &ChemicalEquation.getSubstrates();
  for (i = 0, imax = pElementList->size(); i < imax; i++)
    {
      pName = &(*pElementList)[i]->getMetaboliteName();
      SubstrateNames.push_back(pName->c_str());
      jmax = (unsigned C_INT32)(*pElementList)[i]->getMultiplicity();
      for (j = 0; j < jmax; j++)
        Substrates.push_back(pName);
    }

  /* build list of products */
  QStringList ProductNames;
  std::vector< const std::string * > Products;
  pElementList = &ChemicalEquation.getProducts();
  for (i = 0, imax = pElementList->size(); i < imax; i++)
    {
      pName = &(*pElementList)[i]->getMetaboliteName();
      ProductNames.push_back(pName->c_str());
      jmax = (unsigned C_INT32)(*pElementList)[i]->getMultiplicity();
      for (j = 0; j < jmax; j++)
        Products.push_back(pName);
    }

  /* build list of modifiers */
  QStringList ModifierNames;
  const CCopasiVector< CMetab > & MetaboliteList = mModel->getMetabolites();
  for (i = 0, imax = MetaboliteList.size(); i < imax; i++)
    ModifierNames.push_back(MetaboliteList[i]->getName().c_str());

  /* build list of all needed variables */
  std::vector< std::pair< QString, QString > > VariableList;
  std::pair< QString, QString > Variable;
  const CFunctionParameter * pParameter;

  for (i = 0, imax = functionParameters.size(); i < imax; i++)
    {
      pParameter = functionParameters[i];
      Variable.second = pParameter->getUsage().c_str();

      if (pParameter->getType() < CFunctionParameter::VINT32)
        {
          Variable.first = pParameter->getName().c_str();
          VariableList.push_back(Variable);
        }
      else
        {
          if (Variable.second == "SUBSTRATE")
            jmax = Substrates.size();
          else if (Variable.second == "PRODUCT")
            jmax = Products.size();
          else
            jmax = 0;

          for (j = 0; j < jmax; j++)
            {
              Variable.first = pParameter->getName().c_str();
              Variable.first += "_";
              Variable.first += QString::number(j);
              VariableList.push_back(Variable);
            }
        }
    }

  //for clearing the values of the table
  for (i = 0, imax = table->numRows(); i < imax; i++)
    table->clearCell(i, 0);

  /* build the table according to the variable list*/
  table->setNumRows(VariableList.size());
  table->ensureCellVisible(VariableList.size() + 1, 0);

  QComboTableItem * pItem;
  unsigned C_INT32 nSubstrate = 0;
  unsigned C_INT32 nProduct = 0;
  bool HaveParameters = false;
  CReaction *pReaction = mModel->getReactions()[name.latin1()];
  if (&pReaction->getFunction() == pFunction)
    HaveParameters = true;
  for (i = 0, imax = table->numRows(); i < imax; i++)
    {
      table->verticalHeader()->setLabel(i, VariableList[i].first);
      if (VariableList[i].second == "SUBSTRATE")
        {
          pItem = new QComboTableItem(table, SubstrateNames, true);
          pItem->setCurrentItem(Substrates[nSubstrate++]->c_str());
          table->setItem(i, 0, pItem);
        }
      else if (VariableList[i].second == "PRODUCT")
        {
          pItem = new QComboTableItem(table, ProductNames, true);
          pItem->setCurrentItem(Products[nProduct++]->c_str());
          table->setItem(i, 0, pItem);
        }
      else if (VariableList[i].second == "MODIFIER")
        {
          pItem = new QComboTableItem(table, ModifierNames, true);
          j = pReaction->
              getId2Modifiers().getIndex(VariableList[i].first.latin1());
          if (HaveParameters && j != C_INVALID_INDEX)
            pItem->setCurrentItem(pReaction->getId2Modifiers()[j]->getMetaboliteName().c_str());
          else
            pItem->setCurrentItem(0);
          table->setItem(i, 0, pItem);
        }
      else if (VariableList[i].second == "PARAMETER")
        {
          //Liang
          QTableItem * pItem;
          j = pReaction->
              getId2Parameters().getIndex(VariableList[i].first.latin1());
          if (HaveParameters && j != C_INVALID_INDEX)
            pItem = new QTableItem (table, QTableItem::Always, QString::number(pReaction->getId2Parameters()[j]->getValue()));
          else
            pItem = new QTableItem (table, QTableItem::Always, QString::number(1.0));
          table->setItem(i, 0, pItem);
          /*
                    j = pReaction->
                        getId2Parameters().getIndex(VariableList[i].first.latin1());
                    if (HaveParameters && j != C_INVALID_INDEX)
                      table->setText(i, 0, QString::number(pReaction->getId2Parameters()[j]->getValue()));
                    else
                      table->setText(i, 0, QString::number(1.0));
          */
        }
    }
}

/*This function is called when the "Chemical Reaction" LineEdit is changed.*/
void ReactionsWidget1::slotLineEditChanged()
{
  const QString & chemreactn = LineEdit2->text();
  const std::string & changed_chemical_reaction = chemreactn.latin1();
  CCopasiVectorNS < CReaction > & reactions1 = mModel->getReactions();
  CReaction *reactn1;
  reactn1 = reactions1[(std::string)name.latin1()];
  const CChemEq * chemEq1;
  chemEq1 = & reactn1->getChemEq();
  //bool status;
  //status = chemEq1->setChemicalEquation(changed_chemical_reaction); //is this necessary?
  if (changed_chemical_reaction.length())
    reactn1->setChemEq(changed_chemical_reaction);
  if (reactn1->isReversible() == true)
    {
      CheckBox->setChecked(true);
    }
  else
    {
      CheckBox->setChecked(false);
    }
  slotCheckBoxClicked();
}

void ReactionsWidget1::slotBtnNewClicked()
{
  // QMessageBox::information(this, "Reactions Widget", "adding a new reaction");
  emit new_reaction();
}
