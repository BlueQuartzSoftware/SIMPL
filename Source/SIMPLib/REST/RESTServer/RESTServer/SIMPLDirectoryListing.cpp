/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLDirectoryListing.h"

#include <QtCore/QFileInfoList>
#include <QtCore/QTextStream>

#include "V1Controllers/SIMPLStaticFileController.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SIMPLDirectoryListing::ParseDirForTable(QDir directory)
{
  QVector<QFileInfo> fileNames;
  ParseDir(directory, fileNames);
  return CreateHTMLTable(fileNames);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLDirectoryListing::ParseDir(QDir directory, QVector<QFileInfo>& fileNames)
{
  QFileInfoList fiList = directory.entryInfoList();
  fileNames.clear();
  int count = fiList.size();
  for(int i = 0; i < count; i++)
  {
    fileNames.push_back(fiList[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SIMPLDirectoryListing::CreateHTMLTable(QVector<QFileInfo>& fileInfos)
{
  SIMPLStaticFileController* staticFileController = SIMPLStaticFileController::Instance();

  QString docRoot = staticFileController->getDocRoot();

  QString html;
  QTextStream ss(&html);

  ss << "<table cellpadding=\"2\" cellspacing=\"0\" border=\"0\" width=\"300px\">\n";
  ss << "<tbody>\n";
  ss << "<tr>\n";
  ss << "<th align=\"right\">File</th>\n";
  ss << "<th align=\"right\">Date</th>\n";
  ss << "</tr>\n";

  char rowColor = 0;
  QString red("#FFAAAA");
  QString odd("#FFFFFF");
  QString even("#B0E4FF");
  QString color = odd;

  for(int i = 0; i < fileInfos.size(); i++)
  {
    if(rowColor == 0)
    {
      rowColor = 1;
      color = odd;
    }
    else
    {
      rowColor = 0;
      color = even;
    }

    QFileInfo fi = fileInfos[i];

    QString link;
    QTextStream linkSSlink(&link);

    QString absFilePath = fi.absoluteFilePath();

    // Verify that the absolute path begins with the docRoot path
    if(absFilePath.startsWith(docRoot))
    {
      QString webPath = absFilePath.mid(docRoot.size());
      linkSSlink << "<a href=\"" << webPath << "\">" << fi.fileName() << "</a>";
    }

    ss << "<tr bgcolor=\"" << color << "\"><td>" << link << "</td><td>" << fi.lastModified().toString() << "</td></tr>\n";
  }

  ss << "</tbody></table>\n";

  return html;
}
