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

/**
 *  CCreators: Stores author name, email and other (vCard) details.
 *
 *
 */

#ifndef COPASI_CCreator
#define COPASI_CCreator

#include <string>

#include "copasi/MIRIAM/CRDFTriplet.h"
#include "copasi/MIRIAM/CRDFPredicate.h"

#include "copasi/core/CDataContainer.h"

class CCreator : public CDataContainer
{
  // Attributes
private:
  /**
   * Triplet in the RDF graph representing the reference.
   */
  CRDFTriplet mTriplet;

  /**
   * The predicate path to the node.
   */
  CRDFPredicate::Path mNodePath;

  /**
   * The key of the object needed for the COPASI tables.
   */
  std::string mKey;

  // Operations
public:
  /**
   * Static method to create a CDataObject based on the provided data
   * @param const CData & data
   * @return CCreator * pDataObject
   */
  static CCreator * fromData(const CData & data);

  /**
   * Retrieve the data describing the object
   * @return CData data
   */
  virtual CData toData() const;

  /**
   * Apply the provided data to the object
   * @param const CData & data
   * @return bool success
   */
  virtual bool applyData(const CData & data);

  /**
   * Default constructor
   * @param const std::string & objectName
   * @param const CDataContainer * pParent (default: NULL)
   */
  CCreator(const std::string & objectName,
           const CDataContainer * pParent = NO_PARENT);

  /**
   * Specific constructor
   * @param const CRDFTriplet & triplet
   * @param const std::string & objectName (default: "")
   * @param const CDataContainer * pParent (default: NULL)
   */
  CCreator(const CRDFTriplet & triplet,
           const std::string & objectName = "",
           const CDataContainer * pParent = NO_PARENT);

  /**
   * Copy constructor
   * @param const CCreator & src
   * @param const CDataContainer * pParent (default: NULL)
   */
  CCreator(const CCreator & src,
           const CDataContainer * pParent);

  /**
   * Destructor
   */
  ~CCreator();

  const CRDFTriplet & getTriplet() const;

  virtual const std::string & getKey() const;

  /**
   *  Add the given name to Last name to get the full Name.
   */
  const std::string & getFamilyName() const;

  const std::string & getGivenName() const;

  const std::string & getEmail() const;

  const std::string & getORG() const;

  void setFamilyName(const std::string & familyName);

  void setGivenName(const std::string & givenName);

  void setEmail(const std::string & Email);

  void setORG(const std::string & Orgname);
};

#endif //COPASI_CCreator
