// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/model/CModelParameterSet.h,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2011/10/21 17:45:15 $
// End CVS Header

// Copyright (C) 2011 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#ifndef COPASI_CModelParameterSet
#define COPASI_CModelParameterSet

#include <string>

#include "copasi/report/CCopasiContainer.h"
#include "copasi/model/CModelParameterGroup.h"
#include "copasi/model/CAnnotation.h"

class CModel;

class CModelParameterSet: public CCopasiContainer, public CModelParameterGroup, public CAnnotation
{
public:
  CModelParameterSet(const std::string & name,
                     const CCopasiContainer * pParent = NULL);

  CModelParameterSet(const CModelParameterSet & src,
                     const CCopasiContainer * pParent = NULL);

  virtual const std::string & getKey() const;

  virtual bool setObjectParent(const CCopasiContainer * pParent);

  virtual ~CModelParameterSet();

  void setModel(const CModel * pModel);

  virtual CModel * getModel();

  void createFromModel();

  bool compareWithModel();

  virtual bool updateModel();

private:
  std::string mName;

  std::string mKey;

  CModel * mpModel;
};

#endif // COPASI_CModelParameterSet