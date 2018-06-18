

#include <functional>
#include <iostream>

#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"

#include "IFilter.h"
#include "FilterWidget.h"

//#define ADD_NEW_FILTER_PARAMETER(Params, Class, Desc, Prop, Category, Type)                                                                                                                                             \
//  Class::SetterCallbackType _##Prop##_ParamSetter = [this](Type i) { return this->set##Prop(i); };                                                                                                       \
//  Class::GetterCallbackType _##Prop##_ParamGetter = [this] { return this->get##Prop(); };                                                                                                                \
//  Params.push_back(Class::New(Desc, #Prop, get##Prop(), Category, _##Prop##_ParamSetter, _##Prop##_ParamGetter));

//#define ADD_DOUBLE_PARAMETER(Params, Desc, Prop, Category) \
//  ADD_NEW_FILTER_PARAMETER(Params, DoubleParameter, Desc, Prop, Category, double)

//#define ADD_INT32_PARAMETER(Params, Desc, Prop) \
//  ADD_NEW_FILTER_PARAMETER(Params, Int32Parameter, Desc, Prop, int)

//#define ADD_DATAARRAYPATH_PARAMETER(Params, Desc, Prop) \
//  ADD_NEW_FILTER_PARAMETER(Params, DataArrayPathParameter, Desc, Prop, DataArrayPath)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
const QString FilterName("FilterName");
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  IFilter filter;
  filter.printValues(std::cout);
  std::vector<IFilterParameter::Pointer> params = filter.getFilterParameters();

  Int32Parameter::Pointer intParam = std::dynamic_pointer_cast<Int32Parameter>(params.at(0));
  if(nullptr != intParam.get())
  {
    intParam->getSetterCallback()(234234234);
  }

  DoubleParameter::Pointer dblParam = std::dynamic_pointer_cast<DoubleParameter>(params.at(1));
  if(nullptr != dblParam.get())
  {
    dblParam->getSetterCallback()(6.66666);
  }

  dblParam = std::dynamic_pointer_cast<DoubleParameter>(params.at(2));
  if(nullptr != dblParam.get())
  {
    dblParam->getSetterCallback()(6.66666);
  }
  
  DataArrayPathParameter::Pointer dapParam = std::dynamic_pointer_cast<DataArrayPathParameter>(params.at(3));
  if(dapParam) {
    std::function<void(DataArrayPath)> dapCallback = dapParam->getSetterCallback();
    if(dapCallback) {
      dapCallback(DataArrayPath("Image","CellAM","FeatureIds"));
    }
  }

  filter.printValues(std::cout);

  IntFilterParameterWidget int32Widget_A(&filter, intParam.get());
  int32Widget_A.parametersUpdated();
  filter.printValues(std::cout);

  IntFilterParameterWidget int32Widget_B(&filter, dblParam.get());
  int32Widget_B.parametersUpdated();
  filter.printValues(std::cout);


  QJsonObject root;
  filter.writeParameters(root);

  qDebug() << root;

  return 0;
}
