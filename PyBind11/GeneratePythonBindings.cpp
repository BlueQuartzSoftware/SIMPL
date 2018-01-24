
#include <stdint.h>
#include <iostream>
#include <sstream>



#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUuid>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QSet>


#include "SIMPLPyBind11Config.h"
#define NEWLINE_SIMPL '\n'
#define TAB "  "

#define OVERWRITE_SOURCE_FILE 1

/* These are for the macros that appear in the header files */
#define PYB11_SIMPL_EXPORT_CLASS "PYB11_SIMPL_EXPORT_CLASS"
#define SIMPL_STATIC_NEW_MACRO "SIMPL_STATIC_NEW_MACRO"
#define SIMPL_SHARED_POINTERS "SIMPL_SHARED_POINTERS"
#define PYB11_PROPERTY "PYB11_PROPERTY"
#define PYB11_METHOD "PYB11_METHOD"

/* These are for replacements in the template files */
#define CLASS_NAME "@CLASS_NAME@"
#define HEADER_PATH "@HEADER_PATH@"

namespace  {
  static const QString kRead("READ");
  static const QString kWrite("WRITE");
  static const QString kArgs("ARGS");
}

class PyBind11Generator
{
    
  public:
    PyBind11Generator(const QDir &topLevelDir) :
      m_TopLevelDir(topLevelDir)
    {
      m_SourceDir = m_TopLevelDir;
      m_SourceDir.cdUp(); 
    }
    
    ~PyBind11Generator() = default;
    
    /**
 * @brief quote
 * @param str
 * @return 
 */
    QString quote(const QString& str)
    {
      return QString("\"%1\"").arg(str);
    }
    
    /**
 * @brief writeOutput
 * @param didReplace
 * @param outLines
 * @param filename
 */
    void writeOutput(bool didReplace, const std::string &outLines, QString filename)
    {
      if(didReplace == true)
      {
        QFileInfo fi2(filename);
#if OVERWRITE_SOURCE_FILE
        QFile hOut(filename);
#else
        QString tmpPath = "/tmp/" + fi2.fileName();
        QFile hOut(tmpPath);
#endif
        hOut.open(QFile::WriteOnly);
        QTextStream stream(&hOut);
        stream << QString::fromStdString(outLines);
        hOut.close();
        
        qDebug() << "Saved File " << fi2.absoluteFilePath();
      }
    }
    
    /**
     * @brief execute
     */
    void execute()
    {
      recursiveSearch(m_TopLevelDir);
    }
    
    /**
     * @brief recursiveSearch
     * @param currentDir
     */  
    void recursiveSearch(QDir currentDir)
    {
      QStringList filters;
      filters.append("*.h");
      
      if(currentDir.dirName().compare("zRel") == 0 || currentDir.dirName().compare("Build") == 0)
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
    
    /**
     * @brief generatePybind11Header
     * @param hFile
     */
    void generatePybind11Header(const QString &hFile)
    {
      QString contents;
      QFileInfo fi(hFile);
      
      if (fi.baseName().compare("SIMPLibSetGetMacros") == 0)
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
      subPath = subPath.remove(0,1); //Remove the front / character
      
      QString genHeaderPath = QString("%1/%2/%3_Pybind.h").arg(SIMPL::PyBind11::GetSIMPLPyBind11_SourceDir()).arg(subPath).arg(fi.baseName());
      
      
      bool needsWrapping = false;
      
      QString searchString =  "PYB11_SIMPL_EXPORT_CLASS";
      
      std::stringstream header;
      std::stringstream constructors;
      std::stringstream properties;
      std::stringstream footer;
      
      QString baseName = fi.baseName();
      
      QStringList list = contents.split(QRegExp("\\n"));
      QStringListIterator sourceLines(list);
      
      while (sourceLines.hasNext())
      {
        QString line = sourceLines.next();
        if(line.contains(PYB11_SIMPL_EXPORT_CLASS) )
        {
          needsWrapping = true;
          // Read the Template File
          QFile source(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/Pybind11TopMatter.txt");
          source.open(QFile::ReadOnly);
          QString headerTemplate = source.readAll();
          source.close();
          headerTemplate = headerTemplate.replace(CLASS_NAME, baseName);
          QString headerPath = QString("%1/%2.h").arg(subPath).arg(baseName);
          headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
          
          header << headerTemplate.toStdString();
        }
        
        else if(needsWrapping && line.contains(SIMPL_SHARED_POINTERS) )
        {
          // Read the Template File
          QFile source(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/SharedPointerClassInit.txt");
          source.open(QFile::ReadOnly);
          QString headerTemplate = source.readAll();
          source.close();
          headerTemplate = headerTemplate.replace(CLASS_NAME, baseName);
          QString headerPath = QString("%1/%2.h").arg(subPath).arg(baseName);
          headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
          
          header << headerTemplate.toStdString();
        }
        
        // The class that needs wrapping is a Shared_Pointer type of class.
        else if(needsWrapping && line.contains(SIMPL_STATIC_NEW_MACRO) )
        {
          constructors << TAB << ".def(py::init([](){ return " << baseName.toStdString() << "::New();}))" << NEWLINE_SIMPL;
          constructors << TAB << ".def_static(\"New\", &" << baseName.toStdString() << "::New)" << NEWLINE_SIMPL;
        }
        else if(needsWrapping && line.contains(PYB11_PROPERTY) )
        {
          line = line.trimmed();
          QStringList tokens = line.split("(");
          tokens = tokens[1].replace(")", "").trimmed().split(" ");
          std::string pyType = tokens[0].toStdString();
          std::string varName = tokens[1].toStdString();
          properties << TAB << "/* Property accessors for " << varName << " */" << NEWLINE_SIMPL;
          if(tokens.size() > 3 && tokens[2] == ::kRead)
          { 
            properties << TAB << ".def(\"get" << varName << "\", &" << baseName.toStdString() << "::get" << varName << ")" << NEWLINE_SIMPL;
          }
          else if(tokens.size() > 3 && tokens[2] == ::kWrite)
          {
            properties << TAB << ".def(\"set" << varName << "\", &" << baseName.toStdString() << "::set" << varName << ", py::arg(\""<< varName << "\"))" << NEWLINE_SIMPL;
          }
          
          if(tokens.size() > 5 && tokens[4] == ::kRead)
          {
            properties << TAB << ".def(\"get" << varName << "\", &" << baseName.toStdString() << "::get" << varName << ")" << NEWLINE_SIMPL;
          }
          else if(tokens.size() > 5 && tokens[4] == ::kWrite)
          {
            properties << TAB << ".def(\"set" << varName << "\", &" << baseName.toStdString() << "::set" << varName << ", py::arg(\""<< varName << "\"))" << NEWLINE_SIMPL;
          } 
        }
        else if(needsWrapping && line.contains(PYB11_METHOD) )
        {
          line = line.trimmed();
          QStringList tokens = line.split("(");
          tokens = tokens[1].replace(")", "").trimmed().split(" ");
          std::string returnType = tokens[0].toStdString();
          std::string methodName = tokens[1].toStdString();
          properties << TAB << "/* Class instance method " << methodName << " */" << NEWLINE_SIMPL;
          
          if(tokens.size() == 2)
          {
            properties << TAB << ".def(\"" << methodName << "\", &" << baseName.toStdString() << "::" << methodName << ")" << NEWLINE_SIMPL;
            
          }
          else if(tokens.size() > 3 && tokens[2] == ::kArgs)
          {
            properties << TAB << ".def(\"" << methodName << "\", &" << baseName.toStdString() << "::" << methodName;
            for(int32_t i = 3; i < tokens.size(); i++)
            {
              properties << ", py::arg(\""<< tokens[i].toStdString() << "\"))";
            }
            properties << NEWLINE_SIMPL;
          }
        }
      }
      
      if(needsWrapping)
      {
        footer << TAB << ";" << NEWLINE_SIMPL;
        footer << TAB << "return instance;" << NEWLINE_SIMPL;
        footer << "}" << NEWLINE_SIMPL;
        footer << "\n\n";
        footer << "#endif /* pybind_" << fi.baseName().toStdString() << "_H_  */" << NEWLINE_SIMPL;
      }
      
      std::string output = header.str() + constructors.str() + properties.str() + footer.str();
      
      writeOutput(needsWrapping, output, genHeaderPath);
    }
    
  private:
    
    QDir m_TopLevelDir;
    QDir m_SourceDir;
    
    PyBind11Generator(const PyBind11Generator&) = delete; // Copy Constructor Not Implemented
    void operator=(const PyBind11Generator&) = delete; // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  
  Q_ASSERT(true); // We don't want anyone to run this program.
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("GeneratePythonBindins");
  
  
  
  //ReplaceGrepSearchesRecursively(QDir(D3DTools::GetDREAM3DProjDir() + "/ExternalProjects/SIMPL/Source/SIMPLib"));
  
  PyBind11Generator bindingsGenerator (QDir(SIMPL::PyBind11::GetSIMPLProj_SourceDir() + "/Source/SIMPLib"));
  bindingsGenerator.execute();
  
  return EXIT_SUCCESS;
  
}
