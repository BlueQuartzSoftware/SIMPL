
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QSet>


#include "ToolConfiguration.h"


void parseCompileCommand(const QString& command, const QString &directory, QMap<QString, bool> &includeDirs)
{

  QStringList tokens = command.split(' ');
  foreach(QString token, tokens)
  {
    if(token.startsWith("-I"))
    {
      QString substring = token.mid(2);
      if(!substring.startsWith("/"))
      {
        substring = directory + "/" + substring;
      }

      if(!includeDirs.contains(substring))
      {
        includeDirs[substring] = true;
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeVSCodeCProperties(QMap<QString, bool> includeDirs, const QString filePath)
{
  QJsonObject rootObj;



  QJsonArray configArray;

  QJsonObject browseObj;
  browseObj["limitSymbolsToIncludedHeaders"] = true;
  browseObj["databaseFilename"] = "";

  QJsonObject config;

#if defined (Q_OS_MAC)
  config["name"] = "Mac";
  QJsonArray includes;
  QMap<QString, bool>::const_iterator i = includeDirs.constBegin();
  while (i != includeDirs.constEnd()) {
    includes.push_back(i.key());
    i++;
  }
  config["includPath"] = includes;
  config["browse"] = browseObj;
  configArray.append(config);

#elif defined (Q_OS_WIN)
  config["name"] = "Win32";
  config["includPath"] = includeDirs;
  config["browse"] = browseObj;
  configArray.append(config);
#else

  config["name"] = "Linux";
  config["includPath"] = includes;
  config["browse"] = browseObj;
  configArray.append(config);
#endif

  rootObj["configurations"] = configArray;

  QFile out(filePath);
  if(!out.open(QFile::WriteOnly))
  {
    qDebug() << "Error opening JSON file for writing. No output generated.";
    return;
  }

  qDebug() << "Writing JSON file.. ";
  QJsonDocument doc(rootObj);
  out.write(doc.toJson());
  out.close();
}



int main(int argc, char *argv[])
{


  QString jsonFile = D3DTools::GetDREAM3DBinaryDir() + "/" + "compile_commands.json";

  QByteArray json;

  {
    // Read the Source File
    QFileInfo fi(jsonFile);
    QFile source(jsonFile);
    source.open(QFile::ReadOnly);
    json = source.readAll();
    source.close();
  }

  QJsonParseError parseError;
  QJsonDocument jDoc = QJsonDocument::fromJson(json, &parseError);

  if(parseError.error != QJsonParseError::NoError)
  {

    qDebug() << parseError.errorString();
    return EXIT_FAILURE;
  }

  QJsonArray docArray = jDoc.array();


  QMap<QString, bool> includeDirs;

  for (QJsonArray::Iterator iter = docArray.begin(); iter != docArray.end();iter++)
  {

    QJsonValueRef current = *iter;
    if(current.isObject())
    {
      QJsonObject jObj = current.toObject();
      QString command = jObj["command"].toString();
      QString directory = jObj["directory"].toString();
      parseCompileCommand(command, directory, includeDirs);
    }

  }


  QString filePath = D3DTools::GetSIMPLProjParentDir() + "/DREAM3D/.vscode/c_cpp_properties.json";
  qDebug() << filePath;
  writeVSCodeCProperties(includeDirs, filePath);



  return 0;
}
