set(_filterGroupName CoreFilters)
set(SUBDIR_NAME ${_filterGroupName})
set(${_filterGroupName}_FILTERS_HDRS "")
set(PLUGIN_NAME Core)
get_filename_component(${PLUGIN_NAME}_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
set(${PLUGIN_NAME}_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR} CACHE STRING "")

# add_custom_target(${PLUGIN_NAME}_DOC_FOLDER_COPY ALL
#   COMMAND ${CMAKE_COMMAND} -E copy_directory "${SIMPLib_SOURCE_DIR}/Documentation/SIMPLibFilters/${PLUGIN_NAME}Filters/"
#   "${DREAM3DDocRoot}/ReferenceManual/Plugins/${PLUGIN_NAME}/"
#   COMMENT "${PLUGIN_NAME}: Copying Documentation to build directory")
# set_target_properties(${PLUGIN_NAME}_DOC_FOLDER_COPY PROPERTIES FOLDER ZZ_COPY_FILES)

#--------
# This macro must come first before we start adding any filters
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
  CreateDataArray
  CreateDataContainer
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
  ImportHDF5Dataset
  LinkFeatureMapToElementArray
  MaskCountDecision
  MoveData
  MoveMultiData
  MultiThresholdObjects
  MultiThresholdObjects2
  PipelineAnnotation
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

# -- Run MOC and UIC on the necessary files
QT5_ADD_RESOURCES( SIMPLib_CoreFilters_Generated_RCS_SRCS "${SIMPLib_SOURCE_DIR}/CoreFilters/CoreResources.qrc"  )
foreach(h ${SIMPLib_CoreFilters_Generated_RCS_SRCS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

set(Project_SRCS ${Project_SRCS} ${SIMPLib_CoreFilters_Generated_RCS_SRCS})

#-----------------
# This makes sure some Filter Group meta data is written to build files that is needed later
SIMPL_END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${_filterGroupName}" "Core Filters")


#-------------------------------------------------------------------------------
# Add the unit testing sources
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()

