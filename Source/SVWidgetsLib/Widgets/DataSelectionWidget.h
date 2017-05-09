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

#ifndef _dataselectionwidget_h_
#define _dataselectionwidget_h_

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDropEvent>

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Geometry/IGeometry.h"

#include "SVWidgetsLib/ui_DataSelectionWidget.h"

class QSignalMapper;

class DataSelectionWidget : public QWidget, private Ui::DataSelectionWidget
{
  Q_OBJECT

public:

  enum class ContainerType
  {
    DataContainer = 0,
    AttributeMatrix,
    DataArray
  };

  static const QColor DataContainerColor;
  static const QColor AttributeMatrixColor;
  static const QColor DataArrayColor;

  DataSelectionWidget(QWidget* parent);
  ~DataSelectionWidget();

  void setupGui();

  AbstractFilter::Pointer getFilter();
  void setFilter(AbstractFilter::Pointer filter);

  DataArrayPath getDataArrayPath();
  void setDataArrayPath(DataArrayPath path);
  void setDataArrayPath(QString path);

  ContainerType getContainerType();
  void setContainerType(ContainerType type);

  void updateGui();

  void setGeometryTypes(IGeometry::Types types);
  void setAttributeMatrixTypes(QVector<AttributeMatrix::Type> types);
  void setDataArrayTypes(QVector<QString> daTypes);
  void setComponentDimensions(QVector<QVector<size_t>> cDims);

signals:
  void dataArrayPathChanged();

protected:
  void createSelectionMenu();
  //void createAttributeMatrixSelectionMenu(DataContainer::Pointer dc);
  //void createDataArraySelectionMenu(AttributeMatrix::Pointer am);

  void updateRequirementTooltip();
  void updateSelectionTooltip();

  IGeometry::Types getGeometryTypes();
  QVector<AttributeMatrix::Type> getAttributeMatrixTypes();
  QVector<QString> getDataArrayTypes();
  QVector<QVector<size_t>> getComponentDimensions();

  //void mousePressEvent(QMouseEvent* event);
  void dragEnterEvent(QDragEnterEvent* event);
  void dragLeaveEvent(QDragLeaveEvent* event);
  void dragMoveEvent(QDragMoveEvent* event);
  void dropEvent(QDropEvent* event);

private:
  AbstractFilter::Pointer m_filter;
  
  QPointer<QSignalMapper> m_MenuMapper;

  ContainerType m_containerType;
  DataArrayPath m_dataArrayPath;

  QString m_requirementTooltip;
  QString m_selectionTooltip;

  IGeometry::Types m_geometryTypes;
  QVector<AttributeMatrix::Type> m_attributeMatrixTypes;
  QVector<QString> m_dataArrayTypes;
  QVector<QVector<size_t>> m_componentDimensions;
};

#endif
