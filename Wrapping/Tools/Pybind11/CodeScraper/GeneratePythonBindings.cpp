
#include <stdint.h>

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

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

#include "PyBind11Generator.h"
#include "PythonBindingClass.h"
#include "PythonBindingsModule.h"

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

  if(argc != 5)
  {
    std::cout << "GeneratePythonBindings needs 2 arguments:" << std::endl;
    std::cout << "   [1] Path to the source directory to recursively search" << std::endl;
    std::cout << "   [2] Characters to strip from the path. '-' means do not strip anything" << std::endl;
    std::cout << "   [3] Name of the Library/Plugin" << std::endl;
    std::cout << "   [4] Top Level Directory where to write the generated files" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Source Directory: " << argv[1] << std::endl;
  std::cout << "Characters to strip: " << argv[2] << std::endl;
  std::cout << "Name of the Library: " << argv[3] << std::endl;
  std::cout << "Generated Dir: " << argv[4] << std::endl;

  QString charsToStrip = QString::fromLatin1(argv[2]);
  if(charsToStrip == "-")
  {
    charsToStrip = "";
  }
  QString libName = QString::fromLatin1(argv[3]);
  QString genDir = QString::fromLatin1(argv[4]);

  PyBind11Generator bindingsGenerator(QDir(QString::fromLatin1(argv[1])), charsToStrip, libName, genDir);

  bindingsGenerator.execute();

  return EXIT_SUCCESS;
}
