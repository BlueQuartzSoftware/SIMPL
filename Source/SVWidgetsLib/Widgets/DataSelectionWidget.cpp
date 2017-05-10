#include "DataSelectionWidget.h"

#include <QtCore/QSignalMapper>
#include <QtWidgets/QMenu>
#include <QtCore/QMimeData>

#include "SVWidgetsLib/QtSupport/QtSStyles.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "moc_DataSelectionWidget.cpp"


const QColor DataSelectionWidget::DataContainerColor = QColor::fromRgb(100, 255, 0);
const QColor DataSelectionWidget::AttributeMatrixColor = QColor::fromRgb(0, 255, 0);
const QColor DataSelectionWidget::DataArrayColor = QColor::fromRgb(0, 100, 255);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataSelectionWidget::DataSelectionWidget(QWidget* parent) : QWidget(parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataSelectionWidget::~DataSelectionWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setupGui()
{
  selectedDataPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)), this, SLOT(dataArraySelected(QString)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::updateGui()
{
  if(nullptr == getFilter())
  {
    return;
  }

  createSelectionMenu();

  updateRequirementTooltip();
  updateSelectionTooltip();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::updateRequirementTooltip()
{
  m_requirementTooltip = "";

  // Geometry requirements
  if(m_geometryTypes.isEmpty() == false)
  {
    QString geometryTypesStr = "Required Geometry Types: ";

    for(int i = 0; i < m_geometryTypes.size(); i++)
    {
      switch(m_geometryTypes[i])
      {
      case IGeometry::Type::Image:
        geometryTypesStr += "Image ";
        break;
      // RectGrid has the same value as Image
      //case IGeometry::Type::RectGrid:
      //  geometryTypesStr += "RectGrid ";
      //  break;
      case IGeometry::Type::Vertex:
        geometryTypesStr += "Vertex ";
        break;
      case IGeometry::Type::Edge:
        geometryTypesStr += "Edge ";
        break;
      case IGeometry::Type::Triangle:
        geometryTypesStr += "Triangle ";
        break;
      case IGeometry::Type::Quad:
        geometryTypesStr += "Quad ";
        break;
      case IGeometry::Type::Tetrahedral:
        geometryTypesStr += "Tetrahedral ";
        break;
      case IGeometry::Type::Unknown:
        geometryTypesStr += "Unknown ";
        break;
      case IGeometry::Type::Any:
        geometryTypesStr += "Any ";
        break;
      }
    }

    m_requirementTooltip += geometryTypesStr + "\n";
  }
  
  // Attribute Matrix requirements
  if(m_attributeMatrixTypes.isEmpty() == false)
  {
    QString attributeMatrixStr = "Required Attribute Matrix Types: ";

    for(int i = 0; i < m_attributeMatrixTypes.size(); i++)
    {
      switch(m_attributeMatrixTypes[i])
      {
      case AttributeMatrix::Type::Vertex:
        attributeMatrixStr += "Vertex ";
        break;
      case AttributeMatrix::Type::Edge:
        attributeMatrixStr += "Edge ";
        break;
      case AttributeMatrix::Type::Face:
        attributeMatrixStr += "Face ";
        break;
      case AttributeMatrix::Type::Cell:
        attributeMatrixStr += "Cell ";
        break;
      case AttributeMatrix::Type::VertexFeature:
        attributeMatrixStr += "Vertex Feature ";
        break;
      case AttributeMatrix::Type::EdgeFeature:
        attributeMatrixStr += "Edge Feature ";
        break;
      case AttributeMatrix::Type::FaceFeature:
        attributeMatrixStr += "Face Feature ";
        break;
      case AttributeMatrix::Type::CellFeature:
        attributeMatrixStr += "Cell Feature ";
        break;
      case AttributeMatrix::Type::VertexEnsemble:
        attributeMatrixStr += "Vertex Ensemble ";
        break;
      case AttributeMatrix::Type::EdgeEnsemble:
        attributeMatrixStr += "Edge Ensemble ";
        break;
      case AttributeMatrix::Type::FaceEnsemble:
        attributeMatrixStr += "Face Ensemble ";
        break;
      case AttributeMatrix::Type::CellEnsemble:
        attributeMatrixStr += "Cell Ensemble ";
        break;
      case AttributeMatrix::Type::MetaData:
        attributeMatrixStr += "Meta Data ";
        break;
      case AttributeMatrix::Type::Generic:
        attributeMatrixStr += "Generic ";
        break;
      case AttributeMatrix::Type::Unknown:
        attributeMatrixStr += "Uknown ";
        break;
      case AttributeMatrix::Type::Any:
        attributeMatrixStr += "Any ";
        break;
      }
    }

    m_requirementTooltip += attributeMatrixStr + "\n";
  }

  if(m_componentDimensions.isEmpty() == false)
  {
    // TODO: Component Dimension requirements
  }

  if(m_dataArrayTypes.isEmpty() == false)
  {
    QString dataArrayTypesStr = "Required Data Array Types: ";

    // TODO: Data Array Type requirements
    for(int i = 0; i < m_dataArrayTypes.size(); i++)
    {
      dataArrayTypesStr += m_dataArrayTypes[i] + " ";
    }

    m_requirementTooltip += dataArrayTypesStr + "\n";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::updateSelectionTooltip()
{
  m_selectionTooltip = "";

  if(nullptr == getFilter())
  {
    return;
  }

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  
  // Check current selection
  if(m_dataArrayPath.getDataContainerName().isEmpty() == false)
  {
    m_selectionTooltip = dca->getDataContainer(m_dataArrayPath)->getInfoString(SIMPL::HtmlFormat);

    if(m_dataArrayPath.getAttributeMatrixName().isEmpty() == false)
    {
      m_selectionTooltip = dca->getAttributeMatrix(m_dataArrayPath)->getInfoString(SIMPL::HtmlFormat);

      if(m_dataArrayPath.getDataArrayName().isEmpty())
      {
        m_selectionTooltip = dca->getAttributeMatrix(m_dataArrayPath)->getAttributeArray(m_dataArrayPath.getDataArrayName())->getInfoString(SIMPL::HtmlFormat);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DataSelectionWidget::getFilter()
{
  return m_filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setFilter(AbstractFilter::Pointer filter)
{
  m_filter = filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataSelectionWidget::getDataArrayPath()
{
  return m_dataArrayPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setDataArrayPath(DataArrayPath path)
{
  m_dataArrayPath = path;

  selectedDataPath->setText(path.serialize(Detail::Delimiter));

  emit dataArrayPathChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setDataArrayPath(QString pathStr)
{
  DataArrayPath path = DataArrayPath::Deserialize(pathStr, Detail::Delimiter);
  setDataArrayPath(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataSelectionWidget::checkDataArrayPath(DataArrayPath path)
{
  if(nullptr == getFilter())
  {
    return false;
  }

  // Check DataContainer level
  if(path.isEmpty() || path.getDataContainerName().isEmpty())
  {
    return false;
  }

  if(checkGeometryTypes(path) == false)
  {
    return false;
  }

  // Check AttributeMatrix level
  if(m_containerType == ContainerType::AttributeMatrix || m_containerType == ContainerType::DataArray)
  {
    if(path.getAttributeMatrixName().isEmpty())
    {
      return false;
    }

    if(checkAttributeMatrixTypes(path) == false)
    {
      return false;
    }

    // Check DataArray level
    if(m_containerType == ContainerType::DataArray)
    {
      if(path.getDataArrayName().isEmpty())
      {
        return false;
      }

      if(checkDataArrayTypes(path) == false)
      {
        return false;
      }

      if(checkComponentDimensions(path) == false)
      {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataSelectionWidget::checkDataArrayPath(QString pathStr)
{
  if(nullptr == getFilter())
  {
    return false;
  }

  DataArrayPath path = DataArrayPath::Deserialize(pathStr, Detail::Delimiter);

  return checkDataArrayPath(path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataSelectionWidget::checkGeometryTypes(DataArrayPath path)
{
  if(m_geometryTypes.isEmpty())
  {
    return true;
  }

  if(nullptr == getFilter())
  {
    return false;
  }

  DataContainer::Pointer dc = getFilter()->getDataContainerArray()->getDataContainer(path);
  if(nullptr == dc.get())
  {
    return false;
  }

  IGeometry::Pointer geom = dc->getGeometry();
  if(nullptr == geom.get())
  {
    return false;
  }

  return m_geometryTypes.contains(geom->getGeometryType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataSelectionWidget::checkAttributeMatrixTypes(DataArrayPath path)
{
  if(m_containerType == ContainerType::DataContainer)
  {
    return true;
  }

  if(nullptr == getFilter())
  {
    return false;
  }

  AttributeMatrix::Pointer am = getFilter()->getDataContainerArray()->getAttributeMatrix(path);
  if(nullptr == am.get())
  {
    return false;
  }

  return m_attributeMatrixTypes.contains(am->getType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataSelectionWidget::checkDataArrayTypes(DataArrayPath path)
{
  if(m_containerType != ContainerType::DataArray)
  {
    return true;
  }

  if(nullptr == getFilter())
  {
    return false;
  }

  AttributeMatrix::Pointer am = getFilter()->getDataContainerArray()->getAttributeMatrix(path);
  if(nullptr == am.get())
  {
    return false;
  }

  IDataArray::Pointer da = am->getAttributeArray(path.getDataArrayName());
  if(nullptr == da.get())
  {
    return false;
  }

  return m_dataArrayTypes.contains(da->getTypeAsString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataSelectionWidget::checkComponentDimensions(DataArrayPath path)
{
  if(m_containerType != ContainerType::DataArray)
  {
    return true;
  }

  if(nullptr == getFilter())
  {
    return false;
  }

  AttributeMatrix::Pointer am = getFilter()->getDataContainerArray()->getAttributeMatrix(path);
  if(nullptr == am.get())
  {
    return false;
  }

  IDataArray::Pointer da = am->getAttributeArray(path.getDataArrayName());
  if(nullptr == da.get())
  {
    return false;
  }

  return m_componentDimensions.contains(da->getComponentDimensions());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataSelectionWidget::ContainerType DataSelectionWidget::getContainerType()
{
  return m_containerType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setContainerType(DataSelectionWidget::ContainerType type)
{
  m_containerType = type;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setGeometryTypes(IGeometry::Types types)
{
  m_geometryTypes = types;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setAttributeMatrixTypes(QVector<AttributeMatrix::Type> types)
{
  m_attributeMatrixTypes = types;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setDataArrayTypes(QVector<QString> daTypes)
{
  m_dataArrayTypes = daTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::setComponentDimensions(QVector<QVector<size_t>> cDims)
{
  m_componentDimensions = cDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Types DataSelectionWidget::getGeometryTypes()
{
  return m_geometryTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AttributeMatrix::Type> DataSelectionWidget::getAttributeMatrixTypes()
{
  return m_attributeMatrixTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataSelectionWidget::getDataArrayTypes()
{
  return m_dataArrayTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QVector<size_t>> DataSelectionWidget::getComponentDimensions()
{
  return m_componentDimensions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  // Get the menu and clear it out
  QMenu* menu = selectedDataPath->menu();
  if(!menu)
  {
    menu = new QMenu();
    selectedDataPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu)
  {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  QVector<QString> daTypes = getDataArrayTypes();
  QVector<QVector<size_t>> cDims = getComponentDimensions();
  QVector<AttributeMatrix::Type> amTypes = getAttributeMatrixTypes();
  IGeometry::Types geomTypes = getGeometryTypes();

  QListIterator<DataContainer::Pointer> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainer::Pointer dc = containerIter.next();

    IGeometry::Pointer geom = IGeometry::NullPointer();
    IGeometry::Type geomType = IGeometry::Type::Unknown;
    if(nullptr != dc.get())
    {
      geom = dc->getGeometry();
    }
    if(nullptr != geom.get())
    {
      geomType = geom->getGeometryType();
    }

    QMenu* dcMenu = new QMenu(dc->getName());
    dcMenu->setDisabled(false);
    menu->addMenu(dcMenu);
    if(geomTypes.isEmpty() == false && geomTypes.contains(geomType) == false)
    {
      dcMenu->setDisabled(true);
    }
    if(dc->getAttributeMatrixNames().size() == 0)
    {
      dcMenu->setDisabled(true);
    }

    bool validAmFound = false;

    // We found the proper Data Container, now populate the AttributeMatrix List
    DataContainer::AttributeMatrixMap_t attrMats = dc->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> attrMatsIter(attrMats);
    while(attrMatsIter.hasNext())
    {
      attrMatsIter.next();
      QString amName = attrMatsIter.key();
      AttributeMatrix::Pointer am = attrMatsIter.value();

      QMenu* amMenu = new QMenu(amName);
      dcMenu->addMenu(amMenu);

      if(nullptr != am.get() && amTypes.isEmpty() == false && amTypes.contains(am->getType()) == false)
      {
        amMenu->setDisabled(true);
      }

      bool validDaFound = false;

      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the menus
      QList<QString> attrArrayNames = am->getAttributeArrayNames();
      QListIterator<QString> dataArraysIter(attrArrayNames);
      while(dataArraysIter.hasNext())
      {
        QString attrArrayName = dataArraysIter.next();
        IDataArray::Pointer da = am->getAttributeArray(attrArrayName);
        QAction* action = new QAction(attrArrayName, amMenu);
        DataArrayPath daPath(dc->getName(), amName, attrArrayName);
        QString path = daPath.serialize(Detail::Delimiter);
        action->setData(path);

        connect(action, SIGNAL(triggered(bool)), m_MenuMapper, SLOT(map()));
        m_MenuMapper->setMapping(action, path);
        amMenu->addAction(action);

        bool daIsNotNull = (nullptr != da.get()) ? true : false;
        bool daValidType = (daTypes.isEmpty() == false && daTypes.contains(da->getTypeAsString()) == false) ? true : false;
        bool daValidDims = (cDims.isEmpty() == false && cDims.contains(da->getComponentDimensions()) == false) ? true : false;

        if(daIsNotNull && (daValidType || daValidDims))
        {
          action->setDisabled(true);
        }
        else
        {
          validDaFound = true;
        }
      }

      // Disable AttributeMatrix menu if no valid DataArray found
      if(validDaFound)
      {
        validAmFound = true;
      }
      if(!validAmFound)
      {
        amMenu->setDisabled(true);
      }
    }

    // Disable DataContainer menu if no valid AttributeMatrixes found
    if(!validAmFound)
    {
      dcMenu->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::dragEnterEvent(QDragEnterEvent* event)
{
  if(event->mimeData()->hasText())
  {
    event->acceptProposedAction();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataSelectionWidget::dropEvent(QDropEvent* event)
{
  if(event->mimeData()->hasText())
  {
    QString dataPath = event->mimeData()->text();

    if(checkDataArrayPath(dataPath))
    {
      setDataArrayPath(dataPath);
    }
  }
}