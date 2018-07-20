#pragma once

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

/* *****************************************************************************
 *
 *
 ******************************************************************************/
class PythonBindingsModule
{
public:
  PythonBindingsModule();
  ~PythonBindingsModule();

  SIMPL_INSTANCE_PROPERTY(QString, LibName)
  SIMPL_INSTANCE_PROPERTY(QString, LibNameUpper)
  SIMPL_INSTANCE_PROPERTY(QString, TemplatePath)

  using MapperType = QMap<QString, QString>;
  using MapperType2 = QMap<QString, QVector<QString>>;

  SIMPL_INSTANCE_PROPERTY(MapperType, Headers)
  void addHeader(const QString& className, const QString& header);
  void clearHeaders();

  SIMPL_INSTANCE_PROPERTY(MapperType, InitCodes)
  void addInitCode(const QString& className, const QString& initCode);
  void clearInitCodes();

  SIMPL_INSTANCE_PROPERTY(MapperType, PythonCodes)
  void addPythonCodes(const QString& className, const QString& pyCode);
  void clearPythonCodes();

  SIMPL_INSTANCE_PROPERTY(MapperType2, PythonicCodes)
  void addPythonicCodes(const QString& className, const QVector<QString>& pyCode);
  void clearPythonicCodes();
    
  /**
   * @brief addDependency
   * @param superClassName
   * @param className
   */
  void addDependency(QString superClassName, QString className);

  /**
   * @brief writeOutput
   * @param didReplace
   * @param outLines
   * @param filename
   */
  void writeOutput(bool didReplace, const QString& outLines, QString filename);

  /**
   * @brief generateModuleFile
   * @param outputPath
   * @param isSIMPLib
   */
  void generateModuleFile(const QString& outputPath, const QString& isSIMPLib);
  
  /**
   * @brief generatePythonTestFile
   * @param outputPath
   * @param isSIMPLib
   */
  void generatePythonTestFile(const QString& outputPath, const QString& isSIMPLib);
  
  
  /**
   * @brief generatePythonicInterface
   * @param outputPath
   * @param isSIMPLib
   */
  void generatePythonicInterface(const QString& outputPath, const QString& isSIMPLib);
  
  
protected:
  /**
   * @brief dumpRecursivePythonCode
   * @param level
   * @param object
   * @param out
   */
  void dumpRecursivePythonCode(int level, const QObject* object, QTextStream& out);

  /**
   * @brief dumpRecursiveIncludeList
   * @param level
   * @param object
   * @param out
   */
  void dumpRecursiveIncludeList(int level, const QObject* object, QTextStream& out);

  /**
   * @brief dumpRecursiveInitCode
   * @param level
   * @param object
   * @param out
   */
  void dumpRecursiveInitCode(int level, const QObject* object, QTextStream& out);

  /**
   * @brief md5FileContents
   * @param filename
   * @return 
   */
  QByteArray md5FileContents(const QString &filename);

private:
  std::vector<QObject*> m_ClassVector;
};
