/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportHDF5TreeModelItem.h"

#include <QtCore/QStringList>

#include "H5Support/H5Utilities.h"

ImportHDF5TreeModelItem::ImportHDF5TreeModelItem(hid_t fileId, const QString& data, ImportHDF5TreeModelItem* parent)
: m_ItemData(QVariant(data))
, m_ParentItem(parent)
, m_FileId(fileId)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5TreeModelItem::~ImportHDF5TreeModelItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5TreeModelItem::appendChild(ImportHDF5TreeModelItem* item)
{
  m_ChildItems.append(item);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5TreeModelItem* ImportHDF5TreeModelItem::child(int row)
{
  if(m_ChildItemsInitialized == 0)
  {
    initializeChildItems();
  }
  return m_ChildItems.value(row);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportHDF5TreeModelItem::childCount()
{
  if(m_ChildCount < 0)
  {
    initializeChildCount();
  }
  return m_ChildCount;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportHDF5TreeModelItem::columnCount()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant ImportHDF5TreeModelItem::data(int column)
{
  return m_ItemData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportHDF5TreeModelItem* ImportHDF5TreeModelItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportHDF5TreeModelItem::row()
{
  if(m_ParentItem)
    return m_ParentItem->m_ChildItems.indexOf(const_cast<ImportHDF5TreeModelItem*>(this));

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportHDF5TreeModelItem::isGroup()
{
  if(m_ChildCount < 0)
  {
    initializeChildCount();
  }
  return m_IsGroup;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::CheckState ImportHDF5TreeModelItem::getCheckState()
{
  return m_CheckState;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5TreeModelItem::setCheckState(Qt::CheckState checkState)
{
  m_CheckState = checkState;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportHDF5TreeModelItem::numAttributes()
{
  if(m_NumAttrs < 0)
  {
    initializeChildCount();
  }
  return m_NumAttrs;
}

int ImportHDF5TreeModelItem::numDimensions()
{
  return m_NumDims;
}

bool ImportHDF5TreeModelItem::isImage()
{
  if(m_ChildItemsInitialized == 0)
  {
    initializeChildItems();
  }
  return m_IsImage;
}

bool ImportHDF5TreeModelItem::isString()
{
  if(m_ChildItemsInitialized == 0)
  {
    initializeChildItems();
  }
  return m_IsString;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon ImportHDF5TreeModelItem::icon()
{
  QString iconName = "";

  if(isGroup() == true)
  {
    iconName = ":/SIMPL/icons/images/folder_blue.png";
  }
  else
  {
    iconName = ":/SIMPL/icons/images/cube_molecule.png";
  }

  return QIcon(iconName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5TreeModelItem::initializeChildCount()
{
  if(m_FileId < 0)
  {
    return;
  }
  // std::cout << "ImportHDF5TreeModelItem::initializeChildCount()" << std::endl;
  herr_t err = 0;
  // Build up an HDF path to the current group
  ImportHDF5TreeModelItem* currentParent = m_ParentItem;
  if(NULL == currentParent)
  {
    QString name("/"); // Add the "HDF5 Root Directory"
    ImportHDF5TreeModelItem* item = new ImportHDF5TreeModelItem(m_FileId, name, const_cast<ImportHDF5TreeModelItem*>(this));
    m_ChildItems.append(item);
    m_ChildCount = 1;
    m_NumAttrs = 0;
    m_NumDims = 0;
    m_IsGroup = true;
    return;
  }

  QString path = generateHDFPath();

  hid_t obj_id = H5Utilities::openHDF5Object(m_FileId, path.toStdString());
  if(obj_id > 0)
  {
    H5O_info_t object_info;
    err = H5Oget_info(obj_id, &object_info);
    m_NumAttrs = object_info.num_attrs;
  }
  H5Utilities::closeHDF5Object(obj_id);

  if(H5Utilities::isGroup(m_FileId, path.toStdString()))
  {
    m_IsGroup = true;
    hid_t groupId = H5Gopen(m_FileId, path.toStdString().c_str(), H5P_DEFAULT);
    if(groupId < 0)
    {
      std::cout << "Could not open Group '" << path.toStdString() << "'" << __FILE__ << ":" << __LINE__ << std::endl;
      return;
    }

    H5G_info_t group_info;
    err = H5Gget_info(groupId, &group_info);
    m_ChildCount = group_info.nlinks;

    err = H5Gclose(groupId);
  }
  else
  {
    m_ChildCount = 0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportHDF5TreeModelItem::initializeChildItems()
{
  if(m_FileId < 0)
  {
    return;
  }
  // std::cout << "ImportHDF5TreeModelItem::initializeChildItems()" << std::endl;
  herr_t err = 0;
  // Build up an HDF path to the current group
  ImportHDF5TreeModelItem* currentParent = m_ParentItem;
  if(NULL == currentParent)
  {
    QString name("/"); // Add the "HDF5 Root Directory"
    ImportHDF5TreeModelItem* item = new ImportHDF5TreeModelItem(m_FileId, name, const_cast<ImportHDF5TreeModelItem*>(this));
    m_ChildItems.append(item);
    m_ChildItemsInitialized = 1;
    return;
  }

  QString path = generateHDFPath();

  // std::cout << "ImportHDF5TreeModelItem::initializeChildItems() - Generated Path as: " << path.toStdString() << std::endl;
  // Check to see if the path is a group or data set
  if(H5Utilities::isGroup(m_FileId, path.toStdString()))
  {
    m_IsGroup = true;
    hid_t groupId = H5Gopen(m_FileId, path.toStdString().c_str(), H5P_DEFAULT);
    if(groupId < 0)
    {
      std::cout << "Could not open Group '" << path.toStdString() << "'" << __FILE__ << ":" << __LINE__ << std::endl;
      return;
    }

    H5O_info_t object_info;
    err = H5Oget_info(groupId, &object_info);
    m_NumAttrs = object_info.num_attrs;

    std::list<std::string> itemList;
    herr_t err = H5Utilities::getGroupObjects(groupId, H5Utilities::H5Support_ANY, itemList);
    if(err < 0)
    {
      std::cout << "Error getting group objects. " << __FILE__ << ":" << __LINE__ << std::endl;
      err = H5Gclose(groupId);
      return;
    }

    for(std::list<std::string>::iterator iter = itemList.begin(); iter != itemList.end(); ++iter)
    {
      // std::cout << "Adding Child with name '" << *iter << "'" << std::endl;
      QString name((*iter).c_str());
      ImportHDF5TreeModelItem* item = new ImportHDF5TreeModelItem(m_FileId, name, const_cast<ImportHDF5TreeModelItem*>(this));
      m_ChildItems.append(item);
    }
    m_ChildCount = itemList.size();
    err = H5Gclose(groupId);
  }
  else // Get some basic information about the data set
  {
    // std::cout << "TreeModelItem is a DataSet" << std::endl;
    hid_t obj_id = H5Utilities::openHDF5Object(m_FileId, path.toStdString());
    if(obj_id > 0)
    {
      H5O_info_t object_info;
      err = H5Oget_info(obj_id, &object_info);
      m_NumAttrs = object_info.num_attrs;
    }
    if(m_NumAttrs > 0)
    {
      // Test for Image Class attribute
      std::string data;
      err = H5Lite::readStringAttribute(m_FileId, path.toStdString(), "CLASS", data);
      if(err >= 0)
      {
        m_IsImage = true;
      }
    }

    std::vector<hsize_t> dims;
    H5T_class_t data_type;
    size_t type_size;
    err = H5Lite::getDatasetInfo(m_FileId, path.toStdString(), dims, data_type, type_size);
    m_NumDims = dims.size();

    switch(data_type)
    {
    case H5T_STRING:
      m_IsString = true;
      m_DataType = "H5T_STRING";
      break;
    default:
      m_DataType = "OTHER";
    }

    err = H5Utilities::closeHDF5Object(obj_id);
    if(err < 0)
    {
      // TODO: Catch this error
    }
  }
  m_ChildItemsInitialized = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportHDF5TreeModelItem::generateHDFPath()
{
  if(m_FileId < 0)
  {
    return QString();
  }

  // Build up an HDF path to the current group
  ImportHDF5TreeModelItem* currentParent = m_ParentItem;
  QString path = m_ItemData.toString();
  // std::cout << "Current Item Data: " << path.toStdString() << std::endl;
  if(currentParent != NULL && currentParent->m_ItemData.toString().compare("HEADER") == 0)
  {
    // std::cout << "path=" << path.toStdString() << std::endl;
    currentParent = NULL; // We are at the top
  }
  while(currentParent != NULL)
  {
    QString parentName = currentParent->data(0).toString();
    if(parentName.compare("/") == 0)
    {
      path = parentName + path;
    }
    else
    {
      path = parentName + "/" + path;
    }
    // std::cout << "path=" << path.toStdString() << std::endl;
    // Get the parents Parent..
    currentParent = currentParent->parent();
    if(currentParent->parent() == NULL)
    {
      currentParent = NULL; // We are at the top
    }
  }
  return path;
}
