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

#include "AttributeMatrixSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::AttributeMatrixSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// Include the MOC generated file for this class
#include "moc_AttributeMatrixSelectionWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::AttributeMatrixSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<AttributeMatrixSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "AttributeMatrixSelectionWidget can ONLY be used with an AttributeMatrixSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrixSelectionWidget::~AttributeMatrixSelectionWidget()
{
  if(m_MenuMapper)
  {
    delete m_MenuMapper;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }
  if(getFilterParameter() == nullptr)
  {
    return;
  }

  label->setText(getFilterParameter()->getHumanLabel());

  m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)), this, SLOT(attributeMatrixSelected(QString)));

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  DataArrayPath defaultPath = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<DataArrayPath>();
  m_SelectedAttributeMatrixPath->setText(defaultPath.serialize(Detail::Delimiter));

  changeStyleSheet(Style::FS_STANDARD_STYLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrixSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {
    return filtDcName;
  }
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {
    return curDcName;
  }
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  {
    return curDcName;
  }

  return filtDcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  // Get the menu and clear it out
  QMenu* menu = m_SelectedAttributeMatrixPath->menu();
  if(!menu)
  {
    menu = new QMenu();
    m_SelectedAttributeMatrixPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu)
  {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  QVector<AttributeMatrix::Type> amTypes = m_FilterParameter->getDefaultAttributeMatrixTypes();
  IGeometry::Types geomTypes = m_FilterParameter->getDefaultGeometryTypes();

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

      QAction* action = new QAction(amName, dcMenu);
      DataArrayPath daPath(dc->getName(), amName, "");
      QString path = daPath.serialize(Detail::Delimiter);
      action->setData(path);

      connect(action, SIGNAL(triggered(bool)), m_MenuMapper, SLOT(map()));
      m_MenuMapper->setMapping(action, path);
      dcMenu->addAction(action);

      bool amIsNotNull = (nullptr != am.get()) ? true : false;
      bool amValidType = (amTypes.isEmpty() == false && amTypes.contains(am->getType()) == false) ? true : false;

      if(amIsNotNull && amValidType)
      {
        action->setDisabled(true);
      }
      else
      {
        validAmFound = true;
      }
    }

    // Disable DataContainer menu if no valid AttributeMatrix was found
    if(!validAmFound)
    {
      dcMenu->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrixSelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show && obj == m_SelectedAttributeMatrixPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedAttributeMatrixPath);
    m_SelectedAttributeMatrixPath->menu()->move(pos);
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::attributeMatrixSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath amPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(amPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::setSelectedPath(DataArrayPath amPath)
{
  if (amPath.isEmpty()) { return; }

  m_SelectedAttributeMatrixPath->setText("");
  m_SelectedAttributeMatrixPath->setToolTip("");

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  if (dca->doesAttributeMatrixExist(amPath))
  {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(amPath);
    QString html = am->getInfoString(SIMPL::HtmlFormat);
    m_SelectedAttributeMatrixPath->setToolTip(html);
    m_SelectedAttributeMatrixPath->setText(amPath.serialize(Detail::Delimiter));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  AttributeMatrixSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  createSelectionMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  if (dca->doesAttributeMatrixExist(DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter)))
  {
    AttributeMatrix::Pointer am = dca->getAttributeMatrix(DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter));
    if (nullptr != am.get()) {
      QString html = am->getInfoString(SIMPL::HtmlFormat);
      m_SelectedAttributeMatrixPath->setToolTip(html);
      m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(true));
    }
  }
  else
  {
    m_SelectedAttributeMatrixPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrixSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Geenerate the path to the AttributeArray
  DataArrayPath selectedPath = DataArrayPath::Deserialize(m_SelectedAttributeMatrixPath->text(), Detail::Delimiter);
  QVariant var;
  var.setValue(selectedPath);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }
}
