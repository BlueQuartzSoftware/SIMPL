# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

set(_filterGroupName CoreFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

SIMPL_START_FILTER_GROUP(
  ALL_FILTERS_HEADERFILE ${AllFiltersHeaderFile}
  REGISTER_KNOWN_FILTERS_FILE ${RegisterKnownFiltersFile}
  FILTER_GROUP "${_filterGroupName}"
  BINARY_DIR ${SIMPLProj_BINARY_DIR}
  )

#---------
# List your public filters here

set(_PublicFilters
  ArrayCalculator
  CombineAttributeArrays
  CombineAttributeMatrices
  ConditionalSetValue
  ConvertData
  CopyFeatureArrayToElementArray
  CreateAttributeMatrix
  CreateDataContainer
  CreateDataArray
  CreateFeatureArrayFromElementArray
  CreateImageGeometry
  CreateStringArray
  DataContainerReader
  DataContainerWriter
  ExecuteProcess
  ExtractComponentAsArray
  FeatureCountDecision
  FeatureDataCSVWriter
  FindDerivatives
  GenerateColorTable
  ImportAsciDataArray
  LinkFeatureMapToElementArray
  MaskCountDecision
  MoveData
  PostSlackMessage
  RawBinaryReader
  ReadASCIIData
  RemoveArrays
  RemoveComponentFromArray
  RenameAttributeArray
  RenameAttributeMatrix
  RenameDataContainer
  ReplaceValueInArray
  RequiredZThickness
  ScaleVolume
  SetOriginResolutionImageGeom
  WriteASCIIData
  WriteTriangleGeometry
)



list(LENGTH _PublicFilters PluginNumFilters)
set_property(GLOBAL PROPERTY PluginNumFilters ${PluginNumFilters})

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "SIMPLib" "FilterWidgetsLib"
                        ${_filterGroupName} ${f}
                        ${SIMPLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md TRUE
                        ${SIMPLProj_BINARY_DIR})
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  Breakpoint
  EmptyFilter
  FileReader
  FileWriter
)

#-----------------
# Loop on the Private Filters adding each one to the SIMPLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "SIMPLib" "FilterWidgetsLib"
                        ${_filterGroupName} ${f}
                        ${SIMPLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE
                        ${SIMPLProj_BINARY_DIR})
endforeach()

source_group("${SIMPLib_SOURCE_DIR} ${_filterGroupName} util" FILES ${HEADERS} ${SOURCES})

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util AbstractDataParser.hpp)
ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ASCIIWizardData.hpp)
ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ParserFunctors.hpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorItem.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorItem.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ICalculatorArray.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ICalculatorArray.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorArray.hpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util UnaryOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util UnaryOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util BinaryOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util BinaryOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util IndexOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util IndexOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ABSOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ABSOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util PowOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util PowOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ExpOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ExpOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util SinOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util SinOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CosOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CosOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util TanOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util TanOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ASinOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ASinOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ACosOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ACosOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ATanOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util ATanOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util SqrtOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util SqrtOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util RootOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util RootOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util LogOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util LogOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util Log10Operator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util Log10Operator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util LnOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util LnOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util AdditionOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util AdditionOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util SubtractionOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util SubtractionOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util NegativeOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util NegativeOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util MultiplicationOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util MultiplicationOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util DivisionOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util DivisionOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util LeftParenthesisItem.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util LeftParenthesisItem.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util RightParenthesisItem.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util RightParenthesisItem.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorSeparator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CalculatorSeparator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CommaSeparator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CommaSeparator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util FloorOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util FloorOperator.cpp)

ADD_SIMPL_SUPPORT_HEADER(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CeilOperator.h)
ADD_SIMPL_SUPPORT_SOURCE(${SIMPLib_SOURCE_DIR} ${_filterGroupName}/util CeilOperator.cpp)

# -- Add the binary directory for this subdirectory to the include path which is where the moc files are generated
include_directories( ${SIMPLib_BINARY_DIR}/${_filterGroupName})

# -- Run MOC and UIC on the necessary files
QT5_ADD_RESOURCES( SIMPLib_CoreFilters_Generated_RCS_SRCS "${SIMPLib_SOURCE_DIR}/CoreFilters/CoreResources.qrc"  )
set(Project_SRCS ${Project_SRCS} ${SIMPLib_CoreFilters_Generated_RCS_SRCS})

#-----------------
# This makes sure some Filter Group meta data is written to build files that is needed later
SIMPL_END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${_filterGroupName}" "Generic Filters")
