// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "copasi.h"

#include "SourceParameterHandler.h"
#include "CXMLParser.h"
#include "utilities/CCopasiMessage.h"

#include "model/CModelValue.h"
#include "utilities/CCopasiParameter.h"
#include "function/CCallParameters.h"

/**
 * Replace SourceParameter with the name type of the handler and implement the
 * three methods below.
 */
SourceParameterHandler::SourceParameterHandler(CXMLParser & parser, CXMLParserData & data):
  CXMLHandler(parser, data, CXMLHandler::SourceParameter)
{
  init();
}

// virtual
SourceParameterHandler::~SourceParameterHandler()
{}

// virtual
CXMLHandler * SourceParameterHandler::processStart(const XML_Char * pszName,
    const XML_Char ** papszAttrs)
{
  const char * Reference;
  CDataObject * pObject;
  CCopasiParameter * pParameter;
  CModelEntity * pME;

  switch (mCurrentElement.first)
    {
      case SourceParameter:
        Reference =
          mpParser->getAttributeValue("reference", papszAttrs);

        pObject = mpData->mKeyMap.get(Reference);

        if ((pParameter = dynamic_cast< CCopasiParameter * >(pObject)))
          {
            // We need to assure that the parameter name for variables which are not
            // of type vector match.
            if (mpData->pFunctionVariable->getType() < CFunctionParameter::VINT32)
              pParameter->setObjectName(mpData->pFunctionVariable->getObjectName());

            mpData->SourceParameterKeys[mpData->pFunctionVariable->getObjectName()].push_back(pParameter->getKey());
          }
        else if ((pME = dynamic_cast<CModelEntity*>(pObject)))
          mpData->SourceParameterKeys[mpData->pFunctionVariable->getObjectName()].push_back(pME->getKey());
        else
          mpData->SourceParameterKeys[mpData->pFunctionVariable->getObjectName()].push_back(CFunctionParameterMap::pUnmappedObject->getKey());

        break;

      default:
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCXML + 2,
                       mpParser->getCurrentLineNumber(), mpParser->getCurrentColumnNumber(), pszName);
        break;
    }

  return NULL;
}

// virtual
bool SourceParameterHandler::processEnd(const XML_Char * pszName)
{
  bool finished = false;

  switch (mCurrentElement.first)
    {
      case SourceParameter:
        finished = true;
        break;

      default:
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCXML + 2,
                       mpParser->getCurrentLineNumber(), mpParser->getCurrentColumnNumber(), pszName);
        break;
    }

  return finished;
}

// virtual
CXMLHandler::sProcessLogic * SourceParameterHandler::getProcessLogic() const
{
  static sProcessLogic Elements[] =
  {
    {"BEFORE", BEFORE, BEFORE, {SourceParameter, HANDLER_COUNT}},
    {"SourceParameter", SourceParameter, SourceParameter, {AFTER, HANDLER_COUNT}},
    {"AFTER", AFTER, AFTER, {HANDLER_COUNT}}
  };

  return Elements;
}
