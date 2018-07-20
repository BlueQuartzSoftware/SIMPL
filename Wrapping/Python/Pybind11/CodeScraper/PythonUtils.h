#pragma once


#include <QtCore/QString>
#include <QtCore/QRegularExpression>


namespace SIMPL
{
namespace Python
{

static QString fromCamelCase(const QString &s)
{
  static QRegularExpression regExp1 {"(.)([A-Z][a-z]+)"};
  static QRegularExpression regExp2 {"([a-z0-9])([A-Z])"};
  
  QString result = s;
  result.replace(regExp1, "\\1_\\2");
  result.replace(regExp2, "\\1_\\2");
  
  return result.toLower();
}

#if 0
static QString toCamelCase(const QString& s)
{
  QStringList parts = s.split('_', QString::SkipEmptyParts);
  for (int i=1; i<parts.size(); ++i) 
  {
    parts[i].replace(0, 1, parts[i][0].toUpper());
  }
  return parts.join("");
}
#endif

}
}
