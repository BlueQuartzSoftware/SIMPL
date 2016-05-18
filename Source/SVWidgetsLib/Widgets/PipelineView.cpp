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

#include "PipelineView.h"

#include <iostream>


#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QMimeData>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDrag>
#include <QtGui/QClipboard>
#include <QtWidgets/QLabel>
#include <QtGui/QPixmap>
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QUndoCommand>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/QFilterParametersReader.h"
#include "SIMPLib/FilterParameters/QFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"

#include "SVWidgetsLib/QtSupport/QtSDroppableScrollArea.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/Core/PipelineViewPtrMimeData.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/util/AddFiltersCommand.h"
#include "SVWidgetsLib/Widgets/util/MoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/RemoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/ClearFiltersCommand.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineView::PipelineView(QWidget* parent) :
  m_StatusBar(NULL),
  m_UndoStack(new QUndoStack(NULL))
{
  m_ActionUndo = m_UndoStack->createUndoAction(NULL);
  m_ActionRedo = m_UndoStack->createRedoAction(NULL);
  m_ActionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  m_ActionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineView::~PipelineView()
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineView::filterCount(QUuid startId)
{
  Q_UNUSED(startId)

  // The subclass should reimplement this function
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* PipelineView::filterObjectAt(QVariant value)
{
  // The subclass should reimplement this function
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool PipelineView::containsFilterWidget(PipelineFilterObject* filterWidget)
{
  Q_UNUSED(filterWidget)

  // The subclass should reimplement this function
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant PipelineView::valueOfFilterWidget(PipelineFilterObject* filterWidget)
{
  Q_UNUSED(filterWidget)

  // The subclass should reimplement this function
  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* PipelineView::addFilter(const QString& filterClassName, QVariant value, bool allowUndo, bool connectToStart)
{
  Q_UNUSED(filterClassName)
  Q_UNUSED(value)
  Q_UNUSED(allowUndo)
  Q_UNUSED(connectToStart)

  // The subclass should reimplement this function
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PipelineFilterObject*> PipelineView::addFilters(QList<AbstractFilter::Pointer> filters, QVariant value, bool allowUndo, bool connectToStart)
{
  Q_UNUSED(filters)
  Q_UNUSED(value)
  Q_UNUSED(allowUndo)
  Q_UNUSED(connectToStart)

  // The subclass should reimplement this function
  return QList<PipelineFilterObject*>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineFilterObject* PipelineView::addFilter(AbstractFilter::Pointer filter, QVariant value, bool allowUndo, bool connectToStart)
{
  Q_UNUSED(filter)
  Q_UNUSED(value)
  Q_UNUSED(allowUndo)
  Q_UNUSED(connectToStart)

  // The subclass should reimplement this function
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::addFilterObjects(QList<PipelineFilterObject*> filterObjects, QVariant value, bool allowUndo)
{
  Q_UNUSED(filterObjects)
  Q_UNUSED(value)
  Q_UNUSED(allowUndo)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::addFilterObject(PipelineFilterObject* filterObject, QVariant value, bool allowUndo)
{
  Q_UNUSED(filterObject)
  Q_UNUSED(value)
  Q_UNUSED(allowUndo)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::moveFilterWidget(PipelineFilterObject* fw, QVariant origin, QVariant destination, bool allowUndo)
{
  Q_UNUSED(fw)
  Q_UNUSED(origin)
  Q_UNUSED(destination)
  Q_UNUSED(allowUndo)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::cutFilterWidgets(QList<PipelineFilterObject*> filterWidgets, bool allowUndo)
{
  if (filterWidgets.isEmpty())
  {
    return;
  }

  if (allowUndo == true)
  {
    RemoveFilterCommand* cmd = new RemoveFilterCommand(filterWidgets, this, "Cut");
    addUndoCommand(cmd);
  }
  else
  {
    removeFilterWidgets(filterWidgets, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::pasteFilters(QList<AbstractFilter::Pointer> filters, bool allowUndo)
{
  Q_UNUSED(filters)
  Q_UNUSED(allowUndo)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::pasteFilterWidgets(const QString &jsonString, QVariant value, bool allowUndo)
{
  Q_UNUSED(jsonString)
  Q_UNUSED(value)
  Q_UNUSED(allowUndo)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::removeFilterObject(PipelineFilterObject* filterWidget, bool allowUndo, bool deleteWidget)
{
  Q_UNUSED(filterWidget)
  Q_UNUSED(allowUndo)
  Q_UNUSED(deleteWidget)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::removeFilterWidgets(QList<PipelineFilterObject*> filterWidgets, bool allowUndo)
{
  if (allowUndo == true)
  {
    RemoveFilterCommand* cmd = new RemoveFilterCommand(filterWidgets, this, "Remove");
    addUndoCommand(cmd);
  }
  else
  {
    for (int i=0; i<filterWidgets.size(); i++)
    {
      removeFilterObject(filterWidgets[i], false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineView::openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle, bool connectToStart)
{
  Q_UNUSED(filePath)
  Q_UNUSED(value)
  Q_UNUSED(setOpenedFilePath)
  Q_UNUSED(changeTitle)
  Q_UNUSED(connectToStart)

  // The subclass should reimplement this function
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::addSIMPLViewReaderFilter(const QString& filePath, QVariant value)
{
  Q_UNUSED(filePath)
  Q_UNUSED(value)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::clearWidgets(bool allowUndo)
{
  Q_UNUSED(allowUndo)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::showFilterHelp(const QString& className)
{
  Q_UNUSED(className)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::setSelectedFilterObject(PipelineFilterObject *w, Qt::KeyboardModifiers modifiers)
{
  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::populatePipelineView(FilterPipeline::Pointer pipeline, QVariant value, bool connectToStart)
{
  Q_UNUSED(pipeline)
  Q_UNUSED(value)
  Q_UNUSED(connectToStart)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::preflightPipeline(QUuid id)
{
  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineView::getFilterPipeline(QUuid startId)
{
  Q_UNUSED(startId)

  // The subclass should reimplement this function
  return FilterPipeline::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineView::getCopyOfFilterPipeline()
{
  // The subclass should reimplement this function
  return FilterPipeline::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::clearSelectedFilterObjects()
{
  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::addUndoCommand(QUndoCommand* cmd)
{
  m_UndoStack->push(cmd);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<PipelineFilterObject*> PipelineView::getSelectedFilterObjects()
{
  // The subclass should reimplement this function
  return QList<PipelineFilterObject*>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineView::getActionRedo()
{
  return m_ActionRedo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* PipelineView::getActionUndo()
{
  return m_ActionUndo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStatusBar* PipelineView::getStatusBar()
{
  return m_StatusBar;
}
