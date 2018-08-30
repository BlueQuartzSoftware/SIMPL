/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include <QtCore/QObject>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/SIMPLib.h"

#include "ICalculatorArray.h"

template <typename T>
class SIMPLib_EXPORT CalculatorArray : public ICalculatorArray
{
  public:
    SIMPL_SHARED_POINTERS(CalculatorArray)

    static Pointer New(typename DataArray<T>::Pointer dataArray, ValueType type, bool allocate)
    {
      return Pointer(new CalculatorArray(dataArray, type, allocate));
    }

    ~CalculatorArray() override = default;

    IDataArray::Pointer getArray() override { return m_Array; }

    void setValue(int i, double val) override
    {
      m_Array->setValue(i, val);
    }

    double getValue(int i) override
    {
      if (m_Array->getNumberOfTuples() > 1)
      {
        return static_cast<double>(m_Array->getValue(i));
      }
      else if (m_Array->getNumberOfTuples() == 1)
      {
        return static_cast<double>(m_Array->getValue(0));
      }
      else
      {
        // ERROR: The array is empty!
        return 0.0;
      }
    }

    ICalculatorArray::ValueType getType() override
    {
      return m_Type;
    }

    DoubleArrayType::Pointer reduceToOneComponent(int c, bool allocate = true) override
    {
      if(c >= 0 && c <= m_Array->getNumberOfComponents())
      {
        if(m_Array->getNumberOfComponents() > 1)
        {
          DoubleArrayType::Pointer newArray = DoubleArrayType::CreateArray(m_Array->getNumberOfTuples(), QVector<size_t>(1, 1), m_Array->getName(), allocate);
          if(allocate)
          {
            for(int i = 0; i < m_Array->getNumberOfTuples(); i++)
            {
              newArray->setComponent(i, 0, m_Array->getComponent(i, c));
            }
          }

          return newArray;
        }
      }

      return DoubleArrayType::NullPointer();
    }

    CalculatorItem::ErrorCode checkValidity(QVector<CalculatorItem::Pointer> infixVector, int currentIndex, QString& msg) override
    {
      Q_UNUSED(infixVector)
      Q_UNUSED(currentIndex)

      return CalculatorItem::ErrorCode::SUCCESS;
    }

  protected:
    CalculatorArray() = default;

    CalculatorArray(typename DataArray<T>::Pointer dataArray, ValueType type, bool allocate) :
      ICalculatorArray(),
      m_Type(type)
    {
      m_Array = DoubleArrayType::CreateArray(dataArray->getNumberOfTuples(), dataArray->getComponentDimensions(), dataArray->getName(), allocate);
      if (allocate == true)
      {
        for (int i = 0; i < dataArray->getSize(); i++)
        {
          m_Array->setValue(i, static_cast<double>(dataArray->getValue(i)));
        }
      }
    }

  private:
    DoubleArrayType::Pointer                                  m_Array;
    ValueType                                                 m_Type;

    CalculatorArray(const CalculatorArray&); // Copy Constructor Not Implemented
    void operator=(const CalculatorArray&);  // Move assignment Not Implemented
};

