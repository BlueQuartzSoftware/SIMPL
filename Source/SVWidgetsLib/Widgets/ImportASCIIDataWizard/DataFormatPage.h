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

#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "SIMPLib/DataContainers/DataArrayPath.h"

#include "AbstractWizardPage.h"

#include "ui_DataFormatPage.h"

class ASCIIDataModel;
class EditHeadersDialog;
class QSignalMapper;
class QtSFaderWidget;
class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

/**
 * @brief The DataFormatPage class
 */
class DataFormatPage : public AbstractWizardPage, private Ui::DataFormatPage
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  DataFormatPage(QSharedPointer<ASCIIDataModel> model, const QString& inputFilePath, int numLines, DataContainerArrayShPtrType dca, QWidget* parent = nullptr);

  ~DataFormatPage() override;

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  virtual void setupGui();

  /**
   * @brief Refreshes the model
   */
  void refreshModel() override;

  /**
   * @brief Launches the "Edit Headers" dialog
   */
  void launchEditHeadersDialog();

  /**
   * @brief checkHeaders
   */
  void checkHeaders();

  /**
   * @brief checkHeaders
   * @param headers
   */
  void checkHeaders(QVector<QString> headers);

  /**
   * @brief Controls which page to navigate to after the user clicks "Next" button
   */
  int nextId() const override;

  /**
   * @brief Resets the page when the user hits the "Back" button
   */
  void cleanupPage() override;

  /**
   * @brief isComplete
   * @return
   */
  bool isComplete() const override;

  /**
   * @brief getTupleTable
   * @return
   */
  TupleTableWidget* getTupleTable();

  /**
   * @brief setAutomaticAM
   * @param automatic
   */
  void setAutomaticAM(bool automatic);

  /**
   * @brief getAutomaticAM
   * @return
   */
  bool getAutomaticAM();

  /**
   * @brief getSelectedPath
   * @return
   */
  DataArrayPath getSelectedPath();

  /**
   * @brief setAutomaticAttrMatrixName
   * @param name
   */
  void setAutomaticAttrMatrixName(const QString& name);

  /**
   * @brief setAutomaticAttrMatrixName
   * @param path
   */
  void setAutomaticAttrMatrixName(const DataArrayPath& path);

  /**
   * @brief getAMName
   * @return
   */
  QString getAutomaticAttrMatrixName();

  /**
   * @brief setAttributeMatrixType
   * @param t
   */
  void setAttributeMatrixType(int t);

  /**
   * @brief getAttributeMatrixType
   * @return
   */
  int getAttributeMatrixType();

  /**
   * @brief setUseDefaultHeaders
   * @param ok
   */
  void setUseDefaultHeaders(bool ok);

  /**
   * @brief setUseCustomHeaders
   * @param value
   */
  void setUseCustomHeaders(bool value);

  /**
   * @brief setHeaderLine
   * @param line
   */
  void setHeaderLine(int line);

  /**
   * @brief setEditSettings
   * @param value
   */
  void setEditSettings(bool value);

  /**
   * @brief eventFilter
   * @param obj
   * @param event
   * @return
   */
  bool eventFilter(QObject* obj, QEvent* event) override;

  /**
   * @brief Handle DataArrayPath changes if necessary
   * @param renamePath
   */
  void updateDataArrayPath(DataArrayPath::RenameType renamePath);

public Q_SLOTS:
  /**
   * @brief dcaItemSelected
   * @param path
   */
  void amItemSelected(QString path);

  /**
   * @brief dcaItemSelected
   * @param path
   */
  void dcItemSelected(QString path);

  /**
   * @brief on_amName_returnPressed
   */
  //    void on_amName_returnPressed();

  /**
   * @brief on_applyChangesBtn_clicked
   */
  void on_applyChangesBtn_clicked();

  /**
   * @brief hideButton
   */
  void hideButton();

  /**
   * @brief widgetChanged
   * @param msg
   */
  void widgetChanged(const QString& msg);

protected:
  /**
   * @brief showEvent
   * @param event
   */
  void showEvent(QShowEvent* event) override;

  /**
   * @brief adjustedMenuPosition
   * @param pushButton
   * @return
   */
  QPoint adjustedMenuPosition(QPushButton* pushButton);

  void createDCSelectionMenu();
  void createAMSelectionMenu();

private Q_SLOTS:
  void on_startRowSpin_valueChanged(int i);
  void on_hasHeadersRadio_toggled(bool checked);
  void on_doesNotHaveHeadersRadio_toggled(bool checked);
  void on_useDefaultHeaders_toggled(bool checked);

  void on_headersIndexLineEdit_textChanged(const QString& text);
  void on_editHeadersBtn_clicked();
  void on_dataTypeRadio_clicked();
  void on_skipRadio_clicked();
  void on_dataTypeCB_currentTextChanged(const QString& text);

  void on_amName_returnPressed();

  void updateSelection(const QItemSelection& selected, const QItemSelection& deselected);
  bool checkTupleDimensions(std::vector<size_t> tupleDims);

  void on_createAMRadio_toggled(bool b);
  void on_useAMRadio_toggled(bool b);

private:
  int m_NumLines = -1;
  EditHeadersDialog* m_EditHeadersDialog = nullptr;
  DataContainerArrayShPtrType m_Dca;
  QSharedPointer<ASCIIDataModel> m_ASCIIDataModel;
  QSignalMapper* m_AMMenuMapper = nullptr;
  QSignalMapper* m_DCMenuMapper = nullptr;

  QPointer<QtSFaderWidget> m_FaderWidget;
  bool m_EditSettings = false;

  QMenu* m_DCMenuPtr = nullptr;
  bool m_OwnsDCMenuPtr = false;

  QMenu* m_AttrMatMenuPtr = nullptr;
  bool m_OwnsAttrMatMenuPtr = false;

  bool m_TupleDimsHasErrors = false;
  bool m_HeadersHasErrors = false;

  bool validateHeaders(QVector<QString> headers);
  bool validateTupleDimensions(std::vector<size_t> tupleDims);

public:
  DataFormatPage(const DataFormatPage&) = delete;            // Copy Constructor Not Implemented
  DataFormatPage(DataFormatPage&&) = delete;                 // Move Constructor Not Implemented
  DataFormatPage& operator=(const DataFormatPage&) = delete; // Copy Assignment Not Implemented
  DataFormatPage& operator=(DataFormatPage&&) = delete;      // Move Assignment Not Implemented
};
