/*
 * Your License or Copyright can go here
 */

#include "GenerateColorTable.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/GenerateColorTableFilterParameter.h"
#include "SIMPLib/Utilities/ColorTable.h"
#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
int findRightBinIndex(T nValue, QVector<float> binPoints)
{
  /* This is a brute-force way of finding the proper bins.
     We will need to change this method to a binary search. */
  int rightBinIndex = 0;
  while (binPoints[rightBinIndex] < nValue)
  {
    rightBinIndex++;
  }

  return rightBinIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int findRightBinIndex_Binary(float nValue, QVector<float> binPoints)
{
  int min = 0, max = binPoints.size() - 1;
  while (min < max)
  {
    int middle = (min + max) / 2;
    if (nValue > binPoints[middle])
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
  GenerateColorTableImpl(typename DataArray<T>::Pointer arrayPtr, QVector<float> binPoints,
                         std::vector<std::vector<double> > controlPoints, int numControlColors, UInt8ArrayType::Pointer colorArray) :
    m_ArrayPtr(arrayPtr),
    m_BinPoints(binPoints),
    m_NumControlColors(numControlColors),
    m_ControlPoints(controlPoints),
    m_ColorArray(colorArray)
  {
    m_ArrayMin = arrayPtr->getValue(0);
    m_ArrayMax = arrayPtr->getValue(0);
    for (int i=1; i<arrayPtr->getNumberOfTuples(); i++)
    {
      if (arrayPtr->getValue(i) < m_ArrayMin) { m_ArrayMin = arrayPtr->getValue(i); }
      if (arrayPtr->getValue(i) > m_ArrayMax) { m_ArrayMax = arrayPtr->getValue(i); }
    }
  }
  virtual ~GenerateColorTableImpl() = default;

  void convert(size_t start, size_t end) const
  {
    for (size_t i = start; i < end; i++)
    {
      // Normalize value
      float nValue = (static_cast<float>(m_ArrayPtr->getValue(i) - m_ArrayMin)) / static_cast<float>((m_ArrayMax - m_ArrayMin));

      int rightBinIndex = findRightBinIndex_Binary(nValue, m_BinPoints);

      int leftBinIndex = rightBinIndex - 1;
      if (leftBinIndex < 0)
      {
        leftBinIndex = 0;
        rightBinIndex = 1;
      }

      // Find the fractional distance traveled between the beginning and end of the current color bin
      float currFraction = 0.0f;
      if (rightBinIndex < m_BinPoints.size())
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

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  void operator()(const tbb::blocked_range<size_t>& r) const
  {
    convert(r.begin(), r.end());
  }
#endif
private:
  typename DataArray<T>::Pointer                        m_ArrayPtr;
  QVector<float>                                        m_BinPoints;
  T                                                     m_ArrayMin;
  T                                                     m_ArrayMax;
  int                                                   m_NumControlColors;
  std::vector<std::vector<double> >                     m_ControlPoints;
  UInt8ArrayType::Pointer                               m_ColorArray;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void generateColorArray(typename DataArray<T>::Pointer arrayPtr, QJsonArray presetControlPoints, DataArrayPath selectedDAP, QString rgbArrayName, DataContainerArray::Pointer dca)
{
  if (arrayPtr->getNumberOfTuples() <= 0) { return; }

  if (presetControlPoints.size() <= 0) { return; }

  int numControlColors = presetControlPoints.count() / 4;
  int numComponents = 4;
  std::vector<std::vector<double> > controlPoints(numControlColors, std::vector<double>(numComponents));

  // Migrate colorControlPoints values from QJsonArray to 2D array.  Store A-values in binPoints vector.
  QVector<float> binPoints;
  for (int i=0; i<numControlColors; i++)
  {
    for (int j=0; j<numComponents; j++)
    {
      controlPoints[i][j] = static_cast<float>(presetControlPoints[numComponents*i + j].toDouble());
      if (j == 0)
      {
        binPoints.push_back(controlPoints[i][j]);
      }
    }
  }

  // Normalize binPoints values
  float binMin = binPoints[0];
  float binMax = binPoints[binPoints.size() - 1];
  for (int i=0; i<binPoints.size(); i++)
  {
    binPoints[i] = (binPoints[i] - binMin) / (binMax - binMin);
  }

  DataArrayPath tmpPath = selectedDAP;
  tmpPath.setDataArrayName(rgbArrayName);

  UInt8ArrayType::Pointer colorArray = dca->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(nullptr, tmpPath, QVector<size_t>(1, 3));
  if (colorArray.get() == nullptr) { return; }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, arrayPtr->getNumberOfTuples()), GenerateColorTableImpl<T>(arrayPtr, binPoints, controlPoints, numControlColors, colorArray),
                      tbb::auto_partitioner());
  }
  else
#endif
  {
    GenerateColorTableImpl<T> serial(arrayPtr, binPoints, controlPoints, numControlColors, colorArray);
    serial.convert(0, arrayPtr->getNumberOfTuples());
  }
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
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::setupFilterParameters()
{
  FilterParameterVector parameters;

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

  parameters.push_back(SIMPL_NEW_STRING_FP("RGB Array Name", RgbArrayName, FilterParameter::CreatedArray, GenerateColorTable));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedDataArrayPath());

  DataArrayPath tmpPath = getSelectedDataArrayPath();
  tmpPath.setDataArrayName(getRgbArrayName());

  getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tmpPath, 0, QVector<size_t>(1, 3));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (getDataContainerArray()->getPrereqArrayFromPath<Int8ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int8ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int8ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int8_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt8ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint8_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int16ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int16_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt16ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint16_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int32ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int32_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt32ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint32_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int64ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int64_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt64ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint64_t>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    DoubleArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<double>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    FloatArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<float>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    BoolArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<bool>(ptr, getSelectedPresetControlPoints(), getSelectedDataArrayPath(), getRgbArrayName(), getDataContainerArray());
  }
  else
  {
    QString ss = QObject::tr("The selected array '%1' does not have a compatible type.").arg(getSelectedDataArrayPath().getDataArrayName());
    setErrorCondition(-10000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateColorTable::newFilterInstance(bool copyFilterParameters) const
{
  GenerateColorTable::Pointer filter = GenerateColorTable::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getCompiledLibraryName() const
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getGroupName() const
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid GenerateColorTable::getUuid()
{
  return QUuid("{0d0a6535-6565-51c5-a3fc-fbc00008606d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getSubGroupName() const
{ return SIMPL::FilterSubGroups::ImageFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getHumanLabel() const
{ return "Generate Color Table"; }

