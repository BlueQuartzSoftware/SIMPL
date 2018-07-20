#include "PythonBindingsModule.h"

#include <iostream>

#include <QtCore/QDateTime>
#include <QtCore/QTemporaryFile>
#include <QtCore/QCryptographicHash>
#include <QtCore/QListIterator>

#include "CodeScraper/CodeScraperConstants.h"
#include "CodeScraper/SIMPLPyBind11Config.h"
#include "CodeScraper/PythonUtils.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PythonBindingsModule::PythonBindingsModule()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PythonBindingsModule::~PythonBindingsModule()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::addHeader(const QString& className, const QString& header)
{
  m_Headers[className] = header;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::clearHeaders()
{
  m_Headers.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::addInitCode(const QString& className, const QString& initCode)
{
  m_InitCodes[className] = initCode;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::clearInitCodes()
{
  m_InitCodes.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::addPythonCodes(const QString& className, const QString& pyCode)
{
  m_PythonCodes[className] = pyCode;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::clearPythonCodes()
{
  m_PythonCodes.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::addPythonicCodes(const QString& className, const QVector<QString>& pyCode)
{
  m_PythonicCodes[className] = pyCode;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::clearPythonicCodes()
{
  m_PythonicCodes.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::addDependency(QString superClassName, QString className)
{
  // qDebug() << "--- SuperClassName: " << superClassName << "  ClassName: " << className;
  bool superClassAdded = false;
  bool classNameAdded = false;

  // First check to see if the class has been added as a superclass at some point.
  for(auto object : m_ClassVector)
  {
    if(object->objectName().compare(className) == 0)
    {
      classNameAdded = true;
    }
  }
  // If it has been added, then just bail now.
  if(classNameAdded)
  {
    return;
  }

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QByteArray PythonBindingsModule::md5FileContents(const QString &filename)
{
  QFile destination(filename);
  destination.open(QFile::ReadOnly);
  QCryptographicHash destHash(QCryptographicHash::Md5);
  bool destFileAdded = destHash.addData(&destination);
  if(!destFileAdded)
  {
    qDebug() << "[PythonBindingsModule] QCryptographicHash.add(" << filename << ") FAILED.";
    return QByteArray(nullptr);
  }
  QByteArray desHashResult = destHash.result();
  destination.close();
  //qDebug() << "MD5: " << desHashResult.toHex();
  return desHashResult;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::writeOutput(bool didReplace, const QString& outLines, QString filename)
{ 
  //qDebug() << "Writing File: " << filename;
  if(didReplace == true)
  {
    QFileInfo fi2(filename);
    QString parentPath = fi2.path();
    QDir dir;
    //std::cout << "Creating Path: " << parentPath.toStdString() << std::endl;
    if(!dir.mkpath(parentPath))
    {
      QString ss = QObject::tr("[PythonBindingsModule] Error creating parent path '%1'").arg(parentPath);
      qDebug() << ss;
      return;
    }
    
    QTemporaryFile tempfile;
    QString tempFileName;
    if (tempfile.open()) 
    {
      tempFileName = tempfile.fileName(); // returns the unique file name
      QTextStream stream(&tempfile);
      stream << outLines;
      tempfile.close();
    }
    
    if(!fi2.exists())
    {
      if(!tempfile.copy(filename))
      {
        std::cout << "[PythonBindingsModule] Temp file '" << tempFileName.toStdString() << "' could not be copied to '"
                  << filename.toStdString() << "'" << std::endl;
      }
      else
      {
        qDebug() << "[PythonBindingsModule] Fresh File Generated: " << fi2.absoluteFilePath();
      }
    }
    else
    {

      QByteArray destMd5Hash = md5FileContents(filename);
      QByteArray sourceMd5Hash = md5FileContents(tempfile.fileName());

      // The hashes are different so copy the file over.
      if(destMd5Hash != sourceMd5Hash)
      {
        QFile destFile(filename);
        bool didDelete = destFile.remove();
        if(!didDelete)
        {
          qDebug() << "[PythonBindingsModule] Dest File was NOT removed: " << filename;
        }
        if(!tempfile.copy(filename))
        {
          std::cout << "[PythonBindingsModule] Temp file '" << tempFileName.toStdString() << "' could not be copied to '"
                    << filename.toStdString() << "'" << std::endl;
        }
        else
        {
          qDebug() << "[PythonBindingsModule]: New File Generated: " << fi2.absoluteFilePath();
        }
      }
    }
  
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::generateModuleFile(const QString& outputPath, const QString& isSIMPLib)
{
  Q_UNUSED(isSIMPLib);
  
  // Create the Top part of the file from a template file
  QFile source(m_TemplatePath);

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
  headerTemplate = headerTemplate.replace(FULL_LIB_NAME, m_LibName.toUpper());

  code.clear();
  out << "/* These are all the pybind11 instantiations for each of the exported classes */";
  for(auto object : m_ClassVector)
  {
    out << "\n";
    dumpRecursiveInitCode(0, object, out);
  }

  headerTemplate = headerTemplate.replace(MODULE_INIT_CODE, code);
  headerTemplate = headerTemplate.replace(DATE_TIME_GENERATED, QDateTime::currentDateTime().toString("yyyy:MM:dd hh::mm::ss.zzz"));

  writeOutput(true, headerTemplate, outputPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::generatePythonTestFile(const QString& outputPath, const QString& isSIMPLib)
{ 
  QFileInfo fi(outputPath);
  QString libOutputPath = QString("%1/%2").arg(SIMPL::PyBind11::LibraryOutputDirectory).arg(fi.fileName());
  
  QString code;
  QTextStream out(&code);
  out << "\"\"\"\n"
      << "This is a basic unit test file to ensure that the filters can be instantiated\n"
      << "This file is auto generated as part of the 'CodeScraper' program that is executed\n"
      << "during the compilation phase.\n"
      << "m_LibName=" << m_LibName << "\n"
      << "\"\"\"\n";
  QString shortLibName = m_LibName;
  shortLibName.replace("_py", "");
  out << "import dream3d\n"
    << "import dream3d.dream3d_py as d3d\n"
    << "import dream3d.dream3d_py.simpl_py as simpl\n"
    << "import dream3d.utils.simpl_common as sc\n"
    << "import dream3d.utils.simpl_test_dirs as sd\n"
    << "import dream3d.dream3d_py." << m_LibName << " as " << shortLibName << "\n"
 ;


 // out << "from "<< SIMPL::PyBind11::SIMPL_LibraryName << SIMPL::PyBind11::PythonModuleSuffix << " import *\n";
 // out << "from " << m_LibName << " import *\n";
  out << "\n\n";
  
  out << "def " << m_LibName << "UnitTest () :\n"
      << "  \"\"\"\n"
      << "  Just Try to instantiate all the filters\n"
      << "  \"\"\"\n";
  
  for(auto object : m_ClassVector)
  {
    out << "\n";
    dumpRecursivePythonCode(0, object, out);
  }
  
  
  out << "\n\n";
  out << "\"\"\"\n"
      << "Main Entry point for the python script\n"
      << "\"\"\"\n";
  out << "if __name__ == \"__main__\":\n"
      << "  print(\"" << m_LibName << " UnitTest Starting\")\n"
      << "  " << m_LibName << "UnitTest()\n"
      << "  print(\"" << m_LibName << " UnitTest Complete\")\n";
  
  writeOutput(true, code, libOutputPath);
  
}

// -----------------------------------------------------------------------------
// https://discourse.itk.org/t/pythonic-interface-to-itk-filters/1106
// -----------------------------------------------------------------------------
void PythonBindingsModule::generatePythonicInterface(const QString& outputPath, const QString& isSIMPLib)
{
  QFileInfo fi(outputPath);
  //QString libOutputPath = QString("%1/%2").arg(SIMPL::PyBind11::LibraryOutputDirectory).arg(fi.fileName());
  
  QString code;
  QTextStream out(&code);
  
  out << "\"\"\"\n Pythonic Interface to SIMPL Plugin " << getLibNameUpper() << "\n";
  out << "\"\"\"" << "\n";
  out << "\n\n";
  QString shortLibName = m_LibName;
  shortLibName.replace("_py", "");
  out << "import dream3d\n"
    << "import dream3d.dream3d_py as d3d\n"
    // << "import dream3d.dream3d_py.simpl_py as simpl\n"
    // << "import dream3d.utils.simpl_common as sc\n"
    // << "import dream3d.utils.simpl_test_dirs as sd\n"
    << "import dream3d.dream3d_py." << m_LibName << " as " << shortLibName << "\n"
    << "\n\n\n"
    ;
    
    
  QList<QString> classes = m_Headers.keys();
  QListIterator<QString> iter(classes);
  while(iter.hasNext())
  {
    QString aClass = iter.next();
    QVector<QString> pythonicCodes = m_PythonicCodes[aClass];
    QString initCodes;
    QString bodyCodes;
    if(pythonicCodes.size() >= 1) { initCodes = pythonicCodes[0];}
    if(pythonicCodes.size() >= 2) { bodyCodes = pythonicCodes[1];}
    bodyCodes = bodyCodes.replace("@shortLibName@", shortLibName);
    if(aClass.compare("AbstractFilter") == 0 && isSIMPLib.compare("FALSE") == 0)
    {
      continue;
    }
    out << "def " << SIMPL::Python::fromCamelCase(aClass) << initCodes << ":\n"
        << "    \"\"\"" << "\n"
        << "    Instantiates " << aClass << "\n"
        << "    \"\"\"" << "\n"
        << bodyCodes << "\n"
       
        << "\n\n"    
       ;
    
  }
  writeOutput(true, code, outputPath);
  
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::dumpRecursivePythonCode(int level, const QObject* object, QTextStream& out)
{
  if(object)
  { 
    // These are a list of Abstract or Top level classes that do not need to be tested
    // so let's avoid those.
    QStringList avoidThese = {"AbstractFilter", "FileReader", "FileWriter" };

    QByteArray buf;
    buf.fill(' ', level / 2 * 8);
    if(level % 2) {
      buf += "    ";
    }
    
    QString initCode = m_PythonCodes[object->objectName()];
    QString shortLibName = m_LibName;
    shortLibName.replace("_py", "");

    const char* pycode = R"PY(
  # @FILTER_NAME@
  # @INIT_CODE@
  print("# --- @FILTER_NAME@ ")
  filter = @LIB_NAME@.@FILTER_NAME@.New()
  filter.preflight()
  # print("  Preflight Error Code:%s" % filter.ErrorCondition)
  filterName = filter.NameOfClass
  if filterName != "@FILTER_NAME@" :
    print("  Error: Filter class name is not correct. %s != @FILTER_NAME@" % filterName)

)PY";
    
      
    if(!avoidThese.contains(object->objectName()))
    {
      QString code(pycode);
      code.replace("@LIB_NAME@", shortLibName);
      code.replace("@FILTER_NAME@", object->objectName());
      code.replace("@INIT_CODE@", initCode);
      out << code;
    }
    QObjectList children = object->children();
    if(!children.isEmpty())
    {
      // out << "\n";
      for(int i = 0; i < children.size(); ++i) {
        dumpRecursivePythonCode(level + 1, children.at(i), out);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::dumpRecursiveIncludeList(int level, const QObject* object, QTextStream& out)
{
  if(object)
  {
    QByteArray buf;
    buf.fill(' ', level / 2 * 8);
    if(level % 2) {
      buf += "    ";
    }
    //    QString name = object->objectName();
    //    QString flags = QLatin1String("");
    QString header = m_Headers[object->objectName()];
    if(header.isEmpty())
    {
      // qDebug() << "##### Header was empty: " << object->objectName();
    }
    else
    {
      out << "#include \"" << header << "\"\n";
    }
    QObjectList children = object->children();
    if(!children.isEmpty())
    {
      for(int i = 0; i < children.size(); ++i) {
        dumpRecursiveIncludeList(level + 1, children.at(i), out);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::dumpRecursiveInitCode(int level, const QObject* object, QTextStream& out)
{
  if(object)
  {
    QByteArray buf;
    buf.fill(' ', level / 2 * 8);
    if(level % 2) {
      buf += "    ";
    }
    
    QString initCode = m_InitCodes[object->objectName()];

    out << "  " << initCode << "\n";
    QObjectList children = object->children();
    if(!children.isEmpty())
    {
      // out << "\n";
      for(int i = 0; i < children.size(); ++i) {
        dumpRecursiveInitCode(level + 1, children.at(i), out);
      }
    }
  }
}
