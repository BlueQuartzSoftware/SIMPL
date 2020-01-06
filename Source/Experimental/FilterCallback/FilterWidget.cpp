
#include "FilterWidget.h"

#include "FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntFilterParameterWidget::IntFilterParameterWidget(IFilter* filter, Int32Parameter* parameter)
: m_Filter(filter)
, m_FilterParameter(parameter)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntFilterParameterWidget::IntFilterParameterWidget(IFilter* filter, IFilterParameter* parameter)
: m_Filter(filter)
{
  m_FilterParameter = dynamic_cast<Int32Parameter*>(parameter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IntFilterParameterWidget::~IntFilterParameterWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IntFilterParameterWidget::parametersUpdated()
{
  if(m_FilterParameter)
  {
    std::cout << "Human Label: " << m_FilterParameter->getHumanLabel() << std::endl;
    std::cout << "Property Name: " << m_FilterParameter->getPropertyName() << std::endl;
  }

  // Lets assume that we have a Widget that has a text box that holds an integer
  // we would need to convert that to an actual integer value:
  bool ok = false;
  QString intTextField("999");
  int value = intTextField.toInt(&ok, 10);
  if(!ok)
  {
    qDebug() << "Error converting text to integer";
  }

  // If this simulated widgets knows the "type" of data, i.e., we are collecting
  // an integer from the user, then we know what we should be casting the FilterParameter.
  Int32Parameter* filtParam = dynamic_cast<Int32Parameter*>(m_FilterParameter);
  if(nullptr != filtParam)
  {
    Int32Parameter::SetterCallbackType setter = filtParam->getSetterCallback();
    if(setter)
    {
      setter(value);
    }
  }
  else
  {
    std::cout << "Filter Parameter is NOT Int32Parameter"  << std::endl;
  }
}

