#include "ComparisonSetWidget.h"

#include "SVWidgetsLib/FilterParameterWidgets/ComparisonContainerWidget.h"

#include "moc_ComparisonSetWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSetWidget::ComparisonSetWidget(QWidget* parent, ComparisonSet::Pointer comparisonSet) : IComparisonWidget(parent)
{
  setupUi(this);

  setComparisonSet(comparisonSet);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSetWidget::~ComparisonSetWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AbstractComparison::Pointer> ComparisonSetWidget::getComparisons()
{
  if (nullptr == m_comparisonSet)
  {
    return QVector<AbstractComparison::Pointer>();
  }

  return m_comparisonSet->getComparisons();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<AbstractComparison::Pointer> ComparisonSetWidget::getCurrentComparisons()
{
  QVector<AbstractComparison::Pointer> comparisons;

  // set contents of widget to comparisonSet
  for (int i = 0; i < conditionalWidgetContents->children().size(); i++)
  {
    ComparisonContainerWidget* container = dynamic_cast<ComparisonContainerWidget*>(conditionalWidgetContents->children().at(i));

    if (container)
    {
      comparisons.push_back(container->getCurrentComparison());
    }
  }

  return comparisons;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::setComparisons(QVector<AbstractComparison::Pointer> comparisons)
{
  m_comparisonSet->setComparisons(comparisons);
  
  // create widgets
  for (int i = 0; i < comparisons.size(); i++)
  {
    ComparisonContainerWidget* container = new ComparisonContainerWidget(nullptr);
    container->setComparison(comparisons.at(i));

    conditionalLayout->addWidget(container);
    container->show();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::addComparison(AbstractComparison::Pointer comparison)
{
  m_comparisonSet->addComparison(comparison);
  
  ComparisonContainerWidget* container = new ComparisonContainerWidget(nullptr);
  container->setComparison(comparison);

  conditionalLayout->addWidget(container);
  container->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonSetWidget::getComparison(int index)
{
  QVector<AbstractComparison::Pointer> comparisons = getComparisons();
  if (index < 0 || index >= comparisons.size())
  {
    return nullptr;
  }

  return comparisons.at(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSet::Pointer ComparisonSetWidget::getComparisonSet()
{
  return m_comparisonSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::setComparisonSet(ComparisonSet::Pointer comparisonSet)
{
  m_comparisonSet = comparisonSet;
  
  // clear contents
  for (int i = 0; i < conditionalWidgetContents->children().size(); i++)
  {
    QWidget* widget = dynamic_cast<QWidget*>(conditionalWidgetContents->children().at(i));

    if (widget)
    {
      widget->hide();
      widget->setParent(nullptr);
      widget->deleteLater();
    }
  }

  // set contents of widget to comparisonSet
  QVector<AbstractComparison::Pointer> comparisons = comparisonSet->getComparisons();
  for (int i = 0; i < comparisons.size(); i++)
  {
    ComparisonContainerWidget* container = new ComparisonContainerWidget(nullptr);
    container->setComparison(comparisons.at(i));
    
    conditionalLayout->addWidget(container);
    container->show();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::apply()
{
  QVector<AbstractComparison::Pointer> comparisons = getCurrentComparisons();

  m_comparisonSet->setComparisons(comparisons);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonSetWidget::getCurrentComparison()
{
  ComparisonSet::Pointer comparisonSet = ComparisonSet::New();

  // Set comparisonSet using current comparisons
  comparisonSet->setComparisons(getCurrentComparisons());

  return comparisonSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::addComparisonWidget(AbstractComparison::Pointer comparison)
{
  ComparisonContainerWidget* widget = new ComparisonContainerWidget(nullptr);
  widget->setComparison(comparison);

  conditionalLayout->addWidget(widget);
  widget->show();
}
