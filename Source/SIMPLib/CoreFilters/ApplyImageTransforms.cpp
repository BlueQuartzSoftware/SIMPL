/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ApplyImageTransforms.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/MultiDataContainerSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/TransformContainer.h"

#include "SIMPLib/SIMPLibVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApplyImageTransforms::ApplyImageTransforms()
: AbstractFilter()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ApplyImageTransforms::~ApplyImageTransforms() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplyImageTransforms::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplyImageTransforms::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  {
    MultiDataContainerSelectionFilterParameter::RequirementType req =
        MultiDataContainerSelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_MDC_SELECTION_FP("Image Data Containers", ImageDataContainers, FilterParameter::RequiredArray, ApplyImageTransforms, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplyImageTransforms::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  int selectedDCCount = getImageDataContainers().size();
  if(selectedDCCount < 1)
  {
    QString ss = QObject::tr("At least one Data Container must be selected");
    setErrorCondition(-11001, ss);
    return;
  }

  for(int i = 0; i < selectedDCCount; i++)
  {
    QString dcName = m_ImageDataContainers[i];

    ImageGeom::Pointer imageGeom = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, dcName);
    if(getErrorCode() < 0)
    {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplyImageTransforms::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ApplyImageTransforms::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  int selectedDCCount = getImageDataContainers().size();
  for(int i = 0; i < selectedDCCount; i++)
  {
    QString dcName = m_ImageDataContainers[i];

    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
    ImageGeom::Pointer imageGeom = dc->getGeometryAs<ImageGeom>();

    FloatVec3Type origin;
    imageGeom->getOrigin(origin);

    ::ITransformContainer::Pointer iTransformContainer = imageGeom->getTransformContainer();
    if(iTransformContainer)
    {
      TransformContainer::Pointer transformContainer = std::dynamic_pointer_cast<TransformContainer>(iTransformContainer);
      if(transformContainer)
      {
        TransformContainer::TransformParametersType parameters = transformContainer->getParameters();

        double xTranslation = parameters[9];
        double yTranslation = parameters[10];

        origin[0] = origin[0] - xTranslation;
        origin[1] = origin[1] - yTranslation;

        imageGeom->setOrigin(origin);

        parameters[9] = 0;
        parameters[10] = 0;
        transformContainer->setParameters(parameters);
      }
    }
  }

  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ApplyImageTransforms::newFilterInstance(bool copyFilterParameters) const
{
  ApplyImageTransforms::Pointer filter = ApplyImageTransforms::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ApplyImageTransforms::getCompiledLibraryName() const
{
  return Core::CoreBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ApplyImageTransforms::getBrandingString() const
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ApplyImageTransforms::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ApplyImageTransforms::getGroupName() const
{
  return SIMPL::FilterGroups::CoreFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ApplyImageTransforms::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ImageFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ApplyImageTransforms::getHumanLabel() const
{
  return "Apply Image Transforms";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ApplyImageTransforms::getUuid()
{
  return QUuid("{a4bd18bc-3306-5e41-bc41-4cb4248226bd}");
}
