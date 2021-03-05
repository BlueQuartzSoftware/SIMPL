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

#include <QtCore/QSignalMapper>
#include <QtWidgets/QMenu>

#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/IGeometry.h"
#include "SIMPLib/Utilities/FilterCompatibility.hpp"
#include "SIMPLib/Utilities/STLUtilities.hpp"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetUtils.hpp"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedDataContainerSelectionWidget::LinkedDataContainerSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, LinkedDataContainerSelectionWidget, LinkedDataContainerSelectionFilterParameter);

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
  connect(getFilter(), &AbstractFilter::preflightAboutToExecute, this, &LinkedDataContainerSelectionWidget::beforePreflight);

  // Catch when the filter is finished running the preflight
  connect(getFilter(), &AbstractFilter::preflightExecuted, this, &LinkedDataContainerSelectionWidget::afterPreflight);

  // Catch when the filter wants its values updated
  connect(getFilter(), &AbstractFilter::updateFilterParameters, this, &LinkedDataContainerSelectionWidget::filterNeedsInputParameters);

  if(getFilterParameter() == nullptr)
  {
    return;
  }
  label->setText(getFilterParameter()->getHumanLabel());

  m_SelectedDataContainerPath->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));

  m_MenuMapper = new QSignalMapper(this);
  connect(m_MenuMapper, SIGNAL(mapped(QString)), this, SLOT(dataContainerSelected(QString)));

  DataArrayPath dcPath = m_FilterParameter->getGetterCallback()();
  m_SelectedDataContainerPath->setText(dcPath.getDataContainerName());

  changeStyleSheet(Style::FS_STANDARD_STYLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::widgetChanged()
{
  int index = -1;
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  IGeometry::Pointer igeom = dca->getPrereqGeometryFromDataContainer<IGeometry>(nullptr, m_SelectedDataContainerPath->text());

  if(igeom)
  {
    index = static_cast<int32_t>(igeom->getGeometryType());
  }

  Q_EMIT conditionalPropertyChanged(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString LinkedDataContainerSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() && !filtDcName.isEmpty())
  {
    return filtDcName;
  }
  if(!curDcName.isEmpty() && filtDcName.isEmpty())
  {
    return curDcName;
  }
  if(!curDcName.isEmpty() && !filtDcName.isEmpty() && m_DidCausePreflight)
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
  if(menu == nullptr)
  {
    menu = new QMenu();
    m_SelectedDataContainerPath->setMenu(menu);
    menu->installEventFilter(this);
  }
  if(menu != nullptr)
  {
    menu->clear();
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  DataContainerArray::Container containers = dca->getDataContainers();
  IGeometry::Types geomTypes = m_FilterParameter->getDefaultGeometryTypes();

  for(DataContainer::Pointer dc : containers)
  {
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

    if(!geomTypes.empty() && !SIMPL::contains(geomTypes, geomType) && !SIMPL::contains(geomTypes, IGeometry::Type::Any))
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
  Q_EMIT parametersChanged();
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
  if(dcPath.isEmpty())
  {
    return;
  }

  m_SelectedDataContainerPath->setText("");
  m_SelectedDataContainerPath->setToolTip("");

  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  if(dca->doesDataContainerExist(dcPath.getDataContainerName()))
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
  if(m_DidCausePreflight)
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
  if(nullptr == dca.get())
  {
    return;
  }

  if(dca->doesDataContainerExist(m_SelectedDataContainerPath->text()))
  {
    DataContainer::Pointer dc = dca->getDataContainer(m_SelectedDataContainerPath->text());
    if(nullptr != dc.get())
    {
      QString html = dc->getInfoString(SIMPL::HtmlFormat);
      m_SelectedDataContainerPath->setToolTip(html);
      m_SelectedDataContainerPath->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(true));
    }
  }
  else
  {
    m_SelectedDataContainerPath->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LinkedDataContainerSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  Q_UNUSED(filter)
  LinkedDataContainerSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(DataArrayPath(m_SelectedDataContainerPath->text()));
  }
  else
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
