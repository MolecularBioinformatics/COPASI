// Copyright (C) 2010 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "copasi.h"

#include "CCopasiRootContainer.h"
#include "ArtisticLicense.h"
#include "copasi/function/CFunctionDB.h"
#include "copasi/commandline/CConfigurationFile.h"
#include "copasi/commandline/COptions.h"
#include "copasi/function/CFunction.h"
#include "copasi/function/CEvaluationNodeOperator.h"
#include "copasi/function/CEvaluationNodeVariable.h"
#include "copasi/function/CEvaluationNodeConstant.h"
#include "copasi/utilities/CUnit.h"

#include <copasi/layout/CLGlobalRenderInformation.h>

extern CCopasiVector<CLGlobalRenderInformation>* DEFAULT_STYLES;

extern CCopasiRootContainer * pRootContainer;

/**
 * The default constructor should be private so that nobody can create
 * new instances of the root container.
 * The only way to create a root container is through the static init
 * method.
 */
CCopasiRootContainer::CCopasiRootContainer(const bool & withGUI):
  CCopasiContainer("Root", NULL, "CN", CCopasiObject::Root),
  mKeyFactory(),
  mpUnknownResource(NULL),
  mpFunctionList(NULL),
  mpUnitList(NULL),
  mpConfiguration(NULL),
  mpDataModelList(NULL),
  mWithGUI(withGUI),
  mpUndefined(NULL)
{}

// Destructor
CCopasiRootContainer::~CCopasiRootContainer()
{
  // save and delete the configuration
  if (mpConfiguration != NULL &&
      mWithGUI)
    {
      mpConfiguration->save();
    }

  // delete the configuration
  pdelete(mpConfiguration);

  // delete the unkown resource
  pdelete(mpUnknownResource);

  // delete the model list
  pdelete(mpDataModelList);

  // delete the function list
  pdelete(mpFunctionList);

  // delete the unit list
  pdelete(mpUnitList);

  // delete the undefined and the unsupported delay function
  pdelete(mpUndefined);

  // delete other static variables
  pdelete(DEFAULT_STYLES);
}

/**
 * This method creates the only root container.
 */
void CCopasiRootContainer::init(int argc, char *argv[], const bool & withGUI)
{
  COptions::init(argc, argv);

  CCopasiMessage::setIsGUI(withGUI);

  if (pRootContainer == NULL)
    pRootContainer = new CCopasiRootContainer(withGUI);

  if (pRootContainer != NULL)
    pRootContainer->initializeChildren();
}

// static
void CCopasiRootContainer::destroy()
{
  if (pRootContainer != NULL)
    {
      delete pRootContainer;
      pRootContainer = NULL;
    }

  pdelete(CFunctionParameterMap::pUnmappedObject);

  COptions::cleanup();
}

void CCopasiRootContainer::initializeChildren()
{
  mpUnknownResource = new CMIRIAMResource("Unknown Resource");
  mpUnknownResource->setMIRIAMDisplayName("-- select --");
  mpUnknownResource->setMIRIAMURI("urn:miriam:unknown");

  mpFunctionList = new CFunctionDB("FunctionDB", this);
  mpFunctionList->load();

  mpDataModelList = new CCopasiVector<CCopasiDataModel>("ModelList", this);

  mpConfiguration = new CConfigurationFile;
  mpConfiguration->load();

  mpUndefined = new CFunction("undefined", this);
  mpUndefined->setInfix("nan");
  mpUndefined->compile();

  mpUnitList = new CCopasiVectorN< CUnit >;
  CUnit::updateSIUnits(*mpUnitList, CUnit::Avogadro);
}

// static
const CCopasiContainer * CCopasiRootContainer::getRoot()
{
  return pRootContainer;
}

// static
CConfigurationFile * CCopasiRootContainer::getConfiguration()
{
  return pRootContainer->mpConfiguration;
}

// static
CFunctionDB * CCopasiRootContainer::getFunctionList()
{
  return pRootContainer->mpFunctionList;
}

// static
CCopasiVector< CCopasiDataModel > * CCopasiRootContainer::getDatamodelList()
{
  return pRootContainer->mpDataModelList;
}

// static
CCopasiVectorN< CUnit > * CCopasiRootContainer::getUnitList()
{
  return pRootContainer->mpUnitList;
}

// static
const CUnit * CCopasiRootContainer::getUnitFromSymbol(const std::string symbol)
{
  CCopasiVectorN< CUnit >::const_iterator it = pRootContainer->mpUnitList->begin();
  CCopasiVectorN< CUnit >::const_iterator end = pRootContainer->mpUnitList->end();

  const CUnit * pUnit = NULL;

  for (; it != end; ++it)
    {
      if (symbol == (*it)->getSymbol())
        {
          pUnit = *it;
          break;
        }
    }

  return pUnit;
}

// static
CCopasiDataModel * CCopasiRootContainer::addDatamodel()
{
  CCopasiDataModel* pDataModel = new CCopasiDataModel(pRootContainer->mWithGUI);
  pRootContainer->mpDataModelList->add(pDataModel, true);
  return pDataModel;
}

// static
CFunction * CCopasiRootContainer::getUndefinedFunction()
{
  return pRootContainer->mpUndefined;
}

//static
CKeyFactory* CCopasiRootContainer::getKeyFactory()
{
  return &pRootContainer->mKeyFactory;
}

// static
const CMIRIAMResource & CCopasiRootContainer::getUnknownMiriamResource()
{
  return *pRootContainer->mpUnknownResource;
}

// static
bool CCopasiRootContainer::removeDatamodel(const CCopasiDataModel * pDatamodel)
{
  if (!pDatamodel)
    {
      return false;
    }

  pRootContainer->mpDataModelList->remove((CCopasiDataModel *)pDatamodel);
  pdelete(pDatamodel);

  return true;
}

// static
bool CCopasiRootContainer::removeDatamodel(const unsigned C_INT32 index)
{
  const CCopasiDataModel* pDatamodel = (*CCopasiRootContainer::getDatamodelList())[index];
  return CCopasiRootContainer::removeDatamodel(pDatamodel);
}

// static
const char * CCopasiRootContainer::getLicenseTxt()
{
  return CopasiLicenseTxt;
}

// static
const char * CCopasiRootContainer::getLicenseHTML()
{
  return CopasiLicenseHtml;
}
