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

#pragma once


#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <QtCore/QFileSystemWatcher>
#include <QtGui/QStandardItemModel>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/FilterLibraryTreeWidget.h"
#include "SVWidgetsLib/Widgets/DataStructureItem.h"


class QtSSettings;
class DataStructureItem;

class SVWidgetsLib_EXPORT DataStructureModel : public QStandardItemModel
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(DataStructureModel)

    DataStructureModel(QObject* parent = 0);
    ~DataStructureModel() override;

    using EnumType = unsigned int;

    enum class IndexState : EnumType
    {
      Error = 0,
      Default = 1
    };

    /*
When subclassing QAbstractItemModel, at the very least you must implement index(), parent(),
rowCount(), columnCount(), and data().
These functions are used in all read-only models, and form the basis of editable models.
You can also reimplement hasChildren() to provide special behavior for models where the
implementation of rowCount() is expensive. This makes it possible for models to restrict
the amount of data requested by views, and can be used as a way to implement lazy population
of model data.
To enable editing in your model, you must also implement setData(), and reimplement flags()
to ensure that ItemIsEditable is returned. You can also reimplement headerData() and setHeaderData()
to control the way the headers for your model are presented.
The dataChanged() and headerDataChanged() signals must be emitted explicitly when reimplementing
the setData() and setHeaderData() functions, respectively.
    */

    // Read Only Model Methods
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    // Editable Model Methods
    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
    // void setHeaderData()


    // Extra Convenince Methods
    DataStructureItem* getRootItem();
    /**
     * @brief syncDataModel
     * @param dca
     */
    void syncDataModel(DataContainerArray::Pointer dca);

    bool needsToBeExpanded(const QModelIndex& index);
    void setNeedsToBeExpanded(const QModelIndex& index, bool value);

    /*
    virtual QModelIndex sibling(int row, int column, const QModelIndex& idx) const Q_DECL_OVERRIDE;

    bool isEmpty();

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild) Q_DECL_OVERRIDE;



    QModelIndexList findIndexByPath(QString filePath);
*/




  protected slots:
//    void updateRowState(const QString& path);
//    void updateModel(const QModelIndex& topLeft, const QModelIndex& bottomRight);

  private:
    DataStructureItem*            rootItem;

    DataStructureItem* getItem(const QModelIndex& index) const;

    //    QModelIndexList findIndexByPath(const QModelIndex& index, QString filePath);

    DataStructureModel(const DataStructureModel&);    // Copy Constructor Not Implemented
    void operator=(const DataStructureModel&);        // Move assignment Not Implemented
};

