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

#pragma once

#include <QtCore/QJsonObject>

#include <QtWidgets/QTreeView>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

class FilterListModel;

/**
 * @brief The FilterListView class
 */
class SVWidgetsLib_EXPORT FilterListView : public QTreeView
{
    Q_OBJECT

  public:
    /**
    * @brief FilterListView
    * @param parent
    */
    FilterListView(QWidget* parent = nullptr);

    /**
    * @brief ~FilterListView()
    */
    ~FilterListView() override;

    enum class SearchGroup : unsigned int
    {
      HumanLabel,
      GroupName,
      SubgroupName,
      BrandingName,
      CompiledLibraryName,
      Keywords
    };

    /**
     * @brief addFilter
     * @param filter
     */
    void addFilter(AbstractFilter::Pointer filter);

    /**
     * @brief addFilter
     * @param filter
     * @param group
     */
    void addFilter(AbstractFilter::Pointer filter, SearchGroup group);

    /**
     * @brief addGroup
     * @param group
     */
    void addGroup(SearchGroup group);

    /**
     * @brief findIndexByName
     * @param name
     * @return
     */
    QModelIndex findIndexByName(const QString &name);

    /**
     * @brief clear
     */
    void clear();

  public slots:
    /**
     * @brief listenKeyPressed
     * @param event
     */
    void listenKeyPressed(QKeyEvent* event);

  protected:
    /**
    * @brief Detect a key press
    */
    void keyPressEvent(QKeyEvent* event) override;

    /**
    * @brief mousePressEvent
    * @param event
    */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QMouseEvent* event) override;

  signals:
    void filterItemDoubleClicked(const QString& filterName, int insertIndex = -1, bool useAnimationOnFirstRun = true);

  private slots:
    /**
     * @brief requestContextMenu
     * @param pos
     */
    void requestContextMenu(const QPoint &pos);

  private:
    QPoint m_StartPos;

    QMap<SearchGroup, QPersistentModelIndex> m_SearchGroupIndexMap;

    /**
     * @brief addFilter
     * @param filter
     * @param parent
     */
    void addFilter(AbstractFilter::Pointer filter, const QModelIndex &parent);

    /**
     * @brief connectSignalsSlots
     */
    void connectSignalsSlots();

    /**
     * @brief performDrag
     */
    void performDrag();

    /**
    * @brief launchHelpForItem
    * @param name
    */
    void launchHelpForItem(const QString &name);

    /**
     * @brief findFirstSelectableIndex
     * @return
     */
    QModelIndex findNextSelectableIndex();

    /**
     * @brief findPreviousSelectableIndex
     * @return
     */
    QModelIndex findPreviousSelectableIndex();

    /**
     * @brief getFilterListModel
     * @return
     */
    FilterListModel* getFilterListModel();
};
