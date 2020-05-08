/*
 * Your License or Copyright can go here
 */

#include "GenerateColorTable.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/GenerateColorTableFilterParameter.h"
#include "SIMPLib/Utilities/ColorTable.h"
#include "SIMPLib/Utilities/ParallelDataAlgorithm.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

enum createdPathID : RenameDataPath::DataID_t
{
  ColorArrayID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
int findRightBinIndex(T nValue, std::vector<float> binPoints)
{
  /* This is a brute-force way of finding the proper bins.
     We will need to change this method to a binary search. */
  int rightBinIndex = 0;
  while(binPoints[rightBinIndex] < nValue)
  {
    rightBinIndex++;
  }

  return rightBinIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int findRightBinIndex_Binary(float nValue, std::vector<float> binPoints)
{
  int min = 0, max = binPoints.size() - 1;
  while(min < max)
  {
    int middle = (min + max) / 2;
    if(nValue > binPoints[middle])
    {
      min = middle + 1;
    }
    else
    {
      max = middle;
    }
  }
  return min;
}

/**
 * @brief The GenerateColorTableImpl class implements a threaded algorithm that computes the RGB values
 * for each element in a given array of data
 */
template <typename T>
class GenerateColorTableImpl
{
public:
  GenerateColorTableImpl(typename DataArray<T>::Pointer arrayPtr, std::vector<float> binPoints, std::vector<std::vector<double>> controlPoints, int numControlColors,
                         UInt8ArrayType::Pointer colorArray)
  : m_ArrayPtr(arrayPtr)
  , m_BinPoints(std::move(binPoints))
  , m_NumControlColors(numControlColors)
  , m_ControlPoints(std::move(controlPoints))
  , m_ColorArray(std::move(colorArray))
  {
    m_ArrayMin = arrayPtr->getValue(0);
    m_ArrayMax = arrayPtr->getValue(0);
    for(int i = 1; i < arrayPtr->getNumberOfTuples(); i++)
    {
      if(arrayPtr->getValue(i) < m_ArrayMin)
      {
        m_ArrayMin = arrayPtr->getValue(i);
      }
      if(arrayPtr->getValue(i) > m_ArrayMax)
      {
        m_ArrayMax = arrayPtr->getValue(i);
      }
    }
  }
  virtual ~GenerateColorTableImpl() = default;

  void convert(size_t start, size_t end) const
  {
    for(size_t i = start; i < end; i++)
    {
      // Normalize value
      float nValue = (static_cast<float>(m_ArrayPtr->getValue(i) - m_ArrayMin)) / static_cast<float>((m_ArrayMax - m_ArrayMin));

      int rightBinIndex = findRightBinIndex_Binary(nValue, m_BinPoints);

      int leftBinIndex = rightBinIndex - 1;
      if(leftBinIndex < 0)
      {
        leftBinIndex = 0;
        rightBinIndex = 1;
      }

      // Find the fractional distance traveled between the beginning and end of the current color bin
      float currFraction = 0.0f;
      if(rightBinIndex < m_BinPoints.size())
      {
        currFraction = (nValue - m_BinPoints[leftBinIndex]) / (m_BinPoints[rightBinIndex] - m_BinPoints[leftBinIndex]);
      }
      else
      {
        currFraction = (nValue - m_BinPoints[leftBinIndex]) / (1 - m_BinPoints[leftBinIndex]);
      }

      // If the current color bin index is larger than the total number of control colors, automatically set the currentBinIndex
      // to the last control color.
      if(leftBinIndex > m_NumControlColors - 1)
      {
        leftBinIndex = m_NumControlColors - 1;
      }

      // Calculate the RGB values
      unsigned char r = (m_ControlPoints[leftBinIndex][1] * (1.0 - currFraction) + m_ControlPoints[rightBinIndex][1] * currFraction) * 255;
      unsigned char g = (m_ControlPoints[leftBinIndex][2] * (1.0 - currFraction) + m_ControlPoints[rightBinIndex][2] * currFraction) * 255;
      unsigned char b = (m_ControlPoints[leftBinIndex][3] * (1.0 - currFraction) + m_ControlPoints[rightBinIndex][3] * currFraction) * 255;

      m_ColorArray->setComponent(i, 0, r);
      m_ColorArray->setComponent(i, 1, g);
      m_ColorArray->setComponent(i, 2, b);
    }
  }

  void operator()(const SIMPLRange& r) const
  {
    convert(r.min(), r.max());
  }

private:
  typename DataArray<T>::Pointer m_ArrayPtr;
  std::vector<float> m_BinPoints;
  T m_ArrayMin;
  T m_ArrayMax;
  int m_NumControlColors;
  std::vector<std::vector<double>> m_ControlPoints;
  UInt8ArrayType::Pointer m_ColorArray;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void generateColorArray(typename DataArray<T>::Pointer arrayPtr, QJsonArray presetControlPoints, DataArrayPath selectedDAP, QString rgbArrayName, DataContainerArray::Pointer dca)
{
  if(arrayPtr->getNumberOfTuples() <= 0)
  {
    return;
  }

  if(presetControlPoints.empty())
  {
    return;
  }

  int numControlColors = presetControlPoints.count() / 4;
  int numComponents = 4;
  std::vector<std::vector<double>> controlPoints(numControlColors, std::vector<double>(numComponents));

  // Migrate colorControlPoints values from QJsonArray to 2D array.  Store A-values in binPoints vector.
  std::vector<float> binPoints;
  for(int i = 0; i < numControlColors; i++)
  {
    for(int j = 0; j < numComponents; j++)
    {
      controlPoints[i][j] = static_cast<float>(presetControlPoints[numComponents * i + j].toDouble());
      if(j == 0)
      {
        binPoints.push_back(controlPoints[i][j]);
      }
    }
  }

  // Normalize binPoints values
  float binMin = binPoints[0];
  float binMax = binPoints[binPoints.size() - 1];
  for(int i = 0; i < binPoints.size(); i++)
  {
    binPoints[i] = (binPoints[i] - binMin) / (binMax - binMin);
  }

  DataArrayPath tmpPath = selectedDAP;
  tmpPath.setDataArrayName(rgbArrayName);

  UInt8ArrayType::Pointer colorArray = dca->getPrereqArrayFromPath<UInt8ArrayType>(nullptr, tmpPath, std::vector<size_t>(1, 3));
  if(colorArray.get() == nullptr)
  {
    return;
  }

  ParallelDataAlgorithm dataAlg;
  dataAlg.setRange(0, arrayPtr->getNumberOfTuples());
  dataAlg.execute(GenerateColorTableImpl<T>(arrayPtr, binPoints, controlPoints, numControlColors, colorArray));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateColorTable::GenerateColorTable()
: m_SelectedPresetName("")
, m_SelectedPresetControlPoints(QJsonArray())
, m_SelectedDataArrayPath(DataArrayPath("", "", ""))
, m_RgbArrayName("")
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateColorTable::~GenerateColorTable() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    GenerateColorTableFilterParameter::Pointer parameter = GenerateColorTableFilterParameter::New();
    parameter->setHumanLabel("Select Preset...");
    parameter->setCategory(FilterParameter::Parameter);
    parameter->setPropertyName("SelectedPresetName");
    parameter->setFilter(this);
    parameters.push_back(parameter);
  }

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Data Array", SelectedDataArrayPath, FilterParameter::RequiredArray, GenerateColorTable, req));
  }

  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("RGB Array Name", RgbArrayName, SelectedDataArrayPath, SelectedDataArrayPath, FilterParameter::CreatedArray, GenerateColorTable));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqIDataArrayFromPath(this, getSelectedDataArrayPath());

  DataArrayPath tmpPath = getSelectedDataArrayPath();
  tmpPath.setDataArrayName(getRgbArrayName());

  getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, tmpPath, 0, std::vector<size_t>(1, 3), "", ColorArrayID);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  if(getDataContainerArray()->getPrereqArrayFromPath<Int8ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    Int8ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int8ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<int8_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    UInt8ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<uint8_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    Int16ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<int16_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    UInt16ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<uint16_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    Int32ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<int32_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    UInt32ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<uint32_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    Int64ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<int64_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    UInt64ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<uint64_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    DoubleArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<double>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    FloatArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<float>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if(getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType>(nullptr, getSelectedDataArrayPath(), {static_cast<size_t>(1)}).get() != nullptr)
  {
    BoolArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType>(this, getSelectedDataArrayPath(), {static_cast<size_t>(1)});
    generateColorArray<bool>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else
  {
    QString ss = QObject::tr("The selected array '%1' does not have a compatible type.").arg(getSelectedDataArrayPath().getDataArrayName());
    setErrorCondition(-10000, ss);
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateColorTable::newFilterInstance(bool copyFilterParameters) const
{
  GenerateColorTable::Pointer filter = GenerateColorTable::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateColorTable::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateColorTable::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateColorTable::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateColorTable::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid GenerateColorTable::getUuid() const
{
  return QUuid("{0d0a6535-6565-51c5-a3fc-fbc00008606d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateColorTable::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ImageFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenerateColorTable::getHumanLabel() const
{
  return "Generate Color Table";
}

// -----------------------------------------------------------------------------
GenerateColorTable::Pointer GenerateColorTable::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<GenerateColorTable> GenerateColorTable::New()
{
  struct make_shared_enabler : public GenerateColorTable
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString GenerateColorTable::getNameOfClass() const
{
  return QString("GenerateColorTable");
}

// -----------------------------------------------------------------------------
QString GenerateColorTable::ClassName()
{
  return QString("GenerateColorTable");
}

// -----------------------------------------------------------------------------
void GenerateColorTable::setSelectedPresetName(const QString& value)
{
  m_SelectedPresetName = value;
}

// -----------------------------------------------------------------------------
QString GenerateColorTable::getSelectedPresetName() const
{
  return m_SelectedPresetName;
}

// -----------------------------------------------------------------------------
void GenerateColorTable::setSelectedPresetControlPoints(const QJsonArray& value)
{
  m_SelectedPresetControlPoints = value;
}

// -----------------------------------------------------------------------------
QJsonArray GenerateColorTable::getSelectedPresetControlPoints() const
{
  return m_SelectedPresetControlPoints;
}

// -----------------------------------------------------------------------------
void GenerateColorTable::setSelectedDataArrayPath(const DataArrayPath& value)
{
  m_SelectedDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath GenerateColorTable::getSelectedDataArrayPath() const
{
  return m_SelectedDataArrayPath;
}

// -----------------------------------------------------------------------------
void GenerateColorTable::setRgbArrayName(const QString& value)
{
  m_RgbArrayName = value;
}

// -----------------------------------------------------------------------------
QString GenerateColorTable::getRgbArrayName() const
{
  return m_RgbArrayName;
}
