/* ============================================================================
 * Copyright (c) 2009-2018 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/Geometry/ITransformContainer.h"
#include "SIMPLib/SIMPLib.h"

#include "H5Support/H5Lite.h"

class SIMPLib_EXPORT TransformContainer : public ITransformContainer
{
public:
  using ParametersValueType = double;
  using TransformParametersType = std::vector<ParametersValueType>;
  using TransformFixedParametersType = std::vector<ParametersValueType>;

  TransformContainer();
  ~TransformContainer() override;

  using Self = TransformContainer;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for TransformContainer
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for TransformContainer
   */
  static QString ClassName();

  /**
   * @brief Setter property for Parameters
   */
  void setParameters(const TransformContainer::TransformParametersType& value);
  /**
   * @brief Getter property for Parameters
   * @return Value of Parameters
   */
  TransformContainer::TransformParametersType getParameters() const;

  /**
   * @brief Setter property for FixedParameters
   */
  void setFixedParameters(const TransformContainer::TransformFixedParametersType& value);
  /**
   * @brief Getter property for FixedParameters
   * @return Value of FixedParameters
   */
  TransformContainer::TransformFixedParametersType getFixedParameters() const;

  /**
   * @brief Setter property for TransformTypeAsString
   */
  void setTransformTypeAsString(const std::string& value);
  /**
   * @brief Getter property for TransformTypeAsString
   * @return Value of TransformTypeAsString
   */
  std::string getTransformTypeAsString() const;

  /**
   * @brief Setter property for MovingName
   */
  void setMovingName(const std::string& value);
  /**
   * @brief Getter property for MovingName
   * @return Value of MovingName
   */
  std::string getMovingName() const;

  /**
   * @brief Setter property for ReferenceName
   */
  void setReferenceName(const std::string& value);
  /**
   * @brief Getter property for ReferenceName
   * @return Value of ReferenceName
   */
  std::string getReferenceName() const;

  TransformContainer& operator=(const TransformContainer&);

  int writeTransformContainerToHDF5(hid_t parentId, const std::string& transformContainerName) override;

  int readTransformContainerFromHDF5(hid_t parentId, bool metaDataOnly, const std::string& transformContainerName) override;

private:
  TransformContainer::TransformParametersType m_Parameters = {};
  TransformContainer::TransformFixedParametersType m_FixedParameters = {};
  std::string m_TransformTypeAsString = {};
  std::string m_MovingName = {};
  std::string m_ReferenceName = {};
};
