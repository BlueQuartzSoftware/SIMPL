project(QtWebAppLib)

# INCLUDE_DIRECTORIES(${QtWebAppLib_SOURCE_DIR})

set(QtWebApp_SRCS
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpconnectionhandler.cpp     
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httprequesthandler.cpp
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpconnectionhandlerpool.cpp 
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpresponse.cpp
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpcookie.cpp                
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpsession.cpp
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpglobal.cpp                
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpsessionstore.cpp
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httplistener.cpp              
  ${QtWebAppLib_SOURCE_DIR}/httpserver/staticfilecontroller.cpp
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httprequest.cpp
  
  ${QtWebAppLib_SOURCE_DIR}/logging/dualfilelogger.cpp 
  ${QtWebAppLib_SOURCE_DIR}/logging/filelogger.cpp     
  ${QtWebAppLib_SOURCE_DIR}/logging/logger.cpp         
  ${QtWebAppLib_SOURCE_DIR}/logging/logmessage.cpp
)

set(QtWebApp_HDRS
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpconnectionhandler.h     
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httprequesthandler.h
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpconnectionhandlerpool.h 
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpresponse.h
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpcookie.h                
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpsession.h
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpglobal.h                
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httpsessionstore.h
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httplistener.h              
  ${QtWebAppLib_SOURCE_DIR}/httpserver/staticfilecontroller.h
  ${QtWebAppLib_SOURCE_DIR}/httpserver/httprequest.h

  ${QtWebAppLib_SOURCE_DIR}/logging/dualfilelogger.h 
  ${QtWebAppLib_SOURCE_DIR}/logging/filelogger.h     
  ${QtWebAppLib_SOURCE_DIR}/logging/logger.h         
  ${QtWebAppLib_SOURCE_DIR}/logging/logmessage.h
)


set(QtWebAppLib_SRCS
  ${QtWebApp_SRCS}
  ${QtWebApp_HDRS}
  )


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

add_library(QtWebAppLib ${LIB_TYPE} ${QtWebAppLib_SRCS})
CMP_AddDefinitions(TARGET ${PROJECT_NAME})
target_link_libraries(QtWebAppLib Qt5::Core Qt5::Network)
#-- Configure Link Libraries for the Target
LibraryProperties( QtWebAppLib ${EXE_DEBUG_EXTENSION} )


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


INSTALL(TARGETS ${PROJECT_NAME}
    EXPORT QtWebAppLibTargets
    COMPONENT Applications
    RUNTIME DESTINATION ${install_dir}
    LIBRARY DESTINATION ${lib_install_dir}
    ARCHIVE DESTINATION ${lib_install_dir}
    BUNDLE DESTINATION "."
)


# --------------------------------------------------------------------
# Allow the generation and installation of a CMake configuration file
# which makes using SIMPL from another project easier.
# --------------------------------------------------------------------
export(EXPORT QtWebAppLibTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Targets.cmake"
  NAMESPACE QtWebAppLib::
)

if (QtWebAppLib_INSTALL_FILES)
  install(EXPORT QtWebAppLibTargets
    FILE
      ${PROJECT_NAME}Targets.cmake
    NAMESPACE
      ${PROJECT_NAME}::
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

