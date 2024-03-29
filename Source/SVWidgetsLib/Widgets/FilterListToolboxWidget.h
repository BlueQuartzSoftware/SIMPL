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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>

#include <QtCore/QList>
#include <QtCore/QSignalMapper>

#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/IFilterFactory.hpp"

#include "SVWidgetsLib/QtSupport/QtSSettings.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

#include "ui_FilterListToolboxWidget.h"

/**
 * @brief The FilterListToolboxWidget class
 */
class SVWidgetsLib_EXPORT FilterListToolboxWidget : public QWidget, private Ui::FilterListToolboxWidget
{
  Q_OBJECT

public:
  FilterListToolboxWidget(QWidget* parent = nullptr);
  ~FilterListToolboxWidget() override;

  /**
   * @brief Reads the preferences from the users pref file
   */
  void readSettings(QtSSettings* prefs);

  /**
   * @brief Writes the preferences to the users pref file
   */
  void writeSettings(QtSSettings* prefs);

public Q_SLOTS:

  /**
   * @brief searchFilters triggered when the user types something in the Search Field
   */
  void searchFilters(QString text);

  /**
   * @brief updateFilterList This method extracts all the names of the filters that have been
   * loaded into the application
   */
  void loadFilterList();

  /**
   * @brief searchFieldsChanged
   */
  void searchFieldsChanged(bool isChecked);

  /**
   * @brief getActiveSearchAction
   */
  QAction* getActiveSearchAction();

  /**
   * @brief setActiveSearchAction
   */
  void setActiveSearchAction(QAction* action);

  /**
   * @brief getActiveSearchAction
   */
  QList<QAction*> getSearchActionList();

protected:
  /**
   * @brief setupGui Called to get everything in the GUI setup correctly
   */
  void setupGui();

  /**
   * @brief setupSearchFieldActions
   */
  void setupSearchField();

Q_SIGNALS:
  void filterItemDoubleClicked(const QString& filterName, int insertIndex = -1, bool useAnimationOnFirstRun = true);

private:
  QMenu* m_ContextMenu;
  QSignalMapper* m_Mapper;

  bool m_SearchAnyWords;
  bool m_SearchExactPhrase;
  bool m_SearchAllWords;

  QAction* m_ActionAnyWords;
  QAction* m_ActionExactPhrase;
  QAction* m_ActionAllWords;

  FilterManager::Collection m_LoadedFilters;

  QMap<QString, IFilterFactory::Pointer> getHumanNameMap(const QList<IFilterFactory::Pointer>& list);

  void matchFiltersToSearchGroup(std::vector<IFilterFactory::Pointer> filters, QSet<IFilterFactory*>& addedFiltersSet, const QStringList& searchTokens, FilterListView::SearchGroup searchGroup);

  int getMatchingWordCountForFilter(QStringList searchTokens, IFilterFactory::Pointer filter, FilterListView::SearchGroup searchGroup);

  int getMatchingRelevanceForFilter(QStringList searchTokens, IFilterFactory::Pointer filter, FilterListView::SearchGroup searchGroup);

public:
  FilterListToolboxWidget(const FilterListToolboxWidget&) = delete;            // Copy Constructor Not Implemented
  FilterListToolboxWidget(FilterListToolboxWidget&&) = delete;                 // Move Constructor Not Implemented
  FilterListToolboxWidget& operator=(const FilterListToolboxWidget&) = delete; // Copy Assignment Not Implemented
  FilterListToolboxWidget& operator=(FilterListToolboxWidget&&) = delete;      // Move Assignment Not Implemented
};
