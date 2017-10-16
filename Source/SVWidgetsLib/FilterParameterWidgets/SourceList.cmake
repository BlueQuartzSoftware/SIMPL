
set(SVWidgetsLib_FilterParameterWidgets_HDRS "")
set(SVWidgetsLib_FilterParameterWidgets_SRCS "")
set(SVWidgetsLib_FilterParameterWidgets_UIS "")


set(SIMPLView_PARAMETER_WIDGETS
    AttributeMatrixCreationWidget
    AttributeMatrixSelectionWidget
    AxisAngleWidget
    BooleanWidget
    ChoiceWidget
    ComparisonSelectionAdvancedWidget
    ComparisonSelectionWidget
    ConstrainedDoubleWidget
    ConstrainedIntWidget
    DataArrayCreationWidget
    DataArraySelectionWidget
    DataContainerArrayProxyWidget
    DataContainerCreationWidget
    DataContainerReaderWidget
    DataContainerSelectionWidget
    DoubleWidget
    DynamicChoiceWidget
    DynamicTableWidget
    FileListInfoWidget
    FloatVec2Widget
    FloatVec3Widget
    FloatWidget
    FourthOrderPolynomialWidget
    GenerateColorTableWidget
    IntVec3Widget
    IntWidget
    LinkedBooleanWidget
    MultiAttributeMatrixSelectionWidget
    MultiDataArraySelectionWidget
    NumericTypeWidget
    PreflightUpdatedValueWidget
    RangeWidget
    ScalarTypeWidget
    SecondOrderPolynomialWidget
    SeparatorWidget
    StringWidget
    ThirdOrderPolynomialWidget
)

set(SIMPLView_PARAMETER_WIDGETS_NO_UI
  InputFileWidget
  InputPathWidget
  OutputFileWidget
  OutputPathWidget
)

set(SIMPLView_PARAMETER_WIDGETS_NO_CODEGEN
  AbstractIOFileWidget
  CalculatorWidget
  ImportHDF5FileWidget
  LinkedDataContainerSelectionWidget
  PhaseTypeSelectionWidget
  ReadASCIIDataWidget
  ShapeTypeSelectionWidget
  UnknownWidget
)

set(REGISTER_KNOWN_WIDGETS ${SVWidgetsLib_BINARY_DIR}/FilterWidgetManager_RegisterWidgets_tmp.cpp)
set(FILTER_PARAMETER_CODEGEN_FILE ${SVWidgetsLib_BINARY_DIR}/Filter_Parameter_CodeGenFactory.cpp)
set(FILTER_PARAMETER_CODEGEN_INCLUDES_FILE ${SVWidgetsLib_BINARY_DIR}/Filter_Parameter_CodeGenFactory_Includes.cpp)
set(FILTER_PARAMETER_TYPELIST_FILE ${SVWidgetsLib_BINARY_DIR}/Filter_Parameter_TypeList.cpp)
set(FILTER_WIDGET_HEADERS ${SVWidgetsLib_BINARY_DIR}/FilterWidgetHeaders_tmp.h)

file(WRITE ${REGISTER_KNOWN_WIDGETS}  "/* This file is automatically generated. DO NOT EDIT this file. It will be over written */\n\n")
file(WRITE ${FILTER_PARAMETER_CODEGEN_FILE}  "/* This file is automatically generated. DO NOT EDIT this file. It will be over written */\n\n")
file(WRITE ${FILTER_PARAMETER_CODEGEN_INCLUDES_FILE}  "/* This file is automatically generated. DO NOT EDIT this file. It will be over written */\n\n")
file(WRITE ${FILTER_PARAMETER_TYPELIST_FILE}  "/* This file is automatically generated. DO NOT EDIT this file. It will be over written */\n\n")
file(WRITE ${FILTER_WIDGET_HEADERS}  "/* This file is automatically generated. DO NOT EDIT this file. It will be over written */\n\n")

file(APPEND  ${FILTER_PARAMETER_TYPELIST_FILE} "QList<QString> list;\n")

set(loopCounter 1)
foreach(FPW ${SIMPLView_PARAMETER_WIDGETS})
  # set(SVWidgetsLib_FilterParameterWidgets_MOC_HDRS ${SVWidgetsLib_FilterParameterWidgets_MOC_HDRS}
  #   ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.h
  #   )
  # set(SVWidgetsLib_FilterParameterWidgets_SRCS ${SVWidgetsLib_FilterParameterWidgets_SRCS}
  #   ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.cpp
  #   )
  # set(SVWidgetsLib_FilterParameterWidgets_UIS ${SVWidgetsLib_FilterParameterWidgets_UIS}
  #   ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/UI_Files/${FPW}.ui
  #   )
  list(APPEND SVWidgetsLib_FilterParameterWidgets_MOC_HDRS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.h)
  list(APPEND SVWidgetsLib_FilterParameterWidgets_SRCS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.cpp)
  list(APPEND SVWidgetsLib_FilterParameterWidgets_UIS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/UI_Files/${FPW}.ui)

  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "{\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "  PipelineFilterWidgetFactory<${FPW}>::Pointer factory = PipelineFilterWidgetFactory<${FPW}>::New();\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "  idManager->addFilterWidgetFactory( \"${FPW}\", factory );\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "}\n")
  file(APPEND  ${FILTER_WIDGET_HEADERS} "#include \"SVWidgetsLib/FilterParameterWidgets/${FPW}.h\"\n")

  file(APPEND  ${FILTER_PARAMETER_CODEGEN_INCLUDES_FILE} "#include \"DevHelper/CodeGenerators/${FPW}CodeGenerator.h\"\n")

  file(APPEND  ${FILTER_PARAMETER_TYPELIST_FILE} "list.push_back(\"${FPW}\");\n")

  if (loopCounter EQUAL 1)
    file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "if (fpType == \"${FPW}\") {\n")
  else()
    file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "else if (fpType == \"${FPW}\") {\n")
  endif()

  file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "  ${FPW}CodeGenerator::Pointer ptr = ${FPW}CodeGenerator::New(humanLabel, propertyName, fpCategory, initValue);\n")
      file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "  return ptr;\n}\n\n")

  set(loopCounter loopCounter+1)
endforeach()

foreach(FPW ${SIMPLView_PARAMETER_WIDGETS_NO_UI})
  list(APPEND SVWidgetsLib_FilterParameterWidgets_MOC_HDRS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.h)
  list(APPEND SVWidgetsLib_FilterParameterWidgets_SRCS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.cpp)

  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "{\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "  PipelineFilterWidgetFactory<${FPW}>::Pointer factory = PipelineFilterWidgetFactory<${FPW}>::New();\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "  idManager->addFilterWidgetFactory( \"${FPW}\", factory );\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "}\n")
  file(APPEND  ${FILTER_WIDGET_HEADERS} "#include \"SVWidgetsLib/FilterParameterWidgets/${FPW}.h\"\n")

  file(APPEND  ${FILTER_PARAMETER_CODEGEN_INCLUDES_FILE} "#include \"DevHelper/CodeGenerators/${FPW}CodeGenerator.h\"\n")

  file(APPEND  ${FILTER_PARAMETER_TYPELIST_FILE} "list.push_back(\"${FPW}\");\n")

  if (loopCounter EQUAL 1)
    file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "if (fpType == \"${FPW}\") {\n")
  else()
    file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "else if (fpType == \"${FPW}\") {\n")
  endif()

  file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "  ${FPW}CodeGenerator::Pointer ptr = ${FPW}CodeGenerator::New(humanLabel, propertyName, fpCategory, initValue);\n")
      file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "  return ptr;\n}\n\n")

  set(loopCounter loopCounter+1)
endforeach()

foreach(FPW ${SIMPLView_PARAMETER_WIDGETS_NO_CODEGEN})

  list(APPEND SVWidgetsLib_FilterParameterWidgets_MOC_HDRS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.h)
  list(APPEND SVWidgetsLib_FilterParameterWidgets_SRCS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/${FPW}.cpp)
  list(APPEND SVWidgetsLib_FilterParameterWidgets_UIS ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/UI_Files/${FPW}.ui)

  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "{\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "  PipelineFilterWidgetFactory<${FPW}>::Pointer factory = PipelineFilterWidgetFactory<${FPW}>::New();\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "  idManager->addFilterWidgetFactory( \"${FPW}\", factory );\n")
  file(APPEND  ${REGISTER_KNOWN_WIDGETS} "}\n")
  file(APPEND  ${FILTER_WIDGET_HEADERS} "#include \"SVWidgetsLib/FilterParameterWidgets/${FPW}.h\"\n")
endforeach()

file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "else {\n")
file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "  FPCodeGenerator::Pointer ptr = FPCodeGenerator::New(humanLabel, propertyName, fpCategory, initValue);\n")
file(APPEND ${FILTER_PARAMETER_CODEGEN_FILE} "  return ptr;\n}\n\n")

file(APPEND  ${FILTER_PARAMETER_TYPELIST_FILE} "qSort(list.begin(), list.end());\nreturn list;\n")

cmpReplaceFileIfDifferent(OLD_FILE_PATH ${SVWidgetsLib_BINARY_DIR}/FilterWidgetManager_RegisterWidgets.cpp
                          NEW_FILE_PATH ${SVWidgetsLib_BINARY_DIR}/FilterWidgetManager_RegisterWidgets_tmp.cpp)
cmpReplaceFileIfDifferent(OLD_FILE_PATH ${SVWidgetsLib_BINARY_DIR}/FilterWidgetHeaders.h
                          NEW_FILE_PATH ${SVWidgetsLib_BINARY_DIR}/FilterWidgetHeaders_tmp.h)


# --------------------------------------------------------------------
# Add some support files that still need MOC and UIC to be run on them
set(SVWidgetsLib_FilterParameterWidgets_MOC_HDRS
  ${SVWidgetsLib_FilterParameterWidgets_MOC_HDRS}
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/FilterParameterWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonSelectionTableModel.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonSelectionItemDelegate.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/DynamicTableItemDelegate.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonContainerWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/IComparisonWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonSetWidget.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonValueWidget.h
)

set(SVWidgetsLib_FilterParameterWidgets_HDRS
  ${SVWidgetsLib_FilterParameterWidgets_MOC_HDRS}
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/FilterParameterWidgetsDialogs.h
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/FilterParameterWidgetUtils.hpp
)


set(SVWidgetsLib_FilterParameterWidgets_SRCS
  ${SVWidgetsLib_FilterParameterWidgets_SRCS}
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/FilterParameterWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonSelectionTableModel.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonSelectionItemDelegate.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/DynamicTableItemDelegate.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/FilterParameterWidgetsDialogs.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonContainerWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/IComparisonWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonSetWidget.cpp
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/ComparisonValueWidget.cpp
)
set(SVWidgetsLib_FilterParameterWidgets_UIS
  ${SVWidgetsLib_FilterParameterWidgets_UIS}
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/UI_Files/ComparisonContainerWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/UI_Files/ComparisonSetWidget.ui
  ${SVWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/UI_Files/ComparisonValueWidget.ui
)


cmp_IDE_SOURCE_PROPERTIES( "SVWidgetsLib/FilterParameterWidgets" "${SVWidgetsLib_FilterParameterWidgets_HDRS}" "${SVWidgetsLib_FilterParameterWidgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("SVWidgetsLib/FilterParameterWidgets/UI_Files" "${SVWidgetsLib_FilterParameterWidgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( SVWidgetsLib_ParameterWidgets_Generated_MOC_SRCS ${SVWidgetsLib_FilterParameterWidgets_MOC_HDRS} )
set_source_files_properties( ${SVWidgetsLib_ParameterWidgets_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${SVWidgetsLib_ParameterWidgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


# -- Run MOC and UIC on the necessary files
# QT5_ADD_RESOURCES( SVWidgetsLib_Generated_RC_SRCS "${SIMPLViewProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( SVWidgetsLib_ParameterWidgets_Generated_UI_HDRS ${SVWidgetsLib_FilterParameterWidgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${SVWidgetsLib_ParameterWidgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${SVWidgetsLib_ParameterWidgets_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${SVWidgetsLib_Generated_RC_SRCS}" "" "0")

set(SVWidgetsLib_ParameterFilterWidgets_Files
  ${SVWidgetsLib_FilterParameterWidgets_HDRS}
  ${SVWidgetsLib_FilterParameterWidgets_SRCS}
  ${SVWidgetsLib_FilterParameterWidgets_MOC_HDRS}
  ${SVWidgetsLib_ParameterWidgets_Generated_MOC_SRCS}
  ${SVWidgetsLib_ParameterWidgets_Generated_UI_HDRS}
)


