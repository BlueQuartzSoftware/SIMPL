#include "ComparisonSetWidget.h"

#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>

#include "SVWidgetsLib/FilterParameterWidgets/ComparisonContainerWidget.h"

#include "SIMPLib/Common/ComparisonValue.h"
#include "SIMPLib/Common/Constants.h"

#include "moc_ComparisonSetWidget.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSetWidget::ComparisonSetWidget(QWidget* parent, ComparisonSet::Pointer comparisonSet) : IComparisonWidget(parent)
{
  setupUi(this);

  setComparisonSet(comparisonSet);

  setupGui();
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
void ComparisonSetWidget::setupGui()
{
  conditionalLayout->setAlignment(Qt::AlignTop);
  conditionalLayout->setDirection(QBoxLayout::Direction::TopToBottom);

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
    this, SLOT(showContextMenu(QPoint)));
  connect(addItemBtn, SIGNAL(clicked()),
    this, SLOT(showAddItemMenu()));
  connect(invertConditionalCheckBox, SIGNAL(stateChanged(int)),
    this, SLOT(setInverse(int)));
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

  updateItems();
  return m_comparisonSet->getComparisons();
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
    ComparisonContainerWidget* container = new ComparisonContainerWidget(nullptr, comparisons[i]);

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
  
  ComparisonContainerWidget* container = new ComparisonContainerWidget(nullptr, comparison);

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
  QObjectList childWidgets = conditionalWidgetContents->children();
  for (int i = 0; i < childWidgets.size(); i++)
  {
    ComparisonContainerWidget* widget = dynamic_cast<ComparisonContainerWidget*>(childWidgets.at(i));

    if (widget)
    {
      widget->hide();
      widget->setParent(nullptr);
      delete widget;
    }
  }

  if (nullptr != comparisonSet)
  {
    // set contents of widget to comparisonSet
    QVector<AbstractComparison::Pointer> comparisons = comparisonSet->getComparisons();
    for (int i = 0; i < comparisons.size(); i++)
    {
      ComparisonContainerWidget* container = new ComparisonContainerWidget(nullptr, comparisons[i]);

      conditionalLayout->addWidget(container);
      container->show();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonSetWidget::getComparison()
{
  return m_comparisonSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::setInverse(int newInverse)
{
  bool invert = newInverse == Qt::Checked;
  m_comparisonSet->setInvertComparison(invert);

  emit comparisonChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::showContextMenu(QPoint pos)
{
  QMenu menu(tr("New Comparison Menu"), this);

  QAction comparisonValueAction(tr("Add Comparison Value"), this);
  connect(&comparisonValueAction, SIGNAL(triggered()),
    this, SLOT(createComparisonValue()));
  menu.addAction(&comparisonValueAction);

  QAction comparisonSetAction(tr("Add Comparison Set"), this);
  connect(&comparisonSetAction, SIGNAL(triggered()),
    this, SLOT(createComparisonSet()));
  menu.addAction(&comparisonSetAction);

  menu.exec(mapToGlobal(pos));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::showAddItemMenu()
{
  QPoint point = mapFromGlobal(QCursor::pos());

  showContextMenu(point);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::createComparisonSet()
{
  ComparisonSet::Pointer comparisonSet = ComparisonSet::New();
  addComparisonWidget(comparisonSet);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::createComparisonValue()
{
  ComparisonValue::Pointer comparisonValue = ComparisonValue::New();
  addComparisonWidget(comparisonValue);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::addComparisonWidget(AbstractComparison::Pointer comparison, int index)
{
  if (index < 0)
  {
    index = insertIndex();
  }

  m_comparisonSet->addComparison(comparison);

  ComparisonContainerWidget* comparisonWidget = new ComparisonContainerWidget(this, comparison);
  conditionalLayout->insertWidget(index, comparisonWidget);

  comparisonWidget->getComparisonWidget()->setAttributeMatrix(getAttributeMatrix());
  comparisonWidget->setComparisonSetWidget(this);

  updateItems();
  update();

  emit comparisonChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonSetWidget::insertIndex()
{
  return conditionalWidgetContents->children().size() - 2;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::updateItems()
{
  bool firstItemFound = false;

  QVector<AbstractComparison::Pointer> updatedChildComparisons;

  for (int i = 0; i < conditionalLayout->count(); i++)
  {
    ComparisonContainerWidget* itemWidget = dynamic_cast<ComparisonContainerWidget*>(conditionalLayout->itemAt(i)->widget());

    if (itemWidget)
    {
      itemWidget->showUnionOperator(firstItemFound);
      firstItemFound = true;

      updatedChildComparisons.push_back(itemWidget->getComparisonWidget()->getComparison());
    }
  }

  m_comparisonSet->setComparisons(updatedChildComparisons);

  //emit comparisonChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonSetWidget::insertIndexByPoint(QPoint pos, ComparisonContainerWidget* ignore)
{
  int index = 0;

  pos = conditionalWidgetContents->mapFrom(this, pos);

  for (int i = 0; i < conditionalWidgetContents->children().size(); i++)
  {
    ComparisonContainerWidget* itemWidget = dynamic_cast<ComparisonContainerWidget*>(conditionalWidgetContents->children().at(i));

    if (itemWidget && itemWidget != ignore)
    {
      QPoint filterPoint = itemWidget->pos();
      if (pos.y() > filterPoint.y() + itemWidget->height() / 2)
      {
        index++;
      }
    }
  }

  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::dragEnterEvent(QDragEnterEvent* event)
{
  event->acceptProposedAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::dragMoveEvent(QDragMoveEvent* event)
{
  event->acceptProposedAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::dropEvent(QDropEvent* event)
{
  ComparisonContainerWidget* comparisonItemWidget = dynamic_cast<ComparisonContainerWidget*>(event->source());
  if (nullptr == comparisonItemWidget)
  {
    return;
  }

  comparisonItemWidget->deselect();

  int index = insertIndexByPoint(event->pos(), comparisonItemWidget);
  ComparisonSetWidget* ownerComparisonSet = comparisonItemWidget->getComparisonSetWidget();

  comparisonItemWidget->setParent(nullptr);
  comparisonItemWidget->setComparisonSetWidget(this);
  conditionalLayout->insertWidget(index, comparisonItemWidget);

  QApplication::processEvents();

  updateItems();

  if (ownerComparisonSet)
  {
    ownerComparisonSet->updateItems();
  }

  emit comparisonChanged();

  event->acceptProposedAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonSetWidget::setAttributeMatrix(AttributeMatrix::Pointer am)
{
  IComparisonWidget::setAttributeMatrix(am);

  QVector<IComparisonWidget*> comparisonWidgets = getComparisonWidgets();
  for (int i = 0; i < comparisonWidgets.size(); i++)
  {
    comparisonWidgets[i]->setAttributeMatrix(am);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<IComparisonWidget*> ComparisonSetWidget::getComparisonWidgets()
{
  QVector<IComparisonWidget*> comparisonWidgets;

  for (int i = 0; i < conditionalWidgetContents->children().size(); i++)
  {
    ComparisonContainerWidget* itemWidget = dynamic_cast<ComparisonContainerWidget*>(conditionalWidgetContents->children().at(i));

    if (itemWidget)
    {
      comparisonWidgets.push_back(itemWidget->getComparisonWidget());
    }
  }

  return comparisonWidgets;
}
