py::bind_vector<std::vector<int8_t>>(mod, "VectorInt8", "std::vector<int8_t> binding");
py::bind_vector<std::vector<uint8_t>>(mod, "VectorUInt8", "std::vector<uint8_t> binding");

py::bind_vector<std::vector<int16_t>>(mod, "VectorInt16", "std::vector<int16_t> binding");
py::bind_vector<std::vector<uint16_t>>(mod, "VectorUInt16", "std::vector<uint16_t> binding");

py::bind_vector<std::vector<int32_t>>(mod, "VectorInt32", "std::vector<int32_t> binding");
py::bind_vector<std::vector<uint32_t>>(mod, "VectorUInt32", "std::vector<uint32_t> binding");

py::bind_vector<std::vector<int64_t>>(mod, "VectorInt64", "std::vector<int64_t> binding");
py::bind_vector<std::vector<uint64_t>>(mod, "VectorUInt64", "std::vector<uint64_t> binding");

py::bind_vector<std::vector<float>>(mod, "VectorFloat", "std::vector<float> binding");
py::bind_vector<std::vector<double>>(mod, "VectorDouble", "std::vector<double> binding");

if(py::detail::get_type_info(typeid(std::vector<size_t>)))
{
  mod.attr("VectorSizeT") = mod.attr("VectorUInt64");
}
else
{
  py::bind_vector<std::vector<size_t>>(mod, "VectorSizeT", "std::vector<size_t> binding");
}

py::class_<Float3rdOrderPoly_t> instanceFloat3rdOrderPoly(mod, "Float3rdOrderPoly");

instanceFloat3rdOrderPoly.def(py::init<float, float, float, float, float, float, float, float, float, float>());

py::enum_<RenameErrorCodes> instanceRenameErrorCodes(mod, "RenameErrorCodes");

instanceRenameErrorCodes.value("OLD_DOES_NOT_EXIST", RenameErrorCodes::OLD_DOES_NOT_EXIST)
    .value("SUCCESS", RenameErrorCodes::SUCCESS)
    .value("NEW_EXISTS", RenameErrorCodes::NEW_EXISTS)
    .value("SAME_PATH", RenameErrorCodes::SAME_PATH)
    .export_values();

py::class_<DataContainerGrid> instanceDataContainerGrid(mod, "DataContainerGrid");
