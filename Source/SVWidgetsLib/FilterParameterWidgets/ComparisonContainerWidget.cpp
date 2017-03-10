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

#include "moc_ComparisonContainerWidget.cpp"

ComparisonContainerWidget* ComparisonContainerWidget::SelectedItem = nullptr;
QString ComparisonContainerWidget::BorderStyleSheet = "ComparisonContainerWidget { border-style: outset; border-width: 2px; border-radius: 5px; border-color: blue; }";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonContainerWidget::ComparisonContainerWidget(QWidget* parent) : QWidget(parent)
{
  setupUi(this);

  m_comparisonWidget = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonContainerWidget::~ComparisonContainerWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ComparisonContainerWidget::getUnionOperator()
{
  return m_unionOperator;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setUnionOperator(int unionOperator)
{
  m_unionOperator = unionOperator;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::showUnionOperator()
{
  unionComboBox->show();
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
  setUnionOperator(comparison->getUnionOperator());

  // create widget, replace current widget, and save the pointer
  IComparisonWidget* widget = IComparisonWidget::CreateWidget(comparison);

  setComparisonWidget(widget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::setComparisonWidget(IComparisonWidget* widget)
{
  // replace current widget, save pointer
  if (m_comparisonWidget)
  {
    m_comparisonWidget->hide();
    m_comparisonWidget->setParent(nullptr);
    m_comparisonWidget->deleteLater();
  }

  if (widget)
  {
    contentsLayout->addWidget(widget);
  }

  m_comparisonWidget = widget;
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

  AbstractComparison::Pointer comparison = m_comparisonWidget->getCurrentComparison();
  comparison->setUnionOperator(getUnionOperator());

  return comparison;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void ComparisonContainerWidget::deleteItem()
//{
//  hide();
//  setParent(nullptr);
//
//  if (m_comparisonSetWidget)
//  {
//    m_comparisonSetWidget->update();
//    QCoreApplication::processEvents();
//    m_comparisonSetWidget->updateItems();
//
//    delete m_comparisonWidget;
//    m_comparisonWidget = nullptr;
//  }
//
//  close();
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::mousePressEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;

  m_startDragPoint = event->pos();

  select();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::select()
{
  if (nullptr == this)
    return;

  setStyleSheet(ComparisonContainerWidget::BorderStyleSheet);

  if (ComparisonContainerWidget::SelectedItem)
    ComparisonContainerWidget::SelectedItem->deselect();

  ComparisonContainerWidget::SelectedItem = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::deselect()
{
  if (nullptr == this)
    return;

  if (ComparisonContainerWidget::SelectedItem == this)
    ComparisonContainerWidget::SelectedItem = nullptr;

  setStyleSheet("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - m_startDragPoint).manhattanLength()
    < QApplication::startDragDistance())
    return;

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  drag->setMimeData(mimeData);

  Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ComparisonContainerWidget::mouseReleaseEvent(QMouseEvent* event)
{
  deselect();
}
