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

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

/**
 * @class ReadOrientationData ReadOrientationData.h /FilterCategoryFilters/ReadOrientationData.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class SIMPLib_EXPORT MakeDataContainer : public AbstractFilter
{
  Q_OBJECT

  // Start Python bindings declarations
  PYB11_BEGIN_BINDINGS(MakeDataContainer SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(MakeDataContainer)
  PYB11_FILTER_NEW_MACRO(MakeDataContainer)
  PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
  PYB11_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)
  PYB11_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)
  PYB11_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)
  PYB11_PROPERTY(QString LatticeConstantsArrayName READ getLatticeConstantsArrayName WRITE setLatticeConstantsArrayName)
  PYB11_END_BINDINGS()
  // End Python bindings declarations

public:
  using Self = MakeDataContainer;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;

  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for MakeDataContainer
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for MakeDataContainer
   */
  static QString ClassName();

  ~MakeDataContainer() override;
  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const QString& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  QString getDataContainerName() const;

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  QString getCellEnsembleAttributeMatrixName() const;

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;

  /**
   * @brief Setter property for PhaseNameArrayName
   */
  void setPhaseNameArrayName(const QString& value);
  /**
   * @brief Getter property for PhaseNameArrayName
   * @return Value of PhaseNameArrayName
   */
  QString getPhaseNameArrayName() const;

  /**
   * @brief Setter property for MaterialNameArrayName
   */
  void setMaterialNameArrayName(const QString& value);
  /**
   * @brief Getter property for MaterialNameArrayName
   * @return Value of MaterialNameArrayName
   */
  QString getMaterialNameArrayName() const;

  /**
   * @brief This returns the group that the filter belonds to. You can select
   * a different group if you want. The string returned here will be displayed
   * in the GUI for the filter
   */
  /**
   * @brief Setter property for FeatureIdsArrayName
   */
  void setFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureIdsArrayName
   * @return Value of FeatureIdsArrayName
   */
  QString getFeatureIdsArrayName() const;

  Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

  /**
   * @brief Setter property for CellEulerAnglesArrayName
   */
  void setCellEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for CellEulerAnglesArrayName
   * @return Value of CellEulerAnglesArrayName
   */
  QString getCellEulerAnglesArrayName() const;

  Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

  /**
   * @brief Setter property for CellPhasesArrayName
   */
  void setCellPhasesArrayName(const QString& value);
  /**
   * @brief Getter property for CellPhasesArrayName
   * @return Value of CellPhasesArrayName
   */
  QString getCellPhasesArrayName() const;

  Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

  /**
   * @brief Setter property for CrystalStructuresArrayName
   */
  void setCrystalStructuresArrayName(const QString& value);
  /**
   * @brief Getter property for CrystalStructuresArrayName
   * @return Value of CrystalStructuresArrayName
   */
  QString getCrystalStructuresArrayName() const;

  Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

  /**
   * @brief Setter property for LatticeConstantsArrayName
   */
  void setLatticeConstantsArrayName(const QString& value);
  /**
   * @brief Getter property for LatticeConstantsArrayName
   * @return Value of LatticeConstantsArrayName
   */
  QString getLatticeConstantsArrayName() const;

  Q_PROPERTY(QString LatticeConstantsArrayName READ getLatticeConstantsArrayName WRITE setLatticeConstantsArrayName)

  QString getCompiledLibraryName() const override;
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
  QString getGroupName() const override;

  /**
   * @brief This returns a string that is displayed in the GUI. It should be readable
   * and understandable by humans.
   */
  QString getHumanLabel() const override;

  /**
   * @brief This method will instantiate all the end user settable options/parameters
   * for this filter
   */
  void setupFilterParameters() override; /**
                                          *@brief This returns a string that is displayed in the GUI and helps to sort the filters into
                                          *a subgroup. It should be readable and understandable by humans.
                                          */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief This method will read the options from a file
   * @param reader The reader that is used to read the options from a file
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief Reimplemented from @see AbstractFilter class
   */
  void execute() override;

protected:
  MakeDataContainer();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<float>> m_LatticeConstantsPtr;
  float* m_LatticeConstants = nullptr;

  QString m_DataContainerName = {};
  QString m_CellEnsembleAttributeMatrixName = {};
  QString m_CellAttributeMatrixName = {};
  QString m_PhaseNameArrayName = {};
  QString m_MaterialNameArrayName = {};
  QString m_FeatureIdsArrayName = {};
  QString m_CellEulerAnglesArrayName = {};
  QString m_CellPhasesArrayName = {};
  QString m_CrystalStructuresArrayName = {};
  QString m_LatticeConstantsArrayName = {};

public:
  MakeDataContainer(const MakeDataContainer&) = delete;            // Copy Constructor Not Implemented
  MakeDataContainer(MakeDataContainer&&) = delete;                 // Move Constructor Not Implemented
  MakeDataContainer& operator=(const MakeDataContainer&) = delete; // Copy Assignment Not Implemented
  MakeDataContainer& operator=(MakeDataContainer&&) = delete;      // Move Assignment Not Implemented
};
