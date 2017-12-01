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

#ifndef _dataBrowserWidget_h_
#define _dataBrowserWidget_h_

#include <QtCore/QUuid>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

#include "SIMPLib/Common/IObserver.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "ui_DataStructureWidget.h"

class PipelineFilterObject;
class QStandardItem;

namespace Ui
{
class DataStructureWidget;
}

class SVWidgetsLib_EXPORT DataStructureWidget : public QWidget, public IObserver
{
  Q_OBJECT
public:
  DataStructureWidget(QWidget* parent = nullptr);
  virtual ~DataStructureWidget();

public slots:
  /**
   * @brief Updates the internal DataContainerArray copy from the input DataContainerArray
   * @param dca
   */
  void updateDataContainerArray(DataContainerArray::Pointer dca);

  /**
   * @brief Updates the DataStructureWidget with the latest DataContainerArray from
   * the PipelineFilterObject
   * @param object
   */
  void filterObjectActivated(PipelineFilterObject* object);

  /**
   * @brief Updates the DataStructureWidget with the latest DataContainerArray from
   * the PipelineFilterObject
   * @param object
   */
  void handleFilterParameterChanged(PipelineFilterObject* object);

  /**
   * @brief Forces a refresh of the TreeView from the internal copy of the DataContainerArray
   */
  void refreshData();

  /**
   * @brief Slot to handle when a PipelineFilterObject is removed from a pipeline view.
   * Currently this will clear the QTreeView.
   * @param object
   */
  void handleFilterRemoved(PipelineFilterObject* object);

protected:
  /**
   * @brief Basic GUI customization code.
   */
  void setupGui();

  /**
   * @brief findChildByName
   * @param rootItem
   * @param name
   * @param column
   * @return
   */
  QStandardItem* findChildByName(QStandardItem* rootItem, const QString& name, int column);

  /**
   * @brief Syncs up the QTreeView with the heirarchy from the DataContainerArray
   * @param rootItem RootItem to start the sync
   * @param existing List of existing objects
   * @param column Column to update
   */
  void removeNonexistingEntries(QStandardItem* rootItem, QList<QString> existing, int column);

private:
  DataContainerArray::Pointer  m_Dca = nullptr;
  QSharedPointer<Ui::DataStructureWidget>       m_Ui;

  DataStructureWidget(const DataStructureWidget&) = delete; // Copy Constructor Not Implemented
  void operator=(const DataStructureWidget&);    // Operator '=' Not Implemented
};

#endif /* _dataBrowserWidget_h_   */
