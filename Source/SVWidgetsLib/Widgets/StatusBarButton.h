#ifndef _statusbarbutton_H_
#define _statusbarbutton_H_

#include <QtCore/QObject>
#include <QtWidgets/QToolButton>

class StatusBarButton : public QToolButton
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
    
  public slots:

    void setErrorBadgeCount(int count);
    void setWarningBadgeCount(int count);

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

    void updateMinimumWidth();

  private:
    int m_OriginalWidth = 0;
    int m_ErrorBadgeCount = 0;
    int m_WarningBadgeCount = 0;
    int m_TextMargin = 6;
    
    qreal m_BorderThickness = 0.0;
    qreal m_BorderIncrement = 1.0;

    int m_BadgeWidth = 0;
    int m_BadgeMargin = 4;
    int m_Spacing = 3;
    bool m_Pressed = false;

    /**
   * @brief initialize Calls all the necessary initialization code for the widget
   * @param filter
   */
    void initialize();
    
    StatusBarButton(const StatusBarButton&) = delete; // Copy Constructor Not Implemented
    void operator=(const StatusBarButton&) = delete;  // Operator '=' Not Implemented
};

#endif /* _statusbarbutton_H_ */
