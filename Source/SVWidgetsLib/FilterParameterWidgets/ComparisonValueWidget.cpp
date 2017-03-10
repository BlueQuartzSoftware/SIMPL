#include "ComparisonValueWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValueWidget::ComparisonValueWidget(QWidget* parent, ComparisonValue::Pointer comparisonValue) : IComparisonWidget(parent)
{
  setupUi(this);

  if (nullptr == comparisonValue)
  {
    m_comparisonValue = ComparisonValue::New();
  }
  else
  {
    m_comparisonValue = comparisonValue;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonValueWidget::~ComparisonValueWidget()
{

}

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
  m_comparisonValue = value;

  // Set Widget contents to ComparisonValue
  arrayNameComboBox->setCurrentText(m_comparisonValue->getAttributeArrayName());
  operatorComboBox->setCurrentIndex(m_comparisonValue->getCompOperator());
  valueSpinBox->setValue(m_comparisonValue->getCompValue());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonValueWidget::getCurrentComparison()
{
  ComparisonValue::Pointer value = ComparisonValue::New();

  // Set Widget Contents to Value
  value->setAttributeArrayName(arrayNameComboBox->currentText());
  value->setCompOperator(operatorComboBox->currentIndex());
  value->setCompValue(valueSpinBox->value());

  return value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::apply()
{
  // Set Array Name Combo Box
  m_comparisonValue->setAttributeArrayName(arrayNameComboBox->currentText());
  m_comparisonValue->setCompOperator(operatorComboBox->currentIndex());
  m_comparisonValue->setCompValue(valueSpinBox->value());
}