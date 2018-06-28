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

#ifndef _filelistinfowidget_h_
#define _filelistinfowidget_h_

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"

#include "SVWidgetsLib/QtSupport/QtSPluginFrame.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "ui_FileListInfoWidget.h"

/**
 * @class FileListInfoWidget FileListInfoWidget.h Plugins/EbsdImport/UI/FileListInfoWidget.h
 * @brief This class represents the User Interface for the EBSD file import module
 * of the SIMPLView program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of EBSD data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class SVWidgetsLib_EXPORT FileListInfoWidget : public FilterParameterWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  FileListInfoWidget(FilterParameter* parameter, AbstractFilter* filter = nullptr, QWidget* parent = nullptr);

  virtual ~FileListInfoWidget();

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  virtual void setupGui();

public slots:
  void widgetChanged(const QString& msg);
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

protected slots:

  // Slots to catch signals emitted by the various ui widgets
  void inputDirBtn_clicked();
  void inputDir_textChanged(const QString& text);

protected:
  void setInputDirectory(QString val);
  QString getInputDirectory();

  static void setOpenDialogLastFilePath(QString val)
  {
    m_OpenDialogLastFilePath = val;
  }
  static QString getOpenDialogLastFilePath()
  {
    return m_OpenDialogLastFilePath;
  }

  /**
   * @brief checkIOFiles
   */
  virtual void checkIOFiles();

  /**
   * @brief validateInputFile
   */
  void validateInputFile();

  /**
   * @brief setWidgetListEnabled
   */
  void setWidgetListEnabled(bool v);

  /**
   * @brief Method to attempt the extraction of the file max slice value and prefix
   */
  void findMaxSliceAndPrefix();

  /**
   * @brief generateExampleInputFile
   */
  void generateExampleInputFile();

  /**
   * @brief getGuiParametersFromFilter
   */
  void getGuiParametersFromFilter();

  /**
   * @brief setRefFrameZDir
   * @param ref
   */
  void setOrdering(uint32_t ref);

  uint32_t getOrdering();

  /**
   * @brief
   * @param event
   */
  void keyPressEvent(QKeyEvent* event);

  /**
   * @brief setupMenuField
   */
  void setupMenuField();

private:
  QSharedPointer<Ui::FileListInfoWidget> m_Ui;

  QList<QWidget*> m_WidgetList;
  QButtonGroup* m_OrderingGroup = nullptr;
  static QString m_OpenDialogLastFilePath;

  QAction* m_ShowFileAction = nullptr;
  QString m_CurrentlyValidPath = "";
  QString m_CurrentText = "";
  bool m_DidCausePreflight = false;

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

  FileListInfoWidget(const FileListInfoWidget&) = delete; // Copy Constructor Not Implemented
  void operator=(const FileListInfoWidget&) = delete;     // Move assignment Not Implemented
};

#endif /* EbsdImportWIDGET_H_ */
