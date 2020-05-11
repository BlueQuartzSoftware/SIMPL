#pragma once

#include "SVWidgetsLib/SVWidgetsLib.h"
#include "SVWidgetsLib/Widgets/StatusBarButton.h"

class SVWidgetsLib_EXPORT StatusBarIssuesButton : public StatusBarButton
{
  Q_OBJECT
public:
  StatusBarIssuesButton(QWidget* parent = nullptr);

  ~StatusBarIssuesButton() override;

  /**
   * @brief paintEvent
   * @param event
   */
  void paintEvent(QPaintEvent* event) override;

public slots:
  void setErrorBadgeCount(int count);
  void setWarningBadgeCount(int count);

protected:
  void updateMinimumWidth() override;

private:
  int m_ErrorBadgeCount = 0;
  int m_WarningBadgeCount = 0;

  int m_BadgeMargin = 4;
  int m_BadgeWidth = 0;

public:
  StatusBarIssuesButton(const StatusBarIssuesButton&) = delete;            // Copy Constructor Not Implemented
  StatusBarIssuesButton(StatusBarIssuesButton&&) = delete;                 // Move Constructor Not Implemented
  StatusBarIssuesButton& operator=(const StatusBarIssuesButton&) = delete; // Copy Assignment Not Implemented
  StatusBarIssuesButton& operator=(StatusBarIssuesButton&&) = delete;      // Move Assignment Not Implemented
};
