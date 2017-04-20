// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include "copasi.h"

#include "KineticLawHandler.h"
#include "CXMLParser.h"
#include "utilities/CCopasiMessage.h"

#include "model/CReaction.h"
#include "copasi/core/CRootContainer.h"

/**
 * Replace KineticLaw with the name type of the handler and implement the
 * three methods below.
 */
KineticLawHandler::KineticLawHandler(CXMLParser & parser, CXMLParserData & data):
  CXMLHandler(parser, data, CXMLHandler::KineticLaw)
{
  init();
}

// virtual
KineticLawHandler::~KineticLawHandler()
{}

// virtual
CXMLHandler * KineticLawHandler::processStart(const XML_Char * pszName,
    const XML_Char ** papszAttrs)
{
  CXMLHandler * pHandlerToCall = NULL;
  const char * Function;
  CReaction::KineticLawUnit KineticLawUnitType;
  std::string ScalingCompartment;

  switch (mCurrentElement.first)
    {
      case KineticLaw:
        Function = mpParser->getAttributeValue("function", papszAttrs);
        KineticLawUnitType = toEnum(mpParser->getAttributeValue("unitType", papszAttrs, "Default"), CReaction::KineticLawUnitTypeName, CReaction::Default);
        ScalingCompartment = mpParser->getAttributeValue("scalingCompartment", papszAttrs, "");

        mpData->pFunction =
          dynamic_cast< CFunction* >(mpData->mKeyMap.get(Function));

        if (!mpData->pFunction)
          {
            CCopasiMessage(CCopasiMessage::RAW, MCXML + 7, Function,
                           mpData->pReaction->getObjectName().c_str(),
                           mpParser->getCurrentLineNumber());
            mpData->pFunction = CRootContainer::getUndefinedFunction();
          }

        mpData->pReaction->setKineticLawUnitType(KineticLawUnitType);
        mpData->pReaction->setScalingCompartmentCN(ScalingCompartment);

        // This must be deferred till the end since we need to check for consistency
        // of the parameters first (Bug 832)
        // mpData->pReaction->setFunction(pFunction);
        break;

      case ListOfCallParameters:
        pHandlerToCall = getHandler(mCurrentElement.second);
        break;

      default:
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCXML + 2,
                       mpParser->getCurrentLineNumber(), mpParser->getCurrentColumnNumber(), pszName);
        break;
    }

  return pHandlerToCall;
}

// virtual
bool KineticLawHandler::processEnd(const XML_Char * pszName)
{
  bool finished = false;

  switch (mCurrentElement.first)
    {
      case KineticLaw:
        finished = true;
        mpData->pReaction->setFunction(dynamic_cast< CFunction * >(mpData->pFunction));

        {
          std::map< std::string, std::vector< std::string > >::const_iterator it
            = mpData->SourceParameterKeys.begin();
          std::map< std::string, std::vector< std::string > >::const_iterator end
            = mpData->SourceParameterKeys.end();

          for (; it != end; ++it)
            if (it->second.size() > 0)
              mpData->pReaction->setParameterMappingVector(it->first, it->second);
        }

        mpData->SourceParameterKeys.clear();

        break;

      case ListOfCallParameters:
        break;

      default:
        CCopasiMessage(CCopasiMessage::EXCEPTION, MCXML + 2,
                       mpParser->getCurrentLineNumber(), mpParser->getCurrentColumnNumber(), pszName);
        break;
    }

  return finished;
}

// virtual
CXMLHandler::sProcessLogic * KineticLawHandler::getProcessLogic() const
{
  static sProcessLogic Elements[] =
  {
    {"BEFORE", BEFORE, BEFORE, {KineticLaw, HANDLER_COUNT}},
    {"KineticLaw", KineticLaw, KineticLaw, {ListOfCallParameters, AFTER, HANDLER_COUNT}},
    {"ListOfCallParameters", ListOfCallParameters, ListOfCallParameters, {AFTER, HANDLER_COUNT}},
    {"AFTER", AFTER, AFTER, {HANDLER_COUNT}}
  };

  return Elements;
}
