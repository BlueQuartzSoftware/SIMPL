set ( SimpleITKExplicitSource
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKComposeImageFilter.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKDefaultPixelAccessor.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKDefaultVectorPixelAccessor.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageBase.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImage.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageRegion.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageSource.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageToImageFilter.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImportImageContainer.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKInPlaceImageFilter.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKLabelMap.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKVectorImage.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKVectorIndexSelectionCastImageFilter.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageConstIterator.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageRegionConstIterator.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageScanlineConstIterator.cxx
  ${SimpleITKExplicit_SOURCE_DIR}/Explicit/src/sitkExplicitITKImageScanlineIterator.cxx

  )


# these files are the instantiated ITK filter, but they do not do IO,
# so we don't need to register the IO factoried here.
set ( ITK_NO_IO_FACTORY_REGISTER_MANAGER 1 )
include(${ITK_USE_FILE})

if ( MSVC AND SITK_BUILD_SHARED_LIBS )
  set( SimpleITKExplicit_FORCE_LIBRARY_TYPE "STATIC" )
endif()

add_library ( SimpleITKExplicit ${SimpleITKExplicit_FORCE_LIBRARY_TYPE} ${SimpleITKExplicitSource} )
target_link_libraries ( SimpleITKExplicit 
  ITKCommon  
  # ${ITK_LIBRARIES} 
  )
#set_target_properties( SimpleITKExplicit PROPERTIES SKIP_BUILD_RPATH TRUE )
sitk_install_exported_target( SimpleITKExplicit )

LibraryProperties( SimpleITKExplicit  ${EXE_DEBUG_EXTENSION} )


if(SIMPL_DISABLE_MSVC_WARNINGS)
  target_compile_definitions(SimpleITKExplicit PRIVATE -D_SCL_SECURE_NO_WARNINGS)
endif()
target_include_directories(SimpleITKExplicit 
                            PUBLIC
                              ${SimpleITKExplicit_SOURCE_DIR}/Explicit/include
                              ${SimpleITKExplicit_BINARY_DIR}/Common/include
                          )

if(NOT MSVC)
    INSTALL(TARGETS SimpleITKExplicit
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}
        BUNDLE DESTINATION "."
    )
endif()
