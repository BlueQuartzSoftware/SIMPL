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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtWidgets/QWizard>

#include "SIMPLib/DataContainers/DataArrayPath.h"

/**
 * @brief The ImportASCIIDataBundle struct
 */
struct ImportASCIIDataBundle
{
  QVector<QString> m_ColumnTypes;
  QVector<QString> m_ColumnNames;
  int m_StartLine;
  char m_Delimiter;
  int m_DataSize;
};

Q_DECLARE_METATYPE(ImportASCIIDataBundle)

class ASCIIDataModel;
class ASCIIWizardData;

class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

/**
 * @brief The ImportASCIIDataWizard class
 */
class ImportASCIIDataWizard : public QWizard
{
  Q_OBJECT

public:
  enum WizardPages
  {
    Delimited,
    DataFormat
  };

  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  ImportASCIIDataWizard(const QString& inputFilePath, int numLines, DataContainerArrayShPtrType dca, QWidget* parent = nullptr);

  /**
   * @brief ImportASCIIDataWizard
   * @param wizardData
   */
  ImportASCIIDataWizard(ASCIIWizardData* wizardData, DataContainerArrayShPtrType dca, QWidget* parent = nullptr);

  ~ImportASCIIDataWizard() override;

  static const int TotalPreviewLines = 50;

  /**
   * @brief Static function that converts the boolean delimiter values to a list of delimiter characters
   */
  static QList<char> ConvertToDelimiters(bool tabAsDelimiter, bool semicolonAsDelimiter, bool commaAsDelimiter, bool spaceAsDelimiter);

  /**
   * @brief Static function that will read 'numOfLines' lines from the file at 'inputFilePath', starting at line 'beginIndex'
   * @param inputFilePath The path to the file
   * @param beginLine The line to begin reading at in the file
   * @param numOfLines The number of lines to read from the file
   */
  static QStringList ReadLines(const QString& inputFilePath, int beginLine, int numOfLines);

  /**
   * @brief Static convenience function that will read a certain line from the file 'inputFilePath'.
   * @param inputFilePath The path to the file
   * @param line The line to read from the file
   */
  static QString ReadLine(const QString& inputFilePath, int line);

  /**
   * @brief Static function that loads lines into the table objects.  The full lines are stored in the ASCIIDataItem object that backs up each table item, and are not actually displayed in the table
   * yet.
   * @param inputFilePath The path to the file to load
   * @param beginLine The line to begin loading at in the file
   */
  static void LoadOriginalLines(QStringList lines, ASCIIDataModel* model);

  /**
   * @brief Static function that will insert the tokenized lines into the preview table.
   * @param tokenizedLines List of tokenized lines
   * @param firstRowHeaderIndex The name of the first row's header (an index, such as '2' or '5')
   */
  static void InsertTokenizedLines(QList<QStringList> tokenizedLines, int firstRowHeaderIndex, ASCIIDataModel* model);

  /**
   * @brief Static function that inserts the full strings stored in each row item into one column in the preview table.
   * @param lines List of lines
   * @param firstRowHeaderIndex The name of the first row's header (an index, such as '2' or '5')
   */
  static void InsertLines(QStringList lines, int firstRowHeaderIndex, ASCIIDataModel* model);

  /**
   * @brief Updates the DataArrayPath used for importing data
   * @param renamePath
   */
  void updateDataArrayPath(DataArrayPath::RenameType renamePath);

  QList<char> getDelimiters();
  bool getConsecutiveDelimiters();
  QStringList getHeaders();
  QStringList getDataTypes();
  int getBeginningLineNum();
  QString getInputFilePath();
  std::vector<size_t> getTupleDims();
  bool getAutomaticAM();
  DataArrayPath getSelectedPath();
  int getAttributeMatrixType();

  void setInputFilePath(const QString& inputFilePath);

  void setEditSettings(bool value);

protected Q_SLOTS:
  void refreshModel();
  void cleanupPage(int id) override;

private:
  QString m_InputFilePath;
  int m_NumLines = -1;
  bool m_EditSettings = false;
  DataContainerArrayShPtrType m_Dca = nullptr;

  QPushButton* m_RefreshBtn = nullptr;
  QSharedPointer<ASCIIDataModel> m_ASCIIDataModel;

public:
  ImportASCIIDataWizard(const ImportASCIIDataWizard&) = delete;            // Copy Constructor Not Implemented
  ImportASCIIDataWizard(ImportASCIIDataWizard&&) = delete;                 // Move Constructor Not Implemented
  ImportASCIIDataWizard& operator=(const ImportASCIIDataWizard&) = delete; // Copy Assignment Not Implemented
  ImportASCIIDataWizard& operator=(ImportASCIIDataWizard&&) = delete;      // Move Assignment Not Implemented
};
