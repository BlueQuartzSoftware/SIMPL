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

#include <QtCore/QModelIndex>
#include <QtCore/QTimer>

#include <QtWidgets/QStyledItemDelegate>

#include "SVWidgetsLib/SVWidgetsLib.h"

class PipelineModel;
class SVPipelineView;
class QPushButton;

class SVWidgetsLib_EXPORT PipelineItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:    
    explicit PipelineItemDelegate(SVPipelineView* view);

    virtual ~PipelineItemDelegate();

  protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

  private:
    enum class HoverItem : unsigned int
    {
      DeleteButton,
      DisableButton,
      Widget,
      Unknown
    };

    SVPipelineView* m_View = nullptr;
    const qreal m_BorderThickness = 3.0;
    int m_MousePressIndex = -1;
    HoverItem m_CurrentlyHoveredItem = HoverItem::Unknown;

    QPushButton*              m_DisableBtn = nullptr;
    QPushButton*              m_DeleteBtn = nullptr;

    /**
     * @brief Gets the proper filter index string that refers to the specified index
     * @param index
     * @return
     */
    QString getFilterIndexString(const QModelIndex &index) const;

    /**
     * @brief createDeleteButton
     * @return
     */
    void createDeleteButton();

    /**
     * @brief createDisableButton
     * @return
     */
    void createDisableButton();

    /**
     * @brief Convenience method to get the PipelineModel instance
     * @param index
     * @return
     */
    const PipelineModel* getPipelineModel(const QModelIndex &index) const;

    PipelineItemDelegate(const PipelineItemDelegate&) = delete; // Copy Constructor Not Implemented
    void operator=(const PipelineItemDelegate&) = delete;        // Operator '=' Not Implemented
};
