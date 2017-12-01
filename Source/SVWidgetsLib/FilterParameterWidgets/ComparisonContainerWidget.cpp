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

#include "ComparisonContainerWidget.h"

#include <QtGui/QDrag>
#include <QtCore/QMimeData>

#include "SVWidgetsLib/FilterParameterWidgets/ComparisonSetWidget.h"
#include "SVWidgetsLib/FilterParameterWidgets/ComparisonValueWidget.h"

// Border stylesheet requires QFrame
ComparisonContainerWidget* ComparisonContainerWidget::SelectedItem = nullptr;
QString ComparisonContainerWidget::BorderStyleSheet = "ComparisonContainerWidget { border-style: outset; border-width: 2px; border-radius: 5px; border-color: blue; }";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonContainerWidget::ComparisonContainerWidget(QWidget* parent, AbstractComparison::Pointer comparison) : QFrame(parent)
{
  setupUi(this);

  m_comparisonWidget = nullptr;
  m_comparisonSetWidget = nullptr;
  setComparison(comparison);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonContainerWidget::~ComparisonContainerWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setupGui()
{
  contentsLayout->setAlignment(Qt::AlignTop);
  contentsLayout->setDirection(QBoxLayout::Direction::TopToBottom);

  connect(removeBtn, SIGNAL(clicked()),
    this, SLOT(deleteItem()));
  connect(unionComboBox, SIGNAL(currentIndexChanged(int)),
    this, SLOT(unionOperatorChanged(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonContainerWidget::getUnionOperator()
{
  return unionComboBox->currentIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setUnionOperator(int unionOperator)
{
  if (m_comparisonWidget)
  {
    m_comparisonWidget->getComparison()->setUnionOperator(unionOperator);
  }
  unionComboBox->setCurrentIndex(unionOperator);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::unionOperatorChanged(int unionOp)
{
  if (m_comparisonWidget)
  {
    m_comparisonWidget->getComparison()->setUnionOperator(unionOp);
    emit comparisonChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::showUnionOperator(bool enabled)
{
  if (enabled)
  {
    unionComboBox->show();
  }
  else
  {
    unionComboBox->hide();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::hideUnionOperator()
{
  unionComboBox->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setComparison(AbstractComparison::Pointer comparison)
{
  // create widget, replace current widget, and save the pointer
  IComparisonWidget* widget = IComparisonWidget::CreateWidget(comparison);
  setComparisonWidget(widget);
  setUnionOperator(comparison->getUnionOperator());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setComparisonWidget(IComparisonWidget* widget)
{
  // replace current widget, save pointer
  if (m_comparisonWidget)
  {
    disconnect(m_comparisonWidget, SIGNAL(comparisonChanged()),
      this, SIGNAL(comparisonChanged()));

    m_comparisonWidget->hide();
    m_comparisonWidget->setParent(nullptr);
    m_comparisonWidget->deleteLater();
  }

  if (widget)
  {
    contentsLayout->addWidget(widget);
  }

  m_comparisonWidget = widget;

  connect(widget, SIGNAL(comparisonChanged()),
    this, SIGNAL(comparisonChanged()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractComparison::Pointer ComparisonContainerWidget::getCurrentComparison()
{
  if (nullptr == m_comparisonWidget)
  {
    return nullptr;
  }

  AbstractComparison::Pointer comparison = m_comparisonWidget->getComparison();
  comparison->setUnionOperator(getUnionOperator());

  return comparison;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IComparisonWidget* ComparisonContainerWidget::getComparisonWidget()
{
  return m_comparisonWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonSetWidget* ComparisonContainerWidget::getComparisonSetWidget()
{
  return m_comparisonSetWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setComparisonSetWidget(ComparisonSetWidget* comparisonSetWidget)
{
  if (m_comparisonSetWidget)
  {
    disconnect(this, SIGNAL(comparisonChanged()),
      m_comparisonSetWidget, SIGNAL(comparisonChanged()));
  }

  m_comparisonSetWidget = comparisonSetWidget;
  getComparisonWidget()->setArrayNames(comparisonSetWidget->getArrayNames());

  if (comparisonSetWidget)
  {
    connect(this, SIGNAL(comparisonChanged()),
      comparisonSetWidget, SIGNAL(comparisonChanged()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::deleteItem()
{
  hide();
  setParent(nullptr);

  if (m_comparisonSetWidget)
  {
    m_comparisonSetWidget->updateItems();
    emit m_comparisonSetWidget->comparisonChanged();

    m_comparisonSetWidget->update();
    QCoreApplication::processEvents();
    m_comparisonSetWidget->updateItems();

    delete m_comparisonWidget;
    m_comparisonWidget = nullptr;
  }

  close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::mousePressEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton))
  {
    return;
  }

  m_startDragPoint = event->pos();

  select();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::select()
{
  setStyleSheet(ComparisonContainerWidget::BorderStyleSheet);
  update();

  if (nullptr != ComparisonContainerWidget::SelectedItem && this != ComparisonContainerWidget::SelectedItem)
  {
    ComparisonContainerWidget::SelectedItem->deselect();
  }

  ComparisonContainerWidget::SelectedItem = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::deselect()
{
  if (ComparisonContainerWidget::SelectedItem == this)
  {
    ComparisonContainerWidget::SelectedItem = nullptr;
  }

  setStyleSheet("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton))
  {
    return;
  }

  if ((event->pos() - m_startDragPoint).manhattanLength()
    < QApplication::startDragDistance())
  {
    return;
  }

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  drag->setMimeData(mimeData);

  drag->exec(Qt::MoveAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::mouseReleaseEvent(QMouseEvent* event)
{
  Q_UNUSED(event)
  deselect();
}
