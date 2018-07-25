#pragma once

#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QListView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>


#include "SVWidgetsLib/SVWidgetsLib.h"

/**
 * @brief The SVPushButton class Our stylable QPushButton class that has its
 * own set of CSS rules.
 */
class SVWidgetsLib_EXPORT SVPushButton : public QPushButton
{
  Q_OBJECT
  
  public:
  SVPushButton(QWidget* parent = nullptr);
  virtual ~SVPushButton();
  
};

/**
 * @brief The SVIconPushButton class is used if you just need an icon-only
 * push button, i.e., NO text will EVER appear on the button.
 */
class SVWidgetsLib_EXPORT SVIconPushButton : public QPushButton
{
  Q_OBJECT
  
  public:
  SVIconPushButton(QWidget* parent = nullptr);
  virtual ~SVIconPushButton();
  
};


/**
 * @brief The SVToolButton class
 */
class SVWidgetsLib_EXPORT SVToolButton : public QToolButton
{
  Q_OBJECT
  
  public:
  SVToolButton(QWidget* parent = nullptr);
  virtual ~SVToolButton();
  
};

/**
 * @brief The SVSpinBox class Our stylable QSpinBox class that has its
 * own set of CSS rules.
 */
class SVWidgetsLib_EXPORT SVSpinBox : public QSpinBox
{
  Q_OBJECT

  public:
  SVSpinBox(QWidget* parent = nullptr);
  virtual ~SVSpinBox();

};

/**
 * @brief The SVDoubleSpinBox class Our stylable QDoubleSpinBox class that has its
 * own set of CSS rules.
 */
class SVWidgetsLib_EXPORT SVDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

  public:
  SVDoubleSpinBox(QWidget* parent = nullptr);
  virtual ~SVDoubleSpinBox();

};


/**
 * @brief The SVLineEdit class
 */
class SVWidgetsLib_EXPORT SVLineEdit : public QLineEdit
{
  Q_OBJECT
  
  public:
  SVLineEdit(QWidget* parent = nullptr);
  virtual ~SVLineEdit();
  
};

/**
 * @brief The SVLineEdit class
 */
class SVWidgetsLib_EXPORT SVSmallLabel : public QLabel
{
  Q_OBJECT
  
  public:
  SVSmallLabel(QWidget* parent = nullptr);
  virtual ~SVSmallLabel();
  
};




/**
 * @brief The SVTreeView class
 */
class SVWidgetsLib_EXPORT SVTreeView : public QTreeView
{
  Q_OBJECT
  
  public:
  SVTreeView(QWidget* parent = nullptr);
  virtual ~SVTreeView();
  
};




/**
 * @brief The SVListView class
 */
class SVWidgetsLib_EXPORT SVListView : public QListView
{
  Q_OBJECT
  
  public:
  SVListView(QWidget* parent = nullptr);
  virtual ~SVListView();
  
};


/**
 * @brief The SVListWidget class
 */
class SVWidgetsLib_EXPORT SVListWidget : public QListWidget
{
  Q_OBJECT
  
  public:
  SVListWidget(QWidget* parent = nullptr);
  virtual ~SVListWidget();
  
};

/**
 * @brief The SVTreeWidget class
 */
class SVWidgetsLib_EXPORT SVTreeWidget : public QTreeWidget
{
  Q_OBJECT

  public:
  SVTreeWidget(QWidget* parent = nullptr);
  virtual ~SVTreeWidget();

};

/**
* @brief The SVTabWidget class
*/
class SVWidgetsLib_EXPORT SVTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  SVTabWidget(QWidget* parent = nullptr);
  virtual ~SVTabWidget();

};
