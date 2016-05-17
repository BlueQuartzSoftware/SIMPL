



set(SVWidgetsLib_Dialogs_HDRS

)

set(SVWidgetsLib_Dialogs_MOC_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/AboutPlugins.h
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/FavoritesChangedDialog.h
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/PluginDetails.h
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UpdateCheck.h
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UpdateCheckData.h
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UpdateCheckDialog.h
)

set(SVWidgetsLib_Dialogs_SRCS
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/AboutPlugins.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/FavoritesChangedDialog.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/PluginDetails.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UpdateCheck.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UpdateCheckData.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UpdateCheckDialog.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib" "${SVWidgetsLib_Dialogs_HDRS};${SVWidgetsLib_Dialogs_MOC_HDRS}" "${SVWidgetsLib_Dialogs_SRCS}" "0")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( SVWidgetsLib_Dialogs_Generated_MOC_SRCS ${SVWidgetsLib_Dialogs_MOC_HDRS} )
set_source_files_properties( ${SVWidgetsLib_Dialogs_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SVWidgetsLib_Dialogs_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)



# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( SVWidgetsLib_Dialogs_Generated_UI_HDRS   
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UI_Files/AboutPlugins.ui
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UI_Files/PluginDetails.ui
  ${SVWidgetsLib_SOURCE_DIR}/Dialogs/UI_Files/FavoritesChangedDialog.ui
	${SVWidgetsLib_SOURCE_DIR}/Dialogs/UI_Files/UpdateCheckDialog.ui
)

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_Dialogs_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SVWidgetsLib_Dialogs_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${SVWidgetsLib_Dialogs_Generated_RC_SRCS}" "" "0")

set(SVWidgets_Dialogs_Files
  ${SVWidgetsLib_Dialogs_HDRS}
  ${SVWidgetsLib_Dialogs_MOC_HDRS}
  ${SVWidgetsLib_Dialogs_SRCS}
  ${SVWidgetsLib_Dialogs_Generated_MOC_SRCS}
  ${SVWidgetsLib_Dialogs_Generated_UI_HDRS}
  ${SVWidgetsLib_Dialogs_Generated_RC_SRCS}
)
