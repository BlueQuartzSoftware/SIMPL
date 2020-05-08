#pragma once

#include <cassert>

#include <QtCore/QString>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QStandardItemModel>

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"

/**
 * @brief The FilterParameterWidgetUtils class implements some common methods that
 * can be used by widgets that present QComboBoxes populated with DataContainers,
 * AttributeMatrices or DataArrays.
 */
class FilterParameterWidgetUtils
{
public:
  /**
   * @brief PopulateDataContainerComboBox
   * @param filter
   * @param filterParameter
   * @param dcCombo
   * @param dcaProxy
   */
  template <typename FilterParameterType>
  static void PopulateDataContainerComboBox(AbstractFilter* filter, FilterParameter* filterParameter, QComboBox* dcCombo, DataContainerArrayProxy& dcaProxy)
  {
    FilterParameterType* fp = dynamic_cast<FilterParameterType*>(filterParameter);
    assert(fp != nullptr);
    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    // Populate the DataContainerArray Combo Box with all the DataContainers
    QList<DataContainerProxy> dcList = dcaProxy.getDataContainers().values();
    QListIterator<DataContainerProxy> iter(dcList);
    dcCombo->clear();
    IGeometry::Types geomTypes = fp->getDefaultGeometryTypes();
    while(iter.hasNext())
    {
      DataContainerProxy dcProxy = iter.next();
      DataContainer::Pointer dc = dca->getDataContainer(dcProxy.getName());
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
      dcCombo->addItem(dcProxy.getName());

      if(!geomTypes.isEmpty())
      {
        if(!geomTypes.contains(geomType))
        {
          QStandardItemModel* model = qobject_cast<QStandardItemModel*>(dcCombo->model());
          if(nullptr != model)
          {
            QStandardItem* item = model->item(dcCombo->findText(dcProxy.getName()));
            if(nullptr != item)
            {
              item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            }
          }
        }
      }
    }
  }

  /**
   * @brief PopulateAttributeMatrixComboBox
   * @param filter
   * @param filterParameter
   * @param dcCombo
   * @param amCombo
   * @param dcaProxy
   */
  template <typename FilterParameterType>
  static void PopulateAttributeMatrixComboBox(AbstractFilter* filter, FilterParameter* filterParameter, QComboBox* dcCombo, QComboBox* amCombo, DataContainerArrayProxy& dcaProxy)
  {
    FilterParameterType* fp = dynamic_cast<FilterParameterType*>(filterParameter);
    assert(fp != nullptr);
    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    if(nullptr == dca.get())
    {
      return;
    }

    QString dcName = dcCombo->currentText();

    // Clear the AttributeMatrix List
    bool alreadyBlocked = false;
    if(amCombo->signalsBlocked())
    {
      alreadyBlocked = true;
    }
    amCombo->blockSignals(true);
    amCombo->clear();

    // Loop over the data containers until we find the proper data container
    QList<DataContainerProxy> containers = dcaProxy.getDataContainers().values();
    QListIterator<DataContainerProxy> containerIter(containers);
    QVector<AttributeMatrix::Type> defVec = fp->getDefaultAttributeMatrixTypes();
    while(containerIter.hasNext())
    {
      DataContainerProxy dc = containerIter.next();

      if(dc.getName() == dcName)
      {
        // We found the proper Data Container, now populate the AttributeMatrix List
        QMap<QString, AttributeMatrixProxy> attrMats = dc.getAttributeMatricies();
        QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
        while(attrMatsIter.hasNext())
        {
          attrMatsIter.next();
          QString amName = attrMatsIter.key();
          AttributeMatrix::Pointer am = dca->getAttributeMatrix(DataArrayPath(dc.getName(), amName, ""));
          amCombo->addItem(amName);

          if(nullptr != am.get() && !defVec.isEmpty() && !defVec.contains(am->getType()))
          {
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(amCombo->model());
            if(nullptr != model)
            {
              QStandardItem* item = model->item(amCombo->findText(amName));
              if(nullptr != item)
              {
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
              }
            }
          }
        }
      }
    }

    if(!alreadyBlocked)
    { // Only unblock if this function blocked the signals.
      amCombo->blockSignals(false);
    }
  }

  /**
   * @brief PopulateAttributeArrayComboBox
   * @param filter
   * @param filterParameter
   * @param dcCombo
   * @param amCombo
   * @param aaCombo
   * @param dcaProxy
   */
  template <typename FilterParameterType>
  static void PopulateAttributeArrayComboBox(AbstractFilter* filter, FilterParameter* filterParameter, QComboBox* dcCombo, QComboBox* amCombo, QComboBox* aaCombo, DataContainerArrayProxy& dcaProxy)
  {
    FilterParameterType* fp = dynamic_cast<FilterParameterType*>(filterParameter);
    assert(fp != nullptr);

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    if(nullptr == dca.get())
    {
      return;
    }
    bool alreadyBlocked = false;
    if(aaCombo->signalsBlocked())
    {
      alreadyBlocked = true;
    }
    aaCombo->blockSignals(true);
    aaCombo->clear();

    // Get the selected Data Container Name from the DataContainerList Widget
    QString currentDCName = dcCombo->currentText();
    QString currentAttrMatName = amCombo->currentText();

    // Loop over the data containers until we find the proper data container
    DataContainerArrayProxy::StorageType& dcMap = dcaProxy.getDataContainers();

    QVector<QString> daTypes = fp->getDefaultAttributeArrayTypes();
    std::vector<std::vector<size_t>> cDims = fp->getDefaultComponentDimensions();
    for(auto& dc : dcMap)
    {
      if(dc.getName() == currentDCName)
      {
        // We found the proper Data Container, now populate the AttributeMatrix List
        DataContainerProxy::StorageType& attrMats = dc.getAttributeMatricies();
        for(auto& amProxy : attrMats)
        {
          QString amName = amProxy.getName();
          if(amName.compare(currentAttrMatName) == 0)
          {
            // Clear the list of arrays from the QListWidget
            aaCombo->clear();
            // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
            QMap<QString, DataArrayProxy>& dataArrays = amProxy.getDataArrays();
            for(auto& daProxy : dataArrays)
            {
              QString daName = daProxy.getName();
              IDataArray::Pointer da = dca->getPrereqIDataArrayFromPath(nullptr, DataArrayPath(dc.getName(), amProxy.getName(), daName));
              aaCombo->addItem(daName);

              if(nullptr != da.get() && ((!daTypes.isEmpty() && !daTypes.contains(da->getTypeAsString())) || (!cDims.empty() && !VectorContains<size_t>(cDims, da->getComponentDimensions()))))
              {
                QStandardItemModel* model = qobject_cast<QStandardItemModel*>(aaCombo->model());
                if(nullptr != model)
                {
                  QStandardItem* item = model->item(aaCombo->findText(daName));
                  if(nullptr != item)
                  {
                    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                  }
                }
              }
            }
          }
        }
      }

      aaCombo->setCurrentIndex(-1);
      if(!alreadyBlocked)
      {
        aaCombo->blockSignals(false);
      }
    }
  }

  // -----------------------------------------------------------------------------
  template <typename T>
  static bool VectorContains(const std::vector<std::vector<T>>& container, const std::vector<T>& comparison)
  {
    auto pos = std::find(container.begin(), container.end(), comparison);
    return (pos != container.end());
  }
  // -----------------------------------------------------------------------------
  template <typename T>
  static bool VectorContains(std::vector<std::vector<T>>& container, const std::vector<T>& comparison)
  {
    auto pos = std::find(container.begin(), container.end(), comparison);
    return (pos != container.end());
  }
  /**
   * @brief PopulateAttributeArrayList This method populates a QListWidget with
   * items representing AttributeArrays
   * @param filter
   * @param filterParameter
   * @param dcCombo
   * @param amCombo
   * @param attributeArraysWidget
   * @param dcaProxy
   * @param selectedPaths The paths that need to be set to selected
   */
  template <typename FilterParameterType, typename WidgetType>
  static void PopulateAttributeArrayList(AbstractFilter* filter, FilterParameter* filterParameter, QComboBox* dcCombo, QComboBox* amCombo, WidgetType* attributeArraysWidget,
                                         DataContainerArrayProxy& dcaProxy, const QVector<DataArrayPath>& selectedPaths)
  {
    FilterParameterType* fp = dynamic_cast<FilterParameterType*>(filterParameter);
    assert(fp != nullptr);

    DataContainerArray::Pointer dca = filter->getDataContainerArray();
    if(nullptr == dca.get())
    {
      return;
    }

    attributeArraysWidget->blockSignals(true);
    attributeArraysWidget->clear();

    // Get the selected Data Container Name from the DataContainerList Widget
    QString currentDCName = dcCombo->currentText();
    QString currentAttrMatName = amCombo->currentText();

    // Loop over the data containers until we find the proper data container
    DataContainerArrayProxy::StorageType& dcMap = dcaProxy.getDataContainers();

    QVector<QString> daTypes = fp->getDefaultAttributeArrayTypes();
    QVector<std::vector<size_t>> cDims = fp->getDefaultComponentDimensions();
    for(auto& dc : dcMap)
    {
      if(dc.getName() == currentDCName)
      {
        // We found the proper Data Container, now populate the AttributeMatrix List
        DataContainerProxy::StorageType& attrMats = dc.getAttributeMatricies();
        for(auto& amProxy : attrMats)
        {
          QString amName = amProxy.getName();
          if(amName == currentAttrMatName)
          {
            // Clear the list of arrays from the QListWidget
            attributeArraysWidget->clear();
            // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
            QMap<QString, DataArrayProxy>& dataArrays = amProxy.getDataArrays();
            for(auto& daProxy : dataArrays)
            {
              QString daName = daProxy.getName();
              QListWidgetItem* daItem = new QListWidgetItem(daName);
              daItem->setCheckState(Qt::Unchecked);

              for(const auto& selectedPath : selectedPaths)
              {
                if(selectedPath.getDataArrayName() == daName)
                {
                  daItem->setCheckState(Qt::Checked);
                }
              }

              IDataArray::Pointer da = dca->getPrereqIDataArrayFromPath(nullptr, DataArrayPath(dc.getName(), amProxy.getName(), daName));
              attributeArraysWidget->addItem(daItem);

              if(nullptr != da.get() && ((!daTypes.isEmpty() && !daTypes.contains(da->getTypeAsString())) || (!cDims.isEmpty() && !cDims.contains(da->getComponentDimensions()))))
              {
                QList<QListWidgetItem*> rejectList = attributeArraysWidget->findItems(daName, Qt::MatchRecursive);
                for(const auto& item : rejectList)
                {
                  item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                }
              }
            }
          }
        }
      }
    }

    attributeArraysWidget->blockSignals(false);
  }

protected:
  FilterParameterWidgetUtils() = default;

private:
};
