// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2013 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#ifndef COPASI_CQParameterSetsDM
#define COPASI_CQParameterSetsDM

#include "CQBaseDataModel.h"

#define COL_ROW_NUMBER   0
#define COL_NAME         1
#define TOTAL_COLS_PARAMETERSET       2

class CModelParameterSet;
template <class CType> class CDataVectorN;

class CQParameterSetsDM : public CQBaseDataModel
{
  Q_OBJECT

public:
  CQParameterSetsDM(QObject *parent = 0);
  virtual ~CQParameterSetsDM();

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value,
                       int role = Qt::EditRole);
  virtual bool isDefaultRow(const QModelIndex& i) const;
  bool removeRows(QModelIndexList rows, const QModelIndex &index = QModelIndex());
  void setListOfModelParameterSets(CDataVectorN< CModelParameterSet > * pListOfModelParameterSets);

  virtual bool insertRows(int position, int rows, const QModelIndex & source);

  virtual bool clear();

protected:
  virtual bool removeRows(int position, int rows);

private:
  CDataVectorN< CModelParameterSet > * mpListOfParameterSets;
};

#endif // COPASI_CQParameterSetsDM
