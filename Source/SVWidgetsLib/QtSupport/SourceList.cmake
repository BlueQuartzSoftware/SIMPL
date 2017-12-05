



set(SVWidgetsLib_QtSupport_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSApplicationFileInfo.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSMacros.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSHelpUrlGenerator.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFileUtils.h
)

set(SVWidgetsLib_QtSupport_MOC_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSApplicationAboutBoxDialog.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSBookmarkMissingDialog.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSCheckboxDialog.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QRoundProgressBar.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSColorComboPicker.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDisclosableGroupBox.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDistributionTypeWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDroppableScrollArea.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFSDropLabel.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFSDropLineEdit.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFaderWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFileCompleter.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFileDragMessageBox.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSGraphicsView.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSHoverButton.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSImageGraphicsDelegate.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSPluginFrame.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSRecentFileList.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSLineEdit.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSSettings.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSSplitter.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSSplitterHandle.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSStyles.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDisclosableGroupBox.h
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSStringEdit.h
)

set(SVWidgetsLib_QtSupport_SRCS
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSApplicationAboutBoxDialog.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSApplicationFileInfo.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSBookmarkMissingDialog.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSCheckboxDialog.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QRoundProgressBar.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSColorComboPicker.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDisclosableGroupBox.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDistributionTypeWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSDroppableScrollArea.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFSDropLabel.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFSDropLineEdit.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFaderWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFileCompleter.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFileDragMessageBox.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSGraphicsView.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSHelpUrlGenerator.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSHoverButton.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSImageGraphicsDelegate.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSPluginFrame.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSRecentFileList.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSLineEdit.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSSettings.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSSplitter.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSSplitterHandle.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSStyles.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSStringEdit.cpp
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSFileUtils.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib" "${SVWidgetsLib_QtSupport_HDRS};${SVWidgetsLib_QtSupport_MOC_HDRS}" "${SVWidgetsLib_QtSupport_SRCS}" "0")

# -- Add Qt resource file
SET(SVWidgetsLib_QtSupport_RCS
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSupportResources.qrc
)

QT5_ADD_RESOURCES( SVWidgetsLib_QtSupport_Generated_RC_SRCS "${SVWidgetsLib_SOURCE_DIR}/QtSupport/QtSupportResources.qrc"  )


# --------------------------------------------------------------------
# and finally this will run moc:
# QT5_WRAP_CPP( SVWidgetsLib_QtSupport_Generated_MOC_SRCS ${SVWidgetsLib_QtSupport_MOC_HDRS} )
set_source_files_properties( ${SVWidgetsLib_QtSupport_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SVWidgetsLib_QtSupport_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)



# --------------------------------------------------------------------
# Wrap UI files so they are AUTO UIC'ed
QT5_WRAP_UI( SVWidgetsLib_QtSupport_Generated_UI_HDRS   
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/UI_Files/QtSApplicationAboutBoxDialog.ui
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/UI_Files/QtSBookmarkMissingDialog.ui
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/UI_Files/QtSDistributionTypeWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/UI_Files/QtSFileDragMessageBox.ui
  ${SVWidgetsLib_SOURCE_DIR}/QtSupport/UI_Files/QtSStringEdit.ui
)
foreach(h ${SVWidgetsLib_QtSupport_Generated_UI_HDRS} ${SVWidgetsLib_QtSupport_Generated_RC_SRCS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_QtSupport_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SVWidgetsLib_QtSupport_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${SVWidgetsLib_QtSupport_Generated_RC_SRCS}" "" "0")

set(SVWidgets_QtSupport_Files
  ${SVWidgetsLib_QtSupport_HDRS}
  ${SVWidgetsLib_QtSupport_MOC_HDRS}
  ${SVWidgetsLib_QtSupport_SRCS}
  ${SVWidgetsLib_QtSupport_Generated_MOC_SRCS}
  ${SVWidgetsLib_QtSupport_Generated_UI_HDRS}
  ${SVWidgetsLib_QtSupport_Generated_RC_SRCS}
  ${SVWidgetsLib_QtSupport_RCS}
)
