/* ============================================================================
 * Copyright (c) 2021 BlueQuartz Software, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLib/Python/PythonLoader.h"

#include <optional>
#include <unordered_set>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QtGlobal>

// undef slots since a Python header uses slots
#undef slots

#include <pybind11/embed.h>

#include "SIMPLib/Python/FilterPyObject.h"
#include "SIMPLib/Python/PythonFilterFactory.hpp"

namespace
{
constexpr char k_PYTHONHOME[] = "PYTHONHOME";
#ifdef _WIN32
constexpr char k_Separator = ';';
#else
constexpr char k_Separator = ':';
#endif

#if defined(_WIN32)
// -----------------------------------------------------------------------------
std::string WindowsPythonPath()
{
  return QString("%1/Python").arg(QCoreApplication::applicationDirPath()).toStdString();
}
#elif defined(__APPLE__)
// -----------------------------------------------------------------------------
std::string MacOSPythonPath()
{
  return "";
}
#else
// -----------------------------------------------------------------------------
std::string LinuxPythonPath()
{
  return "";
}
#endif

// -----------------------------------------------------------------------------
std::string PlatformDependentPythonPath()
{
#if defined(_WIN32)
  return WindowsPythonPath();
#elif defined(__APPLE__)
  return MacOSPythonPath();
#else
  return LinuxPythonPath();
#endif
}

} // namespace

// -----------------------------------------------------------------------------
bool PythonLoader::checkPythonHome()
{
  return !qEnvironmentVariable(k_PYTHONHOME).isEmpty();
}

// -----------------------------------------------------------------------------
std::vector<std::string> PythonLoader::defaultPythonFilterPaths()
{
  std::vector<std::string> paths{PlatformDependentPythonPath()};
  QString pythonPath = qEnvironmentVariable("SIMPL_PYTHONPATH");
  if(!pythonPath.isEmpty())
  {
    QStringList pyPaths = pythonPath.split(k_Separator);
    for(const QString& path : pyPaths)
    {
      paths.push_back(path.toStdString());
    }
  }

  return paths;
}

// -----------------------------------------------------------------------------
std::string PythonLoader::defaultSIMPLPythonLibPath()
{
  return QCoreApplication::applicationDirPath().toStdString();
}

// -----------------------------------------------------------------------------
bool PythonLoader::setPythonHome(const std::string& value)
{
  return qputenv(k_PYTHONHOME, value.c_str());
}

// -----------------------------------------------------------------------------
void PythonLoader::addToPythonPath(const std::string& value)
{
  pybind11::gil_scoped_acquire gil_acquire_guard{};
  pybind11::module_::import("sys").attr("path").cast<pybind11::list>().append(value);
}

// -----------------------------------------------------------------------------
size_t PythonLoader::loadPythonFilters(FilterManager& filterManager, const std::vector<std::string>& paths, ErrorCallback errorCallBack, LoadedCallback loadedCallback)
{
  QFileInfoList files{};

  std::unordered_set<std::string> processedFiles{};

  for(const auto& path : paths)
  {
    if(path.empty())
    {
      continue;
    }
    QDir pythonDir = QDir(QString::fromStdString(path));
    QFileInfoList pyFiles = pythonDir.entryInfoList({"*.py"}, QDir::Filter::Files);
    files.append(pyFiles);
  }

  if(files.empty())
  {
    return 0;
  }

  size_t numberLoaded = 0;

  pybind11::gil_scoped_acquire gil_acquire_guard{};

  pybind11::object filterBaseClass;

  try
  {
    filterBaseClass = pybind11::module_::import("Filter").attr("Filter");
  } catch(const pybind11::error_already_set& exception)
  {
    if(errorCallBack)
    {
      errorCallBack(exception.what(), "Filter.py");
    }
    return 0;
  }

  pybind11::dict globals = pybind11::globals();

  for(const QFileInfo& fileInfo : files)
  {
    QString filePath = fileInfo.canonicalFilePath();

    std::string filePathStr = filePath.toStdString();

    if(processedFiles.count(filePathStr) == 0)
    {
      processedFiles.insert(filePathStr);
    }
    else
    {
      continue;
    }

    try
    {
      auto globalsCopy = pybind11::reinterpret_steal<pybind11::dict>(PyDict_Copy(globals.ptr()));
      pybind11::eval_file(filePathStr, globalsCopy);
      pybind11::object filtersObject = pybind11::eval("filters", globalsCopy);
      if(!pybind11::isinstance(filtersObject, pybind11::type::of(pybind11::list())))
      {
        if(errorCallBack)
        {
          errorCallBack("\"filters\" is not a List", filePathStr);
        }
        continue;
      }
      pybind11::list pyFilters = filtersObject.cast<pybind11::list>();

      size_t i = -1;
      for(pybind11::handle filter : pyFilters)
      {
        i++;
        if(PyType_Check(filter.ptr()) == 0)
        {
          QString message = QString("Item %1 in \"filters\" is not a class type").arg(i);
          errorCallBack(message.toStdString(), filePathStr);
          continue;
        }
        auto pyType = filter.cast<pybind11::type>();
        auto name = pyType.attr("__name__").cast<std::string>();
        if(!pybind11::isinstance(pyType(), filterBaseClass))
        {
          if(errorCallBack)
          {
            QString message = QString("\"%1\" is not derived from Filter").arg(QString::fromStdString(name));
            errorCallBack(message.toStdString(), filePathStr);
          }
          continue;
        }

        auto filterFactory = PythonFilterFactory::New(pyType);
        filterManager.addPythonFilterFactory(filterFactory->getFilterClassName(), filterFactory);
        numberLoaded++;
        if(loadedCallback)
        {
          loadedCallback(name, filePathStr);
        }
      }
    } catch(const pybind11::error_already_set& exception)
    {
      if(errorCallBack)
      {
        QString message = QString("Python exception: %1").arg(exception.what());
        errorCallBack(message.toStdString(), filePathStr);
      }
      continue;
    } catch(const std::exception& exception)
    {
      if(errorCallBack)
      {
        QString message = QString("std::exception: %1").arg(exception.what());
        errorCallBack(message.toStdString(), filePathStr);
      }
      continue;
    }
  }
  return numberLoaded;
}

// -----------------------------------------------------------------------------
struct PythonLoader::ScopedInterpreter::Impl
{
  pybind11::scoped_interpreter interpreter_guard{};
};

// -----------------------------------------------------------------------------
PythonLoader::ScopedInterpreter::ScopedInterpreter(bool enable)
{
  m_Impl = enable ? std::make_unique<Impl>() : nullptr;
}

// -----------------------------------------------------------------------------
PythonLoader::ScopedInterpreter::~ScopedInterpreter() = default;

// -----------------------------------------------------------------------------
struct PythonLoader::GILScopedRelease::Impl
{
  pybind11::gil_scoped_release gil_release_guard{};
};

// -----------------------------------------------------------------------------
PythonLoader::GILScopedRelease::GILScopedRelease(bool enable)
{
  m_Impl = enable ? std::make_unique<Impl>() : nullptr;
}

// -----------------------------------------------------------------------------
PythonLoader::GILScopedRelease::~GILScopedRelease() = default;

// -----------------------------------------------------------------------------
struct PythonLoader::GILScopedAcquire::Impl
{
  pybind11::gil_scoped_acquire gil_acquire_guard{};
};

// -----------------------------------------------------------------------------
PythonLoader::GILScopedAcquire::GILScopedAcquire(bool enable)
{
  m_Impl = enable ? std::make_unique<Impl>() : nullptr;
}

// -----------------------------------------------------------------------------
PythonLoader::GILScopedAcquire::~GILScopedAcquire() = default;
