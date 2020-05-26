/*
 * Your License or Copyright Information can go here
 */

#include <memory>

#include "Filt0.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt0::Filt0()
: m_DataContainerName(SIMPL::Defaults::DataContainerName)
, m_Filt0_Float(6.6f)
, m_Filt0_Integer(15)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt0::~Filt0() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  /* Place all your option initialization code here */

  /*  For an Integer use this code*/
  {
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Int Value", Filt0_Integer, FilterParameter::Parameter, Filt0));
  }
  /*  For a Floating point value use this code*/
  {
    parameters.push_back(SIMPL_NEW_FLOAT_FP("Float Value", Filt0_Float, FilterParameter::Parameter, Filt0));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  float dummyFloat = 0.0f;
  int dummyInt = 0;
  reader->openFilterGroup(this, index);
  setFilt0_Float(reader->readValue("Filt0_Float", dummyFloat));
  setFilt0_Integer(reader->readValue("Filt0_Integer", dummyInt));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::execute()
{
  clearErrorCode();
  clearWarningCode();
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  if(nullptr == m)
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
QUuid Filt0::getUuid() const
{
  return QUuid("{2df74121-7513-598e-8d73-9a990303c600}");
}

// -----------------------------------------------------------------------------
Filt0::Pointer Filt0::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<Filt0> Filt0::New()
{
  struct make_shared_enabler : public Filt0
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString Filt0::getNameOfClass() const
{
  return QString("Filt0");
}

// -----------------------------------------------------------------------------
QString Filt0::ClassName()
{
  return QString("Filt0");
}

// -----------------------------------------------------------------------------
void Filt0::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString Filt0::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void Filt0::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString Filt0::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void Filt0::setFilt0_Float(float value)
{
  m_Filt0_Float = value;
}

// -----------------------------------------------------------------------------
float Filt0::getFilt0_Float() const
{
  return m_Filt0_Float;
}

// -----------------------------------------------------------------------------
void Filt0::setFilt0_Integer(int32_t value)
{
  m_Filt0_Integer = value;
}

// -----------------------------------------------------------------------------
int32_t Filt0::getFilt0_Integer() const
{
  return m_Filt0_Integer;
}
