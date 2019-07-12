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

#include <stdlib.h>

// This is core/vnl/examples/vnl_lsqr_test.cxx
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/vnl_vector.h>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "itkCommand.h"
#include "itkProcessObject.h"

namespace itk
{
class ProgressObserver : public itk::Command
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(ProgressObserver);

  /** Standard class type aliases. */
  using Self = ProgressObserver;
  using Pointer = SmartPointer<Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ProgressObserver, itk::Command);

  void Execute(itk::Object* caller, const itk::EventObject& event) override
  {
    Execute((const itk::Object*)caller, event);
  }

  void Execute(const itk::Object* caller, const itk::EventObject& event) override
  {
    if(!itk::ProgressEvent().CheckEvent(&event))
    {
      return;
    }
    const auto* processObject = dynamic_cast<const itk::ProcessObject*>(caller);
    if(processObject == nullptr)
    {
      return;
    }
    A.set_row(row_, cols, vals);
    A.scale_row(row_, 1.0 / sum);
  }

    QString progressStr = QString::number(processObject->GetProgress() * 100, 'f', 0);

    QString ss;
    if(m_MessagePrefix.isEmpty())
    {
      ss = QObject::tr("%1%").arg(progressStr);
    }
    else
    {
      ss = QObject::tr("%1: %2%").arg(m_MessagePrefix).arg(progressStr);
    }

    if(m_Filter != nullptr)
    {
      m_Filter->notifyStatusMessage(ss);
    }
  }

  void SetMessagePrefix(const QString& prefix)
  {
    m_MessagePrefix = prefix;
  }

  void SetFilter(AbstractFilter* filter)
  {
    m_Filter = filter;
  }

protected:
  ProgressObserver() = default;

  ~ProgressObserver() override = default;

private:
  AbstractFilter* m_Filter = nullptr;
  QString m_MessagePrefix;
};
} // namespace itk
