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

#include "LinkedDataContainerSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>

#include <QtGui/QStandardItemModel>

#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QMenu>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSStyles.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetUtils.hpp"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedDataContainerSelectionWidget::LinkedDataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<LinkedDataContainerSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "LinkedDataContainerSelectionWidget can ONLY be used with a LinkedDataContainerSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedDataContainerSelectionWidget::LinkedDataContainerSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedDataContainerSelectionWidget::~LinkedDataContainerSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::setupGui()
{
  if(getFilter() == nullptr)
  {
    return;
  }
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()), this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()), this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)), this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if(getFilterParameter() == nullptr)
  {
    return;
  }
  label->setText(getFilterParameter()->getHumanLabel());

  m_SelectedDataContainerPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)),
            this, SLOT(dataContainerSelected(QString)));

  QString dcName = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<QString>();
  m_SelectedDataContainerPath->setText(dcName);

  changeStyleSheet(Style::FS_STANDARD_STYLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::widgetChanged()
{
  int index = -1;
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  IGeometry::Pointer igeom = dca->getPrereqGeometryFromDataContainer<IGeometry, AbstractFilter>(nullptr, m_SelectedDataContainerPath->text());
  
  if(igeom)
  {
    index = static_cast<int32_t>(igeom->getGeometryType());
  }

  emit conditionalPropertyChanged(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LinkedDataContainerSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
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
void LinkedDataContainerSelectionWidget::createSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  // Get the menu and clear it out
  QMenu* menu = m_SelectedDataContainerPath->menu();
  if(!menu)
  {
    menu = new QMenu();
    m_SelectedDataContainerPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu)
  {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  QList<DataContainer::Pointer> containers = dca->getDataContainers();
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

    QString dcName = dc->getName();
    QAction* action = new QAction(dcName, menu);
    DataArrayPath dcPath(dcName, "", "");
    QString path = dcPath.serialize(Detail::Delimiter);
    action->setData(path);

    connect(action, SIGNAL(triggered(bool)), m_MenuMapper, SLOT(map()));
    m_MenuMapper->setMapping(action, path);
    menu->addAction(action);

    if(!geomTypes.isEmpty() && !geomTypes.contains(geomType) && !geomTypes.contains(IGeometry::Type::Any))
    {
      action->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool LinkedDataContainerSelectionWidget::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show && obj == m_SelectedDataContainerPath->menu())
  {
    QPoint pos = adjustedMenuPosition(m_SelectedDataContainerPath);
    m_SelectedDataContainerPath->menu()->move(pos);
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::dataContainerSelected(QString path)
{
  setSelectedPath(path);

  m_DidCausePreflight = true;
  widgetChanged();
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::setSelectedPath(QString path)
{
  DataArrayPath dcPath = DataArrayPath::Deserialize(path, Detail::Delimiter);
  setSelectedPath(dcPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::setSelectedPath(DataArrayPath dcPath)
{
  if (dcPath.isEmpty()) { return; }

  m_SelectedDataContainerPath->setText("");
  m_SelectedDataContainerPath->setToolTip("");

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  if (dca->doesDataContainerExist(dcPath.getDataContainerName()))
  {
    DataContainer::Pointer dc = dca->getDataContainer(dcPath.getDataContainerName());
    QString html = dc->getInfoString(SIMPL::HtmlFormat);
    m_SelectedDataContainerPath->setToolTip(html);
    m_SelectedDataContainerPath->setText(dcPath.getDataContainerName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight == true)
  {
    // std::cout << "***  DataContainerSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  createSelectionMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if (NULL == dca.get()) { return; }

  if (dca->doesDataContainerExist(m_SelectedDataContainerPath->text()))
  {
    DataContainer::Pointer dc = dca->getDataContainer(m_SelectedDataContainerPath->text());
    if (nullptr != dc.get()) {
      QString html = dc->getInfoString(SIMPL::HtmlFormat);
      m_SelectedDataContainerPath->setToolTip(html);
      m_SelectedDataContainerPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(true));
    }
  }
  else
  {
    m_SelectedDataContainerPath->setStyleSheet(QtSStyles::QToolSelectionButtonStyle(false));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  // Generate the path to the AttributeArray
  // DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), attributeArrayList->currentText());
  QVariant var(m_SelectedDataContainerPath->text());
  // var.setValue(path);
  bool ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
