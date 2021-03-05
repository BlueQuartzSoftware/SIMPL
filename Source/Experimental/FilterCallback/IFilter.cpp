#include "IFilter.h"

#include "FilterParameter.h"

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

  params.push_back(SIMPL_NEW_INT32_FP("Parameter 1", Parameter1, IFilterParameter::Category::Parameter));
  params.push_back(SIMPL_NEW_DOUBLE_FP("Parameter 2", Parameter2, IFilterParameter::Category::Parameter));
  params.push_back(SIMPL_NEW_INT32_FP("Index", Index, IFilterParameter::Category::Parameter, groupIndex));
  params.push_back(SIMPL_NEW_DATA_ARRAY_PATH_FP("FeatureIdsPath", FeatureIdsPath, IFilterParameter::Category::Parameter, groupIndex));

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
  //  Q_EMIT preflightAboutToExecute();
  //  Q_EMIT updateFilterParameters(this);
  //  dataCheck();
  //  Q_EMIT preflightExecuted();
  //  setInPreflight(false);
}

// -----------------------------------------------------------------------------
void IFilter::setIndex(int value)
{
  m_Index = value;
}

// -----------------------------------------------------------------------------
int IFilter::getIndex() const
{
  return m_Index;
}

// -----------------------------------------------------------------------------
void IFilter::setParameter1(int value)
{
  m_Parameter1 = value;
}

// -----------------------------------------------------------------------------
int IFilter::getParameter1() const
{
  return m_Parameter1;
}

// -----------------------------------------------------------------------------
void IFilter::setParameter2(double value)
{
  m_Parameter2 = value;
}

// -----------------------------------------------------------------------------
double IFilter::getParameter2() const
{
  return m_Parameter2;
}

// -----------------------------------------------------------------------------
void IFilter::setFeatureIdsPath(const DataArrayPath& value)
{
  m_FeatureIdsPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath IFilter::getFeatureIdsPath() const
{
  return m_FeatureIdsPath;
}

// -----------------------------------------------------------------------------
void IFilter::setInPreflight(bool value)
{
  m_InPreflight = value;
}

// -----------------------------------------------------------------------------
bool IFilter::getInPreflight() const
{
  return m_InPreflight;
}
