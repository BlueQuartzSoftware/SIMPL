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

#include <QtCore/QMap>
#include <QtCore/QString>

#include "SVWidgetsLib/Core/IFilterWidgetFactory.h"
#include "SVWidgetsLib/SVWidgetsLib.h"

/**
 * @class FilterWidgetManager FilterWidgetManager.h FilterWidgets/FilterWidgetManager.h
 * @brief This class is a singleton based manager for creating Qt based widgets
 * that expose the various options for each filter.
 *
 * @date Jan 6, 2012
 * @version 1.0
 */
class SVWidgetsLib_EXPORT FilterWidgetManager
{
public:
  // SIMPL_SHARED_POINTERS(FilterWidgetManager)
  /**
   * @brief Returns the name of the class for FilterWidgetManager
   */
  virtual QString getNameOfClass() const;
  /**
   * @brief Returns the name of the class for FilterWidgetManager
   */
  static QString ClassName();

  virtual ~FilterWidgetManager();

  typedef QMap<QString, IFilterWidgetFactory::Pointer> Collection;

  /**
   * @brief Static instance to retrieve the global instance of this class
   * @return
   */
  static FilterWidgetManager* Instance();

  /**
   * @brief Registers a PipelineFilterWidgetFactory instance for a give name
   * @param name The name of the filter
   * @param factory An instance of the factory
   */
  static void RegisterFilterWidgetFactory(const QString& name, IFilterWidgetFactory::Pointer factory);

  static void RegisterKnownFilterWidgets();

  /**
   * @brief Returns the mapping of names to Factory instances for all the
   * factories that are registered.
   * @return
   */
  Collection getFactories();

  /**
   * @brief Adds a Factory that creates PipelineFilterWidgets
   * @param name
   * @param factory
   */
  void addFilterWidgetFactory(const QString& widgetType, IFilterWidgetFactory::Pointer factory);

  /**
   * @brief createWidget
   * @param parameter
   * @param filter
   * @param parent
   * @return
   */
  QWidget* createWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent);

protected:
  FilterWidgetManager();

private:
  Collection m_Factories;

  static FilterWidgetManager* self;

public:
  FilterWidgetManager(const FilterWidgetManager&) = delete;            // Copy Constructor Not Implemented
  FilterWidgetManager(FilterWidgetManager&&) = delete;                 // Move Constructor Not Implemented
  FilterWidgetManager& operator=(const FilterWidgetManager&) = delete; // Copy Assignment Not Implemented
  FilterWidgetManager& operator=(FilterWidgetManager&&) = delete;      // Move Assignment Not Implemented
};
