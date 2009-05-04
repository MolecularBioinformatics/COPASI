// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/UI/CQBaseDataModel.cpp,v $
//   $Revision: 1.5 $
//   $Name:  $
//   $Author: aekamal $
//   $Date: 2009/05/04 15:24:00 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include "copasi.h"
#include "CQBaseDataModel.h"

CQBaseDataModel::CQBaseDataModel(QObject *parent)
    : QAbstractTableModel(parent)

{}

Qt::ItemFlags CQBaseDataModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  if (index.column() == COL_ROW_NUMBER)
    return QAbstractItemModel::flags(index);
  else
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool CQBaseDataModel::insertRow()
{
  return insertRows(rowCount() - 1, 1);
}

bool CQBaseDataModel::removeRow(int position)
{
  if (position >= 0 && (position < rowCount() - 1) && !isDefaultRow(position))
    return removeRows(position, 1);
  else
    return false;
}

bool CQBaseDataModel::clear()
{
  return removeRows(0, rowCount() - 1);
}

bool CQBaseDataModel::isDefaultRow(const QModelIndex& i) const
{
  if (!i.isValid())
    {return false;}

  return (i.row() == rowCount() - 1);
}

bool CQBaseDataModel::isDefaultRow(const int row) const
{
  if (row < 0 || row >= rowCount())
    {return false;}

  return (row == rowCount() - 1);
}

QString CQBaseDataModel::createNewName(const QString name, const int nameCol)
{
  QString nname = name;
  unsigned C_INT32 j, jmax = rowCount();

  for (unsigned C_INT32 i = 1;; ++i)
    {
      nname = name + " " + QString::number(i);

      for (j = 0; j < jmax; ++j)
        if (index(j, nameCol).data() == nname) break;

      if (j == jmax) break;
    }

  return nname;
}
