// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

#ifndef COPASI_CUndoData
#define COPASI_CUndoData

#include <ctime>
#include <set>

#include "CData.h"

class CDataObject;
class CDataContainer;
class CDataModel;
class CMetab;

template < class CType > class CDataVector;

class CUndoData
{
private:
  CUndoData();

public:
  enum struct Type
  {
    INSERT = 0,
    CHANGE,
    REMOVE,
    __SIZE
  };

  static const CEnumAnnotation< std::string, Type > TypeName;

  CUndoData(const Type & type, const CDataObject * pObject, const size_t & authorId = C_INVALID_INDEX);

  CUndoData(const CUndoData & src);

  ~CUndoData();

  const Type & getType() const;

  bool addProperty(const CData::Property & property, const CDataValue & value);

  bool addProperty(const CData::Property & property, const CDataValue & oldValue, const CDataValue & newValue);

  bool isSetProperty(const CData::Property & property) const;

  bool appendData(const CData & data);

  bool appendData(const CData & oldData, const CData & newData);

  bool addDependentData(const CUndoData & dependentData);

  bool addDependentData(std::vector< CUndoData > & dependentData, bool sort);

  const CData & getOldData() const;

  const CData & getNewData() const;

  const std::vector< CUndoData > & getDependentData() const;

  std::vector< CUndoData > & getDependentData();

  const std::set< std::string > & getChangedProperties() const;

  bool apply(const CDataModel & dataModel) const;

  bool undo(const CDataModel & dataModel) const;

  const std::time_t & getTime() const;

  std::tm * getLocalTime() const;

  std::tm * getUTCTime() const;

  const size_t getAuthorID() const;

  std::string getObjectDisplayName() const;

  std::string getObjectType() const;

  bool operator < (const CUndoData & rhs) const;

private:
  bool addProperty(const std::string & name, const CDataValue & value);

  bool addProperty(const std::string & name, const CDataValue & oldValue, const CDataValue & newValue);

  bool isSetProperty(const std::string & name) const;

  bool insert(const CDataModel & dataModel, const bool & apply) const;
  bool remove(const CDataModel & dataModel, const bool & apply) const;
  bool change(const CDataModel & dataModel, const bool & apply) const;
  bool processDependentData(const CDataModel & dataModel, const bool & apply) const;

  const CData & getData(const bool & apply) const;

  static CDataContainer * getParent(const CDataModel & dataModel, const CData & data);
  static CDataObject * getObject(const CDataModel & dataModel, const CData & data);

  Type mType;

  CData mOldData;

  CData mNewData;

  std::vector< CUndoData > mDependentData;

  std::time_t mTime;

  size_t mAuthorID;

  std::set< std::string > mChangedProperties;
};

#endif // COPASI_CUndoData
