/* ============================================================================
 * Copyright (c) 2021 BlueQuartz Software, LLC
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include "SIMPLib/FilterParameters/FilterParameter.h"

/**
 * @class AbstractIOFilterParameter AbstractIOFilterParameter.h SIMPLib/FilterParameters/AbstractIOFilterParameter.h
 * @brief Base class for file/path input/output filter parameters
 */
class SIMPLib_EXPORT AbstractIOFilterParameter : public FilterParameter
{
public:
  using SetterCallbackType = std::function<void(QString)>;
  using GetterCallbackType = std::function<QString(void)>;

  ~AbstractIOFilterParameter() override = default;

  AbstractIOFilterParameter(const AbstractIOFilterParameter&) = delete;
  AbstractIOFilterParameter(AbstractIOFilterParameter&&) = delete;
  AbstractIOFilterParameter& operator=(const AbstractIOFilterParameter&) = delete;
  AbstractIOFilterParameter& operator=(AbstractIOFilterParameter&&) = delete;

  /**
   * @brief readJson
   * @return
   */
  void readJson(const QJsonObject& json) override;

  /**
   * @brief writeJson
   * @return
   */
  void writeJson(QJsonObject& json) const override;

  /**
   * @brief Sets setter
   * @param setter
   * @return
   */
  void setSetterCallback(const SetterCallbackType& setter);

  /**
   * @brief Sets getter
   * @param getter
   * @return
   */
  void setGetterCallback(const GetterCallbackType& getter);

  /**
   * @brief Returns setter
   * @return
   */
  SetterCallbackType getSetterCallback() const;

  /**
   * @brief Returns getter
   * @return
   */
  GetterCallbackType getGetterCallback() const;

protected:
  AbstractIOFilterParameter();

private:
  SetterCallbackType m_SetterCallback;
  GetterCallbackType m_GetterCallback;
};
