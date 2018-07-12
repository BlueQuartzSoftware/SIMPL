project(SIMPLRESTClient)

set(SIMPLRESTClient_MOC_HDRS
  ${SIMPLRESTClient_SOURCE_DIR}/Core/config.h
  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequest.h
  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequestmanager.h
  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequestptr.h
  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequestqueue.h
  ${SIMPLRESTClient_SOURCE_DIR}/Core/SIMPLRestClient.h
  ${SIMPLRESTClient_SOURCE_DIR}/Core/userptr.h

  ${SIMPLRESTClient_SOURCE_DIR}/SIMPLRestClient/SIMPLClientRequest.h

  ${SIMPLRESTClient_SOURCE_DIR}/SIMPLRestClient_UI.h
)

set(SIMPLRESTClient_HDRS

)

set(SIMPLRESTClient_SRCS
  ${SIMPLRESTClient_SOURCE_DIR}/main.cpp

  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequest.cpp
  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequestmanager.cpp
  ${SIMPLRESTClient_SOURCE_DIR}/Core/mrestrequestqueue.cpp
  ${SIMPLRESTClient_SOURCE_DIR}/Core/SIMPLRestClient.cpp
  ${SIMPLRESTClient_SOURCE_DIR}/Core/config.cpp

  ${SIMPLRESTClient_SOURCE_DIR}/SIMPLRestClient/SIMPLClientRequest.cpp

  ${SIMPLRESTClient_SOURCE_DIR}/SIMPLRestClient_UI.cpp
)

set(SIMPLRESTClient_UIS
  ${SIMPLRESTClient_SOURCE_DIR}/UI_Files/SIMPLRestClient_UI.ui
  )

cmp_IDE_SOURCE_PROPERTIES( "RestClient" "${SIMPLRESTClient_MOC_HDRS};${SIMPLRESTClient_HDRS}" "${SIMPLRESTClient_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("RestClient/UI_Files" "${SIMPLRESTClient_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( SIMPLRESTClient_Generated_MOC_SRCS ${SIMPLRESTClient_MOC_HDRS} )
QT5_WRAP_UI( SIMPLRESTClient_Generated_UI_HDRS ${SIMPLRESTClient_UIS} )
set_source_files_properties( ${SIMPLRESTClient_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
# set_source_files_properties( ${SIMPLRESTClient_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SIMPLRESTClient_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SIMPLRESTClient_Generated_UI_HDRS}" "" "0")


set(SIMPLRESTClient_SRCS_Compiled
  ${SIMPLRESTClient_MOC_HDRS}
  ${SIMPLRESTClient_SRCS}
  ${SIMPLRESTClient_HDRS}
  ${SIMPLRESTClient_Generated_MOC_SRCS}
  ${SIMPLRESTClient_Generated_UI_HDRS}
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


BuildToolBundle(
    TARGET SIMPLRESTClient
    SOURCES ${SIMPLRESTClient_SRCS_Compiled}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${SIMPL_VER_MAJOR}
    VERSION_MINOR ${SIMPL_VER_MINOR}
    VERSION_PATCH ${SIMPL_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES Qt5::Core Qt5::Widgets Qt5::Network SIMPLib
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Tools
    INSTALL_DEST  "${install_dir}"
    SOLUTION_FOLDER "Applications"
)

