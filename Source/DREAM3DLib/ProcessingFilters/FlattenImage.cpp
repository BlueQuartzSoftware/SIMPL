/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FlattenImage.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Math/DREAM3DMath.h"

class FlattenImageImpl
{

  public:
    FlattenImageImpl(unsigned char* data, int32_t* newdata, float Rfactor, float Gfactor, float Bfactor, size_t comp) :
      m_ImageData(data),
      m_FlatImageData(newdata),
      convRFactor(Rfactor),
      convGFactor(Gfactor),
      convBFactor(Bfactor),
      numComp(comp)
    {}
    virtual ~FlattenImageImpl() {}

    void convert(size_t start, size_t end) const
    {
      for (size_t i = start; i < end; i++)
      {
        m_FlatImageData[i] = int((m_ImageData[numComp * i] * convRFactor) + (m_ImageData[numComp * i + 1] * convGFactor) + (m_ImageData[numComp * i + 2] * convBFactor));
      }
    }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
      convert(r.begin(), r.end());
    }
#endif
  private:
    unsigned char* m_ImageData;
    int32_t* m_FlatImageData;
    float  convRFactor;
    float  convGFactor;
    float  convBFactor;
    size_t numComp;

};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FlattenImage::FlattenImage() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_FlattenMethod(DREAM3D::FlattenImageMethod::Luminosity),
  m_ImageDataArrayName(DREAM3D::CellData::ImageData),
  m_ImageData(NULL),
  m_FlatImageDataArrayName(DREAM3D::CellData::FlatImageData),
  m_FlatImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FlattenImage::~FlattenImage()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Flattening Method");
    option->setPropertyName("FlattenMethod");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Lightness");
    choices.push_back("Average");
    choices.push_back("Luminosity");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setFlattenMethod( reader->readValue("FlattenMethod", getFlattenMethod()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FlattenImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("FlattenMethod", getFlattenMethod() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, FlattenImage>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  int numImageComp = 1;
  IDataArray::Pointer iDataArray = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_ImageDataArrayName);
  if(NULL != iDataArray.get())
  {
    UInt8ArrayType* imageDataPtr = UInt8ArrayType::SafePointerDownCast(iDataArray.get());
    if (NULL != imageDataPtr)
    {
      numImageComp = imageDataPtr->getNumberOfComponents();
    }
  }

  QVector<size_t> dims(1, numImageComp);
  m_ImageDataPtr = cellAttrMat->getPrereqArray<DataArray<unsigned char>, AbstractFilter>(this, m_ImageDataArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_ImageData = m_ImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  dims[0] = 1;
  m_FlatImageDataPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_FlatImageDataArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FlatImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FlatImageData = m_FlatImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::preflight()
{
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FlattenImage::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  float Rfactor = 1.0;
  float Gfactor = 1.0;
  float Bfactor = 1.0;
  if (m_FlattenMethod == DREAM3D::FlattenImageMethod::Average)
  {
    Rfactor = 1.0 / 3.0;
    Gfactor = 1.0 / 3.0;
    Bfactor = 1.0 / 3.0;
  }
  else if (m_FlattenMethod == DREAM3D::FlattenImageMethod::Luminosity)
  {
    Rfactor = 0.21;
    Gfactor = 0.72;
    Bfactor = 0.07;
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

  size_t comp = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(m_ImageDataArrayName)->getNumberOfComponents();

  //  qDebug() << "FlattenImage: " << m_ConversionFactor << "\n";
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                      FlattenImageImpl(m_ImageData, m_FlatImageData, Rfactor, Gfactor, Bfactor, comp), tbb::auto_partitioner());

  }
  else
#endif
  {
    FlattenImageImpl serial(m_ImageData, m_FlatImageData, Rfactor, Gfactor, Bfactor, comp);
    serial.convert(0, totalPoints);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

