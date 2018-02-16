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

// Copyright (C) 2004 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 *
 */

#ifndef COPASI_PLOT_SPECIFICATION
#define COPASI_PLOT_SPECIFICATION

#include <string>
#include <set>

#include "plot/CPlotItem.h"
#include "copasi/core/CDataVector.h"
#include "utilities/CCopasiTask.h"

class CModel;

class CPlotSpecification : public CPlotItem
{
private:

  CDataVector<CPlotItem> items;
  bool mActive;
  std::set<CTaskEnum::Task> mTaskTypes;

  /**
   * Initialize the contained CDataObjects
   */
  void initObjects();

protected:
  CPlotSpecification(const CPlotSpecification & src);

public:
  /**
   * Static method to create a CDataObject based on the provided data
   * @param const CData & data
   * @return CPlotSpecification * pDataObject
   */
  static CPlotSpecification * fromData(const CData & data);

  /**
   * Retrieve the data describing the object
   * @return CData data
   */
  virtual CData toData() const;

  /**
   * Apply the provided data to the object
   * @param const CData & data
   * @return bool success
   */
  virtual bool applyData(const CData & data);

  CPlotSpecification(const std::string & name = "NoName",
                     const CDataContainer * pParent = NO_PARENT,
                     const CPlotItem::Type & type = plot2d);

  CPlotSpecification(const CPlotSpecification & src,
                     const CDataContainer * pParent);

  /**
   * Destructor
   */
  virtual ~CPlotSpecification();

  virtual void cleanup();

  const CDataVector<CPlotItem> & getItems() const;

  CDataVector<CPlotItem> & getItems();

  CPlotItem* createItem(const std::string & name, CPlotItem::Type type);

  bool createDefaultPlot(const CModel* model);

  void setActive(const bool & active);

  const bool & isActive() const;

  bool appliesTo(const CObjectInterface::ContainerList& list) const;

  std::string getTaskTypes() const;

  void setTaskTypes(const std::string& taskTypes);
  void setTaskTypes(const std::set<CTaskEnum::Task>& taskTypes);
  void addTaskType(const CTaskEnum::Task& type);

  bool isLogX() const;
  bool isLogY() const;

  void setLogX(bool l);
  void setLogY(bool l);
};

#endif
