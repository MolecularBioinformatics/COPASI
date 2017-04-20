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

// Copyright (C) 2006 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 

%{

#include "copasi/core/CDataContainer.h"

%}


%ignore operator++;
%ignore operator!=;
%ignore CDataContainer::CObjectMap::iterator;
%ignore CDataContainer::CObjectMap::iterator::operator++;
%ignore CDataContainer::CObjectMap::const_iterator::operator++;
%ignore CDataContainer::CDataContainer(const std::string& name,
                                           const CDataContainer * pParent = NULL,
                                           const std::string & type = "CN",
                                           const unsigned C_INT32 & flag);

%ignore CDataContainer::add(CDataObject*,const bool&); // needs to be hidden since it conflicts with methods in derived classes 
%ignore CDataContainer::remove(CDataObject*); // needs to be hidden since it conflicts with methods in derived classes 
%ignore CDataContainer::getObjects;

%include "report/CDataContainer.h"

