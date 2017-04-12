/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "StringOperations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringOperations::StringOperations()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StringOperations::~StringOperations()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList StringOperations::TokenizeString(QString line, QList<char> delimiters, bool consecutiveDelimiters)
{
  QStringList tokenList;

  if(delimiters.isEmpty() == true)
  {
    tokenList.push_back(line);
    return tokenList;
  }

  int start = 0;
  for(int i = 0; i < line.size(); i++)
  {
    char character = line.at(i).toLatin1();
    if(delimiters.contains(character) == true)
    {
      QString token = line.mid(start, i - start);
      if(token.isEmpty() == false)
      {
        tokenList.push_back(token);

        if(consecutiveDelimiters == true)
        {
          while(i < line.size() - 1 && delimiters.contains(character) == true)
          {
            i++;
            character = line.at(i).toLatin1();
          }
          i--;
        }
      }

      start = i + 1;
    }
  }

  QString token = line.mid(start, line.size() - start);
  if(token.isEmpty() == false)
  {
    tokenList.push_back(token);
  }

  return tokenList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QStringList> StringOperations::TokenizeStrings(QStringList lines, QList<char> delimiters, bool consecutiveDelimiters)
{
  QList<QStringList> tokenizedLines;
  for(int row = 0; row < lines.size(); row++)
  {
    QString line = lines[row];
    QStringList tokenizedLine = TokenizeString(line, delimiters, consecutiveDelimiters);

    tokenizedLines.push_back(tokenizedLine);
  }

  return tokenizedLines;
}
