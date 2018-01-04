set(_filterGroupName TestFilters)
set(SUBDIR_NAME ${_filterGroupName})
set(${_filterGroupName}_FILTERS_HDRS "")
set(PLUGIN_NAME Test)
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
  ThresholdExample
  ArraySelectionExample
  GenericExample
  DynamicTableExample
  MakeDataContainer
  ErrorWarningFilter
  FilterGroup01
  FilterGroup02
  FilterGroup03
  FilterGroup04
  FilterGroup05
  FilterGroup06
  FilterGroup07
  FilterGroup08
  FilterGroup09
  FilterGroup10
  FilterGroup12
  FilterGroup13
)



list(LENGTH _PublicFilters PluginNumFilters)
set_property(GLOBAL PROPERTY PluginNumFilters ${PluginNumFilters})

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "SIMPLib" "FilterWidgetsLib"
                        ${_filterGroupName} ${f}
                        ${SIMPLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  TestFilters
  ChangeGlobalValue
)

#-----------------
# Loop on the Private Filters adding each one to the SIMPLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "SIMPLib" "FilterWidgetsLib"
                        ${_filterGroupName} ${f}
                        ${SIMPLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()

#---------------------
# This macro must come last after we are done adding all the filters and support files.
SIMPL_END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${_filterGroupName}" "TestFilters")


#-------------------------------------------------------------------------------
# Add the unit testing sources
if(SIMPL_BUILD_TESTING)
  include(${SIMPLib_SOURCE_DIR}/${SUBDIR_NAME}/Testing/Cxx/SourceList.cmake)
endif()
