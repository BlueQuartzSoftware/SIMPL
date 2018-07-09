#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QToolButton>

#include "SVWidgetsLib/SVWidgetsLib.h"

class SVWidgetsLib_EXPORT StatusBarButton : public QToolButton
{
    Q_OBJECT
  public:
    StatusBarButton(QWidget* parent = nullptr);
    
    virtual ~StatusBarButton();
    
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
    virtual void mousePressEvent( QMouseEvent* event ) override;
    
    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    virtual void mouseReleaseEvent( QMouseEvent* event ) override;

    virtual void updateMinimumWidth();

    /**
     * @brief getTextMargin
     * @return
     */
    int getTextMargin();

  signals:
    void visibilityChanged(bool visible);

  private:
    int m_TextMargin = 6;
    
    bool m_Pressed = false;
    
    StatusBarButton(const StatusBarButton&) = delete; // Copy Constructor Not Implemented
    void operator=(const StatusBarButton&) = delete;  // Move assignment Not Implemented
};

