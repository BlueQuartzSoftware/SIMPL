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

#include "ImportASCIIDataWizard.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "SIMPLib/CoreFilters/util/ASCIIWizardData.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"

#include "ASCIIDataModel.h"
#include "DataFormatPage.h"
#include "DelimitedPage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIDataWizard::ImportASCIIDataWizard(const QString& inputFilePath, int numLines, DataContainerArray::Pointer dca, QWidget* parent)
: QWizard(parent)
, m_InputFilePath(inputFilePath)
, m_NumLines(numLines)
, m_Dca(dca)
, m_ASCIIDataModel(new ASCIIDataModel())
{
  setWindowTitle("ASCII Data Import Wizard");
  setOptions(QWizard::NoBackButtonOnStartPage /*| QWizard::HaveHelpButton */);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  resize(721, 683);

  // This adds the refresh button that refreshes the preview table.  We may use this on a future release.
  // m_RefreshBtn = new QPushButton("Refresh", this);
  // connect(m_RefreshBtn, SIGNAL(clicked()), this, SLOT(refreshModel()));
  // setButton(QWizard::HelpButton, m_RefreshBtn);

  DelimitedPage* dPage = new DelimitedPage(m_ASCIIDataModel, inputFilePath, numLines, this);
  setPage(Delimited, dPage);

  DataFormatPage* dfPage = new DataFormatPage(m_ASCIIDataModel, inputFilePath, numLines, dca, this);
  setPage(DataFormat, dfPage);

#ifndef Q_OS_MAC
  setWizardStyle(ModernStyle);
#else
  setWizardStyle(MacStyle);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::setEditSettings(bool value)
{
  m_EditSettings = value;
  DelimitedPage* delimitedPage = qobject_cast<DelimitedPage*>(this->page(Delimited));
  if(delimitedPage != nullptr)
  {
    delimitedPage->setEditSettings(value);
  }

  DataFormatPage* dataFormatPage = qobject_cast<DataFormatPage*>(this->page(DataFormat));
  if(dataFormatPage != nullptr)
  {
    dataFormatPage->setEditSettings(value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIDataWizard::ImportASCIIDataWizard(ASCIIWizardData* wizardData, DataContainerArray::Pointer dca, QWidget* parent)
: QWizard(parent)
, m_InputFilePath("")
, m_Dca(dca)
, m_ASCIIDataModel(new ASCIIDataModel())
{
  setWindowTitle("ASCII Data Import Wizard");
  setOptions(QWizard::NoBackButtonOnStartPage /*| QWizard::HaveHelpButton */);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
  resize(721, 683);

  // This adds the refresh button that refreshes the preview table.  We may use this on a future release.
  // m_RefreshBtn = new QPushButton("Refresh", this);
  // connect(m_RefreshBtn, SIGNAL(clicked()), this, SLOT(refreshModel()));
  // setButton(QWizard::HelpButton, m_RefreshBtn);
  m_InputFilePath = wizardData->inputFilePath;
  m_NumLines = wizardData->numberOfLines;

  DelimitedPage* dPage = new DelimitedPage(m_ASCIIDataModel, m_InputFilePath, m_NumLines, this);
  setPage(Delimited, dPage);
  dPage->setEditSettings(m_EditSettings);

  DataFormatPage* dfPage = new DataFormatPage(m_ASCIIDataModel, m_InputFilePath, m_NumLines, dca, this);
  setPage(DataFormat, dfPage);
  dPage->setEditSettings(m_EditSettings);
  dfPage->getTupleTable()->clearTupleDimensions();
  dfPage->getTupleTable()->addTupleDimensions(wizardData->tupleDims);
  dfPage->setUseDefaultHeaders(false);
  dfPage->setHeaderLine(0);
  dfPage->setUseCustomHeaders(false);

  if(wizardData->headerUsesDefaults)
  {
    dfPage->setUseDefaultHeaders(true);
  }

  if(wizardData->headerLine >= 0)
  {
    dfPage->setHeaderLine(wizardData->headerLine);
  }

  if(wizardData->headerIsCustom)
  {
    dfPage->setUseCustomHeaders(true);
  }

  setField("consecutiveDelimiters", wizardData->consecutiveDelimiters);

  setField("tabAsDelimiter", false);
  setField("semicolonAsDelimiter", false);
  setField("commaAsDelimiter", false);
  setField("spaceAsDelimiter", false);

  if(wizardData->delimiters.contains('\t'))
  {
    setField("tabAsDelimiter", true);
  }
  if(wizardData->delimiters.contains(';'))
  {
    setField("semicolonAsDelimiter", true);
  }
  if(wizardData->delimiters.contains(','))
  {
    setField("commaAsDelimiter", true);
  }
  if(wizardData->delimiters.contains(' '))
  {
    setField("spaceAsDelimiter", true);
  }

  setField("startRow", wizardData->beginIndex);

  QStringList dataHeaders = wizardData->dataHeaders;

  if(m_ASCIIDataModel->columnCount() < dataHeaders.size())
  {
    m_ASCIIDataModel->clearContents();
    m_ASCIIDataModel->insertColumns(0, dataHeaders.size());
  }
  for(int col = 0; col < dataHeaders.size(); col++)
  {
    m_ASCIIDataModel->setHeaderData(col, Qt::Horizontal, dataHeaders.at(col), Qt::DisplayRole);
  }

  QStringList dataTypes = wizardData->dataTypes;
  for(int i = 0; i < dataTypes.size(); i++)
  {
    m_ASCIIDataModel->setColumnDataType(i, dataTypes.at(i));
  }

  dfPage->setAutomaticAttrMatrixName(wizardData->selectedPath);
  dfPage->setAutomaticAM(wizardData->automaticAM);
  if(wizardData->automaticAM)
  {
    dfPage->dcItemSelected(wizardData->selectedPath.getDataContainerName());
  }

  dfPage->checkHeaders();

#ifndef Q_OS_MAC
  setWizardStyle(ModernStyle);
#else
  setWizardStyle(MacStyle);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportASCIIDataWizard::~ImportASCIIDataWizard() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::cleanupPage(int id)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<char> ImportASCIIDataWizard::ConvertToDelimiters(bool tabAsDelimiter, bool semicolonAsDelimiter, bool commaAsDelimiter, bool spaceAsDelimiter)
{
  QList<char> delimiters;
  if(tabAsDelimiter)
  {
    delimiters.push_back('\t');
  }
  if(semicolonAsDelimiter)
  {
    delimiters.push_back(';');
  }
  if(commaAsDelimiter)
  {
    delimiters.push_back(',');
  }
  if(spaceAsDelimiter)
  {
    delimiters.push_back(' ');
  }

  return delimiters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportASCIIDataWizard::ReadLine(const QString& inputFilePath, int line)
{
  QStringList lines = ReadLines(inputFilePath, line, 1);

  if(lines.size() != 1)
  {
    return QString();
  }

  return lines[0];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ImportASCIIDataWizard::ReadLines(const QString& inputFilePath, int beginLine, int numOfLines)
{
  QStringList result;
  if(inputFilePath.isEmpty())
  {
    return result;
  }

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString absInputPath = validator->convertToAbsolutePath(inputFilePath);

  QFile inputFile(absInputPath);
  if(inputFile.open(QIODevice::ReadOnly))
  {
    QTextStream in(&inputFile);

    for(int i = 1; i < beginLine; i++)
    {
      // Skip all lines before "value"
      in.readLine();
    }

    for(int i = beginLine; i < beginLine + numOfLines; i++)
    {
      QString line = in.readLine();
      result.push_back(line);
    }
    inputFile.close();
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::InsertTokenizedLines(QList<QStringList> tokenizedLines, int firstRowHeaderIndex, ASCIIDataModel* model)
{
  model->clearContents();

  int vHeaderIndex = firstRowHeaderIndex;

  for(int row = 0; row < tokenizedLines.size(); row++)
  {
    QStringList tokenizedLine = tokenizedLines[row];

    while(model->columnCount() < tokenizedLine.size())
    {
      model->insertColumn(model->columnCount());
    }

    for(int column = 0; column < tokenizedLine.size(); column++)
    {
      QString token = tokenizedLine[column];
      QModelIndex index = model->index(row, column);
      model->setData(index, token, Qt::DisplayRole);
    }

    model->setHeaderData(row, Qt::Vertical, QString::number(vHeaderIndex), Qt::DisplayRole);
    vHeaderIndex++;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::InsertLines(QStringList lines, int firstRowHeaderIndex, ASCIIDataModel* model)
{

  model->insertColumn(0);
  for(int row = 0; row < lines.size(); row++)
  {
    QString line = lines[row];
    QModelIndex index = model->index(row, 0);
    model->setData(index, line, Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::LoadOriginalLines(QStringList lines, ASCIIDataModel* model)
{

  if(model->rowCount() > 0)
  {
    model->removeRows(0, model->rowCount());
  }

  for(int i = 0; i < lines.size(); i++)
  {
    QString line = lines[i];

    int row = model->rowCount();
    model->insertRow(row);
    model->setOriginalString(row, line);
    model->setHeaderData(row, Qt::Vertical, QString::number(i + 1), Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::refreshModel()
{
  AbstractWizardPage* page = dynamic_cast<AbstractWizardPage*>(currentPage());
  page->refreshModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::setInputFilePath(const QString& inputFilePath)
{
  m_InputFilePath = inputFilePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<char> ImportASCIIDataWizard::getDelimiters()
{
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();

  return ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportASCIIDataWizard::getConsecutiveDelimiters()
{
  return field("consecutiveDelimiters").toBool();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ImportASCIIDataWizard::getHeaders()
{
  QStringList headers;

  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    QString header = m_ASCIIDataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
    headers.push_back(header);
  }

  return headers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList ImportASCIIDataWizard::getDataTypes()
{
  QStringList dataTypes;

  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    QString dataType = m_ASCIIDataModel->columnDataType(i);
    dataTypes.push_back(dataType);
  }

  return dataTypes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportASCIIDataWizard::getBeginningLineNum()
{
  return field("startRow").toInt();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportASCIIDataWizard::getInputFilePath()
{
  return m_InputFilePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<size_t> ImportASCIIDataWizard::getTupleDims()
{
  DataFormatPage* dfPage = dynamic_cast<DataFormatPage*>(page(DataFormat));
  if(nullptr != dfPage)
  {
    return dfPage->getTupleTable()->getData();
  }

  return std::vector<size_t>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImportASCIIDataWizard::getAutomaticAM()
{
  DataFormatPage* dfPage = dynamic_cast<DataFormatPage*>(page(DataFormat));
  if(nullptr != dfPage)
  {
    return dfPage->getAutomaticAM();
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath ImportASCIIDataWizard::getSelectedPath()
{
  DataFormatPage* dfPage = dynamic_cast<DataFormatPage*>(page(DataFormat));
  if(nullptr != dfPage)
  {
    DataArrayPath dap = dfPage->getSelectedPath();
    if(dfPage->getAutomaticAM())
    {
      dap.setAttributeMatrixName(dfPage->getAutomaticAttrMatrixName());
    }

    return dap;
  }

  return DataArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportASCIIDataWizard::getAttributeMatrixType()
{
  DataFormatPage* dfPage = dynamic_cast<DataFormatPage*>(page(DataFormat));
  if(nullptr != dfPage)
  {
    int dap = dfPage->getAttributeMatrixType();
    return dap;
  }

  return static_cast<int>(AttributeMatrix::Type::Generic);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportASCIIDataWizard::updateDataArrayPath(DataArrayPath::RenameType renamePath)
{
  DataFormatPage* dfPage = dynamic_cast<DataFormatPage*>(page(DataFormat));
  if(nullptr != dfPage)
  {
    dfPage->updateDataArrayPath(renamePath);
  }
}
