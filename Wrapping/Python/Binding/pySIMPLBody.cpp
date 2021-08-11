instanceQSetQString.def(py::init<>([](py::set stringSet) {
  QSet<QString> newQStringSet = QSet<QString>();
  for(auto newString : stringSet)
  {
    newQStringSet.insert(py::cast<QString>(newString));
  }
  return newQStringSet;
}));

instanceQDateTime.def(py::init<>([](int year, int month, int day, int seconds) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  QDateTime dateTime(QDate(year, month, day));
#else
  QDateTime dateTime = QDate(year, month, day).startOfDay();
#endif
  dateTime.setTime_t(seconds);
  return dateTime;
}));

instanceQJsonArray.def(py::init<>([](py::list values) {
  QJsonArray qJsonArray;
  for(auto value : values)
  {
    QJsonValue qJsonValue(py::cast<double>(value));
    qJsonArray.push_back(qJsonValue);
  }
  return qJsonArray;
}));

instanceQListImportHDF5DatasetDatasetImportInfo.def(py::init<>([](py::list values) {
  QList<ImportHDF5Dataset::DatasetImportInfo> datasetImportInfoList;
  for(auto value : values)
  {
    py::list valueAsList = py::cast<py::list>(value);
    ImportHDF5Dataset::DatasetImportInfo datasetImportInfo;
    datasetImportInfo.dataSetPath = py::cast<QString>(valueAsList[0]);
    datasetImportInfo.componentDimensions = py::cast<QString>(valueAsList[1]);
    datasetImportInfoList.push_back(datasetImportInfo);
  }
  return datasetImportInfoList;
}));

instanceSIMPLComparisonEnumeration.value("LessThan", SIMPL::Comparison::Enumeration::Operator_LessThan)
    .value("GreaterThan", SIMPL::Comparison::Enumeration::Operator_GreaterThan)
    .value("Equal", SIMPL::Comparison::Enumeration::Operator_Equal)
    .value("NotEqual", SIMPL::Comparison::Enumeration::Operator_NotEqual)
    .value("Unknown", SIMPL::Comparison::Enumeration::Operator_Unknown)
    .export_values();

instanceSIMPLDelimiterTypeType.value("Comma", SIMPL::DelimiterTypes::Type::Comma)
    .value("Semicolon", SIMPL::DelimiterTypes::Type::Semicolon)
    .value("Colon", SIMPL::DelimiterTypes::Type::Colon)
    .value("Tab", SIMPL::DelimiterTypes::Type::Tab)
    .value("Space", SIMPL::DelimiterTypes::Type::Space);

instancePhaseTypeType.value("Primary", PhaseType::Type::Primary)
    .value("Precipitate", PhaseType::Type::Precipitate)
    .value("Transformation", PhaseType::Type::Transformation)
    .value("Matrix", PhaseType::Type::Matrix)
    .value("Boundary", PhaseType::Type::Boundary)
    .value("Unknown", PhaseType::Type::Unknown)
    .value("Any", PhaseType::Type::Any);

instanceShapeTypeType.value("Ellipsoid", ShapeType::Type::Ellipsoid)
    .value("SuperEllipsoid", ShapeType::Type::SuperEllipsoid)
    .value("CubeOctahedron", ShapeType::Type::CubeOctahedron)
    .value("CylinderA", ShapeType::Type::CylinderA)
    .value("CylinderB", ShapeType::Type::CylinderB)
    .value("CylinderC", ShapeType::Type::CylinderC)
    .value("ShapeTypeEnd", ShapeType::Type::ShapeTypeEnd)
    .value("Unknown", ShapeType::Type::Unknown)
    .value("Any", ShapeType::Type::Any);

instanceEnsembleInfoCrystalStructure.value("Hexagonal_High", EnsembleInfo::CrystalStructure::Hexagonal_High)
    .value("Cubic_High", EnsembleInfo::CrystalStructure::Cubic_High)
    .value("Hexagonal_Low", EnsembleInfo::CrystalStructure::Hexagonal_Low)
    .value("Cubic_Low", EnsembleInfo::CrystalStructure::Cubic_Low)
    .value("Triclinic", EnsembleInfo::CrystalStructure::Triclinic)
    .value("Monoclinic", EnsembleInfo::CrystalStructure::Monoclinic)
    .value("OrthoRhombic", EnsembleInfo::CrystalStructure::OrthoRhombic)
    .value("Tetragonal_Low", EnsembleInfo::CrystalStructure::Tetragonal_Low)
    .value("Tetragonal_High", EnsembleInfo::CrystalStructure::Tetragonal_High)
    .value("Trigonal_Low", EnsembleInfo::CrystalStructure::Trigonal_Low)
    .value("Trigonal_High", EnsembleInfo::CrystalStructure::Trigonal_High)
    .value("UnknownCrystalStructure", EnsembleInfo::CrystalStructure::UnknownCrystalStructure);

instanceSIMPLScalarTypesType.value("Int8", SIMPL::ScalarTypes::Type::Int8)
    .value("UInt8", SIMPL::ScalarTypes::Type::UInt8)
    .value("Int16", SIMPL::ScalarTypes::Type::Int16)
    .value("UInt16", SIMPL::ScalarTypes::Type::UInt16)
    .value("Int32", SIMPL::ScalarTypes::Type::Int32)
    .value("UInt32", SIMPL::ScalarTypes::Type::UInt32)
    .value("Int64", SIMPL::ScalarTypes::Type::Int64)
    .value("UInt64", SIMPL::ScalarTypes::Type::UInt64)
    .value("Float", SIMPL::ScalarTypes::Type::Float)
    .value("Double", SIMPL::ScalarTypes::Type::Double)
    .value("Bool", SIMPL::ScalarTypes::Type::Bool);

instanceCreateDataArrayInitializationChoices.value("Manual", CreateDataArray::InitializationChoices::Manual)
    .value("RandomWithRange", CreateDataArray::InitializationChoices::RandomWithRange)
    .export_values();

instanceArrayCalculatorAngleUnits.value("Radians", ArrayCalculator::AngleUnits::Radians).value("Degrees", ArrayCalculator::AngleUnits::Degrees).export_values();

instanceSIMPLNumericTypesType.value("Int8", SIMPL::NumericTypes::Type::Int8)
    .value("UInt8", SIMPL::NumericTypes::Type::UInt8)
    .value("Int16", SIMPL::NumericTypes::Type::Int16)
    .value("UInt16", SIMPL::NumericTypes::Type::UInt16)
    .value("Int32", SIMPL::NumericTypes::Type::Int32)
    .value("UInt32", SIMPL::NumericTypes::Type::UInt32)
    .value("Int64", SIMPL::NumericTypes::Type::Int64)
    .value("UInt64", SIMPL::NumericTypes::Type::UInt64)
    .value("Float", SIMPL::NumericTypes::Type::Float)
    .value("Double", SIMPL::NumericTypes::Type::Double)
    .value("Bool", SIMPL::NumericTypes::Type::Bool)
    .value("SizeT", SIMPL::NumericTypes::Type::SizeT)
    .value("UnknownNumType", SIMPL::NumericTypes::Type::UnknownNumType);

instanceQtCheckState.value("Unchecked", Qt::CheckState::Unchecked).value("PartiallyChecked", Qt::CheckState::PartiallyChecked).value("Checked", Qt::CheckState::Checked).export_values();

instanceSIMPLInfoStringFormat.value("HtmlFormat", SIMPL::InfoStringFormat::HtmlFormat).value("UnknownFormat", SIMPL::InfoStringFormat::UnknownFormat).export_values();

instanceAbstractFilter.def("connectObserver",
                           [](AbstractFilter& filter, Observer& observer) { QObject::connect(&filter, &AbstractFilter::messageGenerated, &observer, &Observer::processPipelineMessage); });
instanceAbstractFilter.def("disconnectObserver",
                           [](AbstractFilter& filter, Observer& observer) { QObject::disconnect(&filter, &AbstractFilter::messageGenerated, &observer, &Observer::processPipelineMessage); });

instanceAttributeMatrixCreationFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&>())
    .def_readwrite("dcGeometryTypes", &AttributeMatrixCreationFilterParameter::RequirementType::dcGeometryTypes);

instanceAttributeMatrixSelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&, const AttributeMatrix::Types&>())
    .def_readwrite("dcGeometryTypes", &AttributeMatrixSelectionFilterParameter::RequirementType::dcGeometryTypes)
    .def_readwrite("amTypes", &AttributeMatrixSelectionFilterParameter::RequirementType::amTypes);

instanceDataArrayCreationFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&, const AttributeMatrix::Types&>())
    .def_readwrite("dcGeometryTypes", &DataArrayCreationFilterParameter::RequirementType::dcGeometryTypes)
    .def_readwrite("amTypes", &DataArrayCreationFilterParameter::RequirementType::amTypes);

instanceDataArraySelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&, const AttributeMatrix::Types&, const std::vector<QString>&, const std::vector<std::vector<size_t>>&>())
    .def_readwrite("dcGeometryTypes", &DataArraySelectionFilterParameter::RequirementType::dcGeometryTypes)
    .def_readwrite("amTypes", &DataArraySelectionFilterParameter::RequirementType::amTypes)
    .def_readwrite("daTypes", &DataArraySelectionFilterParameter::RequirementType::daTypes)
    .def_readwrite("componentDimensions", &DataArraySelectionFilterParameter::RequirementType::componentDimensions);

instanceDataContainerSelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&>())
    .def_readwrite("dcGeometryTypes", &DataContainerSelectionFilterParameter::RequirementType::dcGeometryTypes);

instanceLinkedDataContainerSelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&>())
    .def_readwrite("dcGeometryTypes", &LinkedDataContainerSelectionFilterParameter::RequirementType::dcGeometryTypes);

instanceMultiAttributeMatrixSelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&, const AttributeMatrix::Types&, const std::vector<QString>&, const std::vector<std::vector<size_t>>&>())
    .def_readwrite("dcGeometryTypes", &MultiAttributeMatrixSelectionFilterParameter::RequirementType::dcGeometryTypes)
    .def_readwrite("amTypes", &MultiAttributeMatrixSelectionFilterParameter::RequirementType::amTypes)
    .def_readwrite("daTypes", &MultiAttributeMatrixSelectionFilterParameter::RequirementType::daTypes)
    .def_readwrite("componentDimensions", &MultiAttributeMatrixSelectionFilterParameter::RequirementType::componentDimensions);

instanceMultiDataArraySelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&, const AttributeMatrix::Types&, const std::vector<QString>&, const std::vector<std::vector<size_t>>&>())
    .def_readwrite("dcGeometryTypes", &MultiDataArraySelectionFilterParameter::RequirementType::dcGeometryTypes)
    .def_readwrite("amTypes", &MultiDataArraySelectionFilterParameter::RequirementType::amTypes)
    .def_readwrite("daTypes", &MultiDataArraySelectionFilterParameter::RequirementType::daTypes)
    .def_readwrite("componentDimensions", &MultiDataArraySelectionFilterParameter::RequirementType::componentDimensions);

instanceMultiDataContainerSelectionFilterParameterRequirementType.def(py::init<>())
    .def(py::init<const IGeometry::Types&, const AttributeMatrix::Types&, const std::vector<QString>&, const std::vector<std::vector<size_t>>&>())
    .def_readwrite("dcGeometryTypes", &MultiDataContainerSelectionFilterParameter::RequirementType::dcGeometryTypes)
    .def_readwrite("amTypes", &MultiDataContainerSelectionFilterParameter::RequirementType::amTypes)
    .def_readwrite("daTypes", &MultiDataContainerSelectionFilterParameter::RequirementType::daTypes)
    .def_readwrite("componentDimensions", &MultiDataContainerSelectionFilterParameter::RequirementType::componentDimensions);

instanceDataContainerGrid.def(py::init<SizeVec3Type>()).def(py::init<SizeVec3Type, const std::vector<QString>&>());

registerDataContainerArray(instanceDataContainerArray);
registerDataContainer(instanceDataContainer);
registerAttributeMatrix(instanceAttributeMatrix);
registerDataArrayPath(instanceDataArrayPath);

#ifdef SIMPL_EMBED_PYTHON
py::class_<PythonSupport::FilterDelegate>(mod, "FilterDelegateCpp")
    .def("notifyStatusMessage", &PythonSupport::FilterDelegate::notifyStatusMessage)
    .def("notifyProgressMessage", &PythonSupport::FilterDelegate::notifyProgressMessage)
    .def("setWarningCondition", &PythonSupport::FilterDelegate::setWarningCondition)
    .def_property_readonly("preflight", &PythonSupport::FilterDelegate::preflight);

py::class_<PythonFilter, AbstractFilter, std::shared_ptr<PythonFilter>>(mod, "PythonFilter").def(py::init([](py::object object) { return PythonFilter::New(object); }));

mod.def("registerPythonFilter", [](py::object object) {
  auto filterManager = FilterManager::Instance();
  auto filterFactory = PythonFilterFactory::New(object);
  filterManager->addPythonFilterFactory(filterFactory->getFilterClassName(), filterFactory);
});

mod.def("unregisterPythonFilter", [](const std::string& uuidStr) {
  QUuid uuid(QString::fromStdString(uuidStr));
  auto filterManager = FilterManager::Instance();
  if(!filterManager->isPythonFilter(uuid))
  {
    return false;
  }
  return filterManager->removeFilterFactory(uuid);
});

mod.def("clearPythonFilters", []() {
  auto filterManager = FilterManager::Instance();
  filterManager->clearPythonFilterFactories();
});
#endif
