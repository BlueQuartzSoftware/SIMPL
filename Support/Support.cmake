
set(PLUGIN_NAME "SIMPL")
#-------------------------------------------------------------------------------
#- This installs all the prebuilt pipeline files.
if(EXISTS ${SIMPLProj_SOURCE_DIR}/Support/ExamplePipelines/${PLUGIN_NAME})
  #-------------------------------------------------------------------------------
  #- This copies all the Prebuilt Pipeline files into the Build directory
  add_custom_target(${PLUGIN_NAME}ExamplePipelinesCopy ALL
              COMMAND ${CMAKE_COMMAND} -E copy_directory ${SIMPLProj_SOURCE_DIR}/Support/ExamplePipelines/${PLUGIN_NAME}
              ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/PrebuiltPipelines/Examples/${PLUGIN_NAME}
              COMMENT "Copying ${PLUGIN_NAME} Example Pipelines into Binary Directory")
  set_target_properties(${PLUGIN_NAME}ExamplePipelinesCopy PROPERTIES FOLDER ZZ_COPY_FILES)

  if(APPLE)
    set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}/Resources/PrebuiltPipelines/Examples")
  else()
    set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}/PrebuiltPipelines/Examples")
  endif()

  install(DIRECTORY ${SIMPLProj_SOURCE_DIR}/Support/ExamplePipelines/${PLUGIN_NAME}
          DESTINATION ${INSTALL_DESTINATION}
          COMPONENT Applications )
endif()


#-------------------------------------------------------------------------------
#- This installs all the Data files.
if(EXISTS ${SIMPLProj_SOURCE_DIR}/Support/Data/${PLUGIN_NAME})
  #-------------------------------------------------------------------------------
  # Copy the Data Folder to the build directory
  add_custom_target(${PLUGIN_NAME}DataFolderCopy ALL
              COMMAND ${CMAKE_COMMAND} -E copy_directory ${SIMPLProj_SOURCE_DIR}/Support/Data/${PLUGIN_NAME}
              ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Data/${PLUGIN_NAME}
              COMMENT "Copying ${PLUGIN_NAME} Data Folder into Binary Directory")
  set_target_properties(${PLUGIN_NAME}DataFolderCopy PROPERTIES FOLDER ZZ_COPY_FILES)

  if(APPLE)
    set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}/Resources/Data")
  else()
    set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}/Data")
  endif()

  install(DIRECTORY ${SIMPLProj_SOURCE_DIR}/Support/Data/${PLUGIN_NAME}
          DESTINATION Data
          COMPONENT Applications )
endif()