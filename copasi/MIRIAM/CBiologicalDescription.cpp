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

#include "copasi.h"

#include "CRDFGraph.h"

#include "CModelMIRIAMInfo.h"
#include "CReference.h"

#include "report/CKeyFactory.h"
#include "CopasiDataModel/CDataModel.h"
#include "model/CModel.h"
#include "copasi/core/CRootContainer.h"
#include "undo/CData.h"

// static
CBiologicalDescription * CBiologicalDescription::fromData(const CData & data)
{
  return new CBiologicalDescription(data.getProperty(CData::OBJECT_NAME).toString(),
                                    NO_PARENT);
}

// virtual
CData CBiologicalDescription::toData() const
{
  CData Data = CDataContainer::toData();

  Data.addProperty(CData::MIRIAM_PREDICATE, getPredicate());
  Data.addProperty(CData::MIRIAM_RESOURCE, getResource());
  Data.addProperty(CData::MIRIAM_ID, getId());

  return Data;
}

// virtual
bool CBiologicalDescription::applyData(const CData & data)
{
  bool success = CDataContainer::applyData(data);

  if (data.isSetProperty(CData::MIRIAM_PREDICATE))
    {
      setPredicate(data.getProperty(CData::MIRIAM_PREDICATE).toString());
    }

  if (data.isSetProperty(CData::MIRIAM_RESOURCE))
    {
      setResource(data.getProperty(CData::MIRIAM_RESOURCE).toString());
    }

  if (data.isSetProperty(CData::MIRIAM_ID))
    {
      setId(data.getProperty(CData::MIRIAM_ID).toString());
    }

  return success;
}

CBiologicalDescription::CBiologicalDescription(const std::string & objectName,
    const CDataContainer * pParent):
  CDataContainer(objectName, pParent, "BiologicalDescription"),
  mTriplet(),
  mKey(CRootContainer::getKeyFactory()->add("BiologicalDescription", this)),
  mResource(NULL)
{}

CBiologicalDescription::CBiologicalDescription(const CRDFTriplet & triplet,
    const std::string & objectName,
    const CDataContainer * pParent):
  CDataContainer(objectName, pParent, "BiologicalDescription"),
  mTriplet(triplet),
  mKey(CRootContainer::getKeyFactory()->add("BiologicalDescription", this)),
  mResource(mTriplet.pObject)
{}

CBiologicalDescription::CBiologicalDescription(const CBiologicalDescription & src,
    const CDataContainer * pParent):
  CDataContainer(src, pParent),
  mTriplet(src.mTriplet),
  mKey(CRootContainer::getKeyFactory()->add("Creator", this)),
  mResource(src.mResource)
{}

CBiologicalDescription::~CBiologicalDescription()
{
  CRootContainer::getKeyFactory()->remove(mKey);
}

/**
 * Retrieve the MIRIAM resource object for this reference
 */
const CMIRIAMResourceObject & CBiologicalDescription::getMIRIAMResourceObject() const
{
  return mResource;
}

const CRDFTriplet & CBiologicalDescription::getTriplet() const
{return mTriplet;}

const std::string & CBiologicalDescription::getKey() const
{return mKey;}

std::string CBiologicalDescription::getPredicate() const
{return CRDFPredicate::getDisplayName(mTriplet.Predicate);}

std::string CBiologicalDescription::getResource() const
{return mResource.getDisplayName();}

const std::string & CBiologicalDescription::getId() const
{return mResource.getId();}

void CBiologicalDescription::setPredicate(const std::string & predicate)
{
  CRDFPredicate Predicate(CRDFPredicate::getPredicateFromDisplayName(predicate));

  if (Predicate == mTriplet.Predicate)
    return;

  // Add the edge with the new predicate without any object creation.
  mTriplet.pSubject->addEdge(Predicate, mTriplet.pObject);

  // Remove the edge with the predicate without destroying any objects.
  mTriplet.pSubject->removeEdge(mTriplet.Predicate, mTriplet.pObject);

  // Set the new predicate
  mTriplet.Predicate = Predicate;
}

void CBiologicalDescription::setResource(const std::string & resource)
{
  mResource.setDisplayName(resource);
  mTriplet.pObject->getObject().setResource(mResource.getURI(), false);
}

void CBiologicalDescription::setId(const std::string & id)
{
  mResource.setId(id);
  mTriplet.pObject->getObject().setResource(mResource.getURI(), false);
}

std::string CBiologicalDescription::getURI() const
{return mResource.getURI();}

void CBiologicalDescription::clearInvalidEntries()
{
  if ((mTriplet.Predicate == CRDFPredicate::unknown &&
       mTriplet.Predicate.getURI() == "") ||
      !mResource.isValid())
    {
      // Remove the edge with the predicate and object.
      mTriplet.pSubject->removeEdge(mTriplet.Predicate, mTriplet.pObject);
    }
}
