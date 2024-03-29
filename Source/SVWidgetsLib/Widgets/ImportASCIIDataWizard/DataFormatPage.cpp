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

#include "DataFormatPage.h"

#include <QtCore/QDebug>
#include <QtCore/QSignalMapper>

#include <QtGui/QScreen>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Utilities/STLUtilities.hpp"
#include "SIMPLib/Utilities/StringOperations.h"

#include "SVWidgetsLib/QtSupport/QtSFaderWidget.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "ASCIIDataModel.h"
#include "EditHeadersDialog.h"
#include "ImportASCIIDataWizard.h"

namespace Detail
{
const QString Delimiter(" / ");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::DataFormatPage(QSharedPointer<ASCIIDataModel> model, const QString& inputFilePath, int numLines, DataContainerArray::Pointer dca, QWidget* parent)
: AbstractWizardPage(inputFilePath, parent)
, m_NumLines(numLines)
, m_Dca(dca)
, m_ASCIIDataModel(model)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataFormatPage::~DataFormatPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setupGui()
{

  dataView->setModel(m_ASCIIDataModel.data());

  connect(dataView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(updateSelection(const QItemSelection&, const QItemSelection&)));
  connect(tupleDimsTable, SIGNAL(tupleDimsChanged(std::vector<size_t>)), this, SLOT(checkTupleDimensions(std::vector<size_t>)));
  connect(amName, SIGNAL(textEdited(const QString&)), this, SLOT(widgetChanged(const QString&)));

  headersIndexLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9][0-9]*"), headersIndexLineEdit));

  registerField("startRow", startRowSpin);

  // Do not allow the user to put a forward slash into the attributeMatrixName line edit
  amName->setValidator(new QRegularExpressionValidator(QRegularExpression("[^/]*"), this));

  QStringList dataTypes;
  dataTypes.push_back(SIMPL::TypeNames::Double);
  dataTypes.push_back(SIMPL::TypeNames::Float);
  dataTypes.push_back(SIMPL::TypeNames::Int8);
  dataTypes.push_back(SIMPL::TypeNames::Int16);
  dataTypes.push_back(SIMPL::TypeNames::Int32);
  dataTypes.push_back(SIMPL::TypeNames::Int64);
  dataTypes.push_back(SIMPL::TypeNames::UInt8);
  dataTypes.push_back(SIMPL::TypeNames::UInt16);
  dataTypes.push_back(SIMPL::TypeNames::UInt32);
  dataTypes.push_back(SIMPL::TypeNames::UInt64);
  dataTypes.push_back(SIMPL::TypeNames::String);
  dataTypeCB->addItems(dataTypes);
  dataTypeCB->setCurrentIndex(0);

  // int beginIndex = startRowSpin->value();
  int numOfDataLines = m_NumLines;
  linesImportedLabel->setText(QString::number(numOfDataLines));
  linesInFileLabel->setText(QString::number(m_NumLines));
  //  amTuplesLabel->setText(QString::number(numOfDataLines));

  tupleDimsTable->blockSignals(true);
  tupleDimsTable->addTupleDimensions(std::vector<size_t>(1, numOfDataLines));
  tupleDimsTable->blockSignals(false);

  selectedDCBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));
  selectedAMBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));

  m_AMMenuMapper = new QSignalMapper(this);
  connect(m_AMMenuMapper, SIGNAL(mapped(QString)), this, SLOT(amItemSelected(QString)));

  m_DCMenuMapper = new QSignalMapper(this);
  connect(m_DCMenuMapper, SIGNAL(mapped(QString)), this, SLOT(dcItemSelected(QString)));

  editHeadersBtn->setDisabled(true);
  columnDataGroupBox->setDisabled(true);

  lineNumErrLabel->hide();
  tupleTableErrLabel->hide();
  amSelectionError->hide();
  amCreationError->hide();
  applyChangesBtn->hide();

  selectedAMLabel->hide();
  selectedAMBtn->hide();

  QVector<QString> amTypes = AttributeMatrix::GetTypesAsStrings();

  for(const QString& amType : amTypes)
  {
    attributeMatrixType->addItem(amType);
  }
  attributeMatrixType->setCurrentIndex(3);
  amName->setText("CellData");

  createDCSelectionMenu();
  createAMSelectionMenu();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setEditSettings(bool value)
{
  m_EditSettings = value;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setUseDefaultHeaders(bool ok)
{
  useDefaultHeaders->setChecked(ok);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setUseCustomHeaders(bool value)
{
  doesNotHaveHeadersRadio->setChecked(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setHeaderLine(int line)
{
  if(line >= 0)
  {
    hasHeadersRadio->setChecked(true);
    headersIndexLineEdit->setText(QString::number(line));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::showEvent(QShowEvent* event)
{
  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  if(!m_EditSettings)
  {
    QStringList lines = m_ASCIIDataModel->originalStrings();

    QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

    QList<QStringList> tokenizedLines = StringOperations::TokenizeStrings(lines, delimiters, consecutiveDelimiters);
    ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value(), m_ASCIIDataModel.data());
  }
  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    if(m_ASCIIDataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString().isEmpty())
    {
      m_ASCIIDataModel->setColumnHasErrors(i, true);
      arrayErrLabel->setText("Column headers cannot be empty.");
      arrayErrLabel->show();
    }
    else
    {
      m_ASCIIDataModel->setColumnHasErrors(i, false);
      arrayErrLabel->hide();
    }
  }

  //  if (amAutomatically->isChecked())
  //  {
  //    tupleDimsGB->hide();
  //  }
  //  else
  //  {
  //    tupleDimsGB->show();
  //  }

  if(isComplete())
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::amItemSelected(QString path)
{
  selectedAMBtn->setText(path);

  DataArrayPath dap = DataArrayPath::Deserialize(path, Detail::Delimiter);

  AttributeMatrix::Pointer am = m_Dca->getAttributeMatrix(dap); // This will always be valid, because we create the selection options

  if(nullptr == am.get())
  {
    amSelectionError->setText("An error occured retrieving the Attribute Matrix at " + path);
    selectedAMBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));
  }
  else
  {
    // amSelectionError->hide();
    amSelectionError->setText("The tuple dimensions of the selected Attribute Matrix will be used for the Attribute Arrays.");
    selectedAMBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(true));
  }

  ImportASCIIDataWizard* importWizard = dynamic_cast<ImportASCIIDataWizard*>(wizard());
  if(importWizard == nullptr)
  {
    return;
  }

  QStringList headers = importWizard->getHeaders();

  QStringList amArrays = am->getAttributeArrayNames();
  for(int i = 0; i < amArrays.size(); i++)
  {
    QString amArrayName = amArrays[i];
    for(int j = 0; j < headers.size(); j++)
    {
      QString headerName = headers[j];
      if(amArrayName == headerName)
      {
        QString ss = "The header name \"" + headerName + "\" matches an array name that already exists in the selected attribute matrix.";
        amSelectionError->setText(ss);
        amSelectionError->show();
        selectedAMBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(false));
      }
      else
      {
        amSelectionError->hide();
        selectedAMBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(true));
      }
    }
  }

  if(isComplete())
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::dcItemSelected(QString path)
{
  selectedDCBtn->setText(path);

  DataArrayPath dap = DataArrayPath::Deserialize(path, Detail::Delimiter);
  dap.setAttributeMatrixName(amName->text());

  if(m_Dca->doesAttributeMatrixExist(dap) && createAMRadio->isChecked())
  {
    QString ss = "An AttributeMatrix at the path '" + dap.serialize("/") + "' already exists. Choose a different attribute matrix name.";
    amCreationError->setText(ss);
    amCreationError->show();
    SVStyle::Instance()->LineEditErrorStyle(amName);
    return;
  }

  amCreationError->hide();
  selectedDCBtn->setStyleSheet(SVStyle::Instance()->QToolSelectionButtonStyle(true));
  SVStyle::Instance()->LineEditClearStyle(amName);
  checkTupleDimensions(getTupleTable()->getData());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_createAMRadio_toggled(bool b)
{
  if(b)
  {
    checkTupleDimensions(getTupleTable()->getData());
    Q_EMIT completeChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_useAMRadio_toggled(bool b)
{
  if(b)
  {
    DataArrayPath amPath = DataArrayPath::Deserialize(selectedAMBtn->text(), "/");
    AttributeMatrix::Pointer attrMat = m_Dca->getAttributeMatrix(amPath);
    if(attrMat.get() != nullptr)
    {
      checkTupleDimensions(attrMat->getTupleDimensions());
    }
    else
    {
      Q_EMIT completeChanged();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::widgetChanged(const QString& text)
{
  Q_UNUSED(text)

  SVStyle::Instance()->SetErrorColor("QLineEdit", amName);
  amName->setToolTip("Press the 'Return' key to apply your changes");
  if(!applyChangesBtn->isVisible())
  {
    applyChangesBtn->setVisible(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_applyChangesBtn_clicked()
{
  amName->setStyleSheet(QString(""));

  QPointer<QtSFaderWidget> faderWidget = new QtSFaderWidget(applyChangesBtn);
  m_FaderWidget = faderWidget;

  if(m_FaderWidget != nullptr)
  {
    m_FaderWidget->close();
  }
  m_FaderWidget = new QtSFaderWidget(applyChangesBtn);
  m_FaderWidget->setFadeOut();
  connect(m_FaderWidget, SIGNAL(animationComplete()), this, SLOT(hideButton()));
  m_FaderWidget->start();

  dcItemSelected(selectedDCBtn->text());

  if(isComplete())
  {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
  }
  else
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::hideButton()
{
  amName->setToolTip("");
  applyChangesBtn->setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::createAMSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = m_Dca;
  if(nullptr == dca.get())
  {
    return;
  }

  // Get the menu and clear it out
  QMenu* btnMenu = selectedAMBtn->menu();
  if(btnMenu != nullptr)
  {
    btnMenu->clear();
  }
  else
  {
    m_OwnsAttrMatMenuPtr = true;
    m_AttrMatMenuPtr = new QMenu;
    btnMenu = m_AttrMatMenuPtr;
    selectedAMBtn->setMenu(btnMenu);
    btnMenu->installEventFilter(this);
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  DataContainerArray::Container containers = dca->getDataContainers();
  std::vector<QString> daTypes;       // = m_FilterParameter->getDefaultAttributeArrayTypes();
  QVector<std::vector<size_t>> cDims; // = m_FilterParameter->getDefaultComponentDimensions();
  AttributeMatrix::Types amTypes;     // = m_FilterParameter->getDefaultAttributeMatrixTypes();
  IGeometry::Types geomTypes;         // = m_FilterParameter->getDefaultGeometryTypes();

  for(DataContainer::Pointer dc : containers)
  {
    IGeometry::Pointer geom = IGeometry::NullPointer();
    IGeometry::Type geomType = IGeometry::Type::Unknown;
    if(nullptr != dc.get())
    {
      geom = dc->getGeometry();
    }
    if(nullptr != geom.get())
    {
      geomType = geom->getGeometryType();
    }

    QMenu* dcMenu = btnMenu->addMenu(dc->getName()); // BtnMenu owns the new QMenu
    dcMenu->setDisabled(false);

    if(!geomTypes.empty() && !SIMPL::contains(geomTypes, geomType) && !SIMPL::contains(geomTypes, IGeometry::Type::Any))
    {
      dcMenu->setDisabled(true);
    }

    // We found the proper Data Container, now populate the AttributeMatrix List
    for(const auto& am : dc->getChildren())
    {
      QString amName = am->getName();

      QAction* amAction = dcMenu->addAction(amName); // dcMenu owns the created action
      // QAction* action = new QAction(amName, dcMenu);
      DataArrayPath daPath(dc->getName(), amName, "");
      QString path = daPath.serialize(Detail::Delimiter);
      amAction->setData(path);

      connect(amAction, SIGNAL(triggered(bool)), m_AMMenuMapper, SLOT(map()));
      m_AMMenuMapper->setMapping(amAction, path);

      bool amIsNotNull = nullptr != am.get();
      bool amValidType = !amTypes.empty() && !SIMPL::contains(amTypes, am->getType());

      if(amIsNotNull && amValidType)
      {
        amAction->setDisabled(true);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::createDCSelectionMenu()
{
  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = m_Dca;
  if(nullptr == dca.get())
  {
    return;
  }

  // Get the menu and clear it out
  QMenu* btnMenu = selectedDCBtn->menu();
  if(btnMenu != nullptr)
  {
    btnMenu->clear();
  }
  else
  {
    m_OwnsDCMenuPtr = true;
    m_DCMenuPtr = new QMenu;
    btnMenu = m_DCMenuPtr;
    selectedDCBtn->setMenu(btnMenu);
    btnMenu->installEventFilter(this);
  }

  // Get the DataContainerArray object
  // Loop over the data containers until we find the proper data container
  DataContainerArray::Container containers = dca->getDataContainers();
  IGeometry::Types geomTypes;

  for(DataContainer::Pointer dc : containers)
  {
    IGeometry::Pointer geom = IGeometry::NullPointer();
    IGeometry::Type geomType = IGeometry::Type::Unknown;
    if(nullptr != dc.get())
    {
      geom = dc->getGeometry();
    }
    if(nullptr != geom.get())
    {
      geomType = geom->getGeometryType();
    }

    QString dcName = dc->getName();

    QAction* dcAction = btnMenu->addAction(dcName); // btnMenu owns the created QAction

    DataArrayPath dcPath(dcName, "", "");
    QString path = dcPath.serialize(Detail::Delimiter);
    dcAction->setData(path);

    connect(dcAction, SIGNAL(triggered(bool)), m_DCMenuMapper, SLOT(map()));
    m_DCMenuMapper->setMapping(dcAction, path);

    if(!geomTypes.empty() && !SIMPL::contains(geomTypes, geomType) && !SIMPL::contains(geomTypes, IGeometry::Type::Any))
    {
      dcAction->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::eventFilter(QObject* obj, QEvent* event)
{
  if(event->type() == QEvent::Show)
  {
    if(obj == selectedAMBtn->menu())
    {
      QPoint pos = adjustedMenuPosition(selectedAMBtn);
      selectedAMBtn->menu()->move(pos);
      return true;
    }
    if(obj == selectedDCBtn->menu())
    {
      QPoint pos = adjustedMenuPosition(selectedDCBtn);
      selectedDCBtn->menu()->move(pos);
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QPoint DataFormatPage::adjustedMenuPosition(QPushButton* pushButton)
{
  // Calculate the actual virtual desktop QRect.
  QList<QScreen*> screens = QGuiApplication::screens();
  int xMin = std::numeric_limits<int>::max();
  int yMin = std::numeric_limits<int>::max();
  int xMax = std::numeric_limits<int>::min();
  int yMax = std::numeric_limits<int>::min();
  QRect virtDesktopRect;
  for(const auto& screen : screens)
  {
    QRect rect = screen->availableGeometry();
    if(rect.x() < xMin)
    {
      xMin = rect.x();
    }
    if(rect.y() < yMin)
    {
      yMin = rect.y();
    }
    if(rect.x() + rect.width() > xMax)
    {
      xMax = rect.x() + rect.width();
    }
    if(rect.y() + rect.height() > yMax)
    {
      yMax = rect.y() + rect.height();
    }
  }

  virtDesktopRect.setTopLeft(QPoint(xMin, yMin));
  virtDesktopRect.setBottomRight(QPoint(xMax, yMax));
  QSize menuSize = pushButton->menu()->sizeHint();

  QPoint point = QCursor::pos();

  // Move the x position to the left by half the width of the menu so the menu
  // is centered up under the mouse
  point.setX(point.x() - menuSize.width() / 2);

  // If the menu is going to go off the screen in the X-axis, reposition it until it's completely on the screen
  if(point.x() + menuSize.width() > virtDesktopRect.right())
  {
    //  int diffX = point.x() + menuSize.width() - virtDesktopRect.right();
    point.setX(virtDesktopRect.right() - menuSize.width());
  }

  // Make sure the menu will not get positioned off the left side of the desktop
  if(point.x() - 0.5 * menuSize.width() < virtDesktopRect.left())
  {
    point.setX(virtDesktopRect.left() + 2); //
  }
  // Find the "Y" Position that the menu should be displayed at. We want the menu
  // to appear just below the button so the button and it's text are not obscurred.
  QPoint localButtonCoords = pushButton->geometry().bottomLeft();
  QPoint globalButtonCoords = mapToGlobal(localButtonCoords);

  // point.setY(globalButtonCoords.y());

  int screenNum = QApplication::desktop()->screenNumber(pushButton);
  int desktopHeight = screens.at(screenNum)->availableGeometry().height();

  if(point.y() > desktopHeight)
  {
    localButtonCoords = pushButton->geometry().topLeft();
    globalButtonCoords = mapToGlobal(localButtonCoords);
    point.setY(globalButtonCoords.y() - menuSize.height());
  }

  return point;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_startRowSpin_valueChanged(int value)
{
  if(value > m_NumLines)
  {
    wizard()->button(QWizard::FinishButton)->setDisabled(true);
    //    amTuplesLabel->setText("ERR");
    return;
  }

  wizard()->button(QWizard::FinishButton)->setEnabled(true);

  m_ASCIIDataModel->clearContents();

  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, value, ImportASCIIDataWizard::TotalPreviewLines);
  ImportASCIIDataWizard::LoadOriginalLines(lines, m_ASCIIDataModel.data());

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = StringOperations::TokenizeStrings(lines, delimiters, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value(), m_ASCIIDataModel.data());

  // Update headers
  on_hasHeadersRadio_toggled(hasHeadersRadio->isChecked());
  on_doesNotHaveHeadersRadio_toggled(doesNotHaveHeadersRadio->isChecked());
  on_useDefaultHeaders_toggled(useDefaultHeaders->isChecked());

  // Update Tuple Dimensions
  linesInFileLabel->setText(QString::number(m_NumLines));

  int numOfDataLines = m_NumLines - value + 1;
  linesImportedLabel->setText(QString::number(numOfDataLines));
  if(!tupleDimsTable->didUseEdit())
  {
    tupleDimsTable->clearTupleDimensions();
    tupleDimsTable->addTupleDimensions(std::vector<size_t>(1, numOfDataLines));
  }
  checkTupleDimensions(getTupleTable()->getData());

  Q_EMIT completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::isComplete() const
{

  bool stage1 = true;
  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    if(m_ASCIIDataModel->columnHasErrors(i))
    {
      stage1 = false;
    }
  }

  bool stage2 = true;

  if(useAMRadio->isChecked() && selectedAMBtn->text().isEmpty())
  {
    stage2 = false;
  }

  bool stage3 = true;
  if(createAMRadio->isChecked() && (selectedDCBtn->text().isEmpty() || amName->text().isEmpty()))
  {
    stage3 = false;
  }

  return (stage1 && stage2 && stage3 && !m_TupleDimsHasErrors && !m_HeadersHasErrors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_amName_returnPressed()
{
  applyChangesBtn->click();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_hasHeadersRadio_toggled(bool checked)
{
  if(checked)
  {
    editHeadersBtn->setDisabled(true);
    lineNumberLabel->setEnabled(true);
    headersIndexLineEdit->setEnabled(true);

    // Reload the headers
    on_headersIndexLineEdit_textChanged(headersIndexLineEdit->text());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_doesNotHaveHeadersRadio_toggled(bool checked)
{
  if(checked)
  {
    editHeadersBtn->setEnabled(true);
    lineNumberLabel->setDisabled(true);
    headersIndexLineEdit->setDisabled(true);

    QVector<QString> headers;
    if(m_EditHeadersDialog != nullptr)
    {
      headers = m_EditHeadersDialog->getHeaders();
    }

    for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
    {
      if(i < headers.size())
      {
        m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, headers[i], Qt::DisplayRole);
      }
      else
      {
        m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, "", Qt::DisplayRole);
      }
    }

    checkHeaders();
    Q_EMIT completeChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_useDefaultHeaders_toggled(bool checked)
{
  if(checked)
  {
    editHeadersBtn->setDisabled(true);
    lineNumberLabel->setDisabled(true);
    headersIndexLineEdit->setDisabled(true);

    for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
    {
      QString arrayName = "Array " + QString::number(i + 1);
      m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, arrayName, Qt::DisplayRole);
    }

    checkHeaders();
    Q_EMIT completeChanged();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_headersIndexLineEdit_textChanged(const QString& text)
{

  if(text.isEmpty()) // No test then bail out now.
  {
    m_ASCIIDataModel->clearHeaders(Qt::Horizontal);
    checkHeaders(QVector<QString>());
    Q_EMIT completeChanged();
    return;
  }

  bool ok = false;
  int lineNum = text.toInt(&ok);

  if(!text.isEmpty() && (lineNum > m_NumLines || !ok))
  {
    m_ASCIIDataModel->clearHeaders(Qt::Horizontal);
    checkHeaders(QVector<QString>());
    Q_EMIT completeChanged();
    return;
  }

  headersIndexLineEdit->setStyleSheet("");
  lineNumErrLabel->hide();

  if(text.isEmpty())
  {
    m_ASCIIDataModel->clearHeaders(Qt::Horizontal);
    checkHeaders(QVector<QString>());
    Q_EMIT completeChanged();
    return;
  }

  QString line = ImportASCIIDataWizard::ReadLine(m_InputFilePath, lineNum);

  QStringList list;
  list.push_back(line);

  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> result = StringOperations::TokenizeStrings(list, delimiters, consecutiveDelimiters);

  QStringList tokenizedLine = result[0];

  QVector<QString> headers;
  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    QString header = "";

    if(i < tokenizedLine.size())
    {
      header = tokenizedLine[i];
    }

    m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);

    headers.push_back(header);
  }

  checkHeaders(headers);

  Q_EMIT completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_editHeadersBtn_clicked()
{
  if(nullptr == m_EditHeadersDialog)
  {
    m_EditHeadersDialog = new EditHeadersDialog(m_ASCIIDataModel, this);
  }

  launchEditHeadersDialog();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::updateSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
  QModelIndexList selectedIndexList = selected.indexes();

  if(selectedIndexList.empty())
  {
    columnDataGroupBox->setDisabled(true);
  }
  else
  {
    columnDataGroupBox->setEnabled(true);

    int selectedColumn = selectedIndexList[0].column();
    QString selectedType = m_ASCIIDataModel->columnDataType(selectedColumn);

    if(selectedType.isEmpty())
    {
      dataTypeRadio->setChecked(true);
      dataTypeCB->setEnabled(true);
      dataTypeCB->setCurrentIndex(0);
    }
    else if(selectedType == "Skip")
    {
      skipRadio->setChecked(true);
      dataTypeCB->setDisabled(true);
    }
    else
    {
      dataTypeRadio->setChecked(true);
      dataTypeCB->setEnabled(true);
      dataTypeCB->setCurrentText(selectedType);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::checkTupleDimensions(std::vector<size_t> tupleDims)
{
  if(!validateTupleDimensions(tupleDims))
  {
    tupleTableErrLabel->setText("The current number of tuples in the attribute matrix do not match the total number of lines imported.");
    tupleTableErrLabel->show();
    m_TupleDimsHasErrors = true;
    Q_EMIT completeChanged();
    return false;
  }

  tupleTableErrLabel->setText("");
  tupleTableErrLabel->hide();
  m_TupleDimsHasErrors = false;
  Q_EMIT completeChanged();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::validateTupleDimensions(std::vector<size_t> tupleDims)
{
  size_t tupleTotal = 1;

  for(int i = 0; i < tupleDims.size(); i++)
  {
    tupleTotal = tupleTotal * tupleDims[i];
  }
  //  amTuplesLabel->setText(QString::number(tupleTotal));
  size_t beginIndex = static_cast<size_t>(startRowSpin->value());
  size_t numOfDataLines = m_NumLines - beginIndex + 1;
  return tupleTotal == numOfDataLines;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_dataTypeRadio_clicked()
{
  QModelIndexList indexList = dataView->selectionModel()->selectedColumns();

  if(!indexList.empty())
  {
    QModelIndex index = indexList[0];
    int column = index.column();

    m_ASCIIDataModel->setColumnDataType(column, dataTypeCB->currentText());
  }

  dataTypeCB->setEnabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_dataTypeCB_currentTextChanged(const QString& text)
{
  on_dataTypeRadio_clicked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::on_skipRadio_clicked()
{
  QModelIndexList indexList = dataView->selectionModel()->selectedColumns();

  if(!indexList.empty())
  {
    QModelIndex index = indexList[0];
    int column = index.column();

    m_ASCIIDataModel->setColumnDataType(column, "Skip");
  }

  dataTypeCB->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::cleanupPage()
{
  startRowSpin->setValue(1);
  headersIndexLineEdit->clear();
  hasHeadersRadio->setChecked(true);
  doesNotHaveHeadersRadio->setChecked(false);

  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    m_ASCIIDataModel->setColumnHasErrors(i, false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::refreshModel()
{

  m_ASCIIDataModel->clear();

  bool tabAsDelimiter = field("tabAsDelimiter").toBool();
  bool semicolonAsDelimiter = field("semicolonAsDelimiter").toBool();
  bool commaAsDelimiter = field("commaAsDelimiter").toBool();
  bool spaceAsDelimiter = field("spaceAsDelimiter").toBool();
  bool consecutiveDelimiters = field("consecutiveDelimiters").toBool();

  QStringList lines = ImportASCIIDataWizard::ReadLines(m_InputFilePath, startRowSpin->value(), ImportASCIIDataWizard::TotalPreviewLines);

  ImportASCIIDataWizard::LoadOriginalLines(lines, m_ASCIIDataModel.data());

  QList<char> delimiters = ImportASCIIDataWizard::ConvertToDelimiters(tabAsDelimiter, semicolonAsDelimiter, commaAsDelimiter, spaceAsDelimiter);

  QList<QStringList> tokenizedLines = StringOperations::TokenizeStrings(lines, delimiters, consecutiveDelimiters);
  ImportASCIIDataWizard::InsertTokenizedLines(tokenizedLines, startRowSpin->value(), m_ASCIIDataModel.data());

  // Refresh the headers
  if(hasHeadersRadio->isChecked())
  {
    on_headersIndexLineEdit_textChanged(headersIndexLineEdit->text());
  }
  else
  {
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::launchEditHeadersDialog()
{
  if(nullptr != m_EditHeadersDialog)
  {

    QVector<QString> currentHeaders;
    for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
    {
      currentHeaders.push_back(m_ASCIIDataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
      m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, QString::number(i + 1), Qt::DisplayRole);
    }
    m_EditHeadersDialog->setHeaders(currentHeaders);

    int result = m_EditHeadersDialog->exec();
    if(result == QDialog::Accepted)
    {
      QVector<QString> headers = m_EditHeadersDialog->getHeaders();
      checkHeaders(headers);
    }
    else
    {
      for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
      {
        m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, currentHeaders[i], Qt::DisplayRole);
      }

      m_EditHeadersDialog->setHeaders(currentHeaders);
    }

    if(isComplete())
    {
      wizard()->button(QWizard::FinishButton)->setEnabled(true);
    }
    else
    {
      wizard()->button(QWizard::FinishButton)->setDisabled(true);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::checkHeaders()
{

  QVector<QString> headers;
  for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
  {
    headers.push_back(m_ASCIIDataModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
  }

  checkHeaders(headers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::checkHeaders(QVector<QString> headers)
{
  m_HeadersHasErrors = !validateHeaders(headers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::validateHeaders(QVector<QString> headers)
{

  bool hasSlashes = false;
  bool hasDuplicates = false;

  bool isEmpty = false;
  if(headers.empty())
  {
    isEmpty = true;
    for(int i = 0; i < m_ASCIIDataModel->columnCount(); i++)
    {
      m_ASCIIDataModel->setColumnHasErrors(i, true);
    }
  }
  for(int i = 0; i < headers.size(); i++)
  {
    QString header = headers[i];

    if(header.isEmpty())
    {
      isEmpty = true;
      m_ASCIIDataModel->setColumnHasErrors(i, true);
    }
    else if(header.contains('/') || header.contains('\\'))
    {
      hasSlashes = true;
      m_ASCIIDataModel->setColumnHasErrors(i, true);
    }
    else
    {
      m_ASCIIDataModel->setColumnHasErrors(i, false);
    }

    for(int j = 0; j < headers.size(); j++)
    {
      QString otherHeader = headers[j];

      if(i != j && !header.isEmpty() && !otherHeader.isEmpty() && header == otherHeader)
      {
        hasDuplicates = true;
        m_ASCIIDataModel->setColumnHasErrors(i, true);
        m_ASCIIDataModel->setColumnHasErrors(j, true);
      }
      else if(!hasDuplicates && !isEmpty && !hasSlashes)
      {
        m_ASCIIDataModel->setColumnHasErrors(i, false);
        m_ASCIIDataModel->setColumnHasErrors(j, false);
      }
    }

    m_ASCIIDataModel->setHeaderData(i, Qt::Horizontal, header, Qt::DisplayRole);
  }

  if(isEmpty)
  {
    arrayErrLabel->setText("Column headers cannot be empty.");
    arrayErrLabel->show();
    return false;
  }
  if(hasSlashes)
  {
    arrayErrLabel->setText("Column headers cannot contain slashes.");
    arrayErrLabel->show();
    return false;
  }
  if(hasDuplicates)
  {
    arrayErrLabel->setText("Column headers cannot have the same name.");
    arrayErrLabel->show();
    return false;
  }

  arrayErrLabel->setText("");
  arrayErrLabel->hide();
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TupleTableWidget* DataFormatPage::getTupleTable()
{
  return tupleDimsTable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setAutomaticAM(bool automatic)
{
  createAMRadio->setChecked(automatic);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataFormatPage::getAutomaticAM()
{
  return createAMRadio->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPath DataFormatPage::getSelectedPath()
{
  if(createAMRadio->isChecked())
  {
    return DataArrayPath::Deserialize(selectedDCBtn->text(), Detail::Delimiter);
  }

  return DataArrayPath::Deserialize(selectedAMBtn->text(), Detail::Delimiter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setAutomaticAttrMatrixName(const QString& name)
{
  amName->setText(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setAutomaticAttrMatrixName(const DataArrayPath& path)
{
  amName->setText(path.getAttributeMatrixName());
  selectedDCBtn->setText(path.getDataContainerName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataFormatPage::getAutomaticAttrMatrixName()
{
  return amName->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::setAttributeMatrixType(int t)
{
  attributeMatrixType->setCurrentIndex(t);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataFormatPage::getAttributeMatrixType()
{
  return attributeMatrixType->currentIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataFormatPage::nextId() const
{
  // There is no "Next" page
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataFormatPage::updateDataArrayPath(DataArrayPath::RenameType renamePath)
{
  DataArrayPath oldPath;
  DataArrayPath newPath;
  std::tie(oldPath, newPath) = renamePath;

  bool hasAm = !oldPath.getAttributeMatrixName().isEmpty();

  // Get current path
  DataArrayPath currentPath;
  if(createAMRadio->isChecked())
  {
    currentPath = DataArrayPath::Deserialize(selectedDCBtn->text(), Detail::Delimiter);
    currentPath.setAttributeMatrixName(amName->text());
  }
  else
  {
    currentPath = DataArrayPath::Deserialize(selectedAMBtn->text(), Detail::Delimiter);
  }

  // Update current path
  if(oldPath.getDataContainerName() == currentPath.getDataContainerName())
  {
    if(hasAm)
    {
      if(oldPath.getAttributeMatrixName() == currentPath.getAttributeMatrixName())
      {
        currentPath.setDataContainerName(newPath.getDataContainerName());
        currentPath.setAttributeMatrixName(newPath.getAttributeMatrixName());
      }
    }
    else
    {
      currentPath.setDataContainerName(newPath.getDataContainerName());
    }
  }

  // Update the widget
  if(createAMRadio->isChecked())
  {
    selectedDCBtn->setText(currentPath.getDataContainerName());
    amName->setText(currentPath.getAttributeMatrixName());
  }
  else
  {
    selectedAMBtn->setText(currentPath.serialize(Detail::Delimiter));
  }
}
