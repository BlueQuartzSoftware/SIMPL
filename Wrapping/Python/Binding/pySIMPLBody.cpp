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

py::implicitly_convertible<QString, DataArrayPath>();

py::class_<AxisAngleInput_t>(mod, "AxisAngleInput")
    .def(py::init<float, float, float, float>())
    .def_readwrite("angle", &AxisAngleInput_t::angle)
    .def_readwrite("h", &AxisAngleInput_t::h)
    .def_readwrite("k", &AxisAngleInput_t::k)
    .def_readwrite("l", &AxisAngleInput_t::l);

py::class_<StackFileListInfo>(mod, "FileListInfo")
    .def(py::init<>([](int32_t paddingDigits, uint32_t ordering, int32_t startIndex, int32_t endIndex, int32_t incrementIndex, const QString& inputPath, const QString& filePrefix,
                       const QString& fileSuffix, const QString& fileExtension) {
      StackFileListInfo fileListInfo;
      fileListInfo.PaddingDigits = paddingDigits;
      fileListInfo.Ordering = ordering;
      fileListInfo.StartIndex = startIndex;
      fileListInfo.EndIndex = endIndex;
      fileListInfo.IncrementIndex = incrementIndex;
      fileListInfo.InputPath = inputPath;
      fileListInfo.FilePrefix = filePrefix;
      fileListInfo.FileSuffix = fileSuffix;
      fileListInfo.FileExtension = fileExtension;
      return fileListInfo;
    }));

py::class_<QSet<QString>>(mod, "StringSet").def(py::init<>([](py::set stringSet) {
  QSet<QString> newQStringSet = QSet<QString>();
  for(auto newString : stringSet)
  {
    newQStringSet.insert(py::cast<QString>(newString));
  }
  return newQStringSet;
}));

py::class_<QDateTime>(mod, "DateTime").def(py::init<>([](int year, int month, int day, int seconds) {
  QDateTime dateTime(QDate(year, month, day));
  dateTime.setTime_t(seconds);
  return dateTime;
}));

py::class_<QJsonArray>(mod, "JsonArray").def(py::init<>([](py::list values) {
  QJsonArray qJsonArray;
  for(auto value : values)
  {
    QJsonValue qJsonValue(py::cast<double>(value));
    qJsonArray.push_back(qJsonValue);
  }
  return qJsonArray;
}));

py::class_<QList<ImportHDF5Dataset::DatasetImportInfo>>(mod, "DatasetImportInfoList").def(py::init<>([](py::list values) {
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

py::bind_vector<std::vector<int8_t>>(mod, "VectorInt8");
py::bind_vector<std::vector<uint8_t>>(mod, "VectorUInt8");

py::bind_vector<std::vector<int16_t>>(mod, "VectorInt16");
py::bind_vector<std::vector<uint16_t>>(mod, "VectorUInt16");

py::bind_vector<std::vector<int32_t>>(mod, "VectorInt32");
py::bind_vector<std::vector<uint32_t>>(mod, "VectorUInt32");

py::bind_vector<std::vector<int64_t>>(mod, "VectorInt64");
py::bind_vector<std::vector<uint64_t>>(mod, "VectorUInt64");

py::bind_vector<std::vector<float>>(mod, "VectorFloat");
py::bind_vector<std::vector<double>>(mod, "VectorDouble");

if(py::detail::get_type_info(typeid(std::vector<size_t>)))
{
  mod.attr("VectorSizeT") = mod.attr("VectorUInt64");
}
else
{
  py::bind_vector<std::vector<size_t>>(mod, "VectorSizeT");
}

py::enum_<SIMPL::Comparison::Enumeration>(mod, "ComparisonOperators")
    .value("LessThan", SIMPL::Comparison::Enumeration::Operator_LessThan)
    .value("GreaterThan", SIMPL::Comparison::Enumeration::Operator_GreaterThan)
    .value("Equal", SIMPL::Comparison::Enumeration::Operator_Equal)
    .value("NotEqual", SIMPL::Comparison::Enumeration::Operator_NotEqual)
    .value("Unknown", SIMPL::Comparison::Enumeration::Operator_Unknown)
    .export_values();

py::enum_<SIMPL::DelimiterTypes::Type>(mod, "DelimiterTypes")
    .value("Comma", SIMPL::DelimiterTypes::Type::Comma)
    .value("Semicolon", SIMPL::DelimiterTypes::Type::Semicolon)
    .value("Colon", SIMPL::DelimiterTypes::Type::Colon)
    .value("Tab", SIMPL::DelimiterTypes::Type::Tab)
    .value("Space", SIMPL::DelimiterTypes::Type::Space);

py::enum_<PhaseType::Type>(mod, "PhaseType")
    .value("Primary", PhaseType::Type::Primary)
    .value("Precipitate", PhaseType::Type::Precipitate)
    .value("Transformation", PhaseType::Type::Transformation)
    .value("Matrix", PhaseType::Type::Matrix)
    .value("Boundary", PhaseType::Type::Boundary)
    .value("Unknown", PhaseType::Type::Unknown)
    .value("Any", PhaseType::Type::Any);

py::enum_<ShapeType::Type>(mod, "ShapeType")
    .value("Ellipsoid", ShapeType::Type::Ellipsoid)
    .value("SuperEllipsoid", ShapeType::Type::SuperEllipsoid)
    .value("CubeOctahedron", ShapeType::Type::CubeOctahedron)
    .value("CylinderA", ShapeType::Type::CylinderA)
    .value("CylinderB", ShapeType::Type::CylinderB)
    .value("CylinderC", ShapeType::Type::CylinderC)
    .value("ShapeTypeEnd", ShapeType::Type::ShapeTypeEnd)
    .value("Unknown", ShapeType::Type::Unknown)
    .value("Any", ShapeType::Type::Any);

py::enum_<EnsembleInfo::CrystalStructure>(mod, "CrystalStructure")
    .value("Hexagonal_High", EnsembleInfo::CrystalStructure::Hexagonal_High)
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

py::enum_<SIMPL::ScalarTypes::Type>(mod, "ScalarTypes")
    .value("Int8", SIMPL::ScalarTypes::Type::Int8)
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

py::enum_<CreateDataArray::InitializationChoices>(mod, "InitializationType")
    .value("Manual", CreateDataArray::InitializationChoices::Manual)
    .value("RandomWithRange", CreateDataArray::InitializationChoices::RandomWithRange)
    .export_values();

py::enum_<ArrayCalculator::AngleUnits>(mod, "AngleUnits").value("Radians", ArrayCalculator::AngleUnits::Radians).value("Degrees", ArrayCalculator::AngleUnits::Degrees).export_values();

py::enum_<SIMPL::NumericTypes::Type>(mod, "NumericTypes")
    .value("Int8", SIMPL::NumericTypes::Type::Int8)
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

py::enum_<SIMPL::InfoStringFormat>(mod, "InfoStringFormat").value("HtmlFormat", SIMPL::InfoStringFormat::HtmlFormat).value("UnknownFormat", SIMPL::InfoStringFormat::UnknownFormat).export_values();
