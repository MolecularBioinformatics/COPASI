/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CScanWidgetTask.h,v $
   $Revision: 1.2 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2005/02/27 20:22:57 $
   End CVS Header */

/****************************************************************************
 ** Form interface generated from reading ui file 'CScanWidgetTask.ui'
 **
 ** Created: Do Feb 24 23:35:45 2005
 **      by: The User Interface Compiler ($Id: CScanWidgetTask.h,v 1.2 2005/02/27 20:22:57 ssahle Exp $)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/

#ifndef CSCANWIDGETTASK_H
#define CSCANWIDGETTASK_H

#include <qvariant.h>
#include <qwidget.h>
#include "utilities/CCopasiParameterGroup.h"
#include "scan/CScanProblem.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QLabel;
class QComboBox;
class QToolButton;
class QCheckBox;

class CScanWidgetTask : public QWidget
  {
    Q_OBJECT

  public:
    CScanWidgetTask(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~CScanWidgetTask();

    QFrame* frame;
    QLabel* labelTitle;
    QComboBox* comboType;
    QToolButton* buttonEdit;
    QCheckBox* checkInitialConditions;
    QCheckBox* checkOutput;

    virtual bool initFromScanProblem(CScanProblem * pg, const CModel * model);
    virtual bool saveToScanProblem(CScanProblem * pg) const;

  protected:
    CCopasiObject* mpObject;
    const CModel* mpModel;

    QVBoxLayout* CScanWidgetTaskLayout;
    QVBoxLayout* frameLayout;
    QHBoxLayout* layout1;
    QSpacerItem* spacer1;
    QSpacerItem* spacer2;
    QHBoxLayout* layout2;
    QSpacerItem* spacer3;
    QSpacerItem* spacer4;

  protected slots:
    virtual void languageChange();

  private:
    void init();
  };

#endif // CSCANWIDGETTASK_H
