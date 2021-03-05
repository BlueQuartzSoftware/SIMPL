#pragma once

#include <QtWidgets/QWidget>

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

  ~DataContainerArrayWidget() override;

  /**
   * @brief This method does additional GUI widget connections
   */
  void setupGui();

  void initializeWithFilter(AbstractFilter* filter);

protected Q_SLOTS:

  void updateView();

  void preflightCompleted();

  void dcaProxyView_indexChanged(const QModelIndex& current, const QModelIndex& previous);

  void on_dcaProxyView_updatePreviewWidget(const QModelIndex& index);

protected:
  void clearStandardItemModel();

private:
  AbstractFilter* m_Filter;

public:
  DataContainerArrayWidget(const DataContainerArrayWidget&) = delete;            // Copy Constructor Not Implemented
  DataContainerArrayWidget(DataContainerArrayWidget&&) = delete;                 // Move Constructor Not Implemented
  DataContainerArrayWidget& operator=(const DataContainerArrayWidget&) = delete; // Copy Assignment Not Implemented
  DataContainerArrayWidget& operator=(DataContainerArrayWidget&&) = delete;      // Move Assignment Not Implemented
};
