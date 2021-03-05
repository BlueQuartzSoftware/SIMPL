/* ============================================================================
 * Copyright (c) 2019-2019 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MontageStructureSelectionWidget.h"

#include <QtCore/QSignalBlocker>

#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Utilities/FilterCompatibility.hpp"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "FilterParameterWidgetUtils.hpp"
#include "FilterParameterWidgetsDialogs.h"

#include "SVWidgetsLib/ui_MontageStructureSelectionWidget.h"

namespace
{
const QString SelectItemText = "-- Select Montage --";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageStructureSelectionWidget::MontageStructureSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_Ui(new Ui::MontageStructureSelectionWidget)
, m_DidCausePreflight(false)
{
  m_FilterParameter = SIMPL_FILTER_PARAMETER_COMPATIBILITY_CHECK(filter, parameter, MontageStructureSelectionWidget, MontageStructureSelectionFilterParameter);

  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageStructureSelectionWidget::MontageStructureSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_Ui(new Ui::MontageStructureSelectionWidget)
, m_DidCausePreflight(false)
{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageStructureSelectionWidget::~MontageStructureSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageStructureSelectionWidget::initializeWidget(FilterParameter* parameter, AbstractFilter* filter)
{
  setFilter(filter);
  setFilterParameter(parameter);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageStructureSelectionWidget::setupGui()
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

  connect(m_Ui->montageSelectionComboBox, &QComboBox::currentTextChanged, this, &MontageStructureSelectionWidget::setMontageName);

  if(getFilterParameter() == nullptr)
  {
    return;
  }
  m_Ui->label->setText(getFilterParameter()->getHumanLabel());

  MontageStructureSelectionFilterParameter::GetterCallbackType getter = m_FilterParameter->getGetterCallback();
  if(getter)
  {
    QString montageName = getter();
    QSignalBlocker selectionBlocker(m_Ui->montageSelectionComboBox);
    if(m_Ui->montageSelectionComboBox->findText(montageName) == -1)
    {
      m_Ui->montageSelectionComboBox->addItem(montageName);
    }
    m_Ui->montageSelectionComboBox->setCurrentText(montageName);
  }

  changeStyleSheet(Style::FS_STANDARD_STYLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageStructureSelectionWidget::checkStringValues(const QString& curDcName, const QString& filtDcName)
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
void MontageStructureSelectionWidget::setMontageName(const QString& montage)
{
  if(m_Ui->montageSelectionComboBox->findText(montage) == -1)
  {
    m_Ui->montageSelectionComboBox->addItem(montage);
  }
  m_Ui->montageSelectionComboBox->setCurrentText(montage);

  m_DidCausePreflight = true;
  Q_EMIT parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageStructureSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight)
  {
    // std::cout << "***  MontageStructureSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageStructureSelectionWidget::afterPreflight()
{
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(nullptr == dca.get())
  {
    return;
  }

  QString currentName = m_FilterParameter->getGetterCallback()();
  QStringList montageNames = dca->getMontageCollection().getNameList();

  QSignalBlocker blocker(m_Ui->montageSelectionComboBox);
  m_Ui->montageSelectionComboBox->clear();
  m_Ui->montageSelectionComboBox->addItem(::SelectItemText);
  m_Ui->montageSelectionComboBox->addItems(montageNames);
  m_Ui->montageSelectionComboBox->setCurrentText(currentName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageStructureSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  QString name = m_Ui->montageSelectionComboBox->currentText();
  // The selection string is not a valid montage name
  if(m_Ui->montageSelectionComboBox->currentIndex() == 0 && ::SelectItemText == name)
  {
    name = QString();
  }

  MontageStructureSelectionFilterParameter::SetterCallbackType setter = m_FilterParameter->getSetterCallback();
  if(setter)
  {
    setter(name);
  }
}
