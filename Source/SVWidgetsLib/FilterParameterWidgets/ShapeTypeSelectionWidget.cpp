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

#include "ShapeTypeSelectionWidget.h"

#include <QtCore/QList>
#include <QtCore/QMetaProperty>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/FilterParameters/ShapeTypeSelectionFilterParameter.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypeSelectionWidget::ShapeTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent)
: FilterParameterWidget(parameter, filter, parent)
, m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<ShapeTypeSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != nullptr, "NULL Pointer", "ShapeTypeSelectionWidget can ONLY be used with a ShapeTypeSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypeSelectionWidget::ShapeTypeSelectionWidget(QWidget* parent)
: FilterParameterWidget(nullptr, nullptr, parent)
, m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypeSelectionWidget::~ShapeTypeSelectionWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::setupGui()
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

  updateComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::updateComboBoxes()
{
  StringDataArray::Pointer names = StringDataArray::NullPointer();

  // If there is a PhaseName array in the Ensemble AttributeMatrix then use that to
  // label the QComboBoxes.
  AbstractFilter* filter = getFilter();
  if(filter)
  {
    DataArrayPath phasePath = getFilter()->property("InputPhaseTypesArrayPath").value<DataArrayPath>();
    phasePath.setDataArrayName("PhaseName");
    DataContainer::Pointer dc = filter->getDataContainerArray()->getDataContainer(phasePath);
    if(dc)
    {
      AttributeMatrix::Pointer am = dc->getAttributeMatrix(phasePath);
      if(am)
      {
        names = am->getAttributeArrayAs<StringDataArray>("PhaseName");
      }
    }
  }


  bool ok = false;
  // setup the list of choices for the widget
  ShapeTypeSelectionFilterParameter* shapeType = dynamic_cast<ShapeTypeSelectionFilterParameter*>(getFilterParameter());
  QString countProp = shapeType->getPhaseTypeCountProperty();
  int numPhases = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);
  if(!ok)
  {
    numPhases = 0;
  }

  ShapeType::Types shapeTypesFromFilter = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<ShapeType::Types>();
  if (shapeTypesFromFilter.size() > 1 && numPhases <= 0)
  {
    numPhases = shapeTypesFromFilter.size();
  }

  // Get our list of predefined Shape Type Strings
  QVector<QString> shapeTypeStrings;
  ShapeType::getShapeTypeStrings(shapeTypeStrings);
  // Pop the back value off because it will invariably allow us to walk off the end of an array
  shapeTypeStrings.pop_back();
  shapeTypeStrings.pop_back();
  // Get our list of predefined enumeration values
  ShapeType::Types shapeTypeEnums;
  ShapeType::getShapeTypeEnums(shapeTypeEnums);
  shapeTypeEnums.pop_back();
  shapeTypeEnums.pop_back();

  // We skip the first Ensemble as it is always a dummy
  QLabel* shapeTypeLabel = nullptr;
  QComboBox* shapeTypeComboBox = nullptr;

  for(int i = 1; i < numPhases; i++)
  {
    shapeTypeLabel = nullptr;
    shapeTypeComboBox = nullptr;
    if(i > m_ShapeTypeLabels.size() )
    {
      shapeTypeLabel = new QLabel(m_ShapeTypeScrollContents);
      QString str;
      if(names.get() && names->getNumberOfTuples() > 0)
      {
        str = names->getValue(i);
      }
      else
      {
        str = QString("Phase ");
        str.append(QString::number(i, 10));
      }

      str.append(":");
      shapeTypeLabel->setText(str);
      shapeTypeLabel->setObjectName(str);
      m_ShapeTypeLabels.push_back(shapeTypeLabel);
      m_ShapeTypeScrollContentsLayout->setWidget(i-1, QFormLayout::LabelRole, shapeTypeLabel);
      shapeTypeComboBox = new QComboBox(m_ShapeTypeScrollContents);
      str.append(" ComboBox");
      shapeTypeComboBox->setObjectName(str);
      for(int32_t s = 0; s < static_cast<int>(ShapeType::Type::ShapeTypeEnd); ++s)
      {
        shapeTypeComboBox->addItem((shapeTypeStrings[s]), static_cast<ShapeType::EnumType>(shapeTypeEnums[s]));
        shapeTypeComboBox->setItemData(static_cast<int>(s), static_cast<ShapeType::EnumType>(shapeTypeEnums[s]), Qt::UserRole);
      }
      m_ShapeTypeCombos.push_back(shapeTypeComboBox);
      m_ShapeTypeScrollContentsLayout->setWidget(i-1, QFormLayout::FieldRole, shapeTypeComboBox);
      connect(shapeTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboboxChanged(int)));

    }
    else {
      shapeTypeLabel = m_ShapeTypeLabels.at(i-1);
      shapeTypeComboBox = m_ShapeTypeCombos.at(i-1);

      QString str;
      if(names.get() && names->getNumberOfTuples() > 0)
      {
        str = names->getValue(i);
      }
      else
      {
        str = QString("Phase ");
        str.append(QString::number(i, 10));
      }
      str.append(":");
      shapeTypeLabel->setText(str);
    }

    if(i<shapeTypesFromFilter.size()) {
      shapeTypeComboBox->setCurrentIndex( static_cast<int>(shapeTypesFromFilter[i]));
    }
  }

  // Check to see if we lost a phase during the preflight
  int diff = m_ShapeTypeCombos.size() - numPhases;

  for(int i = 0; i < diff + 1; i++)
  {
    if(!m_ShapeTypeLabels.isEmpty() && !m_ShapeTypeCombos.isEmpty())
    {
      shapeTypeLabel = m_ShapeTypeLabels.takeLast();
      shapeTypeComboBox = m_ShapeTypeCombos.takeLast();
      shapeTypeLabel->setParent(nullptr);
      shapeTypeComboBox->setParent(nullptr);
      shapeTypeLabel->setVisible(false);
      shapeTypeComboBox->setVisible(false);
      shapeTypeLabel->deleteLater();
      shapeTypeComboBox->deleteLater();
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::comboboxChanged(int index)
{
  Q_UNUSED(index)
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::beforePreflight()
{
  if(nullptr == getFilter())
  {
    return;
  }
  if(m_DidCausePreflight == true)
  {
    //  std::cout << "***  ShapeTypeSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::afterPreflight()
{
  updateComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = m_ShapeTypeCombos.count();
  ShapeType::Types shapeTypes(count + 1, static_cast<ShapeType::Type>(ShapeType::Type::Unknown));
  bool ok = false;
  for(int i = 0; i < count; ++i)
  {
    QComboBox* cb = m_ShapeTypeCombos.at(i);
    ShapeType::Type sType = static_cast<ShapeType::Type>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    shapeTypes[i + 1] = sType;
  }

  QVariant var;
  var.setValue(shapeTypes);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    getFilter()->notifyMissingProperty(getFilterParameter());
  }
}
