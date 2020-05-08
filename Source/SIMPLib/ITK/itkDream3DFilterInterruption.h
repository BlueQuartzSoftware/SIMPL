#pragma once

#include "itkConfigure.h"

#if defined(ITK_VERSION_MAJOR) && ITK_VERSION_MAJOR == 4
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-field"
#endif
#endif

#include <itkCommand.h>
#include <itkProcessObject.h>

#include "SIMPLib/Filtering/AbstractFilter.h"

namespace itk
{

class Dream3DFilterInterruption : public Command
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(Dream3DFilterInterruption);

  /** Standard class type aliases. */
  using Self = Dream3DFilterInterruption;
  using Pointer = SmartPointer<Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(Dream3DFilterInterruption, itk::Command);

  void Execute(Object* caller, const EventObject& event) override
  {
    if(m_Filter != nullptr && m_Filter->getCancel())
    {
      ProcessObject* po = dynamic_cast<ProcessObject*>(caller);
      if(po)
      {
        Execute(po);
      }
    }
  }

  void Execute(ProcessObject* object)
  {
    object->AbortGenerateDataOn();
  }

  void Execute(const Object* o_not_used, const EventObject& eo_not_used) override // has to be implemented
  {
  }

  void SetFilter(AbstractFilter* filter)
  {
    m_Filter = filter;
  }

protected:
  Dream3DFilterInterruption() = default;
  ~Dream3DFilterInterruption() override = default;

private:
  AbstractFilter* m_Filter = nullptr;
};

} // namespace itk
