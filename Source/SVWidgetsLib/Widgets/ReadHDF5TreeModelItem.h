/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _readhdf5treemodelitem_h_
#define _readhdf5treemodelitem_h_

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

#include <hdf5.h>

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVWidgetsLib_EXPORT ReadHDF5TreeModelItem
{
public:
  ReadHDF5TreeModelItem(hid_t fileId, const QString &data, ReadHDF5TreeModelItem *parent = 0);
  ~ReadHDF5TreeModelItem();

  void appendChild(ReadHDF5TreeModelItem *child);

  ReadHDF5TreeModelItem *child(int row);
  int childCount() ;
  int columnCount() ;
  QVariant data(int column) ;
  int row() ;
  ReadHDF5TreeModelItem *parent();

  QString generateHDFPath();

  bool isGroup();
  bool isImage();
  bool isTable();
  bool isString();
  Qt::CheckState getCheckState();

  void setCheckState(Qt::CheckState checkState);

  int numAttributes();
  int numDimensions();

  QIcon icon();

protected:
  void initializeChildItems();
  void initializeChildCount();

private:
  QList<ReadHDF5TreeModelItem*>               m_ChildItems;
  int                                         m_ChildItemsInitialized = 0;
  int                                         m_ChildCount = -1;
  QVariant                                    m_ItemData;
  ReadHDF5TreeModelItem*                      m_ParentItem;
  hid_t                                       m_FileId;
  int                                         m_NumAttrs = -1;
  int                                         m_NumDims = -1;
  bool                                        m_IsGroup = false;
  bool                                        m_IsImage = false;
  bool                                        m_IsTable = false;
  bool                                        m_IsString = false;
  std::string                                 m_DataType = "";
  Qt::CheckState                              m_CheckState = Qt::Unchecked;
};

#endif /* _readhdf5treemodelitem_h_ */
