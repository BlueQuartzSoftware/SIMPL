
set(SVWidgetsLib_Widgets_HDRS "")
set(SVWidgetsLib_Widgets_MOC_HDRS "")
set(SVWidgetsLib_Widgets_SRCS "")
set(SVWidgetsLib_Widgets_UIS "")

set(SVWidgetsLib_Widgets_MOC_HDRS ${SVWidgetsLib_Widgets_MOC_HDRS}
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/DelimitedPage.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/DataFormatPage.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/TupleTableWidget.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/TupleTableItemDelegate.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/ImportASCIIDataWizard.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/AbstractWizardPage.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/ASCIIDataModel.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/EditHeadersDialog.h
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/LineCounterObject.h
  )

set(SVWidgetsLib_Widgets_SRCS ${SVWidgetsLib_Widgets_SRCS}
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/DelimitedPage.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/DataFormatPage.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/TupleTableWidget.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/TupleTableItemDelegate.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/ImportASCIIDataWizard.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/AbstractWizardPage.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/ASCIIDataModel.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/ASCIIDataItem.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/EditHeadersDialog.cpp
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/LineCounterObject.cpp
    )

set(SVWidgetsLib_Widgets_UIS ${SVWidgetsLib_Widgets_UIS}
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/UI_Files/DelimitedPage.ui
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/UI_Files/DataFormatPage.ui
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/UI_Files/TupleTableWidget.ui
    ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/UI_Files/EditHeadersDialog.ui
    )

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(SVWidgetsLib_Widgets_HDRS ${SVWidgetsLib_Widgets_HDRS}
      ${SVWidgetsLib_SOURCE_DIR}/Widgets/ImportASCIIDataWizard/ASCIIDataItem.h
    )

set(SVWidgetsLib_Widgets_HDRS ${SVWidgetsLib_Widgets_HDRS}
      ${SVWidgetsLib_Widgets_MOC_HDRS}
  )

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_SOURCE_PROPERTIES( "Widgets/ImportASCIIDataWizard" "${SVWidgetsLib_Widgets_HDRS}" "${SVWidgetsLib_Widgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("Widgets/ImportASCIIDataWizard/UI_Files" "${SVWidgetsLib_Widgets_UIS}" "")

# QT5_WRAP_CPP( SVWidgetsLib_Widgets_Generated_MOC_SRCS ${SVWidgetsLib_Widgets_MOC_HDRS} )

# --------------------------------------------------------------------
# We are using CMake's AuotMoc feature so we do not need to wrap our .cpp files with a specific call to 'moc'

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
# set_source_files_properties( ${SVWidgetsLib_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( SVWidgetsLib_Widgets_Generated_UI_HDRS ${SVWidgetsLib_Widgets_UIS} )
foreach(h ${SVWidgetsLib_Widgets_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SVWidgetsLib_Widgets_Generated_UI_HDRS}" "" "0")

# --------------------------------------------------------------------
# If you are doing more advanced Qt programming where you are including resources you will have to enable this section
# with your own cmake codes to include your resource file (.qrc) and any other needed files
# QT5_ADD_RESOURCES( SVWidgetsLib_Generated_RC_SRCS ""  )
# cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${SVWidgetsLib_Generated_RC_SRCS}" "" "0")




