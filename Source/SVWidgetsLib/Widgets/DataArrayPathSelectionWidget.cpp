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
#include <QtGui/QPainter>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

namespace DataArrayPathColors
{
  namespace DataContainer
  {
    const QString NormalColor("#8f8f91");
    const QString ErrorColor("#BC0000");
    const QString FilterColor("#08a500");
    const QString AcceptColor("#009104");
    const QString RejectColor("#BC0000");
  }

  namespace AttributeMatrix
  {
    const QString NormalColor("#8f8f91");
    const QString ErrorColor("#BC0000");
    const QString FilterColor("#8c00ff");
    const QString AcceptColor("#009104");
    const QString RejectColor("#BC0000");
  }

  namespace DataArray
  {
    const QString NormalColor("#8f8f91");
    const QString ErrorColor("#BC0000");
    const QString FilterColor("#0072ff");
    const QString AcceptColor("#009104");
    const QString RejectColor("#BC0000");
  }
}

QPixmap* DataArrayPathSelectionWidget::s_BaseIcon = nullptr;
QPixmap* DataArrayPathSelectionWidget::s_DataContainerIcon = nullptr;
QPixmap* DataArrayPathSelectionWidget::s_AttributeMatrixIcon = nullptr;
QPixmap* DataArrayPathSelectionWidget::s_DataArrayIcon = nullptr;

const int ICON_SIZE = 35;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::SetupDragIcons()
{
  if(s_BaseIcon)
  {
    return;
  }

  s_BaseIcon = new QPixmap(":bullet_ball_blue.png");
  *s_BaseIcon = s_BaseIcon->scaled(ICON_SIZE, ICON_SIZE);
  QImage baseImage = s_BaseIcon->toImage();
  baseImage.convertToFormat(QImage::Format_Indexed8);
  QImage dcImage = baseImage.copy();
  QImage amImage = baseImage.copy();
  QImage daImage = baseImage.copy();

  int height = baseImage.height();
  int width = baseImage.width();
  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      QColor baseColor = baseImage.pixelColor(x, y);
      QColor dcColor = QColor(GetActiveColor(DataType::DataContainer));
      QColor amColor = QColor(GetActiveColor(DataType::AttributeMatrix));
      QColor daColor = QColor(GetActiveColor(DataType::DataArray));

      int hue;
      int saturation;
      int lightness;
      int alpha;
      baseColor.getHsl(&hue, &saturation, &lightness, &alpha);

      dcColor = QColor::fromHsl(dcColor.hue(), 255, lightness, alpha);
      amColor = QColor::fromHsl(amColor.hue(), 255, lightness, alpha);
      daColor = QColor::fromHsl(daColor.hue(), 255, lightness, alpha);

      //dcColor.setAlpha(alpha);
      //amColor.setAlpha(alpha);
      //daColor.setAlpha(alpha);

      dcImage.setPixelColor(x, y, dcColor);
      amImage.setPixelColor(x, y, amColor);
      daImage.setPixelColor(x, y, daColor);
    }
  }
  
  s_DataContainerIcon = new QPixmap(QPixmap::fromImage(dcImage));
  s_AttributeMatrixIcon = new QPixmap(QPixmap::fromImage(amImage));
  s_DataArrayIcon = new QPixmap(QPixmap::fromImage(daImage));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QPixmap DataArrayPathSelectionWidget::GetDragIcon(DataType type)
{
  if(nullptr == s_BaseIcon)
  {
    SetupDragIcons();
  }

  switch(type)
  {
  case DataType::DataContainer:
    return *s_DataContainerIcon;
  case DataType::AttributeMatrix:
    return *s_AttributeMatrixIcon;
  case DataType::DataArray:
    return *s_DataArrayIcon;
  }

  return *s_BaseIcon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DataArrayPathSelectionWidget::GetActiveColor(DataType type)
{
  switch(type)
  {
  case DataType::DataContainer:
    return DataArrayPathColors::DataContainer::FilterColor;
    break;
  case DataType::AttributeMatrix:
    return DataArrayPathColors::AttributeMatrix::FilterColor;
    break;
  case DataType::DataArray:
    return DataArrayPathColors::DataArray::FilterColor;
    break;
  }

  return "#000000";
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

  // Create drag icons if they do not already exist
  if(nullptr == s_BaseIcon)
  {
    SetupDragIcons();
  }
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
  if(false == path.getAttributeMatrixName().isEmpty() || false == path.getDataArrayName().isEmpty())
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

  changeStyleSheet(Style::Active);
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

  resetStyle();
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
  drag->setPixmap(GetDragIcon(m_DataType));
  //drag->setDragCursor(GetDragIcon(m_DataType), Qt::DropAction::CopyAction);
  drag->exec(Qt::CopyAction);

  // drag->exec is a blocking method
  resetStyle();
  emit endViewPaths();
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
const QString DataArrayPathSelectionWidget::getColor(Style style)
{
  switch(getDataType())
  {
  case DataType::DataContainer:
    switch(style)
    {
    case Style::Normal:
      return DataArrayPathColors::DataContainer::NormalColor;
      break;
    case Style::Active:
      return DataArrayPathColors::DataContainer::FilterColor;
      break;
    case Style::NotFound:
      return DataArrayPathColors::DataContainer::ErrorColor;
      break;
    case Style::DragEnabled:
      return DataArrayPathColors::DataContainer::AcceptColor;
      break;
    case Style::DragDisabled:
      return DataArrayPathColors::DataContainer::RejectColor;
      break;
    }
    break;
  case DataType::AttributeMatrix:
    switch(style)
    {
    case Style::Normal:
      return DataArrayPathColors::AttributeMatrix::NormalColor;
      break;
    case Style::Active:
      return DataArrayPathColors::AttributeMatrix::FilterColor;
      break;
    case Style::NotFound:
      return DataArrayPathColors::AttributeMatrix::ErrorColor;
      break;
    case Style::DragEnabled:
      return DataArrayPathColors::AttributeMatrix::AcceptColor;
      break;
    case Style::DragDisabled:
      return DataArrayPathColors::AttributeMatrix::RejectColor;
      break;
    }
    break;
  case DataType::DataArray:
    switch(style)
    {
    case Style::Normal:
      return DataArrayPathColors::DataArray::NormalColor;
      break;
    case Style::Active:
      return DataArrayPathColors::DataArray::FilterColor;
      break;
    case Style::NotFound:
      return DataArrayPathColors::DataArray::ErrorColor;
      break;
    case Style::DragEnabled:
      return DataArrayPathColors::DataArray::AcceptColor;
      break;
    case Style::DragDisabled:
      return DataArrayPathColors::DataArray::RejectColor;
      break;
    }
    break;
  }

  return "#000000";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::changeStyleSheet(Style styleType)
{
  m_Style = styleType;

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
  ss << " border: 1px solid " << getColor(styleType) << ";\n";
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

  ss << "QToolButton:checked {\n";
  ss << " background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\nstop: 0 " << getColor(styleType) << ", stop: 1 #FFFFFF);\n";
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathSelectionWidget::paintEvent(QPaintEvent* event)
{
  QToolButton::paintEvent(event);

  int rectWidth = height();
  int penWidth = 2;
  int radius = 6;
  QRect rect(width() - rectWidth - penWidth / 2, penWidth / 2, rectWidth, height() - penWidth);
  QColor penColor(getColor(Style::Active));
  QColor fillColor(getColor(Style::Active));
  if(m_Style == Style::NotFound)
  {
    penColor = getColor(Style::NotFound);
  }
  
  QPen pen;
  pen.setColor(penColor);
  pen.setWidth(penWidth);

  QPainter painter{ this };
  painter.setPen(pen);
  painter.fillRect(rect, fillColor);
  painter.drawRoundRect(rect, radius, radius);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSize DataArrayPathSelectionWidget::minimumSizeHint() const
{
  QSize minHint = QToolButton::minimumSizeHint();
  minHint.setWidth(minHint.width() + 2 * minHint.height());
  return minHint;
}
