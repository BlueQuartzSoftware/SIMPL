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
} // namespace

// -----------------------------------------------------------------------------
bool PythonLoader::checkPythonHome()
{
  return !qEnvironmentVariable(k_PYTHONHOME).isEmpty();
}

// -----------------------------------------------------------------------------
std::vector<std::string> PythonLoader::defaultPythonFilterPaths()
{
  std::vector<std::string> paths{QString("%1/Python").arg(QCoreApplication::applicationDirPath()).toStdString()};
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
bool PythonLoader::setPythonHome(const std::string& value)
{
  return qputenv(k_PYTHONHOME, value.c_str());
}

// -----------------------------------------------------------------------------
void PythonLoader::loadPythonFilters(FilterManager& filterManager, const std::vector<std::string>& paths, ErrorCallback errorCallBack, LoadedCallback loadedCallback)
{
  QFileInfoList files{};

  std::unordered_set<std::string> processedFiles{};

  for(const auto& path : paths)
  {
    QDir pythonDir = QDir(QString::fromStdString(path));
    QFileInfoList pyFiles = pythonDir.entryInfoList({"*.py"}, QDir::Filter::Files);
    files.append(pyFiles);
  }

  if(files.empty())
  {
    return;
  }

  pybind11::gil_scoped_acquire gil_acquire_guard{};

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

      pybind11::object filterBaseClass = pybind11::eval("Filter", globalsCopy);
      for(pybind11::handle filter : pyFilters)
      {
        auto name = filter.cast<std::string>();
        pybind11::object pyType = pybind11::eval(name, globalsCopy);
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
}
