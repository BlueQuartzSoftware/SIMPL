/*
 * Your License or Copyright Information can go here
 */

#include <memory>

#include "Filt1.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt1::Filt1()
: m_DataContainerName(SIMPL::Defaults::DataContainerName)
, m_Filt1_Float(9.9f)
, m_Filt1_Integer(123)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt1::~Filt1() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  /* Place all your option initialization code here */

  /*  For an Integer use this code*/
  {
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Int Value", Filt1_Integer, FilterParameter::Category::Parameter, Filt1));
  }
  /*  For a Floating point value use this code*/
  {
    parameters.push_back(SIMPL_NEW_FLOAT_FP("Float Value", Filt1_Float, FilterParameter::Category::Parameter, Filt1));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  float dummyFloat = 0.0f;
  int dummyInt = 0;
  reader->openFilterGroup(this, index);
  setFilt1_Float(reader->readValue("Filt1_Float", dummyFloat));
  setFilt1_Integer(reader->readValue("Filt1_Integer", dummyInt));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::execute()
{
  clearErrorCode();
  clearWarningCode();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  if(nullptr == m.get())
  {
    QString ss = QObject::tr(" DataContainer was nullptr");
    setErrorCondition(-1, QObject::tr("VolumeDataContainer was nullptr. Returning from Execute Method for filter %1").arg(getHumanLabel()));
    return;
  }
  clearErrorCode();
  clearWarningCode();

  /* Place all your code to execute your filter here. */
}

// -----------------------------------------------------------------------------
QUuid Filt1::getUuid() const
{
  return QUuid("{436a83fd-20f0-5920-89d1-4bfde900499e}");
}

// -----------------------------------------------------------------------------
Filt1::Pointer Filt1::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<Filt1> Filt1::New()
{
  struct make_shared_enabler : public Filt1
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString Filt1::getNameOfClass() const
{
  return QString("Filt1");
}

// -----------------------------------------------------------------------------
QString Filt1::ClassName()
{
  return QString("Filt1");
}

// -----------------------------------------------------------------------------
void Filt1::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString Filt1::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void Filt1::setFilt1_Float(float value)
{
  m_Filt1_Float = value;
}

// -----------------------------------------------------------------------------
float Filt1::getFilt1_Float() const
{
  return m_Filt1_Float;
}

// -----------------------------------------------------------------------------
void Filt1::setFilt1_Integer(int32_t value)
{
  m_Filt1_Integer = value;
}

// -----------------------------------------------------------------------------
int32_t Filt1::getFilt1_Integer() const
{
  return m_Filt1_Integer;
}
