/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/ScanWidget.h,v $
   $Revision: 1.53 $
   $Name:  $
   $Author: ssahle $ 
   $Date: 2005/02/22 15:59:05 $
   End CVS Header */

/****************************************************************************
 **  $ CopasiUI/ScanWidget.h               
 **  $ Author  : Liang Xu
 **  
 ** This is the header file for the Scan Widget
 *****************************************************************************/
#ifndef SCANWIDGET_H
#define SCANWIDGET_H

#include "copasi.h"
#include <qvariant.h>
#include <qwidget.h> 
//#include <qscrollview.h>
#include "copasiWidget.h"
#include "utilities/CVector.h"
#include <vector>

class QVBox;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QListBox;
class QListBoxItem;
class QVBoxLayout;
class CScanTask;
class CScanProblem;
class CScanMethod;
class CModel;
class QToolButton;
class SteadyStateWidget;
class TrajectoryWidget;
class CCopasiObject;
//class ScanWidget;
class CScanContainerWidget;

class ScanWidget : public CopasiWidget
  {
    Q_OBJECT

  private:
    std::string scanTaskKey;

  public:
    ScanWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~ScanWidget();
    virtual bool update(ListViews::ObjectType objectType, ListViews::Action action, const std::string & key);
    virtual bool leave();
    virtual bool enter(const std::string & key = "");

  protected:
    bool loadScan();

    bool saveScan() const;

    //inline std::string getScanTaskKey() const {return scanTaskKey;};

    CScanContainerWidget* scrollview;
    QCheckBox* sExecutable;
    QPushButton* scanButton;
    QPushButton* cancelChange;

    QPushButton* buttonNewItem;

    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLabel* TextLabel3;

    QPushButton* reportDefinitionButton;

    QGridLayout* ScanWidgetLayout;
    QHBoxLayout* Layout2;
    QHBoxLayout* Layout24;
    CModel *mModel;

  public slots:
    virtual void CancelChangeButton();
    virtual void ScanCheckBoxClicked();

    virtual void runScanTask();
    //virtual void SteadyStateButtonClicked();
    //virtual void TrajectoryButtonClicked();

    //virtual void SteadyStateEditing();
    //virtual void TrajectoryEditing();

    virtual void ReportDefinitionClicked();

  protected slots:
    bool slotAddItem();
  };

#endif
