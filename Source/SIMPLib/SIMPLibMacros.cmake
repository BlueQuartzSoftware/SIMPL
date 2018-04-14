#-------------------------------------------------------------------------------
# Add Unit Test for Plugins and Filters
# 
function(SIMPL_GenerateUnitTestFile)
  set(options)
  set(oneValueArgs PLUGIN_NAME TEST_DATA_DIR)
  set(multiValueArgs SOURCES LINK_LIBRARIES INCLUDE_DIRS EXTRA_SOURCES)
  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(TEST_TEMP_DIR ${${P_PLUGIN_NAME}_BINARY_DIR}/Test/Temp)
  # Make sure the directory is created during CMake time
  file(MAKE_DIRECTORY ${TEST_TEMP_DIR})
  
  set(TESTFILES_SOURCE_DIR "${${P_PLUGIN_NAME}_SOURCE_DIR}/Test/TestFiles")


  configure_file(${${P_PLUGIN_NAME}Test_SOURCE_DIR}/TestFileLocations.h.in
                 ${${P_PLUGIN_NAME}Test_BINARY_DIR}/${P_PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE)

  configure_file(${SIMPLProj_SOURCE_DIR}/Resources/UnitTestSupport.hpp
                 ${${P_PLUGIN_NAME}Test_BINARY_DIR}/UnitTestSupport.hpp COPYONLY IMMEDIATE)


  set( ${P_PLUGIN_NAME}_TEST_SRCS )
  set(FilterTestIncludes "")
  set(TestMainFunctors "")

  foreach(name  ${TEST_NAMES})
    set( ${P_PLUGIN_NAME}_TEST_SRCS
      ${${P_PLUGIN_NAME}_TEST_SRCS}
      "${${P_PLUGIN_NAME}_SOURCE_DIR}/Test/${name}.cpp"
      )
    string(CONCAT
      FilterTestIncludes
      ${FilterTestIncludes}
      "#include \"${${P_PLUGIN_NAME}_SOURCE_DIR}/Test/${name}.cpp\"\n"
      )

    string(CONCAT
      TestMainFunctors
     ${TestMainFunctors}
     "  ${name}()()|\n")
  endforeach()

  if(NOT "${TestMainFunctors}" STREQUAL "")
    STRING(REPLACE "|" ";" TestMainFunctors ${TestMainFunctors}   )
  endif()

  configure_file(${SIMPLProj_SOURCE_DIR}/Source/SIMPLib/Testing/TestMain.cpp.in
                 ${${P_PLUGIN_NAME}Test_BINARY_DIR}/${P_PLUGIN_NAME}UnitTest.cpp @ONLY)


  # Set the source files properties on each source file.
  foreach(f ${${P_PLUGIN_NAME}_TEST_SRCS})
    set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
  endforeach()


  AddSIMPLUnitTest(TESTNAME ${P_PLUGIN_NAME}UnitTest
    SOURCES 
      ${${P_PLUGIN_NAME}Test_BINARY_DIR}/${P_PLUGIN_NAME}UnitTest.cpp 
      ${${P_PLUGIN_NAME}_TEST_SRCS}
      ${P_EXTRA_SOURCES}
    FOLDER "${P_PLUGIN_NAME}Plugin/Test"
    LINK_LIBRARIES ${P_LINK_LIBRARIES}
    INCLUDE_DIRS ${P_INCLUDE_DIRS}
    )

  if(MSVC)
    set_source_files_properties(${${P_PLUGIN_NAME}Test_BINARY_DIR}/${P_PLUGIN_NAME}UnitTest.cpp PROPERTIES COMPILE_FLAGS /bigobj)
  endif()

  set(AllFilters ${_PublicFilters} ${_PrivateFilters})
  list(LENGTH AllFilters totalFilters)
  list(LENGTH P_SOURCES totalTests)
  set(numTests "0")

  file(APPEND ${UnitTestAnalysisFile} "\n## ${P_PLUGIN_NAME} ##\n\n")
  file(APPEND ${UnitTestAnalysisFile} "| Filter Name | Unit Test |\n")
  file(APPEND ${UnitTestAnalysisFile} "| ----------- | --------- |\n")
  foreach(ut ${AllFilters})
    list(FIND P_SOURCES ${ut}Test TestFound)
    set(MDTestFound "TRUE")
    if(TestFound STREQUAL "-1")
      set(MDTestFound "**FALSE**")
    else()
      list(REMOVE_ITEM P_SOURCES ${ut}Test)
      math(EXPR numTests "${numTests} + 1")
    endif()
    file(APPEND ${UnitTestAnalysisFile} "| ${ut} | ${MDTestFound} |\n")
  endforeach()
  file(APPEND ${UnitTestAnalysisFile} "| TotalFilters: ${totalFilters} | Num Tested: ${numTests} |\n")

  file(APPEND ${UnitTestAnalysisFile} "\n### Non-Matching Unit Test Names ###\n")
  foreach(ut ${P_SOURCES})
    file(APPEND ${UnitTestAnalysisFile} "+ ${ut}\n")
  endforeach()



endfunction()




#-------------------------------------------------------------------------------
# SIMPL_ADD_UNIT_TEST
# 
macro(SIMPL_ADD_UNIT_TEST TEST_NAMES UNIT_TEST_SOURCE_DIR)
  foreach(name ${TEST_NAMES})
    set( ${SIMPL_UNIT_TEST}_TEST_SRCS
      ${${SIMPL_UNIT_TEST}_TEST_SRCS}
      "${UNIT_TEST_SOURCE_DIR}/${name}.cpp"
      )

    string(CONCAT
      SIMPLib_UnitTest_Includes
      ${SIMPLib_UnitTest_Includes}
      "#include \"${UNIT_TEST_SOURCE_DIR}/${name}.cpp\"\n"
      )

    string(CONCAT
      SIMPLib_UnitTest_MainFunctors
      ${SIMPLib_UnitTest_MainFunctors}
     "  ${name}()()|\n")
  endforeach()
endmacro()

#-------------------------------------------------------------------------------
# SIMPL_ADD_UNIT_TEST_MOC_FILE
# 
macro(SIMPL_ADD_UNIT_TEST_MOC_FILE SOURCE_FILE UNIT_TEST_SOURCE_DIR)

  # QT5_WRAP_CPP( simpl_unit_test_moc ${UNIT_TEST_SOURCE_DIR}/${SOURCE_FILE}.cpp)
  set_source_files_properties( ${simpl_unit_test_moc} PROPERTIES GENERATED TRUE)
  set_source_files_properties( ${simpl_unit_test_moc} PROPERTIES HEADER_FILE_ONLY TRUE)

  set( ${SIMPL_UNIT_TEST}_TEST_SRCS
      ${${SIMPL_UNIT_TEST}_TEST_SRCS}
      "${simpl_unit_test_moc}"
      )
endmacro()

#-------------------------------------------------------------------------------
# Function START_FILTER_GROUP
# @param ALL_FILTERS_HEADERFILE
# @param REGISTER_KNOWN_FILTERS_FILE
# @param FILTER_GROUP
# @param BINARY_DIR
function(SIMPL_START_FILTER_GROUP)

  set(options)
  set(oneValueArgs ALL_FILTERS_HEADERFILE REGISTER_KNOWN_FILTERS_FILE FILTER_GROUP BINARY_DIR)
  set(multiValueArgs)

  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  file(APPEND ${P_ALL_FILTERS_HEADERFILE} "\n/* ------ ${P_FILTER_GROUP} --------- */\n")
  file(APPEND ${P_REGISTER_KNOWN_FILTERS_FILE} "\n    /* ------ ${P_FILTER_GROUP} --------- */\n")

  set_property(GLOBAL APPEND_STRING PROPERTY ${P_FILTER_GROUP}_ALL_FILTERS_HEADER "\n/* ------ ${P_FILTER_GROUP} --------- */\n")
  set_property(GLOBAL APPEND_STRING PROPERTY ${P_FILTER_GROUP}_REGISTER_KNOWN_FILTERS "\n    /* ------ ${P_FILTER_GROUP} --------- */\n")
  STRING(REPLACE "Filters" "" P_FILTER_GROUP ${P_FILTER_GROUP})
  
  set_property(GLOBAL APPEND PROPERTY DREAM3DDoc_GROUPS ${P_FILTER_GROUP})
endfunction()

#-------------------------------------------------------------------------------
# Macro END_FILTER_GROUP
# @param WidgetsBinaryDir
# @param filterGroup
# @param humanGroup
macro(SIMPL_END_FILTER_GROUP WidgetsBinaryDir filterGroup humanGroup)
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_SIMPL_SUPPORT_HEADER
macro(ADD_SIMPL_SUPPORT_HEADER SourceDir filterGroup headerFileName)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${headerFileName})
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${SourceDir}/${filterGroup}/${headerFileName}" "" "0")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_SIMPL_SUPPORT_MOC_HEADER
macro(ADD_SIMPL_SUPPORT_MOC_HEADER SourceDir filterGroup headerFileName)
  # QT5_WRAP_CPP( _moc_filter_source  ${SourceDir}/${filterGroup}/${headerFileName})
  # set_source_files_properties( ${_moc_filter_source} PROPERTIES GENERATED TRUE)
  # set_source_files_properties( ${_moc_filter_source} PROPERTIES HEADER_FILE_ONLY TRUE)

  set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${headerFileName}
  #                  ${_moc_filter_source}
  )
  cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${SourceDir}/${filterGroup}/${headerFileName}" "" "0")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_SIMPL_SUPPORT_HEADER_SUBDIR
macro(ADD_SIMPL_SUPPORT_HEADER_SUBDIR SourceDir filterGroup headerFileName subdir)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${subdir}/${headerFileName})
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}/${subdir}" "${SourceDir}/${filterGroup}/${subdir}/${headerFileName}" "" "0")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_SIMPL_SUPPORT_SOURCE
macro(ADD_SIMPL_SUPPORT_SOURCE SourceDir filterGroup sourceFileName)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${sourceFileName})
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "" "${SourceDir}/${filterGroup}/${sourceFileName}" "0")
endmacro()
#-------------------------------------------------------------------------------
# Macro ADD_SIMPL_SUPPORT_CLASS
macro(ADD_SIMPL_SUPPORT_CLASS SourceDir filterGroup className)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${className}.h
                    ${SourceDir}/${filterGroup}/${className}.cpp)
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${SourceDir}/${filterGroup}/${className}.h" "${SourceDir}/${filterGroup}/${className}.cpp" "0")
endmacro()


#-------------------------------------------------------------------------------
# Macro ADD_SIMPL_FILTER
# @param FilterLib The Library/Plugin the filter belongs to.
# @param WidgetLib The Widgets library the filter belongs to
# @param filterGroup The group or plugin that the filter belongs to.
# @param filterName The base filename of the filter, i.e., SomeFilter.cpp would be "SomeFilter"
# @param filterDocPath The absolute path to the .md documentation file
# @param publicFilter  Boolean TRUE or FALSE
macro(ADD_SIMPL_FILTER FilterLib WidgetLib filterGroup filterName filterDocPath publicFilter)

  # QT5_WRAP_CPP( _moc_filter_source  ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h)
  set_source_files_properties( ${_moc_filter_source} PROPERTIES GENERATED TRUE)
  set_source_files_properties( ${_moc_filter_source} PROPERTIES HEADER_FILE_ONLY TRUE)

  set(Project_SRCS ${Project_SRCS}
                  ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h
                  ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.cpp
                  ${_moc_filter_source}
                  )
  #--- Organize inside the Visual Studio/Xcode Projects
  cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h" "${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.cpp" ${PROJECT_INSTALL_HEADERS})
  cmp_IDE_GENERATED_PROPERTIES ( "Generated/${FilterLib}/${filterGroup}" "" "${_moc_filter_source}" "0")

  #-- Create an Install Rule for the headers
#  if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
#      INSTALL (FILES ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h
#          DESTINATION include/${FilterLib}/${filterGroup}
#          COMPONENT Headers)
#  endif()

  file(APPEND ${AllFiltersHeaderFile} "#include \"${FilterLib}/${filterGroup}/${filterName}.h\"\n")

  if( ${publicFilter} STREQUAL TRUE)
      file(APPEND ${RegisterKnownFiltersFile} "   FilterFactory<${filterName}>::Pointer ${filterName}Factory = FilterFactory<${filterName}>::New();\n")
      file(APPEND ${RegisterKnownFiltersFile} "   fm->addFilterFactory(\"${filterName}\",${filterName}Factory);\n\n")

      #-- Check to make sure we have a Documentation file for the filter
      if(NOT EXISTS ${filterDocPath} )
        message(FATAL_ERROR "*** Missing Documenation File for ${filterDocPath}")
      endif()

      get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
      set_property(GLOBAL APPEND PROPERTY DREAM3DDoc_${filterGroup} ${filterDocPath})
  endif()
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_FILTER_LIST
macro(ADD_FILTER_LIST)

  file(APPEND ${RegisterKnownFiltersFile} "    QList<QString> pluginList;\n\n")

  foreach(f ${_PublicFilters} )
    file(APPEND ${RegisterKnownFiltersFile} "    pluginList.append(\"${f}\");\n")
  endforeach()

  file(APPEND ${RegisterKnownFiltersFile} "\n    return pluginList;")

endmacro()
