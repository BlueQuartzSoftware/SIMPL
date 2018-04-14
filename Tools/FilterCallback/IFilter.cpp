#include "IFilter.h"




#include "FilterCallback/FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilter::IFilter()
  : m_Index(0)
  , m_Parameter1(std::numeric_limits<int>::max())
  , m_Parameter2(std::numeric_limits<double>::max())
  , m_FeatureIdsPath("Foo", "Bar", "FeatureIds")
{
  setupParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IFilter::~IFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IFilter::setupParameters()
{
  std::vector<IFilterParameter::Pointer> params;
  int groupIndex = -1;
  
  params.push_back( SIMPL_NEW_INT32_FP("Parameter 1", Parameter1, IFilterParameter::Category::Parameter) );
  params.push_back( SIMPL_NEW_DOUBLE_FP("Parameter 2", Parameter2, IFilterParameter::Category::Parameter) );
  params.push_back( SIMPL_NEW_INT32_FP("Index", Index, IFilterParameter::Category::Parameter, groupIndex) );
  params.push_back( SIMPL_NEW_DATA_ARRAY_PATH_FP("FeatureIdsPath", FeatureIdsPath, IFilterParameter::Category::Parameter, groupIndex) );
  
  m_FilterParameters = params;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IFilter::readFilterParameters(QJsonObject& root)
{
  QJsonValue jsonValue = root[QString::number(getIndex())];
  if(!jsonValue.isUndefined() && jsonValue.isObject())
  {
    QJsonObject obj = jsonValue.toObject();
    for(auto const& parameter : m_FilterParameters)
    {
      parameter->readJson(obj);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IFilter::writeParameters(QJsonObject& root)
{
  QJsonObject obj;
  obj["FilterName"] = QString::fromStdString(getName());
  for(auto const& parameter : m_FilterParameters)
  {
    parameter->writeJson(obj);
  }
  root.insert(QString::number(getIndex()), obj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IFilter::printValues(std::ostream& out)
{
  out << "m_P1: " << m_Parameter1 << std::endl;
  out << "m_P2: " << m_Parameter2 << std::endl;
  out << "m_Index: " << m_Index << std::endl;
  out << "FeatureIds Path: " << m_FeatureIdsPath.serialize("/").toStdString() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string IFilter::getName()
{
  return std::string("CoolFilter");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<IFilterParameter::Pointer> IFilter::getFilterParameters()
{
  return m_FilterParameters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IFilter::dataCheck()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void IFilter::preflight()
{
//  setInPreflight(true);
//  emit preflightAboutToExecute();
//  emit updateFilterParameters(this);
//  dataCheck();
//  emit preflightExecuted();
//  setInPreflight(false);
}
