// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/plotUI/plotwindow.cpp,v $
//   $Revision: 1.36 $
//   $Name:  $
//   $Author: pwilly $
//   $Date: 2008/07/03 12:13:21 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

// the window containing the plot and buttons for supported operations

#include <qtoolbar.h>
#include <qprinter.h>
#include <qpixmap.h>
#include <qpicture.h>
#include <qfiledialog.h>
#include <qcursor.h>
#include <qregexp.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include "plotwindow.h"
#include "CopasiPlot.h"
#include "plot/CPlotSpecification.h"
#include "COutputHandlerPlot.h"

#include "UI/CQMessageBox.h"
#include "UI/qtUtilities.h"

// taken from qwt examples/bode
class PrintFilter: public QwtPlotPrintFilter
  {
  public:
    PrintFilter() {};

    virtual QFont font(const QFont &f, Item, int) const
      {
        QFont f2 = f;
        f2.setPointSize((int)(f.pointSize() * 0.75));
        return f2;
      }
  };

//-----------------------------------------------------------------------------

PlotWindow::PlotWindow(COutputHandlerPlot * pHandler, const CPlotSpecification* ptrSpec):
    mpPlot(NULL),
    mpHandler(pHandler)
{
  this->resize(640, 480);
  this->setCaption(("Copasi Plot: " + ptrSpec->getTitle()).c_str());

  // set up the GUI - the toolbar
  QToolBar * plotTools = new QToolBar(this, "plot operations");
  plotTools->setLabel("Plot Operations");

  printButton = new QToolButton(plotTools, "print plot");
  printButton -> setTextLabel("Print plot");
  printButton -> setText("Print");

  print2Button = new QToolButton(plotTools, "print image");
  print2Button -> setTextLabel("Print image");
  print2Button -> setText("Print As Image");

  saveButton = new QToolButton(plotTools, "save data");
  saveButton -> setTextLabel("Save data...");
  saveButton -> setText("Save data");

  zoomButton = new QToolButton(plotTools, "zoom");
  zoomButton->setText("Zoom out");
  zoomButton->setTextLabel("Zoom out");
  //zoomButton->setToggleButton(true);

  //TODO button icons...

  plotTools->setStretchableWidget(new QWidget(plotTools));

  mpPlot = new CopasiPlot(ptrSpec, this);
  setCentralWidget(mpPlot);

  connect(zoomButton, SIGNAL(clicked()), this, SLOT(slotZoomOut()));
  connect(printButton, SIGNAL(clicked()), this, SLOT(printPlot()));
  connect(print2Button, SIGNAL(clicked()), this, SLOT(printAsImage()));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSaveData()));
  //connect(mpPlot, SIGNAL(plotMouseReleased(const QMouseEvent &)), this, SLOT(mouseReleased(const QMouseEvent&)));
}

bool PlotWindow::initFromSpec(const CPlotSpecification* ptrSpec)
{
  this->setCaption(("Copasi Plot: " + ptrSpec->getTitle()).c_str());
  return mpPlot->initFromSpec(ptrSpec);
}

//-----------------------------------------------------------------------------

/*void PlotWindow::enableZoom()
{
  zoomButton->setEnabled(false);
  mpPlot->enableZoom(true);
}*/

//-----------------------------------------------------------------------------

/*void PlotWindow::mouseReleased(const QMouseEvent &e)
{
  //TODO: if midbutton is clicked and we're zoomed out completely, zoomButton need to be enabled as well

  if (e.button() == RightButton)
    zoomButton->setEnabled(true);
}*/

//-----------------------------------------------------------------------------

void PlotWindow::printAsImage()
{
  // take a name from QFileDialog

  C_INT32 Answer = QMessageBox::No;
  QString fileName, extensionName = "";

  while (Answer == QMessageBox::No)
    {
      QString filter;
      fileName = CopasiFileDialog::getSaveFileNameAndFilter(filter, this, "Save File Dialog",
                 QString::null, "PNG Files (*.png);;SVG Files (*.svg);;", "Save to");

      std::cout << "fileName: " << fileName << " - selected filter: " << filter << std::endl;

      if (fileName.isEmpty()) return;

      QFileInfo fileInfo(fileName);
      extensionName = fileInfo.extension();

      if (filter.contains("png"))
        {
          if (extensionName != "png")
            fileName.replace("." + extensionName, ".png");

          fileInfo.setFile(fileName);
          extensionName = fileInfo.extension();

          if (extensionName == "")
            fileName += ".png";
        }
      else if (filter.contains("svg"))
        {
          if (extensionName != "svg")
            fileName.replace("." + extensionName, ".svg");

          fileInfo.setFile(fileName);
          extensionName = fileInfo.extension();

          if (extensionName == "")
            fileName += ".svg";
        }

      std::cout << "fileName = " << fileName << std::endl;

      fileInfo.setFile(fileName);
      extensionName = fileInfo.extension();

      Answer = checkSelection(fileName);

      if (Answer == QMessageBox::Cancel) return;
    }

  // print plot as an image

  QPainter painter;
  QRect rect;
  rect.setSize(this->size());

  //  std::cout << "size: " << rect.width() << " x " << rect.height() << std::endl;

  if (extensionName == "png")
    {
      QPixmap pixmap(rect.width(), rect.height());
      pixmap.fill();

      painter.begin(&pixmap);
      mpPlot->print(&painter, rect, PrintFilter());
      painter.end();

      pixmap.save(fileName, "PNG");
    }

  if (extensionName == "svg") // true
    {
      QPicture pict;
      painter.begin(&pict);
      mpPlot->print(&painter, rect, PrintFilter());
      painter.end();

      pict.save(fileName, "SVG");
    }
}

void PlotWindow::printPlot()
{
  QPrinter printer;

  QString docName = mpPlot->title().text();
  if (docName.isEmpty())
    {
      //docName.replace (QRegExp (QString::fromLatin1 ("\n")), tr (" -- "));
      docName = QString::fromLatin1("A plot of selected Copasi output");
      printer.setDocName (docName);
    }

  printer.setCreator("Copasi");
  printer.setOrientation(QPrinter::Landscape);

  if (printer.setup())
    mpPlot->print(printer, PrintFilter());
}

void PlotWindow::slotSaveData()
{
  C_INT32 Answer = QMessageBox::No;
  QString fileName;

  while (Answer == QMessageBox::No)
    {
      fileName =
        CopasiFileDialog::getSaveFileName(this, "Save File Dialog", QString::null, "TEXT Files (*.txt);;All Files (*.*);;", "Save to");

      //std::cout << "fileName: " << fileName << std::endl;
      if (fileName == NULL) return;

      if (!fileName.endsWith(".txt") &&
          !fileName.endsWith(".")) fileName += ".txt";

      fileName = fileName.remove(QRegExp("\\.$"));

      Answer = checkSelection(fileName);

      if (Answer == QMessageBox::Cancel) return;
    }

  bool success = false;
  if (mpPlot)
    {
      QCursor oldCursor = cursor();
      setCursor(Qt::WaitCursor);
      success = mpPlot->saveData((const char *)fileName.utf8());
      setCursor(oldCursor);
    }
  if (!success)
    {
      std::string s = "Could not save data to ";
      s += fileName.utf8();
      CQMessageBox::critical(this, "Save Error", FROM_UTF8(s), QMessageBox::Ok, QMessageBox::NoButton);
    }
}

#include "scrollzoomer.h"

void PlotWindow::slotZoomOut()
{
  if (mpPlot && mpPlot->mpZoomer)
    mpPlot->mpZoomer->zoom(0);
}

//-----------------------------------------------------------------------------

PlotWindow::~PlotWindow()
{if (mpHandler) mpHandler->removeInterface(this);}

bool PlotWindow::compile(std::vector< CCopasiContainer * > listOfContainer)
{
  mObjects.clear();
  bool success = true;

  if (mpPlot)
    success = mpPlot->compile(listOfContainer);

  return success;
};

void PlotWindow::output(const Activity & activity)
{if (mpPlot) mpPlot->output(activity);}

void PlotWindow::separate(const Activity & activity)
{if (mpPlot) mpPlot->separate(activity);};

void PlotWindow::finish()
{if (mpPlot) mpPlot->finish();};

const std::set< const CCopasiObject * > & PlotWindow::getObjects() const
  {
    if (mpPlot)
      return mpPlot->getObjects();

    return mObjects;
  }
