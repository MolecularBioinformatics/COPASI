// Copyright (C) 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "copasi.h"

#include "ParameterGroupHandler.h"

#include "CXMLParser.h"
#include "utilities/CCopasiMessage.h"
#include "utilities/CCopasiParameterGroup.h"
#include "utilities/CCopasiTask.h"

/**
 * Replace ParameterGroup with the name type of the handler and implement the
 * three methods below.
 */
ParameterGroupHandler::ParameterGroupHandler(CXMLParser & parser, CXMLParserData & data):
  CXMLHandler(parser, data, CXMLHandler::ParameterGroup),
  mDerivedElement(false),
  mLevel(0),
  mParameterGroupStack()
{
  init();
}

// virtual
ParameterGroupHandler::~ParameterGroupHandler()
{}

// virtual
CXMLHandler * ParameterGroupHandler::processStart(const XML_Char * pszName,
    const XML_Char ** papszAttrs)
{
  CXMLHandler * pHandlerToCall = NULL;

  std::string name;

  switch (mCurrentElement)
    {
      case ParameterGroup:
        mLevel++;

        if (mParameterGroupStack.size() != mLevel)
          {
            name = mpParser->getAttributeValue("name", papszAttrs);
            mParameterGroupStack.push(new CCopasiParameterGroup(name));
          }

        break;

      case Parameter:
      case ParameterText:
        pHandlerToCall = mpParser->getHandler(mCurrentElement);
        break;

      default:
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCXML + 2,
                       mpParser->getCurrentLineNumber(), mpParser->getCurrentColumnNumber(), pszName);
        break;
    }

  return pHandlerToCall;
}

// virtual
bool ParameterGroupHandler::processEnd(const XML_Char * pszName)
{
  bool finished = false;

  switch (mCurrentElement)
    {
      case ParameterGroup:
        mLevel--;
        finished = (mLevel == 0);

        mpData->pCurrentParameter = mParameterGroupStack.top();
        mParameterGroupStack.pop();
        break;

      case Parameter:

        // We need to fix the "Key" parameter of each "Experiment" of the the "Parameter Estimation" problem,
        // since they are handled by the elevation of the problem to CFitProblem.
        if (mpData->pCurrentTask != NULL &&
            mpData->pCurrentTask->getType() == CTaskEnum::parameterFitting &&
            (mpData->pCurrentParameter->getObjectName() == "Key" ||
             mpData->pCurrentParameter->getObjectName() == "Experiment Key"))
          {
            if (mpData->UnmappedKeyParameters.size() > 0 &&
                mpData->UnmappedKeyParameters[mpData->UnmappedKeyParameters.size() - 1] == mpData->pCurrentParameter->getKey())
              mpData->UnmappedKeyParameters.erase(mpData->UnmappedKeyParameters.begin() + mpData->UnmappedKeyParameters.size() - 1);
          }

        break;

      case ParameterText:
        break;

      default:
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCXML + 2,
                       mpParser->getCurrentLineNumber(), mpParser->getCurrentColumnNumber(), pszName);
        break;
    }

  // If we are not finished we need to add the current parameter to the group at the top of the stack
  if (!finished)
    {
      CCopasiParameter * pParameter = NULL;

      // Derived elements like methods and problems have already parameters:
      if (mDerivedElement)
        {
          pParameter =
            mParameterGroupStack.top()->getParameter(mpData->pCurrentParameter->getObjectName());
        }

      if (pParameter != NULL)
        {
          CCopasiParameter::Type OriginalType = pParameter->getType();
          *pParameter = *mpData->pCurrentParameter;

          if (mpData->UnmappedKeyParameters.size() > 0 &&
              mpData->UnmappedKeyParameters[mpData->UnmappedKeyParameters.size() - 1] == mpData->pCurrentParameter->getKey())
            {
              if (OriginalType == CCopasiParameter::KEY)
                {
                  mpData->UnmappedKeyParameters[mpData->UnmappedKeyParameters.size() - 1] = pParameter->getKey();
                }
              else
                {
                  mpData->UnmappedKeyParameters.erase(mpData->UnmappedKeyParameters.begin() + mpData->UnmappedKeyParameters.size() - 1);
                }
            }

          pdelete(mpData->pCurrentParameter);
        }
      else
        {
          mParameterGroupStack.top()->addParameter(mpData->pCurrentParameter);
          mpData->pCurrentParameter = NULL;
        }
    }
  else
    {
      mDerivedElement = false;
    }

  return finished;
}

// virtual
CXMLHandler::sProcessLogic * ParameterGroupHandler::getProcessLogic() const
{
  static sProcessLogic Elements[] =
  {
    {"BEFORE", BEFORE, {ParameterGroup, HANDLER_COUNT}},
    {"ParameterGroup", ParameterGroup, {ParameterGroup, Parameter, ParameterText, AFTER, HANDLER_COUNT}},
    {"Parameter", Parameter, {ParameterGroup, Parameter, ParameterText, AFTER, HANDLER_COUNT}},
    {"ParameterText", ParameterText, {ParameterGroup, Parameter, ParameterText, AFTER, HANDLER_COUNT}},
    {"AFTER", AFTER, {HANDLER_COUNT}}
  };

  return Elements;
}

void ParameterGroupHandler::setDerivedElement(CCopasiParameterGroup * pDerivedElement)
{
  assert(mLevel == 0);
  mParameterGroupStack.push(pDerivedElement);
  mDerivedElement = true;
}