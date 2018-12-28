
# set project's name
PROJECT( SIMPLib VERSION ${SIMPLProj_VERSION_MAJOR}.${SIMPLProj_VERSION_MINOR})


set(PROJECT_PREFIX "SIMPLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
# -----------------------------------------------------------------------
# The project generally does NOT want to install headers. See if another
# project has over ridden that property
get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
if("${CMP_PROJECT_INSTALL_HEADERS}" STREQUAL "")
  set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "0")
  get_property(CMP_PROJECT_INSTALL_HEADERS GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS)
endif()
set(PROJECT_INSTALL_HEADERS "${CMP_PROJECT_INSTALL_HEADERS}")

set(SIMPLib_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(SIMPLib_BUILT_AS_DYNAMIC_LIB 1)
endif(BUILD_SHARED_LIBS)

#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${${PROJECT_NAME}_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "${PROJECT_NAME}Configuration.h")
set(CMP_TYPES_FILE_NAME "${PROJECT_NAME}Types.h")
set(CMP_VERSION_HEADER_FILE_NAME "${PROJECT_NAME}Version.h")
set(CMP_VERSION_SOURCE_FILE_NAME "${PROJECT_NAME}Version.cpp")
set(CMP_TOP_HEADER_FILE "${PROJECT_NAME}.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING 1)
set(PROJECT_INSTALL_HEADERS "${CMP_PROJECT_INSTALL_HEADERS}")

set(SIMPLib_FILTER_DOC_DIR ${SIMPLib_SOURCE_DIR}/Documentation/${PROJECT_NAME}Filters)

include(${CMP_SOURCE_DIR}/cmpProject.cmake)

#-- Configure Target Specific configuration file
cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${SIMPLib_SOURCE_DIR}/SIMPLibConfiguration.h.in
                             GENERATED_FILE_PATH ${SIMPLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

cmp_IDE_GENERATED_PROPERTIES("Generated"
                             "${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE};${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME};${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME};${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}"
                             "${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}")

set(Project_SRCS
    ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
    ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

#----- Bring in our CMake file with the macros
include(${SIMPLib_SOURCE_DIR}/SIMPLibMacros.cmake)
set(SIMPLib_UnitTest_Includes "")
set(SIMPLib_UnitTest_MainFunctors "")

#------------------------------------------------------------------------------
# Reset the Unit Test code fragment to an empty string


#-------------------------------------------------------------------------------
macro(D3DCompileDir DirName)
    set(SIMPLib_${DirName}_Generated_MOC_SRCS "")
    include(${SIMPLib_SOURCE_DIR}/${DirName}/SourceList.cmake)

    foreach(f ${SIMPLib_${DirName}_Generated_MOC_SRCS})
        set_source_files_properties( ${f} PROPERTIES GENERATED TRUE)
        set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
    endforeach()
    set(Project_SRCS ${Project_SRCS} ${SIMPLib_${DirName}_SRCS} ${SIMPLib_${DirName}_HDRS} ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS})
  # if(PROJECT_INSTALL_HEADERS)
  #   install(DIRECTORY ${SIMPLib_SOURCE_DIR}/${DirName} 
  #     DESTINATION include/${PROJECT_NAME}
  #       FILES_MATCHING PATTERN "*.h")
  #   install(DIRECTORY ${SIMPLib_SOURCE_DIR}/${DirName} 
  #     DESTINATION include/${PROJECT_NAME}
  #       FILES_MATCHING PATTERN "*.hpp")
  # endif()
endMacro()

#------------------------------------------------------------------------------
# Set up the directories that are used for each Group
set(SIMPL_Group_BASE_MODULES Common DataArrays DataContainers Geometry HDF5 Math StatsData Utilities ITK)
set(SIMPL_Group_FILTERS_MODULES Filtering FilterParameters VTKUtils)
set(SIMPL_Group_PLUGIN_MODULES Plugin)
set(SIMPL_Group_REST_MODULES REST)

if(SIMPL_Group_BASE)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_BASE_MODULES})
    D3DCompileDir(${SIMPLIB_SUB_DIR})
  endforeach()
endif()

if(SIMPL_Group_FILTERS)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_FILTERS_MODULES})
    D3DCompileDir(${SIMPLIB_SUB_DIR})
  endforeach()
endif()

if(SIMPL_Group_PLUGIN)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_PLUGIN_MODULES})
    D3DCompileDir(${SIMPLIB_SUB_DIR})
  endforeach()
endif()

if(SIMPL_Group_REST)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_REST_MODULES})
    D3DCompileDir(${SIMPLIB_SUB_DIR})
  endforeach()
endif()

#-------
# Start writing this header file
set(AllFiltersHeaderFile ${SIMPLib_BINARY_DIR}/CoreFilters/SIMPLibFilters.h_tmp)
file(WRITE ${AllFiltersHeaderFile} "#ifndef _SIMPLib_FILTERS_H_\n#define _SIMPLib_FILTERS_H_\n\n")
file(APPEND ${AllFiltersHeaderFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")


# --------------------------------------------------------------------
# Create a File that will register all of the SIMPLib Filters
set(RegisterKnownFiltersFile ${SIMPLib_BINARY_DIR}/CoreFilters/RegisterKnownFilters.cpp_tmp)
file(WRITE ${RegisterKnownFiltersFile} "/* This file was AUTO GENERATED During the CMake Run. DO NOT EDIT THIS FILE. Edit the CMake files that generate this file */\n")
file(APPEND ${RegisterKnownFiltersFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")
file(APPEND ${RegisterKnownFiltersFile} "/* This file will be directly included in another source file. */\n")
#file(APPEND ${RegisterKnownFiltersFile} "#include \"SIMPLib/Common/FilterManager.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "#include \"SIMPLibFilters.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "\nvoid CorePlugin::registerFilters(FilterManager* fm)\n{\n")


# Add in our Filter classes
# Add the generated moc file to the project sources
set(CORE_FILTER_LIST "")
if(SIMPL_Group_FILTERS)
  include(${SIMPLib_SOURCE_DIR}/CoreFilters/SourceList.cmake)
  set(CORE_FILTER_LIST ${CORE_FILTER_LIST} ${_PublicFilters} )
  #-----------------------------------------------------
  # conditionally compile our example codes- These should be turned off for a release
  if(NOT "${SIMPLProj_RELEASE_TYPE}" STREQUAL "Official")
      # Add the generated moc file to the project sources
      set(Project_SRCS ${Project_SRCS} ${PluginAutoMocSourceFile} )
      include(${SIMPLib_SOURCE_DIR}/TestFilters/SourceList.cmake)
      set(CORE_FILTER_LIST ${CORE_FILTER_LIST} ${_PublicFilters} )
  endif()
endif()

#-----------------------------------------------------
# Close up all the files that we just generated
file(APPEND ${AllFiltersHeaderFile} "\n#endif\n")
file(APPEND ${RegisterKnownFiltersFile} "\n }\n")

#-----------------------------------------------------
# Add function to return all filter names in a list
set(PLUGIN_NAME "Core")
file(APPEND ${RegisterKnownFiltersFile} "\nQList<QString> ${PLUGIN_NAME}Plugin::getFilters()\n{\n")
set(_PublicFilters ${CORE_FILTER_LIST})
ADD_FILTER_LIST()
file(APPEND ${RegisterKnownFiltersFile} "\n}\n\n")

cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${SIMPLib_BINARY_DIR}/CoreFilters/SIMPLibFilters.h
                          NEW_FILE_PATH  ${AllFiltersHeaderFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${SIMPLib_BINARY_DIR}/CoreFilters/RegisterKnownFilters.cpp
                          NEW_FILE_PATH  ${RegisterKnownFiltersFile} )


set(${PROJECT_NAME}_LINK_LIBS Qt5::Core Qt5::Network H5Support)
if( "${SIMPL_USE_MULTITHREADED_ALGOS}" STREQUAL "ON")
  list(APPEND ${PROJECT_NAME}_LINK_LIBS TBB::tbb TBB::tbbmalloc)
endif()

#-- Add a library for the SIMPLib Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${Project_SRCS} )
CMP_AddDefinitions(TARGET ${PROJECT_NAME})

#-- Add Target specific definitions
if(WIN32 AND BUILD_SHARED_LIBS)
      target_compile_definitions(${PROJECT_NAME} PUBLIC "-DSIMPLib_BUILT_AS_DYNAMIC_LIB")
endif()

#-- Configure Target Specific Include Directories
get_filename_component(TARGET_BINARY_DIR_PARENT ${${PROJECT_NAME}_BINARY_DIR} PATH)
get_filename_component(TARGET_SOURCE_DIR_PARENT ${${PROJECT_NAME}_SOURCE_DIR} PATH)

target_include_directories(${PROJECT_NAME} 
                            PUBLIC
                              $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
                              $<BUILD_INTERFACE:${TARGET_SOURCE_DIR_PARENT}>
                              $<BUILD_INTERFACE:${TARGET_BINARY_DIR_PARENT}>
                              $<BUILD_INTERFACE:${TBB_INCLUDE_DIRS}>
                              $<BUILD_INTERFACE:${EIGEN_INCLUDE_DIRS}>
)
CMP_MODULE_INCLUDE_DIRS (TARGET ${PROJECT_NAME} LIBVARS HDF5 Qt5Core Qt5Network)

if(SIMPL_Group_BASE)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_BASE_MODULES})
    target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/${SIMPLIB_SUB_DIR}>)
  endforeach()
endif()

if(SIMPL_Group_FILTERS)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_FILTERS_MODULES})
    target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/${SIMPLIB_SUB_DIR}>)
  endforeach()
endif()

if(SIMPL_Group_PLUGIN)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_PLUGIN_MODULES})
    target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/${SIMPLIB_SUB_DIR}>)
  endforeach()
endif()

if(SIMPL_Group_REST)
  foreach(SIMPLIB_SUB_DIR ${SIMPL_Group_REST_MODULES})
    target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/${SIMPLIB_SUB_DIR}>)
  endforeach()
endif()

if(SIMPL_Group_FILTERS)
  target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/CoreFilters>)
  #-----------------------------------------------------
  # conditionally compile our example codes- These should be turned off for a release
  if(NOT "${SIMPLProj_RELEASE_TYPE}" STREQUAL "Offical")
    target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/TestFilters>)
  endif()
endif()

if(SIMPL_Group_REST)
  target_include_directories(${PROJECT_NAME} PUBLIC 
                                $<BUILD_INTERFACE:${SIMPLProj_SOURCE_DIR}/ThirdParty>)
  target_link_libraries(${PROJECT_NAME} QtWebAppLib)                            
endif()

#-- Configure Link Libraries for the Target
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )
#-- Link the Target to its dependent libraries
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LINK_LIBS})

#-- Configure Target Installation Rules
set(install_dir "tools")
set(lib_install_dir "lib")

if(APPLE)
  get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)
  set(install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}bin")
  set(lib_install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}lib")
elseif(WIN32)
  set(install_dir ".")
  set(lib_install_dir ".")
endif()

INSTALL(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    COMPONENT Applications
    RUNTIME DESTINATION ${install_dir}
    LIBRARY DESTINATION ${lib_install_dir}
    ARCHIVE DESTINATION lib
    BUNDLE DESTINATION "."
)


# --------------------------------------------------------------------
# Allow the generation and installation of a CMake configuration file
# which makes using SIMPL from another project easier.
# --------------------------------------------------------------------
export(EXPORT ${PROJECT_NAME}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::
)

if(PROJECT_INSTALL_HEADERS)
  install(EXPORT SIMPLibTargets
    FILE
      SIMPLibTargets.cmake
    NAMESPACE
      SIMPL::
    DESTINATION
      ${ConfigPackageLocation}
  )

  install(FILES 
      ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
      ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
      ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
      ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
    DESTINATION
      include/${PROJECT_NAME}
  )

endif()


# ------- Enable the CTest testing. ---------------
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
    include(${SIMPLib_SOURCE_DIR}/Testing/CMakeLists.txt)
endif()




