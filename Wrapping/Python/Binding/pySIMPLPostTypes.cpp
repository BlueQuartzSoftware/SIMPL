registerSIMPLArray<float, 2>(mod, "FloatVec2");
registerSIMPLArray<int32_t, 2>(mod, "IntVec2");
registerSIMPLArray<size_t, 2>(mod, "SizeVec2");

registerSIMPLArray<float, 3>(mod, "FloatVec3");
registerSIMPLArray<int32_t, 3>(mod, "IntVec3");
registerSIMPLArray<size_t, 3>(mod, "SizeVec3");

registerSIMPLArray<float, 4>(mod, "FloatVec4");
registerSIMPLArray<int32_t, 4>(mod, "IntVec4");
registerSIMPLArray<size_t, 4>(mod, "SizeVec4");

registerSIMPLArray<float, 6>(mod, "FloatVec6");
registerSIMPLArray<int32_t, 6>(mod, "IntVec6");

registerDataArray<int8_t>(mod, "Int8Array");
registerDataArray<uint8_t>(mod, "UInt8Array");

registerDataArray<int16_t>(mod, "Int16Array");
registerDataArray<uint16_t>(mod, "UInt16Array");

registerDataArray<int32_t>(mod, "Int32Array");
registerDataArray<uint32_t>(mod, "UInt32Array");

registerDataArray<int64_t>(mod, "Int64Array");
registerDataArray<uint64_t>(mod, "UInt64Array");

registerDataArray<float>(mod, "FloatArray");
registerDataArray<double>(mod, "DoubleArray");

registerDataArray<bool>(mod, "BoolArray");

py::implicitly_convertible<QString, DataArrayPath>();

py::class_<QSet<QString>> instanceQSetQString(mod, "StringSet");

py::class_<QDateTime> instanceQDateTime(mod, "DateTime");

py::class_<QJsonArray> instanceQJsonArray(mod, "JsonArray");

py::class_<QList<ImportHDF5Dataset::DatasetImportInfo>> instanceQListImportHDF5DatasetDatasetImportInfo(mod, "DatasetImportInfoList");

py::enum_<SIMPL::Comparison::Enumeration> instanceSIMPLComparisonEnumeration(mod, "ComparisonOperators");

py::enum_<SIMPL::DelimiterTypes::Type> instanceSIMPLDelimiterTypeType(mod, "DelimiterTypes");

py::enum_<PhaseType::Type> instancePhaseTypeType(mod, "PhaseType");

py::enum_<ShapeType::Type> instanceShapeTypeType(mod, "ShapeType");

py::enum_<EnsembleInfo::CrystalStructure> instanceEnsembleInfoCrystalStructure(mod, "CrystalStructure");

py::enum_<SIMPL::ScalarTypes::Type> instanceSIMPLScalarTypesType(mod, "ScalarTypes");

py::enum_<CreateDataArray::InitializationChoices> instanceCreateDataArrayInitializationChoices(mod, "InitializationType");

py::enum_<ArrayCalculator::AngleUnits> instanceArrayCalculatorAngleUnits(mod, "AngleUnits");

py::enum_<SIMPL::NumericTypes::Type> instanceSIMPLNumericTypesType(mod, "NumericTypes");

py::enum_<SIMPL::InfoStringFormat> instanceSIMPLInfoStringFormat(mod, "InfoStringFormat");

py::class_<AttributeMatrixCreationFilterParameter::RequirementType> instanceAttributeMatrixCreationFilterParameterRequirementType(instanceAttributeMatrixCreationFilterParameter, "RequirementType");

py::class_<AttributeMatrixSelectionFilterParameter::RequirementType> instanceAttributeMatrixSelectionFilterParameterRequirementType(instanceAttributeMatrixSelectionFilterParameter, "RequirementType");

py::class_<DataArrayCreationFilterParameter::RequirementType> instanceDataArrayCreationFilterParameterRequirementType(instanceDataArrayCreationFilterParameter, "RequirementType");

py::class_<DataArraySelectionFilterParameter::RequirementType> instanceDataArraySelectionFilterParameterRequirementType(instanceDataArraySelectionFilterParameter, "RequirementType");

py::class_<DataContainerSelectionFilterParameter::RequirementType> instanceDataContainerSelectionFilterParameterRequirementType(instanceDataContainerSelectionFilterParameter, "RequirementType");

py::class_<LinkedDataContainerSelectionFilterParameter::RequirementType> instanceLinkedDataContainerSelectionFilterParameterRequirementType(instanceLinkedDataContainerSelectionFilterParameter,
                                                                                                                                            "RequirementType");

py::class_<MultiAttributeMatrixSelectionFilterParameter::RequirementType> instanceMultiAttributeMatrixSelectionFilterParameterRequirementType(instanceMultiAttributeMatrixSelectionFilterParameter,
                                                                                                                                              "RequirementType");

py::class_<MultiDataArraySelectionFilterParameter::RequirementType> instanceMultiDataArraySelectionFilterParameterRequirementType(instanceMultiDataArraySelectionFilterParameter, "RequirementType");

py::class_<MultiDataContainerSelectionFilterParameter::RequirementType> instanceMultiDataContainerSelectionFilterParameterRequirementType(instanceMultiDataContainerSelectionFilterParameter,
                                                                                                                                          "RequirementType");

py::enum_<Qt::CheckState> instanceQtCheckState(mod, "QtCheckState");
