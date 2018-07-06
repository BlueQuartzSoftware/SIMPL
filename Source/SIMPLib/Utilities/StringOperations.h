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
#pragma once

#include <QtCore/QStringList>

#include "SIMPLib/SIMPLib.h"

class SIMPLib_EXPORT StringOperations
{
  public:
    StringOperations();
    virtual ~StringOperations();

    /**
    * @brief Static function that will split the strings argument into a list of tokens based on a list of delimiters
    * @param strings The strings to tokenize
    * @param delimiters List of delimiters
    * @param consecutiveDelimiters Boolean that determines if consecutive delimiters should be treated as one delimiter
    */
    static QList<QStringList> TokenizeStrings(QStringList strings, QList<char> delimiters, bool consecutiveDelimiters);

    /**
    * @brief Static function that will split the string argument into a list of tokens based on a list of delimiters
    * @param string The string to tokenize
    * @param delimiters List of delimiters
    * @param consecutiveDelimiters Boolean that determines if consecutive delimiters should be treated as one delimiter
    */
    static QStringList TokenizeString(QString string, QList<char> delimiters, bool consecutiveDelimiters);

    /**
     * @brief This function will createa string with the proper amount of padding for the maximum value.
     * @param index
     * @param maxIndex
     * @return
     */
    static QString GenerateIndexString(int index, int maxIndex);

    /**
     * @brief This function will generate a string with the prescribed number of padding digits
     * @param value The integer value
     * @param totalDigits The total number of digits to appear in the string
     * @param padChar The character to use for the padding.
     * @return
     */
    static QString GeneratePaddedString(int value, int totalDigits, char padChar);

  private:
    StringOperations(const StringOperations&) = delete; // Copy Constructor Not Implemented
    void operator=(const StringOperations&) = delete;   // Move assignment Not Implemented
};


