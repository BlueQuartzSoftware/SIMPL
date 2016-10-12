/*
 * Your License or Copyright can go here
 */

#include "GenerateColorTable.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/GenerateColorTableFilterParameter.h"
#include "SIMPLib/Utilities/ColorTable.h"
#include "SIMPLib/SIMPLibVersion.h"

// Include the MOC generated file for this class
#include "moc_GenerateColorTable.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateColorTable::GenerateColorTable() :
  AbstractFilter(),
  m_SelectedPresetName(""),
  m_SelectedPresetControlPoints(QJsonArray()),
  m_SelectedDataArrayPath(DataArrayPath("", "", "")),
  m_RGB_ArrayName("")
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenerateColorTable::~GenerateColorTable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::initialize()
{
  setErrorCondition(0);
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
    parameter->setFilter(this);
    parameters.push_back(parameter);
  }

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Data Array", SelectedDataArrayPath, FilterParameter::RequiredArray, GenerateColorTable, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("RGB Array Name", RGB_ArrayName, FilterParameter::CreatedArray, GenerateColorTable));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateColorTable::dataCheck()
{
  setErrorCondition(0);

  getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedDataArrayPath());

  DataArrayPath tmpPath = getSelectedDataArrayPath();
  tmpPath.setDataArrayName(getRGB_ArrayName());

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
    generateColorArray<int8_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt8ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint8_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int16ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int16ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int16_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt16ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt16ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint16_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int32ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int32ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int32_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt32ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint32_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    Int64ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<Int64ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<int64_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    UInt64ArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<UInt64ArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<uint64_t>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    DoubleArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<double>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    FloatArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<float>(ptr);
  }
  else if (getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType, AbstractFilter>(nullptr, getSelectedDataArrayPath(), QVector<size_t>(1, 1)).get() != nullptr)
  {
    BoolArrayType::Pointer ptr = getDataContainerArray()->getPrereqArrayFromPath<BoolArrayType, AbstractFilter>(this, getSelectedDataArrayPath(), QVector<size_t>(1, 1));
    generateColorArray<bool>(ptr);
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
template <typename T>
void GenerateColorTable::generateColorArray(typename DataArray<T>::Pointer arrayPtr)
{  
  if (arrayPtr->getNumberOfTuples() <= 0) { return; }

  /* For each item in the data array:
   * 1. Normalize the value to between 0 and 1
   * 2. Figure out which bin it is located in
   * 3. Find the location of the value within that bin
   * 4. Calculate the RGB value
   * 5. Add RGB value to colorArray array
  */

  QJsonArray presetControlPoints = getSelectedPresetControlPoints();
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

  T min = arrayPtr->getValue(0);
  T max = arrayPtr->getValue(0);
  for (int i=1; i<arrayPtr->getNumberOfTuples(); i++)
  {
    if (arrayPtr->getValue(i) < min) { min = arrayPtr->getValue(i); }
    if (arrayPtr->getValue(i) > max) { max = arrayPtr->getValue(i); }
  }

  DataArrayPath tmpPath = getSelectedDataArrayPath();
  tmpPath.setDataArrayName(getRGB_ArrayName());

  UInt8ArrayType::Pointer colorArray = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, tmpPath, QVector<size_t>(1, 3));
  if (getErrorCondition() < 0) { return; }

  for (int i=0; i<arrayPtr->getNumberOfTuples(); i++)
  {
    // Normalize value
    T nValue = (arrayPtr->getValue(i) - min) / (max - min);

    int rightBinIndex = findRightBinIndex_Binary<T>(nValue, binPoints);

    int leftBinIndex = rightBinIndex - 1;
    if (leftBinIndex < 0)
    {
      leftBinIndex = 0;
      rightBinIndex = 1;
    }

    // Find the fractional distance traveled between the beginning and end of the current color bin
    float currFraction = 0.0f;
    if (rightBinIndex < binPoints.size())
    {
      currFraction = (nValue - binPoints[leftBinIndex]) / (binPoints[rightBinIndex] - binPoints[leftBinIndex]);
    }
    else
    {
      currFraction = (nValue - binPoints[leftBinIndex]) / (1 - binPoints[leftBinIndex]);
    }

    // If the current color bin index is larger than the total number of control colors, automatically set the currentBinIndex
    // to the last control color.
    if(leftBinIndex > numControlColors - 1)
    {
      leftBinIndex = numControlColors - 1;
    }

    // Calculate the RGB values
    unsigned char r = (controlPoints[leftBinIndex][1] * (1.0 - currFraction) + controlPoints[rightBinIndex][1] * currFraction) * 255;
    unsigned char g = (controlPoints[leftBinIndex][2] * (1.0 - currFraction) + controlPoints[rightBinIndex][2] * currFraction) * 255;
    unsigned char b = (controlPoints[leftBinIndex][3] * (1.0 - currFraction) + controlPoints[rightBinIndex][3] * currFraction) * 255;

    colorArray->setComponent(i, 0, r);
    colorArray->setComponent(i, 1, g);
    colorArray->setComponent(i, 2, b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
int GenerateColorTable::findRightBinIndex(T nValue, QVector<float> binPoints)
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
template <typename T>
int GenerateColorTable::findRightBinIndex_Binary(T nValue, QVector<float> binPoints)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenerateColorTable::newFilterInstance(bool copyFilterParameters)
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
const QString GenerateColorTable::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getGroupName()
{ return SIMPL::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getSubGroupName()
{ return SIMPL::FilterSubGroups::ImageFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GenerateColorTable::getHumanLabel()
{ return "Generate Color Table"; }

