#include "ComparisonValueWidget.h"

#include "SIMPLib/DataContainers/DataArrayPath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValueWidget::ComparisonValueWidget(QWidget* parent, ComparisonValue::Pointer comparisonValue) : IComparisonWidget(parent)
{
  setupUi(this);

  setComparisonValue(comparisonValue);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValueWidget::~ComparisonValueWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValue::Pointer ComparisonValueWidget::getComparisonValue()
{
  return m_comparisonValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::setComparisonValue(ComparisonValue::Pointer value)
{
  if (nullptr == value)
  {
    m_comparisonValue = ComparisonValue::New();
  }
  else
  {
    m_comparisonValue = value;
  }

  setupDataArrayComboBox();

  // Set Widget contents to ComparisonValue
  arrayNameComboBox->setCurrentText(m_comparisonValue->getAttributeArrayName());
  operatorComboBox->setCurrentIndex(m_comparisonValue->getCompOperator());
  valueSpinBox->setValue(m_comparisonValue->getCompValue());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonValueWidget::getComparison()
{
  return m_comparisonValue;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::setupDataArrayComboBox()
{
  arrayNameComboBox->clear();

  AttributeMatrix::Pointer am = getAttributeMatrix();
  if (nullptr != am)
  {
    arrayNameComboBox->addItems(am->getAttributeArrayNames());
  }
  else
  {
    arrayNameComboBox->addItem(m_comparisonValue->getAttributeArrayName());
  }

  arrayNameComboBox->setCurrentText(m_comparisonValue->getAttributeArrayName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::setAttributeMatrix(AttributeMatrix::Pointer am)
{
  IComparisonWidget::setAttributeMatrix(am);

  setupDataArrayComboBox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::dataArrayChanged(int index)
{
  m_comparisonValue->setAttributeArrayName(arrayNameComboBox->currentText());

  emit comparisonChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::comparisonOperatorChanged(int index)
{
  m_comparisonValue->setCompOperator(index);

  emit comparisonChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::comparisonValueChanged(double value)
{
  m_comparisonValue->setCompValue(value);

  emit comparisonChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::setupGui()
{
  connect(arrayNameComboBox, SIGNAL(currentIndexChanged(int)),
    this, SLOT(dataArrayChanged(int)));
  connect(operatorComboBox, SIGNAL(currentIndexChanged(int)),
    this, SLOT(comparisonOperatorChanged(int)));
  connect(valueSpinBox, SIGNAL(valueChanged(double)),
    this, SLOT(comparisonValueChanged(double)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::setArrayNames(QStringList names)
{
  QString currentName = arrayNameComboBox->currentText();

  arrayNameComboBox->clear();
  arrayNameComboBox->addItems(names);

  if(names.contains(currentName))
  {
    arrayNameComboBox->setCurrentText(currentName);
  }
  else
  {
    arrayNameComboBox->setCurrentIndex(-1);
  }
}
