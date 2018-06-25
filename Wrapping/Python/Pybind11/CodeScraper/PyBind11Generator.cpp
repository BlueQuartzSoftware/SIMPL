#include "PyBind11Generator.h"

#include <iostream>

#include <QtCore/QTextStream>

#include "PythonBindingClass.h"

#include "CodeScraper/CodeScraperConstants.h"

//-----------------------------------------------------------------------------
PyBind11Generator::PyBind11Generator(const QDir& topLevelDir, const QString& charsToStrip, const QString& libName, const QString& genDir, const QString& moduleTemplatePath, const QString& isSIMPLib)
: m_TopLevelDir(topLevelDir)
, m_CharsToStrip(charsToStrip)
, m_LibName(libName)
, m_GenDir(genDir)
, m_ModuleTemplatePath(moduleTemplatePath)
, m_IsSIMPLib(isSIMPLib)
{
  m_SourceDir = m_TopLevelDir;
  m_SourceDir.cdUp();

  m_ModuleCode.setLibName(m_LibName);
  m_ModuleCode.setTemplatePath(moduleTemplatePath);
}

//-----------------------------------------------------------------------------
PyBind11Generator::~PyBind11Generator() = default;

//-----------------------------------------------------------------------------
void PyBind11Generator::execute()
{
  recursiveSearch(m_TopLevelDir);

  // If these extensions are changed be sure the WrappingFunctions.cmake file is updated
  QString ext(".h");
  if(m_IsSIMPLib.compare("TRUE") == 0)
  {
    ext = ".cxx";
  }

  QString genHeaderPath;
  QTextStream ss(&genHeaderPath);
  ss << m_GenDir << "/" << m_LibName << "_pybind11_module" << ext;

  m_ModuleCode.generateModuleFile(genHeaderPath, m_IsSIMPLib);
  
  genHeaderPath = QString("");
  ss << m_GenDir << "/" << m_LibName << "_UnitTest.py";
  m_ModuleCode.generatePythonTestFile(genHeaderPath, m_IsSIMPLib);
}

//-----------------------------------------------------------------------------
void PyBind11Generator::recursiveSearch(QDir currentDir)
{
  QStringList filters;
  filters.append("*.h");

  if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0 || currentDir.dirName().compare("pybind11") == 0)
  {
    return;
  }
  // Get a list of all the directories
  QFileInfoList dirList = currentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
  if(dirList.size() > 0)
  {
    foreach(QFileInfo fi, dirList)
    {
      recursiveSearch(QDir(fi.absoluteFilePath())); // Recursive call
    }
  }

  QFileInfoList itemList = currentDir.entryInfoList(filters);
  foreach(QFileInfo itemInfo, itemList)
  {
    QString headerFilePath = itemInfo.absoluteFilePath();
    generatePybind11Header(headerFilePath);
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
