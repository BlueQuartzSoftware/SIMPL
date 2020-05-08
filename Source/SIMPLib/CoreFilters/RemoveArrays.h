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

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

/**
 * @brief The RemoveArrays class. See [Filter documentation](@ref removearrays) for details.
 */
class SIMPLib_EXPORT RemoveArrays : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(RemoveArrays SUPERCLASS AbstractFilter)
  PYB11_FILTER()
  PYB11_SHARED_POINTERS(RemoveArrays)
  PYB11_FILTER_NEW_MACRO(RemoveArrays)
  PYB11_PROPERTY(DataContainerArrayProxy DataArraysToRemove READ getDataArraysToRemove WRITE setDataArraysToRemove)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = RemoveArrays;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static std::shared_ptr<RemoveArrays> New();

  /**
   * @brief Returns the name of the class for RemoveArrays
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RemoveArrays
   */
  static QString ClassName();

  ~RemoveArrays() override;

  /**
   * @brief Setter property for DataArraysToRemove
   */
  void setDataArraysToRemove(const DataContainerArrayProxy& value);
  /**
   * @brief Getter property for DataArraysToRemove
   * @return Value of DataArraysToRemove
   */
  DataContainerArrayProxy getDataArraysToRemove() const;

  Q_PROPERTY(DataContainerArrayProxy DataArraysToRemove READ getDataArraysToRemove WRITE setDataArraysToRemove)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
   */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
   * @brief removeSelectionsFromDataContainerArray Attempts to remove those DataContainers, AttributeMatrices, and DataArrays selected by the user.
   * DataArray items that are flagged as selected by the user/programmer.
   * @param dca DataContainerArray instance pointer
   * @param state Flagged state of items in the DataContainerArray
   */
  void removeSelectionsFromDataContainerArray(DataContainerArray* dca, Qt::CheckState state);

  /**
   * @brief Returns the list of deleted data paths.
   * @return
   */
  std::list<DataArrayPath> getDeletedPaths() override;

protected:
  RemoveArrays();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief markSelectionsForDeletion Marks the items in the data container array with checkstate 'state' for deletion
   */
  void markSelectionsForDeletion(DataContainerArray* dca, Qt::CheckState state);

public:
  RemoveArrays(const RemoveArrays&) = delete;            // Copy Constructor Not Implemented
  RemoveArrays(RemoveArrays&&) = delete;                 // Move Constructor Not Implemented
  RemoveArrays& operator=(const RemoveArrays&) = delete; // Copy Assignment Not Implemented
  RemoveArrays& operator=(RemoveArrays&&) = delete;      // Move Assignment Not Implemented

private:
  DataContainerArrayProxy m_DataArraysToRemove = {};

  std::list<DataArrayPath> m_RemovedPaths;
};
