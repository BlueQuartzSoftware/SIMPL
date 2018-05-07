#include "PythonBindingsModule.h"

#include <iostream>

#include <QtCore/QDateTime>
#include <QtCore/QTemporaryFile>
#include <QtCore/QCryptographicHash>

#include "CodeScraper/CodeScraperConstants.h"
#include "CodeScraper/SIMPLPyBind11Config.h"

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
    return QByteArray(0);
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
    QString extraTemp = QString("/tmp/%1.cxx").arg(fi2.baseName());
    if (tempfile.open()) 
    {
      tempFileName = tempfile.fileName(); // returns the unique file name
      QTextStream stream(&tempfile);
      stream << outLines;
      tempfile.close();
      #if 0
      QFile temp(extraTemp);
      QTextStream tstream(&temp);
      temp.open(QFile::WriteOnly);
      tstream << outLines;

      qDebug() << "Temp File: " << tempFileName;
      #endif
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
    
    
    #if 0
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
#endif

  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingsModule::generateModuleFile(const QString& outputPath)
{
  // Create the Top part of the file from a template file
  //  QFile source(SIMPL::PyBind11::TemplateDir + "/ModuleCode_Template.cpp");
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
void PythonBindingsModule::dumpRecursiveIncludeList(int level, const QObject* object, QTextStream& out)
{
  if(object)
  {
    QByteArray buf;
    buf.fill(' ', level / 2 * 8);
    if(level % 2)
      buf += "    ";
    QString name = object->objectName();
    QString flags = QLatin1String("");
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
      for(int i = 0; i < children.size(); ++i)
        dumpRecursiveIncludeList(level + 1, children.at(i), out);
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
    if(level % 2)
      buf += "    ";
    QString name = object->objectName();
    QString flags = QLatin1String("");
    QString initCode = m_InitCodes[object->objectName()];

    out << "  " << initCode << "\n";
    QObjectList children = object->children();
    if(!children.isEmpty())
    {
      // out << "\n";
      for(int i = 0; i < children.size(); ++i)
        dumpRecursiveInitCode(level + 1, children.at(i), out);
    }
  }
}
