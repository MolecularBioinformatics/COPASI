// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2005 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#ifndef COPASI_COutputAssistant
#define COPASI_COutputAssistant

#include <vector>
#include <string>
#include <map>
#include "copasi.h"

#include "utilities/CCopasiTask.h"

class CPlotSpecification;
class CReportDefinition;
class CDataObject;
class CDataModel;

class CDefaultOutputDescription
{
public:

  CDefaultOutputDescription():
    name(""),
    description(""),
    isPlot(true),
    mTaskType(CTaskEnum::Task::steadyState),
    mSecondaryTask(CTaskEnum::Task::UnsetTask)
  {}

  //C_INT32 id;
  std::string name;
  std::string description;
  bool isPlot;
  CTaskEnum::Task mTaskType;
  CTaskEnum::Task mSecondaryTask;
};

class COutputAssistant
{
public:

  /**
   *  get a list of indices of output definitions that are
   *  suitable for the problem.
   *  If problem=NULL (default) all indices in the map are returned.
   */
  static
  std::vector<C_INT32> getListOfDefaultOutputDescriptions(const CCopasiTask * task = NULL);

  /**
   *  get an index of a default output report that is
   *  recommended  for the given problem.
   *  If no such report exists, -1 is returned
   */
  static
  C_INT32 getDefaultReportIndex(const CCopasiProblem * problem);

  /**
   *  get an index of a default plot that is
   *  recommended  for the given problem.
   *  If no such plot exists, -1 is returned
   */
  static
  C_INT32 getDefaultPlotIndex(const CCopasiProblem * problem);

  /**
   *  get title of output definition with index id.
   */
  static
  const std::string & getItemName(C_INT32 id);

  /**
   *  get complete description of output definition with index id.
   */
  static
  const CDefaultOutputDescription & getItem(C_INT32 id);

  /**
   *  create a plot or report from template with index id.
   *  returns a pointer to the plot or report definition (or NULL)
   *  if activate==true and the ouput is a report the report is set as
   *  the current report for the task (possibly replacing an already set report)
   */
  static
  CDataObject* createDefaultOutput(C_INT32 id, CCopasiTask * task, CDataModel* pDataModel, bool activate = true);

private:           //************************************

  typedef std::map<C_INT32, CDefaultOutputDescription> Map;

  static
  Map mMap;

  //this method creates the lists of output descriptions
  static
  bool initialize();

  static
  CPlotSpecification* createPlot(const std::string & name,
                                 const CDataObject* x,
                                 bool logX,
                                 const std::vector<const CDataObject*> & y,
                                 bool logY,
                                 const CTaskEnum::Task & taskType,
                                 CDataModel* pDataModel,
                                 CCopasiTask *task = NULL);

  static
  CReportDefinition* createTable(const std::string & name,
                                 const std::vector<const CDataObject*> & d,
                                 const std::string & comment,
                                 const CTaskEnum::Task & taskType,
                                 CDataModel* pDataModel);

  static const std::string emptyString;
  static const CDefaultOutputDescription emptyItem;
};

#endif
