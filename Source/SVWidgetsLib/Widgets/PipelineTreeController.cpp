/* ============================================================================
* Copyright (c) 2017 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software nor the names of its
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
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PipelineTreeController.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/FilterParameters/H5FilterParametersReader.h"
#include "SIMPLib/FilterParameters/JsonFilterParametersReader.h"

#include "SVWidgetsLib/Widgets/BreakpointFilterWidget.h"
#include "SVWidgetsLib/Widgets/PipelineTreeModel.h"
#include "SVWidgetsLib/Widgets/PipelineTreeItem.h"
#include "SVWidgetsLib/QtSupport/QtSRecentFileList.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeController::PipelineTreeController(QObject* parent)
: QObject(parent)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeController::~PipelineTreeController()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::preflightPipeline(const QModelIndex &pipelineIndex, PipelineTreeModel* model)
{
  if(m_BlockPreflight || pipelineIndex.isValid() == false)
  {
    return;
  }

  emit pipelineIssuesCleared();
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = getFilterPipeline(pipelineIndex, model);

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  for(int i = 0; i < filters.size(); i++)
  {
    filters.at(i)->setErrorCondition(0);
    filters.at(i)->setCancel(false);

    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      model->setErrorState(childIndex, PipelineTreeItem::ErrorState::Ok);
      if(PipelineTreeItem::WidgetState::Disabled != model->widgetState(childIndex))
      {
        model->setWidgetState(childIndex, PipelineTreeItem::WidgetState::Ready);
      }
    }
  }

//  QSharedPointer<ProgressDialog> progressDialog(new ProgressDialog());
//  progressDialog->setWindowTitle("Pipeline Preflighting");
//  QString msg = QString("Please wait for %1 filters to preflight...").arg(pipeline->getFilterContainer().count());
//  progressDialog->setLabelText(msg);
//  progressDialog->show();
//  progressDialog->raise();
//  progressDialog->activateWindow();

  // Preflight the pipeline
  int err = pipeline->preflightPipeline();
  if(err < 0)
  {
    // FIXME: Implement error handling.
  }

  int count = pipeline->getFilterContainer().size();
  // Now that the preflight has been executed loop through the filters and check their error condition and set the
  // outline on the filter widget if there were errors or warnings
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      AbstractFilter::Pointer filter = model->filter(childIndex);
      if(filter->getWarningCondition() < 0)
      {
        model->setErrorState(childIndex, PipelineTreeItem::ErrorState::Warning);
      }
      if(filter->getErrorCondition() < 0)
      {
        model->setErrorState(childIndex, PipelineTreeItem::ErrorState::Error);
      }
    }
  }
  emit preflightFinished(err);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineTreeController::getFilterPipeline(const QModelIndex &pipelineIndex, PipelineTreeModel* model)
{
  // Create a Pipeline Object and fill it with the filters from this View
  FilterPipeline::Pointer pipeline = FilterPipeline::New();

  qint32 count = model->rowCount(pipelineIndex);
  for(qint32 i = 0; i < count; ++i)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, pipelineIndex);
    if(childIndex.isValid())
    {
      AbstractFilter::Pointer filter = model->filter(childIndex);
      Breakpoint::Pointer breakpoint = std::dynamic_pointer_cast<Breakpoint>(filter);
      if(nullptr != breakpoint)
      {
        connect(pipeline.get(), SIGNAL(pipelineCanceled()), breakpoint.get(), SLOT(resumePipeline()));
      }

      pipeline->pushBack(filter);
    }
  }
  for (int i = 0; i < m_PipelineMessageObservers.size(); i++)
  {
    pipeline->addMessageReceiver(m_PipelineMessageObservers[i]);
  }
  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::addFilterToModel(AbstractFilter::Pointer filter, PipelineTreeModel* model, const QModelIndex &parentIndex, int insertionIndex)
{
  QModelIndex pipelineIndex = parentIndex;
  if (pipelineIndex.isValid() == false)
  {
    pipelineIndex = m_ActivePipelineIndex;
    if (pipelineIndex.isValid() == false)
    {
      FilterPipeline::Pointer pipeline = FilterPipeline::New();
      pipeline->pushBack(filter);
      addPipelineToModel("Untitled Pipeline", pipeline, model, true);
    }
  }

  model->insertRow(insertionIndex, pipelineIndex);
  QModelIndex filterIndex = model->index(insertionIndex, PipelineTreeItem::Name, pipelineIndex);
  model->setData(filterIndex, filter->getHumanLabel(), Qt::DisplayRole);
  model->setItemType(filterIndex, PipelineTreeItem::ItemType::Filter);
  model->setFilter(filterIndex, filter);

  if (m_ActivePipelineIndex.isValid() == false)
  {
    updateActivePipeline(pipelineIndex, model);
  }

  QModelIndexList list;
  list.push_back(filterIndex);
  emit filtersAddedToModel(list);

  preflightPipeline(m_ActivePipelineIndex, model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::addPipelineToModel(const QString &pipelineName, FilterPipeline::Pointer pipeline, PipelineTreeModel* model, bool setAsActive, QModelIndex parentIndex, int insertionIndex)
{
  if (parentIndex == QModelIndex())
  {
    if (model->getMaxNumberOfPipelines() == 1)
    {
      updateActivePipeline(QModelIndex(), model);
      model->removeRow(0);
    }

    int row = model->rowCount();
    model->insertRow(row);
    QModelIndex pipelineIndex = model->index(row, PipelineTreeItem::Name);
    model->setData(pipelineIndex, pipelineName, Qt::DisplayRole);
    model->setItemType(pipelineIndex, PipelineTreeItem::ItemType::Pipeline);
    parentIndex = pipelineIndex;
  }

  if (insertionIndex == -1)
  {
    insertionIndex = model->rowCount(parentIndex);
  }

  FilterPipeline::FilterContainerType filters = pipeline->getFilterContainer();
  QModelIndexList list;
  for (int i = 0; i < filters.size(); i++)
  {
    AbstractFilter::Pointer filter = filters[i];
    blockSignals(true);
    addFilterToModel(filter, model, parentIndex, insertionIndex);
    blockSignals(false);

    QModelIndex filterIndex = model->index(insertionIndex, PipelineTreeItem::Name, parentIndex);
    list.push_back(filterIndex);

    insertionIndex++;
  }

  if (setAsActive == true)
  {
    updateActivePipeline(parentIndex, model);
  }

  emit filtersAddedToModel(list);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PipelineTreeController::addPipelineToModelFromFile(const QString& filePath, PipelineTreeModel* model, const QModelIndex &parentIndex, int insertionIndex)
{
  QFileInfo fi(filePath);
  if(fi.exists() == false)
  {
    QMessageBox::warning(nullptr, QString::fromLatin1("Pipeline Read Error"), QString::fromLatin1("There was an error opening the specified pipeline file. The pipeline file does not exist."));
    return -1;
  }

  QString ext = fi.suffix();
  QString name = fi.fileName();
  QString baseName = fi.baseName();

  // Read the pipeline from the file
  FilterPipeline::Pointer pipeline = getPipelineFromFile(filePath);

  // Check that a valid extension was read...
  if(pipeline == FilterPipeline::NullPointer())
  {
    emit statusMessageGenerated(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    emit standardOutputMessageGenerated(tr("The pipeline was not read correctly from file '%1'. '%2' is an unsupported file extension.").arg(name).arg(ext));
    return -1;
  }

  // Notify user of successful read
  emit statusMessageGenerated(tr("Opened \"%1\" Pipeline").arg(baseName));
  emit standardOutputMessageGenerated(tr("Opened \"%1\" Pipeline").arg(baseName));

  // Populate the pipeline view
  addPipelineToModel(fi.fileName(), pipeline, model, true, parentIndex, insertionIndex);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterPipeline::Pointer PipelineTreeController::getPipelineFromFile(const QString& filePath)
{
  QFileInfo fi(filePath);
  QString ext = fi.suffix();

  FilterPipeline::Pointer pipeline = FilterPipeline::NullPointer();
  if(ext == "dream3d")
  {
    H5FilterParametersReader::Pointer dream3dReader = H5FilterParametersReader::New();
    pipeline = dream3dReader->readPipelineFromFile(filePath);
  }
  else if(ext == "json")
  {
    JsonFilterParametersReader::Pointer jsonReader = JsonFilterParametersReader::New();
    pipeline = jsonReader->readPipelineFromFile(filePath);
  }

  return pipeline;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::updateActivePipeline(const QModelIndex &pipelineIdx, PipelineTreeModel* model)
{
  emit pipelineIssuesCleared();

  model->setActivePipeline(m_ActivePipelineIndex, false);
  model->setActivePipeline(pipelineIdx, true);

  m_ActivePipelineIndex = pipelineIdx;

  if (m_ActivePipelineIndex.isValid() == true)
  {
    preflightPipeline(m_ActivePipelineIndex, model);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::blockPreflightSignals(bool b)
{
  if(b)
  {
    m_BlockPreflightStack.push(b);
  }
  else
  {
    m_BlockPreflightStack.pop();
  }

  m_BlockPreflight = (m_BlockPreflightStack.size() > 0) ? true : false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::addPipelineMessageObserver(QObject* pipelineMessageObserver)
{
  m_PipelineMessageObservers.push_back(pipelineMessageObserver);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject PipelineTreeController::toJsonObject(PipelineTreeModel* model)
{
  PipelineTreeItem* rootItem = model->getRootItem();

  QJsonObject treeObj;
  for(int i = 0; i < rootItem->childCount(); i++)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, QModelIndex());
    QString name = childIndex.data().toString();

    if(name.compare("Prebuilt Pipelines") != 0)
    {
      QJsonObject childObj = wrapModel(childIndex, model);
      treeObj[name] = childObj;
    }
  }

  return treeObj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonObject PipelineTreeController::wrapModel(QModelIndex currentIndex, PipelineTreeModel* model)
{
  QJsonObject obj;

  QString name = model->index(currentIndex.row(), PipelineTreeItem::Name, currentIndex.parent()).data().toString();

  for(int i = 0; i < model->rowCount(currentIndex); i++)
  {
    QModelIndex childIndex = model->index(i, PipelineTreeItem::Name, currentIndex);
    QString childName = childIndex.data().toString();

    QJsonObject childObj = wrapModel(childIndex, model);
    obj[childName] = childObj;
  }

  if(model->flags(currentIndex).testFlag(Qt::ItemIsDropEnabled) == true)
  {
    obj.insert("Expanded", model->needsToBeExpanded(currentIndex));
  }

  return obj;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineTreeModel* PipelineTreeController::fromJsonObject(QJsonObject treeObject, PipelineTreeModel* model)
{
  QStringList keys = treeObject.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    if(keys[i].compare("Prebuilt Pipelines") == 0)
    {
      continue;
    }
    QJsonValue val = treeObject.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), model, QModelIndex());
    }
  }

  return model;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineTreeController::unwrapModel(QString objectName, QJsonObject object, PipelineTreeModel* model, QModelIndex parentIndex)
{
  int row = model->rowCount(parentIndex);
  model->insertRow(row, parentIndex);
  QModelIndex nameIndex = model->index(row, PipelineTreeItem::Name, parentIndex);

  QString path = object["Path"].toString();
  bool expanded = object["Expanded"].toBool();

  QFileInfo fi(path);
  if(path.isEmpty() == false)
  {
    model->setData(nameIndex, QIcon(":/bookmark.png"), Qt::DecorationRole);
    if(fi.exists() == false)
    {
      // Set the itemHasError variable
      model->setData(nameIndex, true, Qt::UserRole);
    }
  }
  else
  {
    model->setData(nameIndex, QIcon(":/folder_blue.png"), Qt::DecorationRole);
  }

  path = QDir::toNativeSeparators(path);

  model->setData(nameIndex, objectName, Qt::DisplayRole);
  model->setNeedsToBeExpanded(nameIndex, expanded);

  QStringList keys = object.keys();
  keys.sort(Qt::CaseInsensitive);
  for(int i = 0; i < keys.size(); i++)
  {
    QJsonValue val = object.value(keys[i]);
    if(val.isObject())
    {
      unwrapModel(keys[i], val.toObject(), model, nameIndex);
    }
  }
}


