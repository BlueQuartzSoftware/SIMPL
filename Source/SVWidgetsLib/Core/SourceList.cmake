



set(SVWidgetsLib_Core_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/Core/FilterWidgetManager.h
  ${SVWidgetsLib_SOURCE_DIR}/Core/IFilterWidgetFactory.h
  ${SVWidgetsLib_SOURCE_DIR}/Core/PipelineFilterWidgetFactory.hpp
  ${SVWidgetsLib_SOURCE_DIR}/Core/SVWidgetsLibDLLExport.h
  ${SVWidgetsLib_SOURCE_DIR}/Core/SVWidgetsLibConstants.h
)

set(SVWidgetsLib_Core_MOC_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/Core/PipelineViewPtrMimeData.h
)

set(SVWidgetsLib_Core_SRCS
  ${SVWidgetsLib_SOURCE_DIR}/Core/FilterWidgetManager.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Core/IFilterWidgetFactory.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Core/PipelineViewPtrMimeData.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib" "${SVWidgetsLib_Core_HDRS};${SVWidgetsLib_Core_MOC_HDRS}" "${SVWidgetsLib_Core_SRCS}" "0")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( SVWidgetsLib_Core_Generated_MOC_SRCS ${SVWidgetsLib_Core_MOC_HDRS} )
set_source_files_properties( ${SVWidgetsLib_Core_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SVWidgetsLib_Core_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)



# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( SVWidgetsLib_Core_Generated_UI_HDRS   )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_Core_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SVWidgetsLib_Core_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${SVWidgetsLib_Core_Generated_RC_SRCS}" "" "0")

set(SVWidgets_Core_Files
  ${SVWidgetsLib_Core_HDRS}
  ${SVWidgetsLib_Core_MOC_HDRS}
  ${SVWidgetsLib_Core_SRCS}
  ${SVWidgetsLib_Core_Generated_MOC_SRCS}
  ${SVWidgetsLib_Core_Generated_UI_HDRS}
  ${SVWidgetsLib_Core_Generated_RC_SRCS}
)
