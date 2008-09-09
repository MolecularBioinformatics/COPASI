// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/layoutUI/CQLayoutMainWindow.cpp,v $
//   $Revision: 1.75 $
//   $Name:  $
//   $Author: gauges $
//   $Date: 2008/09/09 03:40:41 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "CQLayoutMainWindow.h"

#include <qaction.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qvbox.h>

#include <qwt_slider.h>

#include <iostream>
#include <math.h>

#include "copasi/layout/CLBase.h"
#include "copasi/layout/CListOfLayouts.h"
#include "copasi/UI/CopasiFileDialog.h"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "CQCurrentValueTable.h"
#include "CQGLNetworkPainter.h"
#include "CQGLViewport.h"
#include "CVisParameters.h"
#include "FontChooser.h"
#include "NodeSizePanel.h"
#include "ParaPanel.h"
#include "CQPlayerControlWidget.h"
#include "load_data.xpm"

using namespace std;

CQLayoutMainWindow::CQLayoutMainWindow(CLayout* pLayout, QWidget *parent, const char *name) : QMainWindow(parent, name)
{
  mCurrentPlace = QString::null;
  mDataPresent = false;
  pVisParameters = new CVisParameters();
  setCaption(tr("Reaction network graph"));
  createActions();
  createMenus();

  mpMainBox = new QVBox(this);

  // create split window with parameter panel and graph panel
  mpSplitter = new QSplitter(Qt::Horizontal, mpMainBox);
  mpSplitter->setCaption("Test");

  mpInfoBox = new QVBox(mpSplitter);

  mpParaPanel = new ParaPanel(mpInfoBox);

  mpValTable = new CQCurrentValueTable(mpInfoBox);
  mpValTable->setMinimumSize(100, 150);
  mpValTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

  // add two buttons in an horizontal box
  QHBox *buttonBox = new QHBox(mpInfoBox);
  QPushButton *pcheckAllButton = new QPushButton("Check all", buttonBox);
  QPushButton *puncheckAllButton = new QPushButton("Uncheck all", buttonBox);
  connect(pcheckAllButton , SIGNAL(clicked()), this, SLOT(checkAllCheckboxesInTable()));
  connect(puncheckAllButton , SIGNAL(clicked()), this, SLOT(uncheckAllCheckboxesInTable()));

  pcheckAllButton->setMinimumSize(40, 30);
  puncheckAllButton->setMinimumSize(40, 30);

  pcheckAllButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  puncheckAllButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  buttonBox->setFixedHeight(pcheckAllButton->minimumHeight() + 6);

  buttonBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  mpParaPanel->setMinimumHeight(250);
  mpParaPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  mpInfoBox->setMinimumHeight(mpParaPanel->minimumHeight() + mpValTable->minimumHeight() + buttonBox->minimumHeight() + 25);
  // Create OpenGL widget
  mpGLViewport = new CQGLViewport(mpSplitter, "Network layout");
  if (pLayout != NULL)
    {
      CLDimensions dim = pLayout->getDimensions();
      CLPoint c1;
      CLPoint c2(dim.getWidth(), dim.getHeight());
      mpGLViewport->createGraph(pLayout); // create local data structures
    }
  mpSplitter->setResizeMode(mpInfoBox, QSplitter::KeepSize);

  mpFrame = new QFrame(mpMainBox);

  this->mpControlWidget = new CQPlayerControlWidget(mpFrame);
  connect(this->mpControlWidget, SIGNAL(play()), this, SLOT(startAnimation()));
  connect(this->mpControlWidget, SIGNAL(pause()), this, SLOT(pauseAnimation()));
  connect(this->mpControlWidget, SIGNAL(stop()), this, SLOT(stopAnimation()));
  connect(this->mpControlWidget, SIGNAL(forward()), this, SLOT(forwardAnimation()));
  connect(this->mpControlWidget, SIGNAL(backward()), this, SLOT(backwardAnimation()));
  connect(this->mpControlWidget, SIGNAL(step_backward()), this, SLOT(stepBackwardAnimation()));
  connect(this->mpControlWidget, SIGNAL(step_forward()), this, SLOT(stepForwardAnimation()));

  //mStartIcon = createStartIcon();
  //mStopIcon = createStopIcon();

  //mpStartStopButton = new QPushButton(mpFrame, "start/stop button");

  //connect(mpStartStopButton, SIGNAL(clicked()), this, SLOT(startAnimation()));
  //mpStartStopButton->setIconSet(mStartIcon);
  //mpStartStopButton->setEnabled(true);

  mpTimeSlider = new QwtSlider(mpFrame, Qt::Horizontal, QwtSlider::BottomScale, QwtSlider::BgTrough);
  mpTimeSlider->setRange(0, 100, 1, 0);
  mpTimeSlider->setValue(0.0);
  this->mpTimeSlider->setEnabled(false);

  mpTimeSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  mpFrame->setFixedHeight(68);
  connect(mpTimeSlider, SIGNAL(valueChanged(double)),
          this, SLOT(showStep(double)));

  QGridLayout* bottomBoxlayout = new QGridLayout(mpFrame, 2, 2, 3, 6);
  bottomBoxlayout->addMultiCellWidget(mpTimeSlider, 0, 1, 1, 1, Qt::AlignTop);
  //bottomBoxlayout->addWidget(mpStartStopButton, 0, 0);
  bottomBoxlayout->addMultiCellWidget(this->mpControlWidget, 0, 1, 0, 0, Qt::AlignTop);
  QSpacerItem* theSpacer = new QSpacerItem(20, 20);
  bottomBoxlayout->addItem(theSpacer, 1, 0);

  setCentralWidget(mpMainBox);
  loadData(); // try to load data (if already present)
  this->mpToolbar = new QToolBar(this, "layout toolbar");
  this->mpLoadDataAction->addTo(this->mpToolbar);
  this->mpToolbar->addSeparator();
  QLabel* pLabel = new QLabel("zoom factor:", this->mpToolbar);
  pLabel->show();
  this->mpZoomComboBox = new QComboBox("zoom box", this->mpToolbar);
  QStringList l;
  l.push_back("1%");
  l.push_back("2%");
  l.push_back("3%");
  l.push_back("4%");
  l.push_back("5%");
  l.push_back("10%");
  l.push_back("20%");
  l.push_back("30%");
  l.push_back("40%");
  l.push_back("50%");
  l.push_back("100%");
  l.push_back("200%");
  l.push_back("300%");
  l.push_back("400%");
  l.push_back("500%");
  this->mpZoomComboBox->insertStringList(l);
  this->mpZoomComboBox->setCurrentItem(10);
  this->mpZoomComboBox->setEditable(FALSE);
  this->mpZoomComboBox->show();
  connect(this->mpZoomComboBox, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
  connect(this->mpValTable , SIGNAL(changed()), this->mpGLViewport, SLOT(updateWidget()));
  connect(this->mpGLViewport->getPainter() , SIGNAL(signalZoomIn()), this, SLOT(slotZoomIn()));
  connect(this->mpGLViewport->getPainter() , SIGNAL(signalZoomOut()), this, SLOT(slotZoomOut()));
  this->show();
  //glPainter->drawGraph();
}

bool CQLayoutMainWindow::getAnimationRunning()
{
  if (pVisParameters != NULL)
    {
      return pVisParameters->animationRunning;
    }
  else
    return false;
}

void CQLayoutMainWindow::setAnimationRunning(bool animationRunningP)
{
  if (pVisParameters != NULL)
    {
      pVisParameters->animationRunning = animationRunningP;
    }
}

C_FLOAT64 CQLayoutMainWindow::getMinNodeSize()
{
  C_FLOAT64 minNodeSize = 10.0;
  if (pVisParameters != NULL)
    {

      //       if ((pVisParameters->mappingMode == CVisParameters::SIZE_DIAMETER_MODE) ||
      //           (pVisParameters->mappingMode == CVisParameters::SIZE_AREA_MODE))
      minNodeSize = pVisParameters->minNodeSize;
      //       else
      //         minNodeSize = 0.0; // color mode means: min h-value = 0;
    }
  return minNodeSize;
}

C_FLOAT64 CQLayoutMainWindow::getMaxNodeSize()
{
  C_FLOAT64 maxNodeSize = 100.0;
  if (pVisParameters != NULL)
    {
      //       if ((pVisParameters->mappingMode == CVisParameters::SIZE_DIAMETER_MODE) ||
      //           (pVisParameters->mappingMode == CVisParameters::SIZE_AREA_MODE))
      maxNodeSize = pVisParameters->maxNodeSize;
      //       else
      //         maxNodeSize = 359.0; // color mode means: max h-value < 360;
    }
  return maxNodeSize;
}

void CQLayoutMainWindow::setMinNodeSize(C_FLOAT64 minNdSize)
{
  if (pVisParameters != NULL)
    pVisParameters->minNodeSize = minNdSize;
}

void CQLayoutMainWindow::setMaxNodeSize(C_FLOAT64 maxNdSize)
{
  if (pVisParameters != NULL)
    pVisParameters->maxNodeSize = maxNdSize;
}

C_INT16 CQLayoutMainWindow::getFontSize()
{
  return mpGLViewport->getPainter()->getFontSize();
}

C_INT32 CQLayoutMainWindow::getStepsPerSecond()
{
  if (pVisParameters != NULL)
    {
      return pVisParameters->stepsPerSecond;
    }
  else
    return 2;
}

void CQLayoutMainWindow::setStepsPerSecond(C_INT16 val)
{
  if (pVisParameters != NULL)
    {
      pVisParameters->stepsPerSecond = val;
    }
}

C_INT32 CQLayoutMainWindow::getCurrentStep()
{
  return (C_INT32) this->mpTimeSlider->value();
}

C_INT16 CQLayoutMainWindow::getScalingMode()
{
  if (pVisParameters != NULL)
    {
      return pVisParameters->scalingMode;
    }
  else
    return CVisParameters::INDIVIDUAL_SCALING;
}

C_INT16 CQLayoutMainWindow::getMappingMode()
{
  if (pVisParameters != NULL)
    {
      return pVisParameters->mappingMode;
    }
  else
    return CVisParameters::SIZE_DIAMETER_MODE; // default mode
}

void CQLayoutMainWindow::createActions()
{
  //   mpOpenSBMLFile = new QAction("SBML",
  //                              "Load SBML file",
  //                              CTRL + Key_F,
  //                              this);
  //   mpOpenSBMLFile->setStatusTip("Load SBML file with/without layout");
  //   connect(mpOpenSBMLFile, SIGNAL(activated()) , this, SLOT(loadSBMLFile()));

  //   openDataFile = new QAction("data",
  //                              "Load Simulation Data",
  //                              CTRL + Key_D,
  //                              this);
  //   openDataFile->setStatusTip("Load simulation data");
  //   connect(openDataFile, SIGNAL(activated()), this, SLOT(loadData()));

  mpCloseAction = new QAction ("close",
                               "Close Window",
                               CTRL + Key_Q ,
                               this);
  mpCloseAction->setStatusTip("Close Layout Window");
  connect(mpCloseAction, SIGNAL(activated()), this, SLOT(closeApplication()));

  mpRunAnimation = new QAction("animate",
                               "Run animation",
                               CTRL + Key_A,
                               this);
  mpRunAnimation->setStatusTip("show complete animation sequence of current times series");
  connect(mpRunAnimation, SIGNAL(activated()), this, SLOT(showAnimation()));
  mpRunAnimation->setEnabled(true);
  mDataPresent = false;

  mpCreatePicture = new QAction("image",
                                "Create image",
                                CTRL + Key_I,
                                this);
  mpCreatePicture->setStatusTip("create a picture from the current view and save it to file");
  connect(mpCreatePicture, SIGNAL(activated()), this, SLOT(saveImage()));

  mpRectangluarShape = new QAction ("rectangle",
                                    "rectangle",
                                    CTRL + Key_R ,
                                    this);
  mpRectangluarShape->setStatusTip("Show labels as rectangles");
  connect(mpRectangluarShape, SIGNAL(activated()), this, SLOT(mapLabelsToRectangles()));

  mpCircularShape = new QAction ("circle",
                                 "circle",
                                 CTRL + Key_C ,
                                 this);
  connect(mpCircularShape, SIGNAL(activated()), this, SLOT(mapLabelsToCircles()));
  mpCircularShape->setStatusTip("Show labels as circles");

  mpMimaNodeSizes = new QAction ("minmax",
                                 "Set Min/Max Node Sizes",
                                 CTRL + Key_M,
                                 this);

  connect(mpMimaNodeSizes, SIGNAL(activated()), this, SLOT(changeMinMaxNodeSizes()));
  mpMimaNodeSizes->setToolTip("Change Min/Max for node sizes within animation");

  mpSFontSize = new QAction("fontsiz",
                            "Set Font Size",
                            CTRL + Key_F,
                            this);

  connect(mpSFontSize, SIGNAL(activated()), this, SLOT(changeFontSize()));
  mpSFontSize->setToolTip("Change the font size of the node labels in the graph view");

  //automaticRescaleToggle = new QAction ("autorescale",
  //                                      "Automatic Rescaling of Graph",
  //                                      CTRL + Key_A,
  //                                      this);

  //automaticRescaleToggle->setToggleAction(true);
  //automaticRescaleToggle->setOn(true);
  //automaticRescaleToggle = new QCheckBox("Automatic Rescaling of Graph", this);
  //automaticRescaleToggle->setChecked(true);

  //connect(automaticRescaleToggle, SIGNAL(toggled(bool)), this, SLOT(toggleAutomaticRescaling(bool)));
  //automaticRescaleToggle->setToolTip("Enable/disable automatic rescaling of graph when panel is resized");
  this->mpLoadDataAction = new QAction(QPixmap(load_data_xpm),
                                       "load data",
                                       QKeySequence(),
                                       this);
  connect(this->mpLoadDataAction, SIGNAL(activated()), this, SLOT(loadData()));
}

void CQLayoutMainWindow::createMenus()
{
  mpFileMenu = new QPopupMenu(this);
  mpFileMenu->insertSeparator();
  mpCloseAction->addTo(mpFileMenu);

  mpActionsMenu = new QPopupMenu(this);
  mpRunAnimation->addTo(mpActionsMenu);

  mpCreatePicture->addTo(mpActionsMenu);

  mpLabelShapeMenu = new QPopupMenu(this);

  mpRectangluarShape->addTo(mpLabelShapeMenu);
  mpCircularShape->addTo(mpLabelShapeMenu);

  mpViewMenu = new QPopupMenu(this);
  mpViewMenu->insertItem("Reset View", this, SLOT(slotResetView()));
  mpZoomMenu = new QPopupMenu(this);
  mpViewMenu->insertItem("Zoom", mpZoomMenu);
  int id;
  id = mpZoomMenu->insertItem("1%", 1);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("2%", 2);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("3%", 3);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("4%", 4);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("5%", 5);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("10%", 10);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("20%", 20);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("30%", 30);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("40%", 40);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("50%", 50);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("100%", 100);
  mpZoomMenu->setItemChecked(id, true);
  id = mpZoomMenu->insertItem("200%", 200);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("300%", 300);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("400%", 400);
  mpZoomMenu->setItemChecked(id, false);
  id = mpZoomMenu->insertItem("500%", 500);
  mpZoomMenu->setItemChecked(id, false);
  connect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));

  mpOptionsMenu = new QPopupMenu(this);
  mpOptionsMenu->insertItem("Shape of Label", mpLabelShapeMenu);
  mpMimaNodeSizes->addTo(mpOptionsMenu);
  mpSFontSize->addTo(mpOptionsMenu);

  menuBar()->insertItem("File", mpFileMenu);
  menuBar()->insertItem("Actions", mpActionsMenu);
  menuBar()->insertItem("View", this->mpViewMenu);
  menuBar()->insertItem("Options", mpOptionsMenu);
}

//void CQLayoutMainWindow::contextMenuEvent(QContextMenuEvent *cme){
// QPopupMenu *contextMenu = new QPopupMenu(this);
// exitAction->addTo(contextMenu);
// contextMenu->exec(cme->globalPos());
//}

void CQLayoutMainWindow::loadSBMLFile()
{
  //string filename = "/localhome/ulla/project/data/peroxiShortNew.xml"; // test file
  //string filename = "/home/ulla/project/simulation/data/peroxiShortNew.xml";
  //SBMLDocumentLoader docLoader;
  //network *networkP = docLoader.loadDocument(filename.c_str());

  //glPainter->createGraph(networkP);
  std::cout << "load SBMLfile" << std::endl;
  CListOfLayouts *pLayoutList;
  if (CCopasiDataModel::Global != NULL)
    {
      pLayoutList = CCopasiDataModel::Global->getListOfLayouts();
    }
  else
    pLayoutList = NULL;

  // enable double buffering
  //QGLFormat f;
  //f.setDoubleBuffer(TRUE);
  //glPainter = new CQGLNetworkPainter(f, scrollView->viewport());
  mpGLViewport = new CQGLViewport(mpSplitter);
  if (pLayoutList != NULL)
    {
      CLayout * pLayout;
      if (pLayoutList->size() > 0)
        {
          pLayout = (*pLayoutList)[0];
          CLDimensions dim = pLayout->getDimensions();
          CLPoint c1;
          CLPoint c2(dim.getWidth(), dim.getHeight());
          mpGLViewport->createGraph(pLayout); // create local data structures
        }
    }
}

void CQLayoutMainWindow::mapLabelsToCircles()
{
  if (mpGLViewport->getPainter() != NULL)
    {
      mpGLViewport->getPainter()->mapLabelsToCircles();
      if (mpGLViewport->getPainter()->getNumberOfSteps() > 0)
        showStep(this->mpTimeSlider->value());
    }
}

void CQLayoutMainWindow::mapLabelsToRectangles()
{
  if (mpGLViewport->getPainter() != NULL)
    {
      mpGLViewport->getPainter()->mapLabelsToRectangles();
    }
}

void CQLayoutMainWindow::changeMinMaxNodeSizes()
{
  //std::cout << "change min/max values for node sizes" << std::endl;
  NodeSizePanel *panel = new NodeSizePanel(this);
  panel->exec();
}

void CQLayoutMainWindow::changeFontSize()
{
  // choose new font size (setting of size will be invoked from FontChooser panel
  FontChooser *fCh = new FontChooser(this);
  fCh->exec();
}

void CQLayoutMainWindow::loadData()
{
  bool successfulP = mpGLViewport->getPainter()->createDataSets();
  if (successfulP)
    {
      // set the number of steps in the control widget and place the current
      // step at 0
      this->mpControlWidget->setCurrentStep(0);
      this->mpTimeSlider->setEnabled(true);
      this->mDataPresent = true;
      mpParaPanel->enableStepNumberChoice();
      int maxVal = mpGLViewport->getPainter()->getNumberOfSteps();
      this->mpTimeSlider->setRange(0, maxVal - 1);
      this->mpControlWidget->setNumSteps(maxVal);
      mpGLViewport->getPainter()->updateGL();
      if (this->mpGLViewport->getPainter()->isCircleMode())
        {
          showStep(this->mpTimeSlider->value());
        }
    }
}

void CQLayoutMainWindow::insertValueTable(CDataEntity dataSet)
{
  int i = 0;
  std::string key, name;
  C_FLOAT64 val;
  mpValTable->setNumRows(dataSet.getNumberOfElements());
  mpValTable->setNumCols(2);
  while ((key = mpGLViewport->getPainter()->getNodeNameEntry(i)) != "")
    {
      name = this->mpGLViewport->getPainter()->getNameForNodeKey(key);
      val = dataSet.getOrigValueForSpecies(key); // would be (- DBL_MAX) if key not present
      mpValTable->setRowInTable(i, key, name, val);
      //std::cout << i << "   "  << key << "  " << val << std::endl;
      i++;
    }
  //mpParaPanel->update();
}

void CQLayoutMainWindow::updateValueTable(CDataEntity dataSet)
{
  int i = 0;
  std::string key, name;
  C_FLOAT64 val;
  while ((key = mpGLViewport->getPainter()->getNodeNameEntry(i)) != "")
    {
      name = mpGLViewport->getPainter()->getNameForNodeKey(key);
      val = dataSet.getOrigValueForSpecies(key); // would be (- DBL_MAX) if key not present
      if (mpValTable->numRows() > i)
        {
          mpValTable->updateRowInTable(i, val);
        }
      i++;
    }
}

void CQLayoutMainWindow::checkAllCheckboxesInTable()
{
  mpValTable->setAllBoxesChecked();
}

void CQLayoutMainWindow::uncheckAllCheckboxesInTable()
{
  mpValTable->setAllBoxesUnchecked();
}

// adds the item given by s to the list of items to animate (no change, if it is already present)
void CQLayoutMainWindow::addItemInAnimation (std::string key)
{
  //std::cout << "add " << key << std::endl;
  mpGLViewport->getPainter()->setItemAnimated(key, true);
}

// removes the item given by s from the list of items to animate (no change, if it is not present in the list)
void CQLayoutMainWindow::removeItemInAnimation (std::string key)
{
  //std::cout << "remove " << key << std::endl;
  mpGLViewport->getPainter()->setItemAnimated(key, false);
}

void CQLayoutMainWindow::showAnimation()
{
  startAnimation();
}

void CQLayoutMainWindow::startAnimation()
{
  if (!this->mDataPresent)
    this->loadData(); // look for data
  if (this->mDataPresent)
    {// only if time series data present
      pVisParameters->animationRunning = true;
      this->mpTimeSlider->setEnabled(false);
      mpGLViewport->getPainter()->runAnimation();
      this->mpControlWidget->setNumSteps(this->mpGLViewport->getPainter()->getNumberOfSteps());
      //exchange icon and callback for start/stop button
      //disconnect(mpStartStopButton, SIGNAL(clicked()), this, SLOT(startAnimation()));
      //connect(mpStartStopButton, SIGNAL(clicked()), this, SLOT(stopAnimation()));
      //mpStartStopButton->setIconSet(mStopIcon);
      mpParaPanel->disableParameterChoice();
      mpParaPanel->disableStepNumberChoice();
    }
  else
    {
      QMessageBox::warning (this, "Missing Data", "No data found: \nYou first have to create a time course.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    }
}

void CQLayoutMainWindow::saveImage()
{
  QImage img = mpGLViewport->getPainter()->getImage();
  QString filename = CopasiFileDialog::getSaveFileName(this, "Save Image Dialog", mCurrentPlace, "PNG Files (*.png);;All Files (*.*);;", "Choose a filename to save the image under");
  if (filename)
    {
      img.save(filename, "PNG");
      mCurrentPlace = filename;
    }
}

void CQLayoutMainWindow::pauseAnimation()
{
  pVisParameters->animationRunning = false;
  this->mpTimeSlider->setEnabled(true);
  mpParaPanel->enableParameterChoice();
  mpParaPanel->enableStepNumberChoice();
}

void CQLayoutMainWindow::endOfAnimationReached()
{
  this->pauseAnimation();
}

void CQLayoutMainWindow::showStep(double i)
{
  this->mpControlWidget->setCurrentStep(static_cast<int>(i));
  mpGLViewport->getPainter()->showStep(static_cast<int>(i));
  mpGLViewport->getPainter()->updateGL();
  mpParaPanel->setStepNumber(static_cast<int>(i));
  CDataEntity* srcData = mpGLViewport->getPainter()->getDataSetAt(static_cast<int>(i));
  if (srcData)
    {
      updateValueTable(*srcData);
    }
}

void CQLayoutMainWindow::changeStepValue(C_INT32 i)
{
  mpTimeSlider->setValue(i);
}

void CQLayoutMainWindow::setIndividualScaling()
{
  pVisParameters->scalingMode = pVisParameters->INDIVIDUAL_SCALING;
  mpGLViewport->getPainter()->rescaleDataSets(pVisParameters->INDIVIDUAL_SCALING);
  showStep(this->mpTimeSlider->value());
}

void CQLayoutMainWindow::setGlobalScaling()
{
  pVisParameters->scalingMode = pVisParameters->GLOBAL_SCALING;
  mpGLViewport->getPainter()->rescaleDataSets(pVisParameters->GLOBAL_SCALING);
  showStep(this->mpTimeSlider->value());
}

void CQLayoutMainWindow::setSizeMode()
{
  pVisParameters->mappingMode = CVisParameters::SIZE_DIAMETER_MODE;
  //glPainter->changeMinMaxNodeSize(getMinNodeSize(), getMaxNodeSize(),pVisParameters->scalingMode);
  mpGLViewport->getPainter()->rescaleDataSetsWithNewMinMax(0.0, 1.0, getMinNodeSize(), getMaxNodeSize(), pVisParameters->scalingMode); // only [0.240] of possible HSV values (not fill circle in order to get good color range)
  showStep(this->mpTimeSlider->value());
  //std::cout << "show Step: " << this->mpTimeSlider->value() << std::endl;
}

void CQLayoutMainWindow::setColorMode()
{
  pVisParameters->mappingMode = CVisParameters::COLOR_MODE;
  //glPainter->changeMinMaxNodeSize(pVisParameters->scalingMode); // rescaling, because min and max node size changed
  mpGLViewport->getPainter()->rescaleDataSetsWithNewMinMax(getMinNodeSize(), getMaxNodeSize(), 0.0, 1.0, pVisParameters->scalingMode); // rescaling, because min and max node size changed (interpretation as color value takes place elsewhere),only [0.240] of possible HSV values (not fill circle in order to get good color range)
  showStep(this->mpTimeSlider->value());
  //std::cout << "showStep: " << this->mpTimeSlider->value() << std::endl;
}

void CQLayoutMainWindow::setValueOnSlider(C_INT32 val)
{
  mpTimeSlider->setValue(val);
}

// set minimum possible node size for animation
void CQLayoutMainWindow::setMinValue(C_INT32 minNdSize)
{
  mpGLViewport->getPainter()->rescaleDataSetsWithNewMinMax(getMinNodeSize(), getMaxNodeSize(), minNdSize, getMaxNodeSize(), pVisParameters->scalingMode);
  setMinNodeSize(minNdSize);
  showStep(this->mpTimeSlider->value());
}

// set maximum possible node size for animation
void CQLayoutMainWindow::setMaxValue(C_INT32 maxNdSize)
{
  mpGLViewport->getPainter()->rescaleDataSetsWithNewMinMax(getMinNodeSize(), getMaxNodeSize(), getMinNodeSize(), maxNdSize, pVisParameters->scalingMode);
  setMaxNodeSize(maxNdSize);
  showStep(this->mpTimeSlider->value());
}

void CQLayoutMainWindow::setMinAndMaxValue(C_INT32 minNdSize, C_INT32 maxNdSize)
{
  mpGLViewport->getPainter()->rescaleDataSetsWithNewMinMax(getMinNodeSize(), getMaxNodeSize(), minNdSize, maxNdSize, pVisParameters->scalingMode);
  setMinNodeSize(minNdSize);
  setMaxNodeSize(maxNdSize);
  showStep(this->mpTimeSlider->value());
}

void CQLayoutMainWindow::setFontSizeForLabels(C_INT32 size)
{
  mpGLViewport->getPainter()->setFontSizeForLabels((unsigned int) size);
}

void CQLayoutMainWindow::closeApplication()
{
  close();
}

void CQLayoutMainWindow::closeEvent(QCloseEvent *event)
{
  if (maybeSave())
    {
      event->accept();
    }
  else
    {
      event->ignore();
    }
}

QIconSet CQLayoutMainWindow::createStartIcon()
{
  QImage img = QImage();
  C_INT32 w = 19;
  C_INT32 h = 19;
  img.create(w, h, 8, 2);
  img.setColor(0, qRgb(0, 0, 200));
  C_INT16 x, y;

  for (x = 0;x < w;x++)
    {
      for (y = 0;y < h;y++)
        {
          img.setPixel(x, y, 0);
        }
    }

  C_INT32 delta = 0;
  img.setColor(1, qRgb(255, 0, 0));
  for (x = 3;x < w - 3;x++)
    {
      for (y = 3 + delta;y < h - 3 - delta;y++)
        {
          img.setPixel(x, y, 1);
        }
      if (fmod((double) x, 2.0) == 0)
        delta++;
    }

  //QPixmap *pixmap = new QPixmap(20,20);
  QPixmap *pixmap = new QPixmap();
  pixmap->convertFromImage(img);
  //pixmap->fill(QColor(0,255,0));
  QIconSet iconset = QIconSet(*pixmap);
  delete pixmap;
  return iconset;
}

QIconSet CQLayoutMainWindow::createStopIcon()
{
  QImage img = QImage();
  C_INT32 w = 20;
  C_INT32 h = 20;
  img.create(w, h, 8, 2);
  img.setColor(0, qRgb(0, 0, 200));
  C_INT16 x, y;

  for (x = 0;x < w;x++)
    {
      for (y = 0;y < h;y++)
        {
          img.setPixel(x, y, 0);
          //uint *p = (uint *)img.scanLine(y) + x;
          //*p = qRgb(0,0,255);
        }
    }

  C_INT32 delta = 4;
  img.setColor(1, qRgb(255, 0, 0));
  for (x = (delta - 1);x <= (w - delta);x++)
    {
      for (y = (delta - 1);y <= (h - delta);y++)
        {
          img.setPixel(x, y, 1);
          //uint *p = (uint *)img.scanLine(y) + x;
          //*p = qRgb(0,0,255);
        }
    }

  QPixmap *pixmap = new QPixmap();
  //pixmap->fill(QColor(255, 0, 0));
  pixmap->convertFromImage(img);
  QIconSet iconset = QIconSet(*pixmap);
  delete pixmap;
  return iconset;
}

// returns true because window is opened from Copasi and can be easily reopened
bool CQLayoutMainWindow::maybeSave()
{
  //  int ret = QMessageBox::warning(this, "SimWiz",
  //                                 "Do you really want to quit?",
  //                                 //                   tr("Do you really want to quit?\n"
  //                                 //                      "XXXXXXXX"),
  //                                 QMessageBox::Yes | QMessageBox::Default,
  //                                 QMessageBox::No,
  //                                 QMessageBox::Cancel | QMessageBox::Escape);
  //  if (ret == QMessageBox::Yes)
  //    return true;
  //  else if (ret == QMessageBox::Cancel)
  //    return false;

  return true;
}

//int main(int argc, char *argv[]) {
// //cout << argc << "------" << *argv << endl;
// QApplication app(argc,argv);
// CQLayoutMainWindow win;
// //app.setMainWidget(&gui);
// win.resize(400,230);
// win.show();
// return app.exec();
//}

void CQLayoutMainWindow::slotActivated(int index)
{
  QString item = this->mpZoomComboBox->text(index);
  this->setZoomFactor(item.latin1());
  // update menu items
  int ids[] = {1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500};
  if (index >= 0 && index < 15)
    {
      unsigned int id = ids[index];
      disconnect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));
      this->mpZoomMenu->setItemChecked(1, false);
      this->mpZoomMenu->setItemChecked(2, false);
      this->mpZoomMenu->setItemChecked(3, false);
      this->mpZoomMenu->setItemChecked(4, false);
      this->mpZoomMenu->setItemChecked(5, false);
      this->mpZoomMenu->setItemChecked(10, false);
      this->mpZoomMenu->setItemChecked(20, false);
      this->mpZoomMenu->setItemChecked(30, false);
      this->mpZoomMenu->setItemChecked(40, false);
      this->mpZoomMenu->setItemChecked(50, false);
      this->mpZoomMenu->setItemChecked(100, false);
      this->mpZoomMenu->setItemChecked(200, false);
      this->mpZoomMenu->setItemChecked(300, false);
      this->mpZoomMenu->setItemChecked(400, false);
      this->mpZoomMenu->setItemChecked(500, false);
      this->mpZoomMenu->setItemChecked(id, true);
      connect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));
    }
}

void CQLayoutMainWindow::setZoomFactor(std::string s)
{
  s.erase(s.size() - 1);
  // create a number from the text
  double n = strtod(s.c_str(), NULL);
  n /= 100.0;
  this->mpGLViewport->setZoomFactor(n);
}

void CQLayoutMainWindow::slotResetView()
{
  // check the 100% zoom entry
  disconnect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));
  this->mpZoomMenu->setItemChecked(1, false);
  this->mpZoomMenu->setItemChecked(2, false);
  this->mpZoomMenu->setItemChecked(3, false);
  this->mpZoomMenu->setItemChecked(4, false);
  this->mpZoomMenu->setItemChecked(5, false);
  this->mpZoomMenu->setItemChecked(10, false);
  this->mpZoomMenu->setItemChecked(20, false);
  this->mpZoomMenu->setItemChecked(30, false);
  this->mpZoomMenu->setItemChecked(40, false);
  this->mpZoomMenu->setItemChecked(50, false);
  this->mpZoomMenu->setItemChecked(100, true);
  this->mpZoomMenu->setItemChecked(200, false);
  this->mpZoomMenu->setItemChecked(300, false);
  this->mpZoomMenu->setItemChecked(400, false);
  this->mpZoomMenu->setItemChecked(500, false);
  connect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));
  this->mpGLViewport->resetView();
}

void CQLayoutMainWindow::slotZoomItemActivated(int id)
{
  // if the item is not checked, uncheck all other and check this one
  // set the zoom factor
  if (!this->mpZoomMenu->isItemChecked(id))
    {
      disconnect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));
      this->mpZoomMenu->setItemChecked(1, false);
      this->mpZoomMenu->setItemChecked(2, false);
      this->mpZoomMenu->setItemChecked(3, false);
      this->mpZoomMenu->setItemChecked(4, false);
      this->mpZoomMenu->setItemChecked(5, false);
      this->mpZoomMenu->setItemChecked(10, false);
      this->mpZoomMenu->setItemChecked(20, false);
      this->mpZoomMenu->setItemChecked(30, false);
      this->mpZoomMenu->setItemChecked(40, false);
      this->mpZoomMenu->setItemChecked(50, false);
      this->mpZoomMenu->setItemChecked(100, false);
      this->mpZoomMenu->setItemChecked(200, false);
      this->mpZoomMenu->setItemChecked(300, false);
      this->mpZoomMenu->setItemChecked(400, false);
      this->mpZoomMenu->setItemChecked(500, false);
      this->mpZoomMenu->setItemChecked(id, true);
      QString text = this->mpZoomMenu->text(id);
      this->setZoomFactor(text.latin1());
      connect(mpZoomMenu, SIGNAL(activated(int)), this, SLOT(slotZoomItemActivated(int)));
      // update toolbar
      disconnect(this->mpZoomComboBox, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
      this->mpZoomComboBox->setCurrentItem(this->mpZoomMenu->indexOf(id));
      connect(this->mpZoomComboBox, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
    }
}

void CQLayoutMainWindow::slotZoomIn()
{
  int ids[] = {1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500};
  int checkedItem = 0;
  unsigned int i, iMax = 15;
  for (i = 0;i < iMax;++i)
    {
      if (this->mpZoomMenu->isItemChecked(ids[i]))
        {
          checkedItem = i;
          break;
        }
    }
  if (checkedItem != 14)
    {
      slotZoomItemActivated(ids[++checkedItem]);
    }
}

void CQLayoutMainWindow::slotZoomOut()
{
  int ids[] = {1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500};
  int checkedItem = 0;
  unsigned int i, iMax = 15;
  for (i = 0;i < iMax;++i)
    {
      if (this->mpZoomMenu->isItemChecked(ids[i]))
        {
          checkedItem = i;
          break;
        }
    }
  if (checkedItem != 0)
    {
      slotZoomItemActivated(ids[--checkedItem]);
    }
}

void CQLayoutMainWindow::stopAnimation()
{
  // go to step 0 and stop
  this->pauseAnimation();
  this->showStep(0.0);
  // update the slider
  disconnect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
  this->mpTimeSlider->setValue(0.0);
  connect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
}

void CQLayoutMainWindow::forwardAnimation()
{
  // go to last step and redisplay
  this->pauseAnimation();
  double stepNumber = (double)(this->mpGLViewport->getPainter()->getNumberOfSteps() - 1);
  this->showStep(stepNumber);
  // update the slider
  disconnect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
  this->mpTimeSlider->setValue(stepNumber);
  connect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
}

void CQLayoutMainWindow::backwardAnimation()
{
  // go to step 0 and redisplay
  this->stopAnimation();
  if (this->mpControlWidget->isPlaying())
    {
      this->startAnimation();
    }
}

void CQLayoutMainWindow::stepForwardAnimation()
{
  // raise step by one if possible and continue animation
  // go to last step and redisplay
  C_INT32 currentStep = this->getCurrentStep();
  ++currentStep;
  if (currentStep < this->mpGLViewport->getPainter()->getNumberOfSteps())
    {
      this->pauseAnimation();
      this->showStep((double)currentStep);
      // update the slider
      disconnect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
      this->mpTimeSlider->setValue((double)currentStep);
      connect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
    }
}

void CQLayoutMainWindow::stepBackwardAnimation()
{
  // lower step by one if possible and redisplay
  C_INT32 currentStep = this->getCurrentStep();
  if (currentStep > 0)
    {
      --currentStep;
      this->pauseAnimation();
      this->showStep((double)currentStep);
      // update the slider
      disconnect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
      this->mpTimeSlider->setValue((double)currentStep);
      connect(mpTimeSlider, SIGNAL(valueChanged(double)), this, SLOT(showStep(double)));
    }
}
