#include "PyBind11Generator.h"

#include <iostream>

#include <QtCore/QTextStream>

#include "PythonBindingClass.h"

#include "CodeScraper/CodeScraperConstants.h"
#include "CodeScraper/SIMPLPyBind11Config.h"
#include "CodeScraper/PythonUtils.h"

#define PYB11_TRUE 0
//-----------------------------------------------------------------------------
PyBind11Generator::PyBind11Generator(const QDir& topLevelDir, const QString& charsToStrip, const QString& libName, 
const QString& genDir, const QString& moduleTemplatePath, const QString& isSIMPLib, const QString& cfgIntDir)
: m_TopLevelDir(topLevelDir)
, m_CharsToStrip(charsToStrip)
, m_LibNameUpper(libName)
, m_GenDir(genDir)
, m_ModuleTemplatePath(moduleTemplatePath)
, m_IsSIMPLib(isSIMPLib)
, m_CfgIntDir(cfgIntDir)
{

  m_LibName = m_LibNameUpper.toLower();// + "_py";
  m_SourceDir = m_TopLevelDir;
  m_SourceDir.cdUp();

  m_ModuleCode.setLibNameUpper(m_LibNameUpper);
  m_ModuleCode.setLibName(m_LibName);
  m_ModuleCode.setTemplatePath(moduleTemplatePath);
}

//-----------------------------------------------------------------------------
PyBind11Generator::~PyBind11Generator() = default;

//-----------------------------------------------------------------------------
void PyBind11Generator::readFilterList()
{
  QString libName = m_LibNameUpper;
  if(m_IsSIMPLib == "TRUE")
  {
    libName = QString("SIMPLib");
  }
  QString listFilePath = SIMPL::PyBind11::SIMPLProjBinaryDir + "/" + libName + "PublicFilters.txt";
  QFileInfo listFileInfo(listFilePath);
  if(!listFileInfo.exists())
  {
    qDebug() << "Needed Input file to generate Python Bindings was not found for Plugin '" << libName << "'. The file should have been generated at " << listFileInfo.absoluteFilePath();
  }

  m_FilterList.clear();

  // Read the Source File
  QFile source(listFilePath);
  source.open(QFile::ReadOnly);
  QString contents = source.readAll();
  source.close();

  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    QString tLine = line.trimmed();
    if(tLine.startsWith("#")) // Skip the comment line
    {
    }
    else if(!tLine.isEmpty())
    {
      m_FilterList << tLine;
    }
  }
  qDebug() << "[PyBind11Generator] " << libName << ": Generating " << m_FilterList.size() << " Pybind11 Headers ";
}

//-----------------------------------------------------------------------------
void PyBind11Generator::execute()
{
  readFilterList();

  recursiveSearch(m_TopLevelDir);

  // If these extensions are changed be sure the WrappingFunctions.cmake file is updated
  QString ext(".h");
  if(m_IsSIMPLib.compare("TRUE") == PYB11_TRUE)
  {
    ext = ".cxx";
    copyPyInitFiles();
  }

  QString genHeaderPath;
  QTextStream ss(&genHeaderPath);
  ss << m_GenDir << "/" << m_LibName << "_pybind11_module" << ext;

  m_ModuleCode.generateModuleFile(genHeaderPath, m_IsSIMPLib);
  
  genHeaderPath = QString("");
  QString libName = m_LibNameUpper;
  if(m_IsSIMPLib == "TRUE")
  {
    libName = QString("simpl");
  }
  ss << SIMPL::PyBind11::RuntimeOutputDir << "/UnitTest/" << libName << "_UnitTest.py";
  genHeaderPath = genHeaderPath.replace("/./", "/");
  m_ModuleCode.generatePythonTestFile(genHeaderPath, m_IsSIMPLib);
  
  genHeaderPath = QString("");
  libName = m_LibNameUpper;
  if(m_IsSIMPLib == "TRUE")
  {
    libName = QString("simpl");
  }
  ss << SIMPL::PyBind11::RuntimeOutputDir << "/" << SIMPL::PyBind11::SitePackages << "/" << SIMPL::PyBind11::SIMPL_LibraryName 
    << "/" << libName.toLower() << "py.py";
  genHeaderPath = genHeaderPath.replace("/./", "/");
  m_ModuleCode.generatePythonicInterface(genHeaderPath, m_IsSIMPLib);

  generateModuleInitPy();
}
//-----------------------------------------------------------------------------
void PyBind11Generator::generateModuleInitPy()
{
  //std::cout << "PyBind11Generator::generateModuleInitPy(): " << m_LibName.toStdString() << std::endl;
  QString filePath;
  QTextStream out(&filePath);
  out << SIMPL::PyBind11::RuntimeOutputDir << "/" << SIMPL::PyBind11::SitePackages << "/" << SIMPL::PyBind11::SIMPL_LibraryName << "/__init__.py";
  if(m_IsSIMPLib.compare("TRUE") == PYB11_TRUE)
  {
    QFile f(filePath);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream fout(&f);
    fout << "\"\"\" DREAM.3D Python Library\n"
    << "This file is auto generated during each build.\n"
    << "Do NOT edit this file directly.\n"
    << "PyBind11Generator.cpp:" << __LINE__ << "\n"
    << "\"\"\"\n"
    ;


    QFile sourceFile(SIMPL::PyBind11::SIMPLProjBinaryDir + "/Pybind11Plugins.txt");
    sourceFile.open(QFile::ReadOnly);
    QString contents = sourceFile.readAll();
    sourceFile.close();
    QStringList tokens = contents.split("\n");
    QStringList::const_iterator constIterator;
    for (constIterator = tokens.constBegin(); constIterator != tokens.constEnd(); ++constIterator)
    {
      QString lower =  (*constIterator).toLower().trimmed();
      if(lower.isEmpty()) { continue; }
      fout 

      << "# This imports the pythonic interface from the local site-packages\n"
      << "from . import " << lower << "py\n"
      << "# This imports the "<< lower << " submodule from the native python module\n"
      << "from .dream3d import " << lower << "\n"
      << "\n";
    }

    fout
    << "# Import the helper pure python modules\n"
    << "from . import simpl_helpers\n"
    << "from . import simpl_test_dirs\n"
    << "\n";

  }
  // else
  // {
  //   QFile f(filePath);
  //   f.open(QIODevice::Append | QIODevice::Text);
  //   QTextStream fout(&f);
  //   fout << "\n# DREAM3D Plugin " << m_LibName << "\n";
  //   fout << "from .dream3d import " << m_LibName << "\n";
  // }
}

//-----------------------------------------------------------------------------
void PyBind11Generator::copyPyInitFiles()
{
  QString libName = m_LibName;
  libName = libName.replace("_py", "");

  QString inputPath;
  QTextStream ip(&inputPath);
  ip << SIMPL::PyBind11::PythonWrappingDir << "/" << "SIMPL";
  QDir currentDir(inputPath);


  QString outputPath;
  QTextStream op(&outputPath);
  op << SIMPL::PyBind11::RuntimeOutputDir <<  "/" << SIMPL::PyBind11::SitePackages << "/" << SIMPL::PyBind11::SIMPL_LibraryName;
  QDir dir;
  dir.mkpath(outputPath);


  QStringList filters;
  filters.append("*.py");
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

  QFileInfoList itemList = currentDir.entryInfoList(filters);

  foreach(QFileInfo itemInfo, itemList)
  {
    QString pyFilePath = itemInfo.absoluteFilePath();

    // qDebug() << "Copy File: " << itemInfo.absoluteFilePath() << "\n   " 
    //  << "DEST: " << outputPath + "/" + itemInfo.fileName();
    if( pyFilePath.contains(".in.") )
    {
      //qDebug() << "CONFIGURE FILE: " << pyFilePath;
      QFile source(pyFilePath);
      source.open(QFile::ReadOnly);
      QString contents = source.readAll();
      source.close();

      QString outName = outputPath + "/" + itemInfo.fileName();
      outName = outName.replace(".in.", ".");
      //qDebug() << "  outName:" << outName;
      contents = contents.replace("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}", SIMPL::PyBind11::BuildDirectory);
      contents.replace("${SIMPLProj_BINARY_DIR}", SIMPL::PyBind11::SIMPLProjBinaryDir);
      contents.replace("${DREAM3D_DATA_DIR}", SIMPL::PyBind11::DataDirectory);
      QFile hOut(outName);

      hOut.open(QFile::WriteOnly);
      QTextStream stream(&hOut);
      stream << contents;
      hOut.close();

    }
    else
    {
      QFile pyFile(pyFilePath);
      pyFile.copy(outputPath + "/" + itemInfo.fileName());
    }
  }

}

//-----------------------------------------------------------------------------
void PyBind11Generator::recursiveSearch(QDir currentDir)
{
  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0 || currentDir.dirName().compare("pybind11") == 0)
  {
    return;
  }
  // If the currently generated library is SIMPLib itself then ignore the list of
  // files and do a recursive search for the PYBIND macros since everything in
  // SIMPL is going to need to be wrapped.
  if(m_IsSIMPLib == "TRUE")
  {
    // Get a list of all the directories
    QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    if(dirList.size() > 0)
    {
      foreach(QFileInfo fi, dirList)
      {
        recursiveSearch(QDir(fi.absoluteFilePath())); // Recursive call
      }
    }
    QStringList filters;
    filters.append("*.h");
    QFileInfoList itemList = currentDir.entryInfoList(filters);
    foreach(QFileInfo itemInfo, itemList)
    {
      QString headerFilePath = itemInfo.absoluteFilePath();
      generatePybind11Header(headerFilePath);
    }
  }
  else
  {
    for(const auto& item : m_FilterList)
    {
      QString filePath = currentDir.absolutePath() + QDir::separator() + item;
      QFileInfo fi(filePath);
      if(fi.exists())
      {
        generatePybind11Header(fi.absoluteFilePath());
      }
    }
  }
}
//-----------------------------------------------------------------------------
void PyBind11Generator::generatePybind11Header(const QString& hFile)
{
  QString contents;
  QFileInfo fi(hFile);

  if(fi.baseName().compare("SIMPLibSetGetMacros") == 0)
  {
    return;
  }

  {
    // Read the Source File
    QFile source(hFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  QString subPath = fi.absolutePath();
  subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
  if(!m_CharsToStrip.isEmpty())
  {
    subPath = subPath.replace(m_CharsToStrip, ""); // Remove the system dependent file path
  }
  subPath = subPath.remove(0, 1);

  QString genHeaderPath;
  QTextStream ss(&genHeaderPath);
  ss << m_GenDir << "/" << subPath
     //<< "pybind11"
     << "/" << fi.baseName() << "_PY11.h";

  QString baseName = fi.baseName();

  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);

  PythonBindingClass bindingClass(&m_ModuleCode, m_IsSIMPLib);
  bindingClass.setClassName(baseName);
  bindingClass.setLibName(m_LibName);
  bindingClass.setCharsToStrip(m_CharsToStrip);
  bindingClass.setTopLevelDir(m_TopLevelDir);
  bindingClass.setSourceDir(m_SourceDir);
  bindingClass.setSourceFile(hFile);

  QStringList enumerations;

  while(sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    QString tLine = line.trimmed();
    if(tLine.startsWith("//"))
    {
    }
    else if(line.contains(::kPYB11_ENUMERATION))
    {
      line = line.trimmed();
      QStringList tokens = line.split("(");
      tokens = tokens[1].replace(")", "").trimmed().split(" ");
      enumerations << tokens[0];
    }
    else if(line.contains(kPYB11_CREATE_BINDINGS))
    {
      bindingClass.setNeedsWrapping(true);
      line = line.trimmed();
      QStringList tokens = line.split("(");
      tokens = tokens[1].replace(")", "").trimmed().split(" ");
      baseName = tokens[0];

      bindingClass.setClassName(baseName);
      if(tokens.size() > 1 && tokens[1] == ::kSuperClass)
      {
        bindingClass.setHasSuperClass(true);
        bindingClass.setSuperClass(tokens[2]);
      }
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kSIMPL_SHARED_POINTERS)) // The class that needs wrapping is a Shared_Pointer type of class.
    {
      bindingClass.setIsSharedPointer(true);
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kSIMPL_STATIC_NEW_MACRO))
    {
      bindingClass.setHasStaticNewMacro(true);
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kSIMPL_FILTER_NEW_MACRO))
    {
      bindingClass.setHasStaticNewMacro(true);
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kPYB11_PROPERTY))
    {
      line = line.trimmed();
      bindingClass.addProperty(line);
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kPYB11_METHOD))
    {
      line = line.trimmed();
      bindingClass.addMethod(line);
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kPYB11_STATIC_CREATION))
    {
      line = line.trimmed();
      bindingClass.addStaticCreation(line);
    }
    else if(bindingClass.getNeedsWrapping() && line.contains(::kPYB11_CREATION))
    {
      line = line.trimmed();
      bindingClass.addConstructor(line);
    }

    if(enumerations.size() != 0)
    { 
      bool foundEnumerationCode = false;
      for(auto e : enumerations)
      {
        QString search = QString("enum class %1").arg(e);
        if(tLine.contains(search))
        {
          //qDebug() << "Found Enumeration Definition.... " << tLine;
          enumerations.removeAll(e);
          foundEnumerationCode = true;
        }
        
        if(foundEnumerationCode)
        {
          //qDebug() << "Looking for opening brace... ";
          // Find where the opening brace is at.
          while(!tLine.contains("{"))
          {
            line = sourceLines.next();
            tLine = line.trimmed();
          }
          // Grab the first value. They MUST be one value per line.
          line = sourceLines.next();
          tLine = line.trimmed();
          // qDebug() << "Finding enumeration values.....";
          // Grab lines until the closing brace
          QStringList values;
          while(!tLine.contains("};"))
          {
            QStringList tokens = tLine.split(" ");
            values << tokens[0];
            // qDebug() << tokens[0];
            line = sourceLines.next();
            tLine = line.trimmed();
          }  
         // qDebug() << "... Complete";
          bindingClass.addEnumeration(e, values);
        }    
      }
    }
  }

  bindingClass.writeBindingFile(genHeaderPath);
}
