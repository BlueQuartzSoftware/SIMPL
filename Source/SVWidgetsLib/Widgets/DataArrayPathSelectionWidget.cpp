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

#include "DataArrayPathSelectionWidget.h"

#include <QtCore/QMimeData>
#include <QtWidgets\QApplication>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

namespace {
  const QString kNormalColor("#8f8f91");
  const QString kErrorColor("#BC0000");
  const QString kAcceptColor("#8f8f91");
  const QString kRejectColor("#BC0000");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathSelectionWidget::DataArrayPathSelectionWidget(QWidget* parent)
  : QToolButton(parent)
{
  setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));
  setAcceptDrops(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathSelectionWidget::DataType DataArrayPathSelectionWidget::getDataType()
{
  return m_DataType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setDataContainerRequirements(DataContainerSelectionFilterParameter::RequirementType dcReqs)
{
  m_DataType = DataType::DataContainer;
  m_DataContainerReqs = dcReqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setAttrMatrixRequirements(AttributeMatrixSelectionFilterParameter::RequirementType amReqs)
{
  m_DataType = DataType::AttributeMatrix;
  m_AttrMatrixReqs = amReqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setDataArrayRequirements(DataArraySelectionFilterParameter::RequirementType daReqs)
{
  m_DataType = DataType::DataArray;
  m_DataArrayReqs = daReqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerSelectionFilterParameter::RequirementType DataArrayPathSelectionWidget::getDataContainerRequirements()
{
  return m_DataContainerReqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionFilterParameter::RequirementType DataArrayPathSelectionWidget::getAttrMatrixRequirements()
{
  return m_AttrMatrixReqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArraySelectionFilterParameter::RequirementType DataArrayPathSelectionWidget::getDataArrayRequirements()
{
  return m_DataArrayReqs;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setDataArrayPath(DataArrayPath dap)
{
  setText(dap.serialize(Detail::Delimiter));
  emit pathChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataArrayPathSelectionWidget::getDataArrayPath()
{
  return DataArrayPath::Deserialize(text(), Detail::Delimiter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathSelectionWidget::checkCurrentPath()
{
  return checkPathReqs(getDataArrayPath());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathSelectionWidget::checkPathReqs(DataArrayPath path)
{
  switch(m_DataType)
  {
  case DataType::DataContainer:
    return checkDataContainerReqs(path);
  case DataType::AttributeMatrix:
    return checkAttributeMatrixReqs(path);
  case DataType::DataArray:
    return checkDataArrayReqs(path);
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathSelectionWidget::checkDataContainerReqs(DataArrayPath path)
{
  if(path.getDataContainerName().isEmpty())
  {
    return false;
  }
  if(false == path.getAttributeMatrixName().isEmpty() && path.getDataArrayName().isEmpty())
  {
    return false;
  }
  if(nullptr == m_Filter)
  {
    return false;
  }

  // Check that the DataContainer exists
  DataContainer::Pointer dc = m_Filter->getDataContainerArray()->getDataContainer(path);
  if(nullptr == dc)
  {
    return false;
  }

  // Check Geometry
  if(m_DataContainerReqs.dcGeometryTypes.size() > 0)
  {
    IGeometry::Pointer geom = dc->getGeometry();
    if(nullptr == geom)
    {
      return false;
    }
    if(false == m_DataContainerReqs.dcGeometryTypes.contains(geom->getGeometryType()))
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathSelectionWidget::checkAttributeMatrixReqs(DataArrayPath path)
{
  if(path.getDataContainerName().isEmpty() || path.getAttributeMatrixName().isEmpty())
  {
    return false;
  }
  if(false == path.getDataArrayName().isEmpty())
  {
    return false;
  }
  if(nullptr == m_Filter)
  {
    return false;
  }

  // Check that the DataContainer exists
  DataContainer::Pointer dc = m_Filter->getDataContainerArray()->getDataContainer(path);
  if(nullptr == dc)
  {
    return false;
  }

  // Check if geometry exists and matches the requirements only if a geometry is required
  if(m_AttrMatrixReqs.dcGeometryTypes.size() > 0)
  {
    IGeometry::Pointer geom = dc->getGeometry();
    if(nullptr == geom)
    {
      return false;
    }
    if(false == m_AttrMatrixReqs.dcGeometryTypes.contains(geom->getGeometryType()))
    {
      return false;
    }
  }

  // Check AttributeMatrix
  AttributeMatrix::Pointer am = dc->getAttributeMatrix(path);
  if(nullptr == am)
  {
    return false;
  }

  if(false == m_AttrMatrixReqs.amTypes.size() == 0 || m_AttrMatrixReqs.amTypes.contains(am->getType()))
  {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathSelectionWidget::checkDataArrayReqs(DataArrayPath path)
{
  if(path.getDataContainerName().isEmpty() || path.getAttributeMatrixName().isEmpty() || path.getDataArrayName().isEmpty())
  {
    return false;
  }
  if(nullptr == m_Filter)
  {
    return false;
  }

  // Check that the DataContainer exists
  DataContainer::Pointer dc = m_Filter->getDataContainerArray()->getDataContainer(path);
  if(nullptr == dc)
  {
    return false;
  }

  // Check if geometry exists and matches the requirements only if a geometry is required
  if(m_DataArrayReqs.dcGeometryTypes.size() > 0)
  {
    IGeometry::Pointer geom = dc->getGeometry();
    if(nullptr == geom)
    {
      return false;
    }
    if(false == m_DataArrayReqs.dcGeometryTypes.contains(geom->getGeometryType()))
    {
      return false;
    }
  }

  // Check AttributeMatrix
  AttributeMatrix::Pointer am = dc->getAttributeMatrix(path);
  if(nullptr == am)
  {
    return false;
  }

  if(false == m_DataArrayReqs.amTypes.size() == 0 || m_DataArrayReqs.amTypes.contains(am->getType()))
  {
    return false;
  }

  // Check DataArray
  IDataArray::Pointer da = am->getAttributeArray(path.getDataArrayName());
  if(nullptr == da)
  {
    return false;
  }

  if(false == m_DataArrayReqs.daTypes.size() == 0 || m_DataArrayReqs.daTypes.contains(da->getTypeAsString()))
  {
    return false;
  }

  if(false == m_DataArrayReqs.componentDimensions.size() == 0 || m_DataArrayReqs.componentDimensions.contains(da->getComponentDimensions()))
  {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::enterEvent(QEvent* event)
{
  switch(m_DataType)
  {
  case DataType::DataContainer:
    emit viewPathsMatchingReqs(m_DataContainerReqs);
    break;
  case DataType::AttributeMatrix:
    emit viewPathsMatchingReqs(m_AttrMatrixReqs);
    break;
  case DataType::DataArray:
    emit viewPathsMatchingReqs(m_DataArrayReqs);
    break;
  }

  changeStyleSheet(Style::Hover);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::leaveEvent(QEvent* event)
{
  emit endViewPaths();
  changeStyleSheet(Style::Normal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::dragEnterEvent(QDragEnterEvent* event)
{
  const QMimeData* mime = event->mimeData();
  QByteArray data = mime->data(SIMPLView::DragAndDrop::BookmarkItem);
  QString dataStr = QString::fromUtf8(data);

  DataArrayPath path = DataArrayPath::Deserialize(dataStr, "|");

  if(checkPathReqs(path))
  {
    changeStyleSheet(Style::DragEnabled);
    event->accept();
  }
  else
  {
    changeStyleSheet(Style::DragDisabled);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
  changeStyleSheet(Style::Normal);

  QToolButton::dragLeaveEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::dropEvent(QDropEvent* event)
{
  const QMimeData* mime = event->mimeData();
  QByteArray data = mime->data(SIMPLView::DragAndDrop::BookmarkItem);
  QString dataStr = QString::fromUtf8(data);

  setDataArrayPath(DataArrayPath::Deserialize(dataStr, "|"));
  event->accept();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setFilter(AbstractFilter* filter)
{
  m_Filter = filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::changeStyleSheet(Style styleType)
{
  bool exists = checkCurrentPath();

  QString styleSheet;
  QTextStream ss(&styleSheet);

  QFont font;
  font.setBold(true);
  font.setItalic(true);
  font.setWeight(QFont::Bold);
  font.setStyleStrategy(QFont::PreferAntialias);
  font.setFamily(QtSStyles::GetUIFont());

  QString fontString;
  QTextStream in(&fontString);

#if defined(Q_OS_MAC)
  font.setPointSize(12);
#elif defined(Q_OS_WIN)
  font.setPointSize(10);
#else
  font.setPointSize(11);
  in << "color; #000000;\n";
  in << "font-weight: Medium;";
#endif

  in << "font: " << font.weight() << " " << font.pointSize() << "pt \"" << font.family() << "\";";

  ss << "QToolButton {\n";
  switch(styleType)
  {
  case Style::Normal:
  case Style::Hover:
    if(exists)
    {
      ss << " border: 1px solid " << ::kNormalColor << ";\n";
    }
    else
    {
      ss << " border: 1px solid " << ::kErrorColor << ";\n";
    }
    break;
  case Style::DragEnabled:
    ss << " border: 1px solid " << ::kAcceptColor << ";\n";
    break;
  case Style::DragDisabled:
    ss << " border: 1px solid " << ::kRejectColor << ";\n";
    break;
  }
  ss << " border-radius: 4px;\n";
  ss << " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #DDDDDD, stop: 1 #FFFFFF);\n";
  ss << fontString << "\n";
  ss << " padding-left: 16px;\n";
  ss << " padding-right: 12px;\n";
  ss << " padding-top: 2px;\n";
  ss << " padding-bottom: 2px;\n";
  ss << "}\n";

  ss << "QToolButton::menu-indicator {\n";
  ss << " subcontrol-origin: content;\n";
  ss << " subcontrol-position:  right; /* */\n";
  ss << "}\n";

  ss << "QToolButton::menu-indicator:pressed, QToolButton::menu-indicator:open {\n";
  ss << " position: relative;\n";
  ss << "}\n";

  ss << "QToolButton:pressed {\n";
  ss << " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\nstop: 0 " << QApplication::palette().highlight().color().name() << ", stop: 1 #FFFFFF);\n";
  ss << "}\n";

  ss << "QToolButton:flat {\n";
  ss << " border: none;\n";
  ss << "}\n";

  ss << " QToolTip {\
              border: 2px solid #434343;\
              padding: 2px;\
              border-radius: 3px;\
              opacity: 255;\
              background-color: #FFFCEA;\
              color: #000000;\
              }";
  //  ss << "QToolButton:hover {\n";
  //  if(exists)
  //  {
  //    ss << " border: 2px solid  " << ::kNormalColor << ";\n";
  //  }
  //  else
  //  {
  //    ss << " border: 2px solid " << ::kErrorColor << ";\n";
  //  }
  //  ss << " border-radius: 4px;\n";
  //  ss << "}\n";

  setStyleSheet(styleSheet);
}
