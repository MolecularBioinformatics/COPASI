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

#include "model/CReaction.h"

%}
    
%ignore CReaction::load;
%ignore CReaction::KineticLawUnitTypeName;
%ignore CReaction::getSmallestCompartment() const; // not implemented
%ignore CReaction::getParameterMappings() const; 
%ignore CReaction::setFunctionFromExpressionTree(const CExpression & tree , std::map<CDataObject*, SBase*> & , CFunctionDB* );
%ignore CReaction::calculatePartialDerivative;
%ignore CReaction::getExpressionTree;
%ignore CReaction::getDeletedObjects;
%ignore CReaction::getPropensityReference() const;
%ignore CReaction::getParticleFluxReference() const;
%ignore CReaction::getFluxReference() const;
%ignore CReaction::getNoiseExpressionPtr() const;

#if (defined SWIGJAVA || defined SWIGCSHARP)
// remove some const methods to get rid of warnings
%ignore CReaction::getChemEq() const;
%ignore CReaction::getParameters() const;

#endif // SWIGJAVA || CSHARP


%extend CReaction{


  // the CAnnotation functionality has to be added manually because
  // Java does not know about multiple inheritance
  void setNotes(const std::string& notes)
  {
    self->setNotes(notes);
  }

  const std::string& getNotes() const
  {
    return self->getNotes();
  }

  const std::string& getMiriamAnnotation() const
  {
    return self->getMiriamAnnotation();
  }

  void setMiriamAnnotation(const std::string& miriamAnnotation,
                           const std::string& newId,
                           const std::string& oldId)
  {
	self->setMiriamAnnotation(miriamAnnotation,newId,oldId);
  }

  std::string getUnsupportedAnnotation(std::string name)
  {
  	return $self->getUnsupportedAnnotations()[name];
  }
  
  bool hasUnsupportedAnnotation(std::string name)
  {
  	const std::string& annot = $self->getUnsupportedAnnotations()[name];
  	return !(annot.empty());
  }
  
  int getNumUnsupportedAnnotations()
  {
  	return (int)$self->getUnsupportedAnnotations().size();
  }
  
  std::string getUnsupportedAnnotation(int index)
  {		
  	std::map< std::string, std::string > &anot = $self->getUnsupportedAnnotations();
  	if (index >= (int)anot.size()) 
  		return "";
  	std::map< std::string, std::string >::iterator iter = anot.begin();
  	for (int i = 0; i < index; ++i)
  		++iter;
  	return (*iter).second;
  }
  
  std::string getUnsupportedAnnotationName(int index)
  {		
  	std::map< std::string, std::string > &anot = $self->getUnsupportedAnnotations();
  	if (index >= (int)anot.size()) 
  		return "";
  	std::map< std::string, std::string >::iterator iter = anot.begin();
  	for (int i = 0; i < index; ++i)
  		++iter;
  	return (*iter).first;
  }
   
  bool addUnsupportedAnnotation(const std::string & name, const std::string & xml)
  {
	try
	{
		return $self->addUnsupportedAnnotation(name, xml);
	}
	catch(...)
	{
		return false;
	}
  }
  
  bool replaceUnsupportedAnnotation(const std::string & name, const std::string & xml)
  {
	try
	{	
		return $self->replaceUnsupportedAnnotation(name, xml);
	}
	catch(...)
	{
		return false;
	}
  }
  
  bool removeUnsupportedAnnotation(const std::string & name)
  {
  	return $self->removeUnsupportedAnnotation(name);
  }

  
#ifdef SWIGR

//  /**
//   * This function is a hack to be able to set
//   * the function of a reaction in R from a wrapped
//   * CEvaluationTree pointer that points to a CFunction.
//   *
//   * Normally the code generated by SWIG for R language bindings seems to be able
//   * to handle the upcasting to the most specific type quite well without any
//   * additional typemaps. At least in one small example that I wrote to debug this
//   * problem, it worked.
//   * I suspect that the multiple inheritance that we use in COPASI throws SWIG off.
//   */
//  bool setFunction(CEvaluationTree* pTree)
//  {
//      bool result=false;
//      CFunction* pFun=dynamic_cast<CFunction*>(pTree);
//      if(pFun != NULL)
//      {
//        result=self->setFunction(pFun);
//      }
//      return result;
//  }
#endif // SWIGR
}

%catches(CCopasiException) CReaction::getLargestCompartment() const;
%catches(CCopasiException) CReaction::addParameterMapping;
%catches(CCopasiException) CReaction::setParameterMapping;
%catches(CCopasiException) CReaction::clearParameterMapping;
%catches(CCopasiException) CReaction::getParameterMapping;
%catches(CCopasiException) CReaction::getParameterValue;
%catches(CCopasiException) CReaction::setParameterValue;
%catches(CCopasiException) CReaction::setFunction;
%catches(CCopasiException) CReaction::isLocalParameter;

// suppress warnings on multiple inheritance
%warnfilter(813) CReaction;

%ignore CReaction::getParameterIndex;
%include "model/CReaction.h"

