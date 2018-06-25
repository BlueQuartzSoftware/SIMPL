#pragma once

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

class PythonBindingsModule;

/* *****************************************************************************
 *
 *
 ******************************************************************************/
class PythonBindingClass : public QObject
{

public:
  PythonBindingClass(PythonBindingsModule* moduleCode, const QString& isSIMPLib);
  ~PythonBindingClass();

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
  void addProperty(const QString& property);
  void clearProperties();

  SIMPL_INSTANCE_PROPERTY(QVector<QString>, Methods)
  void addMethod(const QString& method);
  void clearMethods();

  SIMPL_INSTANCE_PROPERTY(QVector<QString>, StaticNewMethods)
  void addStaticCreation(const QString& method);
  void clearStaticCreation();
 
  SIMPL_INSTANCE_PROPERTY(QVector<QString>, Constructors)
  void addConstructor(const QString& constructor);
  void clearConstructors();

  using EnumerationType = QMap<QString, QStringList>;
  SIMPL_INSTANCE_PROPERTY(EnumerationType, Enumerations)
  void addEnumeration(const QString& name, const QStringList& values);
  void clearEnumerations(); 
  

  void writeBindingFile(const QString& outputFilePath);

  /**
  *@brief quote
  *@param str
  *@return
  */
  QString quote(const QString& str);

  /**
  *@brief writeOutput
  *@param didReplace
  *@param outLines
  *@param filename
  */
  void writeOutput(bool didReplace, const QString& outLines, QString filename);

protected:
  PythonBindingClass() = delete;

  QString generateTopMatterCode();

  QString generateSharedPointerInitCode();

  QString generateStaticNewCode();

  QString generateConstructorsCodes();

  QString generatePropertiesCode();

  QString generateMethodCode();
  
  QString generateEnumerationCode();

  QString generateFooterCode();

  QByteArray md5FileContents(const QString &filename);

  QString generatePythonTestCode();

private:
  PythonBindingsModule* m_Module = nullptr;
  QString m_IsSIMPLib;

  PythonBindingClass(const PythonBindingClass&) = delete; // Copy Constructor Not Implemented
  void operator=(const PythonBindingClass&) = delete;     // Operator '=' Not Implemented
};
