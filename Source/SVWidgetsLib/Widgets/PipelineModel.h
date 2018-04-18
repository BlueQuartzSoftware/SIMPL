/* ============================================================================
* Copyright (c) 2017 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software nor the names of its
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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _pipelinetreemodel_h_
#define _pipelinetreemodel_h_

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

#include <QtWidgets/QAction>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SVWidgetsLib/Widgets/PipelineItem.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class QtSSettings;

class SVWidgetsLib_EXPORT PipelineModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(PipelineModel)

    PipelineModel(QObject* parent = 0);

    ~PipelineModel();

    SIMPL_INSTANCE_PROPERTY(int, MaxNumberOfPipelines)
    SIMPL_INSTANCE_PROPERTY(QAction*, ActionUndo)
    SIMPL_INSTANCE_PROPERTY(QAction*, ActionRedo)

    /**
     * @brief addPipeline
     * @param pipelineName
     * @param pipeline
     * @param setAsActive
     */
    void addPipeline(const QString &pipelineName, FilterPipeline::Pointer pipeline, bool setAsActive = false, QModelIndex parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief addPipelineFromFile
     * @param filePath
     * @param parentIndex
     * @param insertionIndex
     * @return
     */
    int addPipelineFromFile(const QString& filePath, const QModelIndex &parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief addFilter
     * @param filter
     * @param parentIndex
     */
    void addFilter(AbstractFilter::Pointer filter, const QModelIndex &parentIndex = QModelIndex(), int insertionIndex = -1);

    /**
     * @brief removePipelineFromModel
     * @param pipelineIndex
     * @param parentIndex
     */
    void removePipeline(QModelIndex pipelineIndex);

    /**
     * @brief removeFilter
     * @param filterIndex
     * @param pipelineIndex
     */
    void removeFilter(int filterIndex, const QModelIndex &pipelineIndex);

    /**
     * @brief updateActivePipeline
     * @param pipelineIdx
     */
    void updateActivePipeline(const QModelIndex &pipelineIdx);

    /**
     * @brief getFilterPipeline
     * @param pipelineIndex
     * @return
     */
    FilterPipeline::Pointer getFilterPipeline(const QModelIndex &pipelineIndex);

    /**
     * @brief writePipeline
     * @param pipelineIndex
     * @param outputPath
     * @return
     */
    int writePipeline(const QModelIndex &pipelineIndex, const QString &outputPath);

    /**
     * @brief addPipelineMessageObserver
     * @param pipelineMessageObserver
     */
    void addPipelineMessageObserver(QObject* pipelineMessageObserver);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    AbstractFilter::Pointer filter(const QModelIndex &index);
    void setFilter(const QModelIndex &index, AbstractFilter::Pointer filter);

    QModelIndex indexOfFilter(AbstractFilter::Pointer filter, const QModelIndex &parent);

    FilterInputWidget* filterInputWidget(const QModelIndex &index);

    bool filterEnabled(const QModelIndex &index);
    void setFilterEnabled(const QModelIndex &index, bool enabled);

    virtual QModelIndex sibling(int row, int column, const QModelIndex& idx) const Q_DECL_OVERRIDE;

    bool isEmpty();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;

    PipelineItem::WidgetState widgetState(const QModelIndex &index);
    void setWidgetState(const QModelIndex &index, PipelineItem::WidgetState state);

    PipelineItem::ErrorState errorState(const QModelIndex &index);
    void setErrorState(const QModelIndex &index, PipelineItem::ErrorState state);

    PipelineItem::PipelineState pipelineState(const QModelIndex &index);
    void setPipelineState(const QModelIndex &index, PipelineItem::PipelineState state);

    PipelineItem::ItemType itemType(const QModelIndex &index);
    void setItemType(const QModelIndex &index, PipelineItem::ItemType type);

    bool pipelineSaved(const QModelIndex &index);
    void setPipelineSaved(const QModelIndex &index, bool saved);

    QModelIndex getActivePipeline();
    void setActivePipeline(const QModelIndex &index, bool value);
    void clearActivePipeline();

    bool needsToBeExpanded(const QModelIndex& index);
    void setNeedsToBeExpanded(const QModelIndex& index, bool value);

    PipelineItem* getRootItem();

  signals:
    void clearIssuesTriggered();

    void preflightTriggered(const QModelIndex &pipelineIndex);

    void pipelineDataChanged(const QModelIndex &pipelineIndex);

    void statusMessageGenerated(const QString &msg);
    void standardOutputMessageGenerated(const QString &msg);

  private:
    PipelineItem*                       m_RootItem;

    QPersistentModelIndex               m_ActivePipelineIndex;

    QList<QObject*>                     m_PipelineMessageObservers;

    PipelineItem* getItem(const QModelIndex& index) const;

    QColor getForegroundColor(const QModelIndex &index) const;

    /**
     * @brief getPipelineFromFile
     * @param filePath
     * @return
     */
    FilterPipeline::Pointer getPipelineFromFile(const QString& filePath);

    PipelineModel(const PipelineModel&);    // Copy Constructor Not Implemented
    void operator=(const PipelineModel&);  // Operator '=' Not Implemented
};

#endif // _pipelinetreemodel_h_
