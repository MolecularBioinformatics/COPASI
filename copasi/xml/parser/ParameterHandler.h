// Copyright (C) 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#ifndef COPASI_ParameterHandler
#define COPASI_ParameterHandler

#include "copasi/xml/parser/CXMLHandler.h"

class ParameterHandler : public CXMLHandler
{
private:
  ParameterHandler();

public:
  /**
   * Constructor
   * @param CXMLParser & parser
   * @param CXMLParserData & data
   */
  ParameterHandler(CXMLParser & parser, CXMLParserData & data);

  /**
   * Destructor
   */
  virtual ~ParameterHandler();

protected:

  /**
   * Process the start of an element
   * @param const XML_Char *pszName
   * @param const XML_Char **papszAttrs
   * @return CXMLHandler * pHandlerToCall
   */
  virtual CXMLHandler * processStart(const XML_Char * pszName,
                                     const XML_Char ** papszAttrs);

  /**
   * Process the end of an element
   * @param const XML_Char *pszName
   * @return bool finished
   */
  virtual bool processEnd(const XML_Char * pszName);

  /**
   * Retrieve the structure containing the process logic for the handler.
   * @return sElementInfo *
   */
  virtual sProcessLogic * getProcessLogic() const;
};

#endif //COPASI_ParameterHandler
