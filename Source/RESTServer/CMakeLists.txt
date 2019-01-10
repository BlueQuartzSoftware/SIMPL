
# set project's name
PROJECT( RESTServer VERSION ${SIMPLProj_VERSION_MAJOR}.${SIMPLProj_VERSION_MINOR})


set(RESTServer_HDRS

)

set(RESTServer_SRCS
  ${RESTServer_SOURCE_DIR}/main.cpp
)


set(PROJECT_SRCS
    ${RESTServer_SRCS}
    ${RESTServer_HDRS}
)

configure_file(${RESTServer_SOURCE_DIR}/SIMPLRestServer.ini ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RESTServer.ini )


set(install_dir "bin")
set(lib_install_dir "lib")

if(APPLE)
  get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)
  set(install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}bin")
  set(lib_install_dir "${DREAM3D_PACKAGE_DEST_PREFIX}lib")
elseif(WIN32)
  set(install_dir ".")
  set(lib_install_dir ".")
endif()

BuildToolBundle(
    TARGET RESTServer
    SOURCES ${PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${SIMPL_VER_MAJOR}
    VERSION_MINOR ${SIMPL_VER_MINOR}
    VERSION_PATCH ${SIMPL_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES Qt5::Core Qt5::Network SIMPLib QtWebAppLib
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Tools
    INSTALL_DEST  "${install_dir}"
    SOLUTION_FOLDER "Applications"
)
