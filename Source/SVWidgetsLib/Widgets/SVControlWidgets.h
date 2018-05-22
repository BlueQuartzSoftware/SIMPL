#pragma once

#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QListView>

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
 * @brief The SVToolButton class
 */
class SVWidgetsLib_EXPORT SVLineEdit : public QLineEdit
{
  Q_OBJECT
  
  public:
  SVLineEdit(QWidget* parent = nullptr);
  virtual ~SVLineEdit();
  
};



/**
 * @brief The SVToolButton class
 */
class SVWidgetsLib_EXPORT SVTreeView : public QTreeView
{
  Q_OBJECT
  
  public:
  SVTreeView(QWidget* parent = nullptr);
  virtual ~SVTreeView();
  
};




/**
 * @brief The SVToolButton class
 */
class SVWidgetsLib_EXPORT SVListView : public QListView
{
  Q_OBJECT
  
  public:
  SVListView(QWidget* parent = nullptr);
  virtual ~SVListView();
  
};
