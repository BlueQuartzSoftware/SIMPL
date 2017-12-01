#ifndef _datacontainerarraywidget_h_
#define _datacontainerarraywidget_h_

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtWidgets/QWidget>
#include <QtGui/QStandardItemModel>
#include <QtCore/QItemSelectionModel>

#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"

#include "ui_DataContainerArrayWidget.h"


class QStandardItem;
class AbstractFilter;
class DataContainerArray;

class SVWidgetsLib_EXPORT DataContainerArrayWidget : public QWidget, private Ui::DataContainerArrayWidget
{
    Q_OBJECT

  public:
    DataContainerArrayWidget(QWidget* parent = nullptr);
    DataContainerArrayWidget(AbstractFilter* filter, QWidget* parent = nullptr);

    virtual ~DataContainerArrayWidget();


    /**
    * @brief This method does additional GUI widget connections
    */
    void setupGui();

    void initializeWithFilter(AbstractFilter* filter);



  protected slots:

    void updateView();

    void preflightCompleted();

    void dcaProxyView_indexChanged(const QModelIndex& current, const QModelIndex& previous);

    void on_dcaProxyView_updatePreviewWidget(const QModelIndex& index);



  protected:

    void clearStandardItemModel();

  private:
    AbstractFilter*     m_Filter;

    DataContainerArrayWidget(const DataContainerArrayWidget&) = delete; // Copy Constructor Not Implemented
    void operator=(const DataContainerArrayWidget&) = delete;           // Operator '=' Not Implemented
};

#endif /* _DataContainerArrayWidget_H_ */
