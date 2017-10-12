// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#ifndef CMODELEXPANSION_H
#define CMODELEXPANSION_H

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include "copasi.h"
#include <vector>

class CModelEntity;
class CModel;
class CMetab;
class CCompartment;
class CReaction;
class CModelValue;
class CExpression;
class CEvent;
class CDataObject;

class CModelExpansion
{
public:

  /**
   * This class describes a set of model elements that are supposed to be duplicated.
   * The second element in each of the pairs is used temporarily to hold the copy.
   */
  class SetOfModelElements
  {
  public:

    void addCompartment(const CCompartment* x);
    void addMetab(const CMetab* x);
    void addReaction(const CReaction* x);
    void addGlobalQuantity(const CModelValue* x);
    void addEvent(const CEvent* x);

    bool addObject(const CDataObject* x);

    /**
     * Checks whether an object is present in this set
     */
    bool contains(const CDataObject* x) const;
    bool contains(const std::string & key) const;

    /**
     * Determines which other object have to be duplicated based on those already present.
     */
    void fillDependencies(const CModel* pModel);

    /**
     * enters all the objects from pModel
     */
    void fillComplete(const CModel* pModel);

    //protected:
    std::set<const CCompartment*> mCompartments;
    std::set<const CMetab*> mMetabs;
    std::set<const CReaction*> mReactions;
    std::set<const CModelValue*> mGlobalQuantities;
    std::set<const CEvent*> mEvents;
  };

  /**
   * This class contains a map from original to duplicated sets of model elements
   */
  class ElementsMap
  {
  public:

    ///does a copy of the source object exist?
    bool exists(const CDataObject* source) const;
    bool exists(const std::string & sourceKey) const;

    ///add a source->duplicate mapping
    void add(const CDataObject* source, const CDataObject* copy);

    ///find the pointer of the duplicated object from the pointer to the source object (if it exists)
    const CDataObject* getDuplicatePtr(const CDataObject* source) const;

    ///find the key of the duplicated object from the source object (if it exists)
    std::string getDuplicateKey(const std::string & sourceKey) const;

    const std::map<const CDataObject*, const CDataObject*> & getMap() const;

  protected:

    std::map<const CDataObject*, const CDataObject*> mMap;
  };

  CModelExpansion(CModel* pModel);

  void setModel(CModel* pModel);

  //just a simple method to call during development (as an example for how to use the other methods)
  void simpleCall(const CCompartment* source, std::vector< std::string  > listOfMetabolites, int mult, bool diff);

  /**
   * creates n copies of the objects in souce. Diffusion reactions are created for each of
   * the metabs in listOfMetabolites, so that a linear chain is formed.
   */
  void createLinearArray(const SetOfModelElements & source, size_t n, const std::set< std::string  > & setOfMetabolites, const std::vector< std::string > vecCompartmentLabel = std::vector< std::string >() );

  /**
   * creates nx*ny copies of the objects in souce. Diffusion reactions are created for each of
   * the metabs in listOfMetabolites, so that a rectangular array is formed.
   */
  void createRectangularArray(const SetOfModelElements & source, size_t nx, size_t ny, const std::set< std::string  > & setOfMetabolites);

  /**
   * adds the contents of the source model to the current model.
   * A list of created model elements is returned
   */
  std::set< const CDataObject * > copyCompleteModel(const CModel* pSourceModel);

  /**
   * creates one duplicate of the set of elements specified by source.
   * the string index is added to the object names
   */
  bool duplicate(const SetOfModelElements & source, const std::string & index, ElementsMap & emap);

  void duplicateCompartment(const CCompartment* source, const std::string & index, const SetOfModelElements & sourceSet, ElementsMap & emap);
  void duplicateMetab(const CMetab* source, const std::string & index, const SetOfModelElements & sourceSet, ElementsMap & emap);
  void duplicateReaction(const CReaction* source, const std::string & index, const SetOfModelElements & sourceSet, ElementsMap & emap);
  void duplicateGlobalQuantity(const CModelValue* source, const std::string & index, const SetOfModelElements & sourceSet, ElementsMap & emap);
  void duplicateEvent(CEvent* source, const std::string & index, const SetOfModelElements & sourceSet, ElementsMap & emap);

  /**
   * steps through an expression and replaces references to objects with references to their duplicate.
   * (In case an object that should be duplicated according to sourceSet is not yet duplicated
   *  according to emap, the duplication is performed also)
   */
  void updateExpression(CExpression* exp, const std::string & index, const SetOfModelElements & sourceSet, ElementsMap & emap);

  /**
   * Check if a given expression contains any of the objects in the sourceSet
   */
  bool expressionContainsObject(const CExpression* exp, const SetOfModelElements & sourceSet);

  /**
   * Create a first order reversible mass action reaction.
   * This is intended for diffusion between copies of model parts.
   * The method takes a reaction name, and keys of two metabolites and
   * a global quantity that is used as a rate constant.
   */
  void createDiffusionReaction(const std::string & name,
                               const std::string & metabkey1, const std::string & metabkey2,
                               const std::string & parameterkey);

  void replaceInModel(const ElementsMap & emap, bool remove);

  void replaceInCompartment(CCompartment* pX, const ElementsMap & emap);
  void replaceInMetab(CMetab* pX, const ElementsMap & emap);
  void replaceInReaction(CReaction* pX, const ElementsMap & emap);
  void replaceInModelEntity(CModelEntity* pX, const ElementsMap & emap);
  void replaceInEvent(CEvent* pX, const ElementsMap & emap);

  void replaceInExpression(CExpression* exp, const ElementsMap & emap);

  /**
   *  Tests if there are model elements that refer to the given model entity
   */
  bool existDependentEntities(const CDataObject* pObj);

protected:

  CModel * mpModel;
  const CModel * mpSourceModel;
};

#endif // CMODELEXPANSION_H
