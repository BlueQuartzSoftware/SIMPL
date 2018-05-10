
set(SVWidgetsLib_Animations_HDRS "")
set(SVWidgetsLib_Animations_SRCS "")
set(SVWidgetsLib_Animations_UIS "")


# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(SVWidgetsLib_Animations_MOC_HDRS
  ${SVWidgetsLib_SOURCE_DIR}/Animations/PipelineItemBorderSizeAnimation.h
  ${SVWidgetsLib_SOURCE_DIR}/Animations/PipelineItemHeightAnimation.h
  ${SVWidgetsLib_SOURCE_DIR}/Animations/PipelineItemSlideAnimation.h
)

set(SVWidgetsLib_Animations_HDRS

)

set(SVWidgetsLib_Animations_SRCS
  ${SVWidgetsLib_SOURCE_DIR}/Animations/PipelineItemBorderSizeAnimation.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Animations/PipelineItemHeightAnimation.cpp
  ${SVWidgetsLib_SOURCE_DIR}/Animations/PipelineItemSlideAnimation.cpp
  )

cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib/Animations" "${SVWidgetsLib_Animations_MOC_HDRS};${SVWidgetsLib_Animations_HDRS}" "${SVWidgetsLib_Animations_SRCS}" "0")

# --------------------------------------------------------------------
# and finally this will run moc:
# QT5_WRAP_CPP( SVWidgetsLib_Animations_Generated_MOC_SRCS ${SVWidgetsLib_Animations_MOC_HDRS} )
# set_source_files_properties( ${SVWidgetsLib_Animations_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SVWidgetsLib_Animations_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_Animations_Generated_MOC_SRCS}" "0")

set(SVWidgets_Animations_Files
  ${SVWidgetsLib_Animations_MOC_HDRS}
  ${SVWidgetsLib_Animations_SRCS}
  ${SVWidgetsLib_Animations_HDRS}

  ${SVWidgetsLib_Animations_Generated_MOC_SRCS}
)


