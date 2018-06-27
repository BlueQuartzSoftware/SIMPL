#include "PythonBindingClass.h"

#include <iostream>

#include <QtCore/QDateTime>
#include <QtCore/QTemporaryFile>
#include <QtCore/QCryptographicHash>

#include "CodeScraper/CodeScraperConstants.h"
#include "CodeScraper/SIMPLPyBind11Config.h"
#include "PythonBindingsModule.h"

PythonBindingClass::PythonBindingClass(PythonBindingsModule* moduleCode, const QString& isSIMPLib)
: QObject(nullptr)
, m_NeedsWrapping(false)
, m_HasSuperClass(false)
, m_IsSharedPointer(false)
, m_HasStaticNewMacro(false)
, m_Module(moduleCode)
, m_IsSIMPLib(isSIMPLib)
{
}

PythonBindingClass::~PythonBindingClass() = default;

void PythonBindingClass::addProperty(const QString& property)
{
  m_Properties.push_back(property);
}
void PythonBindingClass::clearProperties()
{
  m_Properties.clear();
}

void PythonBindingClass::addMethod(const QString& method)
{
  m_Methods.push_back(method);
}
void PythonBindingClass::clearMethods()
{
  m_Methods.clear();
}

void PythonBindingClass::addStaticCreation(const QString& method)
{
  m_StaticNewMethods.push_back(method);
}
void PythonBindingClass::clearStaticCreation()
{
  m_StaticNewMethods.clear();
}

void PythonBindingClass::addConstructor(const QString& constructor)
{
  m_Constructors.push_back(constructor);
}
void PythonBindingClass::clearConstructors()
{
  m_Constructors.clear();
}

void PythonBindingClass::addEnumeration(const QString& name, const QStringList& values)
{
  m_Enumerations[name] = values;
}
void PythonBindingClass::clearEnumerations()
{
  m_Enumerations.clear();
}
#if 0
#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingClass::writeBindingFile(const QString& outputFilePath)
{
  if(getNeedsWrapping())
  {
    QString output;
    QTextStream out(&output);
    bool hasInstanceVar = false;
  //  out << "#if 0\n";
  //  out << "Properties.size(): " << m_Properties.size() << "\n";
  //  out << "Methods.size(): " << m_Methods.size() << "\n";
  //  out << "StaticNewMethods.size(): " << m_StaticNewMethods.size() << "\n";
  //  out << "Constructors.size(): " << m_Constructors.size() << "\n";
  //  out << "Enumerations.size(): " << m_Enumerations.size() << "\n";
  //  out << "#endif\n";
    
    out << generateTopMatterCode();
    
    //out << "/* BEFORE generateSharedPointerInitCode CODE */" << "\n";
    out << generateSharedPointerInitCode();
    //out << "/* AFTER generateSharedPointerInitCode CODE */" << "\n";
    
    //out << "/* BEFORE generateConstructorsCodes CODE */" << "\n";
    out << generateConstructorsCodes();
    //out << "/* AFTER generateConstructorsCodes CODE */" << "\n";

    if(getHasStaticNewMacro() || (!m_StaticNewMethods.isEmpty() && getIsSharedPointer()) )
    {
      out << "  /* Instantiate the class instance variable*/\n";
      out << "  instance\n";
      hasInstanceVar = true;
    }
    //out << "/* BEFORE generateStaticNewCode CODE */" << "\n";
    out << generateStaticNewCode();
    //out << "/* AFTER generateStaticNewCode CODE */" << "\n";
    
    if(!getIsSharedPointer())
    {
      hasInstanceVar = true;
    }

    
    if(!m_Properties.isEmpty() && !hasInstanceVar)
    {
      out << "  /* Instantiate the class instance variable*/\n";
      out << "  instance\n";
      hasInstanceVar = true;
    }    
    out << generatePropertiesCode();
    
    
    if(!m_Methods.isEmpty() && !hasInstanceVar)
    {
      out << "  /* Instantiate the class instance variable*/\n";
      out << "  instance\n";
      hasInstanceVar = true;
    }    
    out << generateMethodCode();
    
    if(hasInstanceVar)
    {
      out << "  ;\n";
    }
    out << generateEnumerationCode();
    out << generateFooterCode();

    writeOutput(getNeedsWrapping(), output, outputFilePath);

    QString initCode;
    QTextStream init(&initCode);

    init << "PySharedPtrClass<" << getClassName() << "> ";
    init << getLibName() << "_" << getClassName() << " = ";
    if(getHasSuperClass())
    {
      if(m_IsSIMPLib.compare("TRUE") == 0)
      {
      //  init << "pybind11_init_" << getLibName() << "_" << getClassName() << "(mod, " << getLibName() << "_" << getSuperClass() << ");\n";
        init << "declare" << getClassName() << "(mod, " << getLibName() << "_" << getSuperClass() << ");\n";
      }
      else
      {
      //  init << "pybind11_init_" << getLibName() << "_" << getClassName() << "(mod"
      init << "declare" << getClassName() << "(mod"

             << ");\n";
      }
    }
    else
    {
      //init << "pybind11_init_" << getLibName() << "_" << getClassName() << "(mod);";
      init << "declare" << getClassName() << "(mod);";
    }

    m_Module->addInitCode(getClassName(), initCode);
    m_Module->addPythonCodes(getClassName(), "");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString quote(const QString& str)
{
  return QString("\"%1\"").arg(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QByteArray PythonBindingClass::md5FileContents(const QString &filename)
{
  QFile destination(filename);
  destination.open(QFile::ReadOnly);
  QCryptographicHash destHash(QCryptographicHash::Md5);
  bool destFileAdded = destHash.addData(&destination);
  if(!destFileAdded)
  {
    qDebug() << "[PythonBindingClass] QCryptographicHash.add(" << filename << ") FAILED.";
    return QByteArray(nullptr);
  }
  QByteArray desHashResult = destHash.result();
  destination.close();
  // qDebug() << "MD5: " << desHashResult.toHex();
  return desHashResult;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PythonBindingClass::writeOutput(bool didReplace, const QString& outLines, QString filename)
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
    
    QTemporaryFile tempfile;
    QString tempFileName;
    if (tempfile.open()) {
      tempFileName = tempfile.fileName(); // returns the unique file name
      QTextStream stream(&tempfile);
      stream << outLines;
      tempfile.close();
      //qDebug() << "Wrote " << tempFileName;
    }
    
    if(!fi2.exists())
    { 
      //qDebug() << "DOES NOT EXIST: " << filename;
      if(!tempfile.copy(filename))
      {
        std::cout << "[PythonBindingClass] Temp file '" << tempFileName.toStdString() << "' could not be copied to '"
                  << filename.toStdString() << "'" << std::endl;
      }
      else
      {
        qDebug() << "[PythonBindingClass] Pybind11 Module Generated for: " << fi2.absoluteFilePath();
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
          qDebug() << "[PythonBindingClass] Dest File was NOT removed: " << filename;
        }
        if(!tempfile.copy(filename))
        {
          std::cout << "[PythonBindingClass] Temp file '" << tempFileName.toStdString() << "' could not be copied to '"
                    << filename.toStdString() << "'" << std::endl;
        }
        else
        {
          qDebug() << "[PythonBindingClass]: New File Generated: " << fi2.absoluteFilePath();
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
QString PythonBindingClass::generateTopMatterCode()
{
  QFileInfo fi(m_SourceFile);
  QString subPath = fi.absolutePath();
  subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
  subPath = subPath.remove(0, 1);                            // Remove the front / character

  // Create the Top part of the file from a template file
  QFile source(SIMPL::PyBind11::TemplateDir + "/Pybind11TopMatter.in.h");
  source.open(QFile::ReadOnly);
  QString headerTemplate = source.readAll();
  source.close();
  headerTemplate = headerTemplate.replace(CLASS_NAME, getClassName());
  QString headerPath = QString("%1/%2.h").arg(subPath, 1).arg(getClassName(), 2);
  if(!m_CharsToStrip.isEmpty())
  {
    headerPath = headerPath.replace(m_CharsToStrip, "");
  }

  headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
  headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);
  headerTemplate = headerTemplate.replace(DATE_TIME_GENERATED, QDateTime::currentDateTime().toString("yyyy:MM:dd hh::mm::ss.zzz"));
  return headerTemplate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generateSharedPointerInitCode()
{
  if(!getIsSharedPointer())
  {
    return QString("");
  }
  QFile source;
  QString headerTemplate;

  if(getHasSuperClass() && !getSuperClass().isEmpty())
  {
    source.setFileName(SIMPL::PyBind11::TemplateDir + "/DerivedSharedPointerClassInit.in.h");
    source.open(QFile::ReadOnly);
    headerTemplate = source.readAll();
    source.close();

    QTextStream out(&headerTemplate);
    
    out << "PySharedPtrClass<@CLASS_NAME@> declare@CLASS_NAME@(py::module &m";
    if(m_IsSIMPLib.compare("TRUE") == 0)
    {
      out << ", PySharedPtrClass<@SUPERCLASS_NAME@>& parent";
    }
    out << ")\n{\n";

    if(m_IsSIMPLib.compare("TRUE") == 0)
    {
      out << "  PySharedPtrClass<@CLASS_NAME@> instance(m, \"@CLASS_NAME@\", parent);";
    }
    else
    {
      out << "  /* Create an alias to shorten up the declarations */\n";
      out << "  using Py@CLASS_NAME@Type = py::class_<@CLASS_NAME@, AbstractFilter, std::shared_ptr<@CLASS_NAME@>>;\n";
      out << "\n";
      out << "  /* Import the SIMPL pybind11 module so that we can inherit from AbstractFilter */\n";
      out << "  //py::module::import(\""<< SIMPL::PyBind11::SIMPL_LibraryName << SIMPL::PyBind11::PythonModuleSuffix << "\");\n"
          << "  Py@CLASS_NAME@Type instance(m, \"@CLASS_NAME@\");\n"
          << "\n";
    }
  }
  else
  {
    source.setFileName(SIMPL::PyBind11::TemplateDir + "/SharedPointerClassInit.in.h");
    source.open(QFile::ReadOnly);
    headerTemplate = source.readAll();
    source.close();
  }

  QFileInfo fi(m_SourceFile);
  QString subPath = fi.absolutePath();
  subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
  subPath = subPath.remove(0, 1);                            // Remove the front / character

  headerTemplate = headerTemplate.replace(CLASS_NAME, getClassName());
  QString headerPath = QString("%1/%2.h").arg(subPath, 1).arg(getClassName(), 2);
  headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
  headerTemplate = headerTemplate.replace(SUPERCLASS_NAME, getSuperClass());
  headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);

//  QString pybind11HeaderPath = QString("%1/pybind11/%2_PY11.h").arg(subPath).arg(getClassName());
  QString pybind11HeaderPath = QString("%1/%2_PY11.h").arg(subPath, 1).arg(getClassName(), 2);
  if(!m_CharsToStrip.isEmpty())
  {
    pybind11HeaderPath = pybind11HeaderPath.replace(m_CharsToStrip, "");
  }
    
  m_Module->addHeader(getClassName(), pybind11HeaderPath);
  m_Module->addDependency(getSuperClass(), getClassName());

  return headerTemplate;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generateStaticNewCode()
{
  //      if(!getIsSharedPointer())
  //      {
  //        return QString("");
  //      }
  QString code;
  QTextStream constructors(&code);

  if(getHasStaticNewMacro())
  {
    constructors << TAB << ".def(py::init([]()\n    { \n      return " << getClassName() << "::New();\n    }))" << NEWLINE_SIMPL;
    constructors << TAB << ".def_static(\"New\", &" << getClassName() << "::New)" << NEWLINE_SIMPL;
  }

  for(auto line : m_StaticNewMethods)
  {
    QStringList tokens = line.split("(");
    tokens = tokens[1].replace(")", "").trimmed().split(" ");
    QString methodName = tokens[0];
    if(tokens.size() > 2) // Second Token is ARGS
    {
      QString s1, s2;
      QTextStream o1(&s1);
      QTextStream o2(&s2);
      for(int i = 2; i < tokens.size(); i++)
      {
        o1 << tokens[i] << " var_" << i - 2;
        o2 << "var_" << i - 2;
        if(i != tokens.size() - 1)
        {
          o1 << ", ";
          o2 << ", ";
        }
      }
      constructors << TAB << ".def(py::init([](" << s1 << ") {\n      return " << getClassName() << "::" << methodName << "(" << s2 << ");\n    }))" << NEWLINE_SIMPL;
      constructors << TAB << ".def_static(\"" << methodName << "\", &" << getClassName() << "::" << methodName << ")" << NEWLINE_SIMPL;
    }
  }

  return code;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generateConstructorsCodes()
{
  if(getIsSharedPointer())
  {
    return QString("");
  }

  QString code;
  QTextStream constructors(&code);

  QFile source;
  source.setFileName(SIMPL::PyBind11::TemplateDir + "/SimpleClassInit.in.h");
  source.open(QFile::ReadOnly);
  QString headerTemplate = source.readAll();
  source.close();
  headerTemplate = headerTemplate.replace(CLASS_NAME, getClassName());

  QFileInfo fi(m_SourceFile);
  QString subPath = fi.absolutePath();
  subPath = subPath.replace(m_SourceDir.absolutePath(), ""); // Remove the system dependent file path
  subPath = subPath.remove(0, 1);                            // Remove the front / character

  QString headerPath = QString("%1/%2.h").arg(subPath, getClassName());
  headerTemplate = headerTemplate.replace(HEADER_PATH, headerPath);
  headerTemplate = headerTemplate.replace(SUPERCLASS_NAME, getSuperClass());
  headerTemplate = headerTemplate.replace(LIB_NAME, m_LibName);

  constructors << headerTemplate;
  constructors << TAB << "/* Begin default constructor declarations */" << NEWLINE_SIMPL;
  constructors << TAB << ".def(py::init<" << getClassName() << ">())" << NEWLINE_SIMPL;
  constructors << TAB << ".def(py::init<" << getClassName() << " const &>())" << NEWLINE_SIMPL;
  if(!m_Constructors.isEmpty())
  {
    constructors << TAB << "/* Number of non-default constructors: " <<  m_Constructors.size()  << "*/" << NEWLINE_SIMPL;
  }
  for(auto line : m_Constructors)
  {
    QStringList tokens = line.split("(");
    tokens = tokens[1].replace(")", "").trimmed().split(" ");
    //QString keyword = tokens[0];
    //.def(py::init<const std::string &>())
    constructors << TAB << ".def(py::init<";
    for(int i = 1; i < tokens.size(); i++)
    {
      constructors << "const " << tokens[i] << " &";
      if(i != tokens.size() - 1) { constructors << ", "; }
    }
    constructors << ">())" << NEWLINE_SIMPL;
  }

  QString pybind11HeaderPath = QString("%1/%2_PY11.h").arg(subPath, getClassName());
  if(!m_CharsToStrip.isEmpty())
  {
    pybind11HeaderPath = pybind11HeaderPath.replace(m_CharsToStrip, "");
  }
  m_Module->addHeader(getClassName(), pybind11HeaderPath);
  m_Module->addDependency(getSuperClass(), getClassName());

  return code;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generatePropertiesCode()
{
  QString code;
  QTextStream out(&code);
  for(auto line : m_Properties)
  {
    QStringList tokens = line.split("(");
    tokens = tokens[1].replace(")", "").trimmed().split(" ");
    QString pyType = tokens[0];
    QString varName = tokens[1];

    if(tokens.size() == 6)
    {
      out << TAB << "/* Property accessors for " << varName << " */" << NEWLINE_SIMPL;
      out << TAB << ".def_property(\"" << varName << "\", &" << getClassName() << "::get" << varName << ", &" << getClassName() << "::set" << varName << ")" << NEWLINE_SIMPL;
    }
    else if(tokens.size() == 4 && tokens[2] == ::kRead)
    {
      out << TAB << "/* Read Only Property for " << varName << " */" << NEWLINE_SIMPL;
      out << TAB << ".def_property_readonly(\"" << varName << "\", &" << getClassName() << "::get" << varName << ")" << NEWLINE_SIMPL;
    }
    else if(tokens.size() == 4 && tokens[2] == ::kWrite)
    {
      out << TAB << "/* Write Only Property for " << varName << " */" << NEWLINE_SIMPL;
      out << TAB << ".def(\"set" << varName << "\", &" << getClassName() << "::set" << varName << ", py::arg(\"" << varName << "\"))" << NEWLINE_SIMPL;
    }
  }
  return code;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generatePythonTestCode()
{
  QString code;
  QTextStream out(&code);
  for(auto line : m_Properties)
  {
    QStringList tokens = line.split("(");
    tokens = tokens[1].replace(")", "").trimmed().split(" ");
    QString pyType = tokens[0];
    QString varName = tokens[1];

    if(tokens.size() == 6)
    {
      out << TAB << "/* Property accessors for " << varName << " */" << NEWLINE_SIMPL;
      out << TAB << ".def_property(\"" << varName << "\", &" << getClassName() << "::get" << varName << ", &" << getClassName() << "::set" << varName << ")" << NEWLINE_SIMPL;
    }
    else if(tokens.size() == 4 && tokens[2] == ::kRead)
    {
      out << TAB << "/* Read Only Property for " << varName << " */" << NEWLINE_SIMPL;
      out << TAB << ".def_property_readonly(\"" << varName << "\", &" << getClassName() << "::get" << varName << ")" << NEWLINE_SIMPL;
    }
    else if(tokens.size() == 4 && tokens[2] == ::kWrite)
    {
      out << TAB << "/* Write Only Property for " << varName << " */" << NEWLINE_SIMPL;
      out << TAB << ".def(\"set" << varName << "\", &" << getClassName() << "::set" << varName << ", py::arg(\"" << varName << "\"))" << NEWLINE_SIMPL;
    }
  }
  return code;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generateMethodCode()
{
  QString code;
  QTextStream out(&code);

  for(auto line : m_Methods)
  {
    QStringList tokens = line.split("(");
    tokens = tokens[1].replace(")", "").trimmed().split(" ");
   // QString returnType = tokens[0];
    QString methodName = tokens[1];
    out << TAB << "/* Class instance method " << methodName << " */" << NEWLINE_SIMPL;
    bool methodIsConst = false;
    if(tokens.last().compare(::kConstMethod) == 0)
    {
      methodIsConst = true;
      tokens.pop_back();
    }
    if(tokens.size() == 2)
    {
      out << TAB << ".def(\"" << methodName << "\", &" << getClassName() << "::" << methodName << ")" << NEWLINE_SIMPL;
    }
    else if(tokens.size() >= 3 && tokens[2] == ::kOverload)
    {
      /*
       * C++14 style
      .def("set", py::overload_cast<int>(&Pet::set), "Set the pet's age")
      .def("set", py::overload_cast<const std::string &>(&Pet::set), "Set the pet's name");
      */
#if 1 /* C++14 style */
      out << TAB << ".def(\"" << methodName << "\", py::overload_cast<";
      for(int32_t i = 3; i < tokens.size(); i++)
      {
        QStringList varPair = tokens[i].split(","); // Split the var,type pair using a comma
        out << varPair[0].replace('.', ' ');
        if(i != tokens.size()-1)
        {
          out << ", ";
        }
      }
      out << ">(&" << getClassName() << "::" << methodName;
      if(methodIsConst)
      {
        out << ", py::const_";
      }
      out << ")";

#else /* C++11 Style */
      /*
      * .def("getAttributeMatrix", (AttributeMatrix::Pointer (DataContainer::*)(const QString &)) &DataContainer::getAttributeMatrix, "Set the pet's age")
      * .def("getAttributeMatrix", (AttributeMatrix::Pointer (DataContainer::*)(const DataArrayPath &)) &DataContainer::getAttributeMatrix, "Set the pet's name")
      */
      out << TAB << ".def(\"" << methodName << "\", (" << tokens[0] << " (" << getClassName() << "::*)(";
      for(int32_t i = 3; i < tokens.size(); i++)
      {  
        QStringList varPair = tokens[i].split(","); // Split the var,type pair using a comma
        
        out << varPair[0].replace('.', ' ');
        if(i != tokens.size()-1)
        {
          out << ", ";
        }
      }
      out << ")) &"<<getClassName()<<"::"<<tokens[1];
#endif
      
      for(int32_t i = 3; i < tokens.size(); i++)
      {
        QStringList varPair = tokens[i].split(","); // Split the var,type pair using a comma
        out << ", \n" << TAB << TAB << TAB << TAB << "py::arg(\"" << varPair[1] << "\")";
      }
      out << NEWLINE_SIMPL << TAB << TAB << TAB << ")" << NEWLINE_SIMPL;
           
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generateEnumerationCode()
{
  QString code;
  QTextStream out(&code);
  EnumerationType::Iterator iter = m_Enumerations.begin();
  
  for (iter = m_Enumerations.begin(); iter != m_Enumerations.end(); iter++)
  {
    
    out << "\n\n  /* Enumeration code for " << getClassName() << "::" << iter.key() << " ******************/\n";
    QString n = iter.key(); // Get the variable name of the enumeration
    out << "  py::enum_<" << getClassName() << "::" << n << ">(instance, \"" << n << "\")\n";
    QStringList values = iter.value();
    for(auto v : values)
    { 
      if(v.endsWith(',')) // For those enumerations that do not have an actual value.
      {
        v = v.remove(',');
      }
      out << "    .value(\"" << v << "\", " << getClassName() << "::" << n << "::" << v << ")\n";
    }
    out << "    .export_values();\n";
  } 
  //qDebug() << code;
  return code;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PythonBindingClass::generateFooterCode()
{
  QString code;
  QTextStream out(&code);
  //out << TAB << ";" << NEWLINE_SIMPL;
  out << NEWLINE_SIMPL << "  /* Return the instance */" << NEWLINE_SIMPL;
  out << TAB << "return instance;" << NEWLINE_SIMPL;
  out << "}" << NEWLINE_SIMPL;
  out << "\n\n";
  
  // Close up the Anonymous namespace
  out << "} /* End anonymous namespace */\n\n";
  return code;
  
}
