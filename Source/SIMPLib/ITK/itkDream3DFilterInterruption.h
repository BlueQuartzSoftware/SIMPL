#pragma once

#include <itkCommand.h>
#include <itkProcessObject.h>

namespace itk
{

class Dream3DFilterInterruption : public Command
{
public:
  /** Standard class typedefs. */
  typedef Dream3DFilterInterruption Self;
  typedef Command Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  itkNewMacro(Dream3DFilterInterruption);
  itkSetObjectMacro(Filter, AbstractFilter);

private:
  AbstractFilter* m_Filter;
  Dream3DFilterInterruption()
  {
    m_Filter = nullptr;
  }

public:
  void Execute(Object* caller, const EventObject& event) override
  {
    if(m_Filter && m_Filter->getCancel())
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

  void Execute(const Object*, const EventObject&) override // has to be implemented
  {
  }
};

} // namespace itk

