
#ifndef _filterwidget_H_
#define _filterwidget_H_





class IFilter;
class Int32Parameter;
class IFilterParameter;


class IntFilterParameterWidget /* :  Normally be a QWidget class */
{

public:
  IntFilterParameterWidget(IFilter* filter, Int32Parameter* parameter);
  
  IntFilterParameterWidget(IFilter* filter, IFilterParameter* parameter);
  
  virtual ~IntFilterParameterWidget();
  
  void parametersUpdated();
  
private:

  IFilter* m_Filter = nullptr;
  Int32Parameter* m_FilterParameter = nullptr;

  IntFilterParameterWidget(const IntFilterParameterWidget&) = delete; // Copy Constructor Not Implemented
  void operator=(const IntFilterParameterWidget&) = delete;           // Move assignment Not Implemented
};


#endif /* _filterwidget_H_ */

