
#include <stdint.h>

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QMetaProperty>
#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUuid>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLPyBind11Config.h"

#define NEWLINE_SIMPL '\n'
#define TAB "  "

#define OVERWRITE_SOURCE_FILE 1



/* These are for replacements in the template files */
#define CLASS_NAME "@CLASS_NAME@"
#define HEADER_PATH "@HEADER_PATH@"
#define SUPERCLASS_NAME "@SUPERCLASS_NAME@"
#define LIB_NAME "@LIB_NAME@"
#define MODULE_INIT_CODE "@MODULE_INIT_CODE@"

namespace
{
static const QString kRead("READ");
static const QString kWrite("WRITE");
static const QString kArgs("ARGS");
static const QString kSuperClass("SUPERCLASS");

/* These are for the macros that appear in the header files */
static const QString kPYB11_CREATE_BINDINGS("PYB11_CREATE_BINDINGS");
static const QString kSIMPL_STATIC_NEW_MACRO("SIMPL_STATIC_NEW_MACRO");
static const QString kSIMPL_SHARED_POINTERS("SIMPL_SHARED_POINTERS");
static const QString kPYB11_PROPERTY("PYB11_PROPERTY");
static const QString kPYB11_METHOD("PYB11_METHOD");

}

/* *****************************************************************************
 * 
 * 
 ******************************************************************************/
class PythonBindingsModule
{
  public:
    PythonBindingsModule() {}
    ~PythonBindingsModule() {}
    
    SIMPL_INSTANCE_PROPERTY(QString, LibName)
    
    using MapperType = QMap<QString, QString>;
    
    SIMPL_INSTANCE_PROPERTY(MapperType, Headers)
    void addHeader(const QString &className, const QString &header)
    {
      m_Headers[className] = header;
    }
    void clearHeaders() { m_Headers.clear(); }
    
    
    SIMPL_INSTANCE_PROPERTY(MapperType, InitCodes)
    void addInitCode(const QString &className, const QString &initCode)
    {
      m_InitCodes[className] = initCode;
    }
    void clearInitCodes() { m_InitCodes.clear(); }
    
    /**
     * @brief addDependency
     * @param superClassName
     * @param className
     */
    void addDependency(QString superClassName, QString className)
    {
      //qDebug() << "SuperClassName: " << superClassName << "  ClassName: " << className;
      bool superClassAdded = false;
      bool classNameAdded = false;
      
      for(auto object : m_ClassVector)
      {
        if(object->objectName() == superClassName)
        {
          QObject* child = new QObject(object);
          child->setObjectName(className);
          superClassAdded = true;
          classNameAdded = true;
          break;
        }
        // First try to find the superclass
        if(!superClassName.isEmpty())
        {
          QObject* superchild = object->findChild<QObject*>(superClassName);
          if(superchild)
          {
            QObject* child = new QObject(superchild);
            child->setObjectName(className);
            superClassAdded = true;
            classNameAdded = true;
            break;
          }
        }
        else // The className is alread registered as a superclass or top level class.
        {
          if(className == object->objectName())
          {
            superClassAdded = true;
            classNameAdded = true;
            break;
          }
        }
      }
      
      if(!superClassAdded && !superClassName.isEmpty())
      {
        QObject* object = new QObject(nullptr);
        object->setObjectName(superClassName);
        m_ClassVector.push_back(object);
        
        QObject* obj = new QObject(object);
        obj->setObjectName(className);
        superClassAdded = true;
        classNameAdded = true;
      }
      
      if(!classNameAdded)
      {
        QObject* obj = new QObject(nullptr);
        obj->setObjectName(className);
        m_ClassVector.push_back(obj);
      }
      
    }
    
    /**
     * @brief writeOutput
     * @param didReplace
     * @param outLines
     * @param filename
     */
    void writeOutput(bool didReplace, const QString& outLines, QString filename)
    {
      if(didReplace == true)
      {
        QFileInfo fi2(filename);
        QString parentPath = fi2.path();
        QDir dir;
        // std::cout << "Creating Path: " << parentPath.toStdString() << std::endl;
        if(!dir.mkpath(parentPath))
        {
          QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
          qDebug() << ss;
          return;
        }
#if OVERWRITE_SOURCE_FILE
        QFile hOut(filename);
#else
        QString tmpPath = "/tmp/" + fi2.fileName();
        QFile hOut(tmpPath);
#endif
        hOut.open(QFile::WriteOnly);
        QTextStream stream(&hOut);
        stream << outLines;
        hOut.close();
        
        qDebug() << "Pybind11 Module Generated for: " << fi2.absoluteFilePath();
      }
    }
    
    void generateModuleFile(const QString &outputPath)
    {
      // Create the Top part of the file from a template file
      QFile source(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/ModuleCode_Template.cpp");
      source.open(QFile::ReadOnly);
      QString headerTemplate = source.readAll();
      source.close();
      
      QString code;
      QTextStream out(&code);
      out << "/* These are all the pybind11 headers for each for each of the exported classes */\n";
      
      for(auto object : m_ClassVector)
      {
        dumpRecursiveIncludeList(0, object, out);
      }
      
      headerTemplate = headerTemplate.replace(HEADER_PATH, code);
      headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);
      
      code.clear();
      out << "/* These are all the pybind11 instantiations for each of the exported classes */\n";
      for(auto object : m_ClassVector)
      {
        out << "\n";
        dumpRecursiveInitCode(0, object, out);
      }
      out << "\n";
      headerTemplate = headerTemplate.replace(MODULE_INIT_CODE, code);
      
      writeOutput(true, headerTemplate, outputPath);      
    }
    
  protected:
    
    void dumpRecursiveIncludeList(int level, const QObject *object, QTextStream &out)
    {
      if (object) {
        QByteArray buf;
        buf.fill(' ', level / 2 * 8);
        if (level % 2)
          buf += "    ";
        QString name = object->objectName();
        QString flags = QLatin1String("");
        QString header = m_Headers[object->objectName()];
        if(header.isEmpty())
        {
          qDebug() << "Header was empty: " << object->objectName();
        }
        out << "#include \"" << header << "\"\n";
        
        QObjectList children = object->children();
        if (!children.isEmpty()) {
          for (int i = 0; i < children.size(); ++i)
            dumpRecursiveIncludeList(level+1, children.at(i), out);
        }
      }
    }  
    
    void dumpRecursiveInitCode(int level, const QObject *object, QTextStream &out)
    {
      if (object) {
        QByteArray buf;
        buf.fill(' ', level / 2 * 8);
        if (level % 2)
          buf += "    ";
        QString name = object->objectName();
        QString flags = QLatin1String("");
        QString initCode = m_InitCodes[object->objectName()];
        
        out << "  " << initCode << "\n";
        QObjectList children = object->children();
        if (!children.isEmpty()) {
          out << "\n";
          for (int i = 0; i < children.size(); ++i)
            dumpRecursiveInitCode(level+1, children.at(i), out);
        }
      }
    } 
    
  private:
    std::vector<QObject*> m_ClassVector;
    
};


/* *****************************************************************************
 * 
 * 
 ******************************************************************************/
class PythonBindingClass : public QObject
{
    
  public:
    PythonBindingClass(PythonBindingsModule* moduleCode)
      : QObject(nullptr)
      , m_NeedsWrapping(false) 
      , m_HasSuperClass(false)
      , m_IsSharedPointer(false)
      , m_HasStaticNewMacro(false)
      , m_Module(moduleCode)
    {}
    ~PythonBindingClass() {}
    
    
    SIMPL_INSTANCE_PROPERTY(QString, ClassName)
    SIMPL_INSTANCE_PROPERTY(QString, SuperClass)
    SIMPL_INSTANCE_PROPERTY(QDir, TopLevelDir)
    SIMPL_INSTANCE_PROPERTY(QDir, SourceDir)
    SIMPL_INSTANCE_PROPERTY(QString, CharsToStrip)
    SIMPL_INSTANCE_PROPERTY(QString, LibName)
    SIMPL_INSTANCE_PROPERTY(QString, SourceFile)
    SIMPL_INSTANCE_PROPERTY(bool, NeedsWrapping)
    SIMPL_INSTANCE_PROPERTY(bool, HasSuperClass)
    SIMPL_INSTANCE_PROPERTY(bool, IsSharedPointer)
    SIMPL_INSTANCE_PROPERTY(bool, HasStaticNewMacro)
    
    SIMPL_INSTANCE_PROPERTY(QVector<QString>, Properties)
    void addProperty(const QString &property)
    {
      m_Properties.push_back(property);
    }
    void clearProperties() { m_Properties.clear(); }
    
    SIMPL_INSTANCE_PROPERTY(QVector<QString>, Methods)
    void addMethod(const QString &method)
    {
      m_Methods.push_back(method);
    }
    void clearMethods() { m_Methods.clear(); }
    
    
    void writeBindingFile(const QString &outputFilePath)
    { 
      if(getNeedsWrapping())
      {
        QString output;
        QTextStream out(&output);
        out << generateTopMatterCode();
        out << generateSharedPointerInitCode();
        out << generateStaticNewCode();
        out << generateConstructorsCodes();
        out << generatePropertiesCode();
        out << generateMethodCode();
        out << generateFooterCode();
        
        writeOutput(getNeedsWrapping(), output, outputFilePath);
        
        
        QString initCode;
        QTextStream init(&initCode);
        
        init << "PySharedPtrClass<" << getClassName() << "> ";
        init << getLibName() << "_" << getClassName() << " = ";
        if(getHasSuperClass())
        {
          init << "pybind11_init_" << getLibName() << "_" << getClassName() << "(mod, " << getLibName() << "_" << getSuperClass() << ");";
        }
        else
        {
          init << "pybind11_init_" << getLibName() << "_" << getClassName() << "(mod);";
        }
        
        m_Module->addInitCode(getClassName(), initCode);
      }
    }
    
    /**
    *@brief quote
    *@param str
    *@return
    */
    QString quote(const QString& str)
    {
      return QString("\"%1\"").arg(str);
    }
    
    /**
    *@brief writeOutput
    *@param didReplace
    *@param outLines
    *@param filename
    */
    void writeOutput(bool didReplace, const QString& outLines, QString filename)
    {
      if(didReplace == true)
      {
        QFileInfo fi2(filename);
        QString parentPath = fi2.path();
        QDir dir;
        // std::cout << "Creating Path: " << parentPath.toStdString() << std::endl;
        if(!dir.mkpath(parentPath))
        {
          QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
          qDebug() << ss;
          return;
        }
#if OVERWRITE_SOURCE_FILE
        QFile hOut(filename);
#else
        QString tmpPath = "/tmp/" + fi2.fileName();
        QFile hOut(tmpPath);
#endif
        hOut.open(QFile::WriteOnly);
        QTextStream stream(&hOut);
        stream << outLines;
        hOut.close();
        
        qDebug() << "Pybind11 Generated for: " << fi2.absoluteFilePath();
      }
    }
    
    
  protected:
    PythonBindingClass() = delete;
    
    QString generateTopMatterCode()
    {
      QFileInfo fi(m_SourceFile);
      QString subPath = fi.absolutePath();
      subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
      subPath = subPath.remove(0,1); //Remove the front / character
      
      // Create the Top part of the file from a template file
      QFile source(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/Pybind11TopMatter.txt");
      source.open(QFile::ReadOnly);
      QString headerTemplate = source.readAll();
      source.close();
      headerTemplate = headerTemplate.replace(CLASS_NAME, getClassName());
      QString headerPath = QString("%1/%2.h").arg(subPath).arg(getClassName());
      if(!m_CharsToStrip.isEmpty())
      {
        headerPath = headerPath.replace(m_CharsToStrip, "");
      }
      
      headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
      headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);
      return headerTemplate;
    }
    
    QString generateSharedPointerInitCode()
    {
      if(!getIsSharedPointer())
      {
        return QString("");
      }
      QFile source;
      
      if(getHasSuperClass() && !getSuperClass().isEmpty())
      {
        source.setFileName(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/DerivedSharedPointerClassInit.txt");
      }
      else
      {
        source.setFileName(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/SharedPointerClassInit.txt");
      }
      source.open(QFile::ReadOnly);
      QString headerTemplate = source.readAll();
      source.close();
      
      QFileInfo fi(m_SourceFile);
      QString subPath = fi.absolutePath();
      subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
      subPath = subPath.remove(0,1); //Remove the front / character
      
      headerTemplate = headerTemplate.replace(CLASS_NAME, getClassName());
      QString headerPath = QString("%1/%2.h").arg(subPath).arg(getClassName());
      headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
      headerTemplate = headerTemplate.replace(SUPERCLASS_NAME, getSuperClass());
      headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);
      
      QString pybind11HeaderPath = QString("%1/pybind11/%2_PY11.h").arg(subPath).arg(getClassName());
      if(!m_CharsToStrip.isEmpty())
      {
        pybind11HeaderPath = pybind11HeaderPath.replace(m_CharsToStrip, "");
      }
      
      m_Module->addHeader(getClassName(), pybind11HeaderPath);
      m_Module->addDependency(getSuperClass(), getClassName());
      
      return headerTemplate;
    }
    
    QString generateStaticNewCode()
    {
      if(!getIsSharedPointer())
      {
        return QString("");
      }
      QString code;
      QTextStream constructors(&code);
      constructors << TAB << ".def(py::init([](){ return " << getClassName() << "::New();}))" << NEWLINE_SIMPL;
      constructors << TAB << ".def_static(\"New\", &" << getClassName() << "::New)" << NEWLINE_SIMPL;      
      return code;
    }
    
    QString generateConstructorsCodes()
    {
      if(getIsSharedPointer())
      {
        return QString("");
      }
      
      QString code;
      QTextStream constructors(&code);
      
      QFile source;
      source.setFileName(SIMPL::PyBind11::GetSIMPLPyBind11_TemplatesDir() + "/SimpleClassInit.txt");
      source.open(QFile::ReadOnly);
      QString headerTemplate = source.readAll();
      source.close();
      headerTemplate = headerTemplate.replace(CLASS_NAME, getClassName());
      
      QFileInfo fi(m_SourceFile);
      QString subPath = fi.absolutePath();
      subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
      subPath = subPath.remove(0,1); //Remove the front / character
      
      QString headerPath = QString("%1/%2.h").arg(subPath).arg(getClassName());
      headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
      headerTemplate = headerTemplate.replace(SUPERCLASS_NAME, getSuperClass());
      headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);
      
      constructors << headerTemplate;
      
      constructors << TAB << ".def(py::init<" << getClassName() << ">())" << NEWLINE_SIMPL;
      constructors << TAB << ".def(py::init<" << getClassName() << " const &>())" << NEWLINE_SIMPL;
      
      QString pybind11HeaderPath = QString("%1/pybind11/%2_PY11.h").arg(subPath).arg(getClassName());
      if(!m_CharsToStrip.isEmpty())
      {
        pybind11HeaderPath = pybind11HeaderPath.replace(m_CharsToStrip, "");
      }
      m_Module->addHeader(getClassName(), pybind11HeaderPath);
      m_Module->addDependency(getSuperClass(), getClassName());


      return code;
    }
    
    QString generatePropertiesCode()
    {
      QString code;
      QTextStream out(&code);
      for(auto line : m_Properties)
      {
        QStringList tokens = line.split("(");
        tokens = tokens[1].replace(")", "").trimmed().split(" ");
        QString pyType = tokens[0];
        QString varName = tokens[1];
        out << TAB << "/* Property accessors for " << varName << " */" << NEWLINE_SIMPL;
        if(tokens.size() > 3 && tokens[2] == ::kRead)
        {
          out << TAB << ".def(\"get" << varName << "\", &" << getClassName() << "::get" << varName << ")" << NEWLINE_SIMPL;
        }
        else if(tokens.size() > 3 && tokens[2] == ::kWrite)
        {
          out << TAB << ".def(\"set" << varName << "\", &" << getClassName() << "::set" << varName << ", py::arg(\"" << varName << "\"))" << NEWLINE_SIMPL;
        }
        
        if(tokens.size() > 5 && tokens[4] == ::kRead)
        {
          out << TAB << ".def(\"get" << varName << "\", &" << getClassName() << "::get" << varName << ")" << NEWLINE_SIMPL;
        }
        else if(tokens.size() > 5 && tokens[4] == ::kWrite)
        {
          out << TAB << ".def(\"set" << varName << "\", &" << getClassName() << "::set" << varName << ", py::arg(\"" << varName << "\"))" << NEWLINE_SIMPL;
        }
      }
      return code;
    }
    
    QString generateMethodCode()
    {
      QString code;
      QTextStream out(&code);
      for(auto line : m_Methods)
      {
        QStringList tokens = line.split("(");
        tokens = tokens[1].replace(")", "").trimmed().split(" ");
        QString returnType = tokens[0];
        QString methodName = tokens[1];
        out << TAB << "/* Class instance method " << methodName << " */" << NEWLINE_SIMPL;
        
        if(tokens.size() == 2)
        {
          out << TAB << ".def(\"" << methodName << "\", &" << getClassName() << "::" << methodName << ")" << NEWLINE_SIMPL;
        }
        else if(tokens.size() > 3 && tokens[2] == ::kArgs)
        {
          out << TAB << ".def(\"" << methodName << "\", &" << getClassName() << "::" << methodName;
          for(int32_t i = 3; i < tokens.size(); i++)
          {
            out << ", \n" << TAB << TAB << TAB << TAB << "py::arg(\"" << tokens[i] << "\")";
          }
          out << NEWLINE_SIMPL << TAB << TAB << TAB << ")" << NEWLINE_SIMPL;
        }
      }
      return code;
    }
    
    
    QString generateFooterCode()
    {
      QString code;
      QTextStream out(&code);
      out << TAB << ";" << NEWLINE_SIMPL;
      out << TAB << "return instance;" << NEWLINE_SIMPL;
      out << "}" << NEWLINE_SIMPL;
      out << "\n\n";
      out << "#endif /* pybind_" << getClassName() << "_H_  */" << NEWLINE_SIMPL;
      return code;
    }
    
  private:
    
    PythonBindingsModule* m_Module = nullptr;
    PythonBindingClass(const PythonBindingClass&) = delete; // Copy Constructor Not Implemented
    void operator=(const PythonBindingClass&) = delete; // Operator '=' Not Implemented   
};

/* *****************************************************************************
 * 
 * 
 ******************************************************************************/
class PyBind11Generator
{
    
  public:
    /**
   * @brief PyBind11Generator
   * @param topLevelDir
   * @param charsToStrip
   * @param libName
   */
    PyBind11Generator(const QDir& topLevelDir, const QString& charsToStrip, const QString& libName)
      : m_TopLevelDir(topLevelDir)
      , m_CharsToStrip(charsToStrip)
      , m_LibName(libName)
    {
      m_SourceDir = m_TopLevelDir;
      m_SourceDir.cdUp();
      
      
      m_ModuleCode.setLibName(m_LibName);
    }
    
    ~PyBind11Generator() = default;
    
    
    /**
   * @brief execute
   */
    void execute()
    {
      recursiveSearch(m_TopLevelDir);
      
      // QFileInfo fi(m_TopLevelDir);
      QString genHeaderPath;
      QTextStream ss(&genHeaderPath);
      ss << m_TopLevelDir.absolutePath() << "/" << "pybind11" << "/"  << m_LibName << "_Pybindings.cxx";
      
      m_ModuleCode.generateModuleFile(genHeaderPath);
    }
    
    /**
   * @brief recursiveSearch
   * @param currentDir
   */
    void recursiveSearch(QDir currentDir)
    {
      QStringList filters;
      filters.append("*.h");
      
      if(currentDir.dirName().compare("zRel") == 0 
         || currentDir.dirName().compare("Build") == 0
         || currentDir.dirName().compare("pybind11") == 0)
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
    void generatePybind11Header(const QString& hFile)
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
      ss << fi.absolutePath() << "/" << "pybind11" << "/"  << fi.baseName() << "_PY11.h";
      
      QString baseName = fi.baseName();
      
      QStringList list = contents.split(QRegExp("\\n"));
      QStringListIterator sourceLines(list);
      
      
      PythonBindingClass bindingClass(&m_ModuleCode);
      bindingClass.setClassName(baseName);
      bindingClass.setLibName(m_LibName);
      bindingClass.setCharsToStrip(m_CharsToStrip);
      bindingClass.setTopLevelDir(m_TopLevelDir);
      bindingClass.setSourceDir(m_SourceDir);
      bindingClass.setSourceFile(hFile);
      
      while(sourceLines.hasNext())
      {
        QString line = sourceLines.next();
        if(line.contains(kPYB11_CREATE_BINDINGS))
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
      }
      
      bindingClass.writeBindingFile(genHeaderPath);
    }
    
  private:
    QDir m_TopLevelDir;
    QDir m_SourceDir;
    QString m_CharsToStrip;
    QString m_LibName;
    PythonBindingsModule m_ModuleCode;
    
    
    PyBind11Generator(const PyBind11Generator&) = delete; // Copy Constructor Not Implemented
    void operator=(const PyBind11Generator&) = delete;    // Operator '=' Not Implemented
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
  QCoreApplication::setApplicationName("GeneratePythonBindings");

  if(argc != 4)
  {
    std::cout << "GeneratePythonBindings needs 2 arguments:" << std::endl;
    std::cout << "   [1] Path to the source directory to recursively search" << std::endl;
    std::cout << "   [2] Characters to strip from the path. '-' means do not strip anything" << std::endl;
    std::cout << "   [3] Name of the Library/Plugin" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::cout << "Source Directory: " << argv[1] << std::endl;
  std::cout << "Characters to strip: " << argv[2] << std::endl;
  std::cout << "Name of the Library: " << argv[3] << std::endl;
  
  
  QString charsToStrip = QString::fromLatin1(argv[2]);
  if(charsToStrip == "-")
  {
    charsToStrip = "";
  }
  QString libName = QString::fromLatin1(argv[3]);
  
  PyBind11Generator bindingsGenerator(QDir(QString::fromLatin1(argv[1])), charsToStrip, libName);
  
  bindingsGenerator.execute();
  
  return EXIT_SUCCESS;
}
