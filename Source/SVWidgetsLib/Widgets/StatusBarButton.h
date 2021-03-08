#pragma once

#include <QtWidgets/QToolButton>

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVWidgetsLib_EXPORT StatusBarButton : public QToolButton
{
  Q_OBJECT
public:
  StatusBarButton(QWidget* parent = nullptr);

  ~StatusBarButton() override;

  /**
   * @brief paintEvent
   * @param event
   */
  void paintEvent(QPaintEvent* event) override;

  void setFlat(bool b){};

protected:
  /**
   * @brief mousePressEvent
   * @param event
   */
  void mousePressEvent(QMouseEvent* event) override;

  /**
   * @brief mouseReleaseEvent
   * @param event
   */
  void mouseReleaseEvent(QMouseEvent* event) override;

  virtual void updateMinimumWidth();

  /**
   * @brief getTextMargin
   * @return
   */
  int getTextMargin();

Q_SIGNALS:
  void visibilityChanged(bool visible);

private:
  int m_TextMargin = 6;

  bool m_Pressed = false;

public:
  StatusBarButton(const StatusBarButton&) = delete;            // Copy Constructor Not Implemented
  StatusBarButton(StatusBarButton&&) = delete;                 // Move Constructor Not Implemented
  StatusBarButton& operator=(const StatusBarButton&) = delete; // Copy Assignment Not Implemented
  StatusBarButton& operator=(StatusBarButton&&) = delete;      // Move Assignment Not Implemented
};
