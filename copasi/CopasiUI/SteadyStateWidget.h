/****************************************************************************
 ** Form interface generated from reading ui file '.\SteadyStateWidget.ui'
 **
 ** Created: Sun Mar 2 20:14:23 2003
 **      by:  The User Interface Compiler (uic)
 **
 ** WARNING! All changes made in this file will be lost!
 ****************************************************************************/
#ifndef STEADYSTATEWIDGET_H
#define STEADYSTATEWIDGET_H

#include "copasi.h"
#include <qvariant.h>
#include <qwidget.h>
#include "copasiWidget.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTable;
class CSteadyStateTask;
class CSteadyStateProblem;
class CSteadyStateMethod;
class CModel;

class SteadyStateWidget : public CopasiWidget
  {
    Q_OBJECT

  public:
    SteadyStateWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~SteadyStateWidget();
    CSteadyStateTask *mSteadyStateTask;
    void loadSteadyStateTask(CSteadyStateTask* steadystatetask);

    QFrame* line7;
    QFrame* line6;
    QFrame* line8;
    QFrame* line8_2;
    QLabel* taskNameLabel;
    QRadioButton* taskJacobian;
    QLabel* taskDescriptionLabel;
    QLabel* parameterValueLabel;
    QRadioButton* taskStability;
    QCheckBox* bExecutable;
    QLineEdit* taskName;
    QTable* parameterTable;
    QPushButton* bRunButton;
    QPushButton* commitChange;
    QPushButton* cancelChange;
    QPushButton* ExportFileButton;

  signals:
    virtual void runFinished(CModel*);

  public slots:
    virtual void CancelChange();
    virtual void CommitChange();
    virtual void ExportToFile();
    virtual void parameterValueChanged();
    virtual void RunButtonClicked();
    virtual void RunTask();

  protected:
    QGridLayout* SteadyStateWidgetLayout;
    QHBoxLayout* Layout2;
  };

#endif // STEADYSTATEWIDGET_H
