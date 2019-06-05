/*
 * Your License or Copyright Information can go here
 */

#include "Filt1.h"
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
    parameters.push_back(SIMPL_NEW_INTEGER_FP("Int Value", Filt1_Integer, FilterParameter::Parameter, Filt1));
  }
  /*  For a Floating point value use this code*/
  {
    parameters.push_back(SIMPL_NEW_FLOAT_FP("Float Value", Filt1_Float, FilterParameter::Parameter, Filt1));
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
void Filt1::preflight()
{

  dataCheck();
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
TestFilters::Pointer TestFilters::NullPointer()
{
  return Pointer(static_cast<Filt0*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<Filt0> TestFilters::New()
{
  struct make_shared_enabler : public Filt0  
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
void TestFilters::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString TestFilters::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void TestFilters::setCellAttributeMatrixName(const QString& value)
{
  m_CellAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString TestFilters::getCellAttributeMatrixName() const
{
  return m_CellAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void TestFilters::setFilt0_Float(const float& value)
{
  m_Filt0_Float = value;
}

// -----------------------------------------------------------------------------
float TestFilters::getFilt0_Float() const
{
  return m_Filt0_Float;
}

// -----------------------------------------------------------------------------
void TestFilters::setFilt0_Integer(const int32_t& value)
{
  m_Filt0_Integer = value;
}

// -----------------------------------------------------------------------------
int32_t TestFilters::getFilt0_Integer() const
{
  return m_Filt0_Integer;
}

// -----------------------------------------------------------------------------
TestFilters::Pointer TestFilters::NullPointer()
{
  return Pointer(static_cast<Filt1*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<Filt1> TestFilters::New()
{
  struct make_shared_enabler : public Filt1  
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
void TestFilters::setDataContainerName(const QString& value)
{
  m_DataContainerName = value;
}

// -----------------------------------------------------------------------------
QString TestFilters::getDataContainerName() const
{
  return m_DataContainerName;
}

// -----------------------------------------------------------------------------
void TestFilters::setFilt1_Float(const float& value)
{
  m_Filt1_Float = value;
}

// -----------------------------------------------------------------------------
float TestFilters::getFilt1_Float() const
{
  return m_Filt1_Float;
}

// -----------------------------------------------------------------------------
void TestFilters::setFilt1_Integer(const int32_t& value)
{
  m_Filt1_Integer = value;
}

// -----------------------------------------------------------------------------
int32_t TestFilters::getFilt1_Integer() const
{
  return m_Filt1_Integer;
}


