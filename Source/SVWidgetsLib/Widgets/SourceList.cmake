
set(SVWidgetsLib_Widgets_HDRS "")
set(SVWidgetsLib_Widgets_SRCS "")
set(SVWidgetsLib_Widgets_UIS "")


# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(SVWidgetsLib_Widgets_MOC_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItemDelegate.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksModel.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksToolboxWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksTreeView.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BreakpointFilterWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/DREAM3DListWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/DataContainerArrayWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/DropBoxWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterLibraryToolboxWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterLibraryTreeWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterListToolboxWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterListWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/IssuesDockWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/StandardOutputDockWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterInputWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SVPipelineFilterWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SVPipelineViewWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SIMPLViewToolbox.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SIMPLViewMenuItems.h
)

set(SVWidgetsLib_Widgets_HDRS
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItem.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/PipelineView.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/PipelineFilterObject.h
)

set(SVWidgetsLib_Widgets_SRCS
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItem.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItemDelegate.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksModel.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksToolboxWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BookmarksTreeView.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/BreakpointFilterWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/DREAM3DListWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/DataContainerArrayWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/DropBoxWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterLibraryToolboxWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterLibraryTreeWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterListToolboxWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterListWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/IssuesDockWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/StandardOutputDockWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/FilterInputWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SVPipelineFilterWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SVPipelineViewWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/PipelineFilterObject.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/PipelineView.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SIMPLViewToolbox.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/SIMPLViewMenuItems.cpp
  )

set(SVWidgetsLib_Widgets_UIS
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/BookmarksToolboxWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/DataContainerArrayWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/DropBoxWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/FavoritesDockWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/FilterInputWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/FilterLibraryToolboxWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/FilterListToolboxWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/IssuesDockWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/SVPipelineFilterWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/PrebuiltsToolboxWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/StandardOutputDockWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/SIMPLViewToolbox.ui
  )

SET(SVWidgetsLib_Widgets_Util_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/util/MoveFilterCommand.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/util/AddFilterCommand.h
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/util/RemoveFilterCommand.h
)

SET(SVWidgetsLib_Widgets_Util_SRCS
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/util/MoveFilterCommand.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/util/AddFilterCommand.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Widgets/util/RemoveFilterCommand.cpp
  )

cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib/Widgets/util" "${SVWidgetsLib_Widgets_Util_HDRS}" "${SVWidgetsLib_Widgets_Util_SRCS}" "${PROJECT_INSTALL_HEADERS}")
source_group("${SVWidgetsLib_SOURCE_DIR}/Widgets util" FILES ${HEADERS} ${SOURCES})

cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib/Widgets" "${SVWidgetsLib_Widgets_MOC_HDRS};${SVWidgetsLib_Widgets_HDRS}" "${SVWidgetsLib_Widgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("SVWidgetsLib/Widgets/UI_Files" "${SVWidgetsLib_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( SVWidgetsLib_Widgets_Generated_MOC_SRCS ${SVWidgetsLib_Widgets_MOC_HDRS} )
set_source_files_properties( ${SVWidgetsLib_Widgets_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SVWidgetsLib_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT5_ADD_RESOURCES( SVWidgetsLib_Generated_RC_SRCS "${SIMPLViewProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( SVWidgetsLib_Widgets_Generated_UI_HDRS ${SVWidgetsLib_Widgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SVWidgetsLib_Widgets_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${SVWidgetsLib_Generated_RC_SRCS}" "" "0")

set(SVWidgets_Widgets_Files
  ${SVWidgetsLib_Widgets_MOC_HDRS}
  ${SVWidgetsLib_Widgets_SRCS}
  ${SVWidgetsLib_Widgets_HDRS}
  ${SVWidgetsLib_Widgets_Util_HDRS}
  ${SVWidgetsLib_Widgets_Util_SRCS}

  ${SVWidgetsLib_Widgets_Generated_MOC_SRCS}
  ${SVWidgetsLib_Widgets_Generated_UI_HDRS}
)


