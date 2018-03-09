#include "PythonBindingsModule.h"

#include <QtCore/QDateTime>
#include <iostream>

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
void PythonBindingsModule::writeOutput(bool didReplace, const QString& outLines, QString filename)
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
  out << "/* These are all the pybind11 headers for each for each of the exported classes */";

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
