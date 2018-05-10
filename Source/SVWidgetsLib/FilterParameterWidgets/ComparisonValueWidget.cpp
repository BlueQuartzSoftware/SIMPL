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
void ComparisonValueWidget::setComparisonValue(const ComparisonValue::Pointer &value)
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
  double compValue = m_comparisonValue->getCompValue();
  valueSpinBox->setText(QString::number(compValue));
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
void ComparisonValueWidget::comparisonValueChanged()
{
  
  bool ok = false;
  double d = valueSpinBox->text().toDouble(&ok);
  m_comparisonValue->setCompValue(d);

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
  connect(valueSpinBox, SIGNAL(editingFinished()),
    this, SLOT(comparisonValueChanged()));
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonValueWidget::renameDataArrayPath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  if(oldPath.getDataArrayName().isEmpty())
  {
    return;
  }

  int index = arrayNameComboBox->findText(oldPath.getDataArrayName());
  if(index >= 0)
  {
    arrayNameComboBox->setItemText(index, newPath.getDataArrayName());
  }

  arrayNameComboBox->update();
  arrayNameComboBox->setCurrentText(getComparisonValue()->getAttributeArrayName());
}
