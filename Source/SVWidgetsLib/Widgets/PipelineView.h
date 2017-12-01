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


#ifndef _PipelineView_h_
#define _PipelineView_h_

#include <vector>

#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtCore/QUuid>

#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStatusBar>

#include <QtGui/QPainter>

#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/H5FilterParametersWriter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterWidget.h"
#include "SVWidgetsLib/Widgets/SVPipelineFilterOutlineWidget.h"
#include "SVWidgetsLib/Widgets/util/AddFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/MoveFilterCommand.h"
#include "SVWidgetsLib/Widgets/util/RemoveFilterCommand.h"
#include "SVWidgetsLib/QtSupport/QtSFileDragMessageBox.h"

class QScrollArea;
class QContextMenuEvent;
class QLabel;
class QEvent;
class QUndoStack;
class QUndoCommand;
class DataStructureWidget;

/*
 *
 */
class SVWidgetsLib_EXPORT PipelineView
{
  public:

    typedef std::pair<int, PipelineFilterObject*> IndexedFilterObject;
    SIMPL_INSTANCE_PROPERTY(QList<IndexedFilterObject>, DraggedFilterObjects)

    PipelineView(QWidget* parent = 0);
    virtual ~PipelineView();

    // -----------------------------------------------------------------------------
    // We normally don't use the friend keyword, but we have to here so that the
    // undo command classes are able to add/remove filter objects with the private
    // addFilterObject(...) and removeFilterObject(...) PipelineView functions.
    // This allows us to keep these functions private from everything else except
    // this class and the undo commands.  The undo commands have to call these
    // functions in their undo and redo functions.
    // -----------------------------------------------------------------------------
    friend void AddFilterCommand::undo();
    friend void AddFilterCommand::redo();

    friend void MoveFilterCommand::undo();
    friend void MoveFilterCommand::redo();

    friend void RemoveFilterCommand::undo();
    friend void RemoveFilterCommand::redo();
    // -----------------------------------------------------------------------------
    // *****************************************************************************
    // -----------------------------------------------------------------------------

    /**
     * @brief filterCount
     * @return
     */
    virtual int filterCount(QUuid startId = QUuid());

    /**
     * @brief filterObjectAt
     * @param index
     * @return
     */
    virtual PipelineFilterObject* filterObjectAt(QVariant value);

    /**
    * @brief getAllFilterObjects
    * @return
    */
    virtual QList<PipelineFilterObject*> getAllFilterObjects();

    /**
    * @brief containsFilterWidget
    * @param filterWidget
    * @return
    */
    virtual bool containsFilterWidget(PipelineFilterObject* filterWidget);

    /**
    * @brief valueOfFilterWidget
    * @param filterWidget
    * @return
    */
    virtual QVariant valueOfFilterWidget(PipelineFilterObject* filterWidget);

    /**
     * @brief getFilterPipeline
     * @return
     */
    virtual FilterPipeline::Pointer getFilterPipeline(QUuid startId = QUuid());

    /**
     * @brief Returns a FilterPipeline Object with a new filter instance that has the input parameters copied
     * from the filter instance that is embedded in the SVPipelineFilterWidget instance. This function does NOT perform
     * a DEEP copy of the filter.
     * @return
     */
    virtual FilterPipeline::Pointer getCopyOfFilterPipeline();

    /**
    * @brief clearSelectedFilterObjects
    */
    virtual void clearSelectedFilterObjects();

    /**
    * @brief getSelectedFilterWidgets
    * @return
    */
    virtual QList<PipelineFilterObject*> getSelectedFilterObjects();

    /**
    * @brief getSelectedIndexedFilterObject
    * @return
    */
    virtual QList<IndexedFilterObject> getSelectedIndexedFilterObjects();

    /**
     * @brief populatePipelineView
     * @param jsonString
     */
    virtual void populatePipelineView(QString jsonString, QVariant value);

    /**
     * @brief moveFilterWidget
     * @param fw
     * @param origin
     * @param destination
     */
    virtual void moveFilterWidget(PipelineFilterObject* fw, QVariant origin, QVariant destination);

    /**
     * @brief setSelectedFilterObject
     * @param w
     * @param modifiers
     */
    virtual void setSelectedFilterObject(PipelineFilterObject* w, Qt::KeyboardModifiers modifiers);

    /**
     * @brief setDataStructureWidget
     * @param w
     */
    virtual void setDataStructureWidget(DataStructureWidget* w);

    /**
     * @brief recheckWindowTitleAndModification
     */
    virtual void recheckWindowTitleAndModification();

    /**
     * @brief preflightPipeline
     */
    virtual void preflightPipeline(QUuid id = QUuid());

    /**
    * @brief Open pipeline to a file
    */
    virtual int openPipeline(const QString& filePath, QVariant value, const bool& setOpenedFilePath, const bool& changeTitle);

    /**
     * @brief addSIMPLViewReaderFilter
     * @param filePath
     */
    virtual void addSIMPLViewReaderFilter(const QString& filePath, QVariant value);

    /**
    * @brief clearWidgets
    */
    virtual void clearFilterWidgets();

    /**
     * @brief reindexWidgetTitles
     */
    virtual void reindexWidgetTitles();

    /**
    * @brief addUndoCommand
    */
    virtual void addUndoCommand(QUndoCommand* cmd);

    /**
     * @brief showFilterHelp
     * @param className
     */
    void showFilterHelp(const QString& className);

    /**
     * @brief createFilterObjectFromFilter
     * @param filter
     * @return
     */
    virtual PipelineFilterObject* createFilterObjectFromFilter(AbstractFilter::Pointer filter);

  protected:
    /**
     * @brief addFilterObject
     * @param filterObject
     * @param value
     */
    virtual void addFilterObject(PipelineFilterObject* filterObject, QVariant value);

    /**
     * @brief removeFilterObject
     * @param filterWidget
     * @param deleteWidget
     */
    virtual void removeFilterObject(PipelineFilterObject* filterWidget, bool deleteWidget = true);

  private:
    PipelineView(const PipelineView&) = delete;   // Copy Constructor Not Implemented
    void operator=(const PipelineView&) = delete; // Operator '=' Not Implemented
};

#endif /* _PipelineView_H */

