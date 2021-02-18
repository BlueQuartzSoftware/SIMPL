#pragma once

#include <functional>
#include <string>
#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/FilterManager.h"

namespace PythonLoader
{
// error message, file
using ErrorCallback = std::function<void(const std::string&, const std::string&)>;
// class, file
using LoadedCallback = std::function<void(const std::string&, const std::string&)>;

SIMPLib_EXPORT std::vector<std::string> defaultPythonFilterPaths();

SIMPLib_EXPORT void loadPythonFilters(FilterManager& filterManager, const std::vector<std::string>& paths = defaultPythonFilterPaths(), ErrorCallback errorCallBack = {},
                                      LoadedCallback loadedCallback = {});

SIMPLib_EXPORT bool checkPythonHome();

SIMPLib_EXPORT bool setPythonHome(const std::string& value);
} // namespace PythonLoader
