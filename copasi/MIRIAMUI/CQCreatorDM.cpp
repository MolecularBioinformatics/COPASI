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

#include "CQCreatorDM.h"
#include "UI/CQMessageBox.h"

#include "copasi.h"

#include "UI/qtUtilities.h"

#include "MIRIAM/CModelMIRIAMInfo.h"

CQCreatorDM::CQCreatorDM(CMIRIAMInfo* MIRIAMInfo, QObject *parent)
  : CQBaseDataModel(parent, NULL)

{
  mpMIRIAMInfo = MIRIAMInfo;
}

int CQCreatorDM::rowCount(const QModelIndex&) const
{
  return (int) mpMIRIAMInfo->getCreators().size() + 1;
}
int CQCreatorDM::columnCount(const QModelIndex&) const
{
  return TOTAL_COLS_CREATORS;
}

QVariant CQCreatorDM::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= rowCount())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      if (isDefaultRow(index))
        {
          if (index.column() == COL_ROW_NUMBER)
            return QVariant(QString(""));
          else
            return QVariant(QString(""));
        }
      else
        {
          switch (index.column())
            {
              case COL_ROW_NUMBER:
                return QVariant(index.row() + 1);

              case COL_FAMILY_NAME:
                return QVariant(QString(FROM_UTF8(mpMIRIAMInfo->getCreators()[index.row()].getFamilyName())));

              case COL_GIVEN_NAME:
                return QVariant(QString(FROM_UTF8(mpMIRIAMInfo->getCreators()[index.row()].getGivenName())));

              case COL_EMAIL:
                return QVariant(QString(FROM_UTF8(mpMIRIAMInfo->getCreators()[index.row()].getEmail())));

              case COL_ORG:
                return QVariant(QString(FROM_UTF8(mpMIRIAMInfo->getCreators()[index.row()].getORG())));
            }
        }
    }

  return QVariant();
}

QVariant CQCreatorDM::headerData(int section, Qt::Orientation orientation,
                                 int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal)
    {
      switch (section)
        {
          case COL_ROW_NUMBER:
            return QVariant(QString("#"));

          case COL_FAMILY_NAME:
            return QVariant(QString("Family Name"));

          case COL_GIVEN_NAME:
            return QVariant(QString("Given Name"));

          case COL_EMAIL:
            return QVariant(QString("Email"));

          case COL_ORG:
            return QVariant(QString("Organization"));

          default:
            return QVariant();
        }
    }
  else
    return QString("%1").arg(section + 1);
}

bool CQCreatorDM::setData(const QModelIndex &index, const QVariant &value,
                          int role)
{
  if (index.isValid() && role == Qt::EditRole)
    {
      if (isDefaultRow(index))
        {
          if (index.data() != value)
            insertRow(rowCount(), index);
          else
            return false;
        }

      switch (index.column())
        {
          case COL_FAMILY_NAME:
            mpMIRIAMInfo->getCreators()[index.row()].setFamilyName(TO_UTF8(value.toString()));
            break;

          case COL_GIVEN_NAME:
            mpMIRIAMInfo->getCreators()[index.row()].setGivenName(TO_UTF8(value.toString()));
            break;

          case COL_EMAIL:
            mpMIRIAMInfo->getCreators()[index.row()].setEmail(TO_UTF8(value.toString()));
            break;

          case COL_ORG:
            mpMIRIAMInfo->getCreators()[index.row()].setORG(TO_UTF8(value.toString()));
            break;
        }

      //emit dataChanged(index, index);
      emit notifyGUI(ListViews::MIRIAM, ListViews::CHANGE, "");
      return true;
    }

  return false;
}

bool CQCreatorDM::insertRows(int position, int rows, const QModelIndex&)
{
  beginInsertRows(QModelIndex(), position, position + rows - 1);

  for (int row = 0; row < rows; ++row)
    {
      mpMIRIAMInfo->createCreator("");
    }

  endInsertRows();

  emit notifyGUI(ListViews::MIRIAM, ListViews::ADD, "");
  return true;
}

bool CQCreatorDM::removeRows(int position, int rows)
{
  if (rows <= 0)
    return true;

  beginRemoveRows(QModelIndex(), position, position + rows - 1);

  for (int row = 0; row < rows; ++row)
    {
      mpMIRIAMInfo->removeCreator(position);
    }

  endRemoveRows();

  emit notifyGUI(ListViews::MIRIAM, ListViews::DELETE, "");
  return true;
}

bool CQCreatorDM::removeRows(QModelIndexList rows, const QModelIndex&)
{
  if (rows.isEmpty())
    return false;

//Build the list of pointers to items to be deleted
//before actually deleting any item.
  QList <CCreator *> pCreators;
  QModelIndexList::const_iterator i;

  for (i = rows.begin(); i != rows.end(); ++i)
    {
      if (!isDefaultRow(*i) && &mpMIRIAMInfo->getCreators()[i->row()])
        pCreators.append(&mpMIRIAMInfo->getCreators()[i->row()]);
    }

  bool retVal = false, askEveryItem = true;
  QMessageBox::StandardButton choice = QMessageBox::NoToAll;
  QList <CCreator *>::const_iterator j;

  for (j = pCreators.begin(); j != pCreators.end(); ++j)
    {
      CCreator * pCreator = *j;

      size_t delRow =
        mpMIRIAMInfo->getCreators().CDataVector< CCreator >::getIndex(pCreator);

      if (delRow != C_INVALID_INDEX)
        {
          if (askEveryItem)
            {
              QString givenName = data(this->index((int) delRow, COL_GIVEN_NAME), Qt::DisplayRole).toString();
              QString familyName = data(this->index((int) delRow, COL_FAMILY_NAME), Qt::DisplayRole).toString();
              QString msg = "Do you want to delete author '";

              if (!givenName.isNull())
                {
                  msg.append(givenName);
                }

              if (!familyName.isNull())
                {
                  msg.append(" ");
                  msg.append(familyName);
                }

              msg.append("'?");

              choice = CQMessageBox::question(NULL, tr("Confirm Delete"), msg,
                                              QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll,
                                              QMessageBox::No);
            }

          if (choice == QMessageBox::NoToAll)
            {return retVal;}
          else if (choice == QMessageBox::Yes)
            {retVal = removeRow((int) delRow);}
          else if (choice == QMessageBox::YesToAll)
            {
              askEveryItem = false;
              retVal = removeRow((int) delRow);
            }
        }
    }

  return true;
}
