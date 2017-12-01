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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMimeData>
#include <QtCore/QTemporaryFile>
#include <QtCore/QUrl>

#include <QtGui/QClipboard>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QUndoCommand>
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QVBoxLayout>

#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/Breakpoint.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/SIMPLib.h"

#include "SVWidgetsLib/Core/FilterWidgetManager.h"
#include "SVWidgetsLib/Core/PipelineViewPtrMimeData.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidgetsDialogs.h"
#include "SVWidgetsLib/QtSupport/QtSDroppableScrollArea.h"
#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/util/AddFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/MoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/RemoveFilterCommand.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineView::PipelineView(QWidget* parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineView::~PipelineView() = default;

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
QList<PipelineFilterObject*> PipelineView::getAllFilterObjects()
{
  QList<PipelineFilterObject*> filterObjects;

  for(int i = 0; i < filterCount(); i++)
  {
    PipelineFilterObject* filterObject = filterObjectAt(i);
    if(filterObject)
    {
      filterObjects.push_back(filterObject);
    }
  }

  return filterObjects;
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
void PipelineView::addFilterObject(PipelineFilterObject* filterObject, QVariant value)
{
  Q_UNUSED(filterObject)
  Q_UNUSED(value)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::removeFilterObject(PipelineFilterObject* filterWidget, bool deleteWidget)
{
  Q_UNUSED(filterWidget)
  Q_UNUSED(deleteWidget)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::moveFilterWidget(PipelineFilterObject* fw, QVariant origin, QVariant destination)
{
  Q_UNUSED(fw)
  Q_UNUSED(origin)
  Q_UNUSED(destination)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineView::openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle)
{
  Q_UNUSED(filePath)
  Q_UNUSED(value)
  Q_UNUSED(setOpenedFilePath)
  Q_UNUSED(changeTitle)

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
void PipelineView::clearFilterWidgets()
{
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
PipelineFilterObject* PipelineView::createFilterObjectFromFilter(AbstractFilter::Pointer filter)
{
  Q_UNUSED(filter)

  // The subclass should reimplement this function
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers)
{
  Q_UNUSED(w)
  Q_UNUSED(modifiers)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::setDataStructureWidget(DataStructureWidget* w)
{
  Q_UNUSED(w)

  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::recheckWindowTitleAndModification()
{
  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::reindexWidgetTitles()
{
  // The subclass should reimplement this function
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineView::populatePipelineView(QString jsonString, QVariant value)
{
  Q_UNUSED(jsonString)
  Q_UNUSED(value)

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
  Q_UNUSED(cmd)

  // The subclass should reimplement this function
  return;
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
QList<PipelineView::IndexedFilterObject> PipelineView::getSelectedIndexedFilterObjects()
{
  // The subclass should reimplement this function
  return QList<IndexedFilterObject>();
}
