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
#include <QtGui/QDrag>
#include <QtWidgets/QApplication>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

namespace {
  const QString kNormalColor("#8f8f91");
  const QString kErrorColor("#BC0000");
  const QString kFilterColor("#0072ff");
  const QString kAcceptColor("#009104");
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
  setCheckable(true);
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

  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";
  ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "<tr><td colspan=2><b><i>Drag and Drop from the DataStructure</i></b></td></tr>";
  ss << createGeomReqString(dcReqs.dcGeometryTypes);
  ss << "</tbody></table><br/>";
  ss << "</body></html>";

  setToolTip(html);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setAttrMatrixRequirements(AttributeMatrixSelectionFilterParameter::RequirementType amReqs)
{
  m_DataType = DataType::AttributeMatrix;
  m_AttrMatrixReqs = amReqs;

  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";
  ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "<tr><td colspan=2><b><i>Drag and Drop from the DataStructure</i></b></td></tr>";
  ss << createGeomReqString(amReqs.dcGeometryTypes);
  ss << createAttrMatrixReqString(amReqs.amTypes);
  ss << "</tbody></table><br/>";
  ss << "</body></html>";

  setToolTip(html);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::setDataArrayRequirements(DataArraySelectionFilterParameter::RequirementType daReqs)
{
  m_DataType = DataType::DataArray;
  m_DataArrayReqs = daReqs;

  QString html;
  QTextStream ss(&html);
  ss << "<html><head></head>\n";
  ss << "<body>\n";
  ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
  ss << "<tbody>\n";
  ss << "<tr><td colspan=2><b><i>Drag and Drop from the DataStructure</i></b></td></tr>";
  ss << createGeomReqString(daReqs.dcGeometryTypes);
  ss << createAttrMatrixReqString(daReqs.amTypes);
  ss << createDataArrayTypeString(daReqs.daTypes);
  ss << createComponentReqString(daReqs.componentDimensions);
  ss << "</tbody></table><br/>";
  ss << "</body></html>";

  setToolTip(html);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathSelectionWidget::createGeomReqString(QVector<IGeometry::Type> geomTypes)
{
  QString reqStr = "<tr><td><i>Required Geometries:</i></td>";
  if(geomTypes.size() == 0)
  {
    reqStr += "<td>None</td>";
  }
  else
  {
    for(IGeometry::Type type : geomTypes)
    {
      switch(type)
      {
      case IGeometry::Type::Any:
        reqStr += "<td>Any</td>";
        break;
      case IGeometry::Type::Edge:
        reqStr += "<td>Edge</td>";
        break;
      case IGeometry::Type::Image:
        reqStr += "<td>Image</td>";
        break;
      case IGeometry::Type::Quad:
        reqStr += "<td>Quad</td>";
        break;
      case IGeometry::Type::RectGrid:
        reqStr += "<td>RectGrid</td>";
        break;
      case IGeometry::Type::Tetrahedral:
        reqStr += "<td>Tetrahedral</td>";
        break;
      case IGeometry::Type::Triangle:
        reqStr += "<td>Triangle</td>";
        break;
      case IGeometry::Type::Unknown:
        reqStr += "<td>Unknown</td>";
        break;
      case IGeometry::Type::Vertex:
        reqStr += "<td>Vertex</td>";
        break;
      }
    }
  }

  reqStr += "</tr>";
  return reqStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathSelectionWidget::createAttrMatrixReqString(QVector<AttributeMatrix::Type> amTypes)
{
  QString reqStr = "<tr><td><i>Required Matrix Type:</i></td>";
  if(amTypes.size() == 0)
  {
    reqStr += "<td>None</td>";
  }
  else
  {
    for(AttributeMatrix::Type type : amTypes)
    {
      switch(type)
      {
      case AttributeMatrix::Type::Any:
        reqStr += "<td>Any</td>";
        break;
      case AttributeMatrix::Type::Cell:
        reqStr += "<td>Cell</td>";
        break;
      case AttributeMatrix::Type::CellEnsemble:
        reqStr += "<td>Cell Ensemble</td>";
        break;
      case AttributeMatrix::Type::CellFeature:
        reqStr += "<td>Cell Feature</td>";
        break;
      case AttributeMatrix::Type::Edge:
        reqStr += "<td>Edge</td>";
        break;
      case AttributeMatrix::Type::EdgeEnsemble:
        reqStr += "<td>Edge Ensemble</td>";
        break;
      case AttributeMatrix::Type::EdgeFeature:
        reqStr += "<td>Edge Feature</td>";
        break;
      case AttributeMatrix::Type::Face:
        reqStr += "<td>Face</td>";
        break;
      case AttributeMatrix::Type::FaceEnsemble:
        reqStr += "<td>Face Ensemble</td>";
        break;
      case AttributeMatrix::Type::FaceFeature:
        reqStr += "<td>Face Feature</td>";
        break;
      case AttributeMatrix::Type::Generic:
        reqStr += "<td>Generic</td>";
        break;
      case AttributeMatrix::Type::MetaData:
        reqStr += "<td>MetaData</td>";
        break;
      case AttributeMatrix::Type::Unknown:
        reqStr += "<td>Unknown</td>";
        break;
      case AttributeMatrix::Type::Vertex:
        reqStr += "<td>Vertex</td>";
        break;
      case AttributeMatrix::Type::VertexEnsemble:
        reqStr += "<td>Vertex Ensemble</td>";
        break;
      case AttributeMatrix::Type::VertexFeature:
        reqStr += "<td>Vertex Feature</td>";
        break;
      }
    }
  }

  reqStr += "</tr>";
  return reqStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathSelectionWidget::createDataArrayTypeString(QVector<QString> daTypes)
{
  QString reqStr = "<tr><td><i>Required Array Type:</i></td>";
  if(daTypes.size() == 0)
  {
    reqStr += "<td>None</td>";
  }
  else
  {
    for(QString type : daTypes)
    {
      reqStr += "<td>" + type + "</td>";
    }
  }

  reqStr += "</tr>";
  return reqStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathSelectionWidget::createComponentReqString(QVector<QVector<size_t>> comps)
{
  QString reqStr = "<tr><td><i>Required Component Size:</i></td>";
  if(comps.size() == 0)
  {
    reqStr += "<td>None</td>";
  }
  else
  {
    for(QVector<size_t> comp : comps)
    {
      reqStr += "<td>[";

      int compSize = comp.size();
      for(int i = 0; i < compSize; i++)
      {
        if(i != 0)
        {
          reqStr += ",";
        }

        reqStr += QString::number(comp[i]);
      }

      reqStr += "]</tr></td>";
    }
  }

  reqStr += "</tr>";
  return reqStr;
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
  resetStyle();
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

  if(m_AttrMatrixReqs.amTypes.size() > 0 && false == m_AttrMatrixReqs.amTypes.contains(am->getType()))
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

  if(m_DataArrayReqs.amTypes.size() > 0 && false == m_DataArrayReqs.amTypes.contains(am->getType()))
  {
    return false;
  }

  // Check DataArray
  IDataArray::Pointer da = am->getAttributeArray(path.getDataArrayName());
  if(nullptr == da)
  {
    return false;
  }

  if(m_DataArrayReqs.daTypes.size() > 0 && false == m_DataArrayReqs.daTypes.contains(da->getTypeAsString()))
  {
    return false;
  }

  if(m_DataArrayReqs.componentDimensions.size() > 0 && false == m_DataArrayReqs.componentDimensions.contains(da->getComponentDimensions()))
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
  // Do not end filtering if the button state is checked
  if(isChecked())
  {
    return;
  }

  emit endViewPaths();
  if(checkCurrentPath())
  {
    changeStyleSheet(Style::Normal);
  }
  else
  {
    changeStyleSheet(Style::NotFound);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::dragEnterEvent(QDragEnterEvent* event)
{
  if(false == event->mimeData()->hasFormat(SIMPLView::DragAndDrop::DataArrayPath))
  {
    return;
  }

  const QMimeData* mime = event->mimeData();
  QByteArray data = mime->data(SIMPLView::DragAndDrop::DataArrayPath);
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
  QToolButton::dragLeaveEvent(event);

  if(isChecked())
  {
    return;
  }

  if(checkCurrentPath())
  {
    changeStyleSheet(Style::Normal);
  }
  else
  {
    changeStyleSheet(Style::NotFound);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::dropEvent(QDropEvent* event)
{
  const QMimeData* mime = event->mimeData();
  QByteArray data = mime->data(SIMPLView::DragAndDrop::DataArrayPath);
  QString dataStr = QString::fromUtf8(data);

  setDataArrayPath(DataArrayPath::Deserialize(dataStr, "|"));
  
  setChecked(false);

  event->accept();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    m_StartPos = event->pos();
  }

  QToolButton::mousePressEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::mouseMoveEvent(QMouseEvent* event)
{
  if(event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - m_StartPos).manhattanLength();
    if(distance >= QApplication::startDragDistance())
    {
      performDrag();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::performDrag()
{
  setChecked(true);
  DataArrayPath path = getDataArrayPath();

  // MimeData stores the current path AND marks itself as a SelectionWidget drag 
  // for connecting to the DataStructureWidget.
  QMimeData* mimeData = new QMimeData;
  mimeData->setData(SIMPLView::DragAndDrop::DataArrayPath, path.serialize().toUtf8());
  mimeData->setData(SIMPLView::DragAndDrop::SelectionWidget, path.serialize().toUtf8());

  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
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
void DataArrayPathSelectionWidget::setPathFiltering(bool active)
{
  setChecked(active);
  
  if(false == active && false == underMouse())
  {
    if(checkCurrentPath())
    {
      changeStyleSheet(Style::Normal);
    }
    else
    {
      changeStyleSheet(Style::NotFound);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::resetStyle()
{
  setPathFiltering(false);
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
    ss << " border: 1px solid " << ::kNormalColor << ";\n";
    break;
  case Style::Hover:
    ss << " border: 1px solid " << ::kFilterColor << ";\n";
    break;
  case Style::NotFound:
    ss << " border: 1px solid " << ::kErrorColor << ";\n";
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
