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
  SIMPL_INSTANCE_PROPERTY(QString, TemplatePath)

  using MapperType = QMap<QString, QString>;

  SIMPL_INSTANCE_PROPERTY(MapperType, Headers)
  void addHeader(const QString& className, const QString& header);
  void clearHeaders();

  SIMPL_INSTANCE_PROPERTY(MapperType, InitCodes)
  void addInitCode(const QString& className, const QString& initCode);
  void clearInitCodes();

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

  void generateModuleFile(const QString& outputPath);

protected:
  void dumpRecursiveIncludeList(int level, const QObject* object, QTextStream& out);

  void dumpRecursiveInitCode(int level, const QObject* object, QTextStream& out);

  QByteArray md5FileContents(const QString &filename);

private:
  std::vector<QObject*> m_ClassVector;
};