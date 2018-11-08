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

#pragma once

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

    ~PipelineModel() override;

    enum Roles
    {
      WidgetStateRole = Qt::UserRole + 1,
      ErrorStateRole,
      PipelineStateRole,
      ItemTypeRole,
      ExpandedRole,
      BorderSizeRole,
      HeightRole,
      WidthRole,
      XOffsetRole,
      YOffsetRole,
      AnimationTypeRole
    };

    SIMPL_INSTANCE_PROPERTY(int, MaxNumberOfPipelines)

    /**
     * @brief updateActivePipeline
     * @param pipelineIdx
     */
    void updateActivePipeline(const QModelIndex &pipelineIdx);

    QVariant data(const QModelIndex& index, int role) const override;
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    AbstractFilter::Pointer filter(const QModelIndex &index) const;
    void setFilter(const QModelIndex &index, AbstractFilter::Pointer filter);

    QString dropIndicatorText(const QModelIndex &index) const;
    void setDropIndicatorText(const QModelIndex &index, const QString &text);

    QModelIndex indexOfFilter(AbstractFilter *filter, const QModelIndex &parent = QModelIndex());

    FilterInputWidget* filterInputWidget(const QModelIndex &index);

    bool isEmpty();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;

    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;
    
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    bool pipelineSaved(const QModelIndex &index);
    void setPipelineSaved(const QModelIndex &index, bool saved);

    QModelIndex getActivePipeline();
    void setActivePipeline(const QModelIndex &index, bool value);
    void clearActivePipeline();

    PipelineItem* getRootItem();

    int getMaxFilterCount() const;

    QList<QObject*> getPipelineMessageObservers();

  signals:
    void clearIssuesTriggered();

    void preflightTriggered(const QModelIndex &pipelineIndex, PipelineModel* model);

    void pipelineDataChanged(const QModelIndex &pipelineIndex);

    void statusMessageGenerated(const QString &msg);
    void standardOutputMessageGenerated(const QString &msg);

  private:
    PipelineItem*                       m_RootItem;

    QPersistentModelIndex               m_ActivePipelineIndex;

    PipelineItem* getItem(const QModelIndex& index) const;

    QColor getForegroundColor(const QModelIndex &index) const;

  public:
    PipelineModel(const PipelineModel&) = delete;            // Copy Constructor Not Implemented
    PipelineModel(PipelineModel&&) = delete;                 // Move Constructor Not Implemented
    PipelineModel& operator=(const PipelineModel&) = delete; // Copy Assignment Not Implemented
    PipelineModel& operator=(PipelineModel&&) = delete;      // Move Assignment Not Implemented
};

