/* ============================================================================
* Copyright (c) 2017 BlueQuartz Software, LLC
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
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PipelineListWidget.h"

#include "SVWidgetsLib/Widgets/PipelineModel.h"
#include "SVWidgetsLib/Widgets/SVStyle.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListWidget::PipelineListWidget(QWidget* parent)
  : QFrame(parent)
{
  setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PipelineListWidget::~PipelineListWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListWidget::setupGui()
{
  setPipelineState(SVPipelineView::PipelineViewState::Idle);
  startPipelineBtn->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListWidget::on_startPipelineBtn_clicked()
{
  PipelineModel* model = pipelineView->getPipelineModel();
  if(pipelineView->getPipelineState() == SVPipelineView::PipelineViewState::Running)
  {
    QModelIndex pipelineIndex = model->index(0, PipelineItem::Contents);
    emit pipelineCanceled(pipelineIndex);

    setPipelineState(SVPipelineView::PipelineViewState::Cancelling);

    //    // Enable FilterListToolboxWidget signals - resume adding filters
    //    getFilterListToolboxWidget()->blockSignals(false);

    //    // Enable FilterLibraryToolboxWidget signals - resume adding filters
    //    getFilterLibraryToolboxWidget()->blockSignals(false);

    return;
  }

  if(pipelineView->getPipelineState() == SVPipelineView::PipelineViewState::Cancelling)
  {
    return;
  }

  if (model->rowCount() > 0)
  {
    setPipelineState(SVPipelineView::PipelineViewState::Running);
    pipelineView->executePipeline();
    update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListWidget::preflightFinished(FilterPipeline::Pointer pipeline, int err)
{
  if(err >= 0 && !pipeline->getFilterContainer().empty())
  {
    startPipelineBtn->setEnabled(true);
  }
  else
  {
    startPipelineBtn->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListWidget::pipelineFinished()
{
  startPipelineBtn->setText("Start Pipeline");
  startPipelineBtn->setIcon(QIcon(":/SIMPL/icons/images/media_play_white.png"));
  startPipelineBtn->setStyleSheet(getStartPipelineIdleStyle());
  startPipelineBtn->repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListWidget::setProgressValue(float percent)
{
  startPipelineBtn->setStyleSheet(getStartPipelineInProgressStyle(percent));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListWidget::getStartPipelineIdleStyle()
{
  QFont font = SVStyle::Instance()->GetHumanLabelFont();
  QString fontString;
  QTextStream in(&fontString);
  in << "font: " << font.weight() << " " <<
#if defined(Q_OS_MAC)
      font.pointSize() - 2
#elif defined(Q_OS_WIN)
      font.pointSize() - 3
#else
      font.pointSize()
#endif
     << "pt \"" << font.family() << "\";"
     << "font-weight: bold;";

  QString cssStr;
  QTextStream ss(&cssStr);
  ss << "QPushButton:enabled { ";
  ss << "background-color: rgb(0, 118, 6);\n";
  ss << "color: white;\n";
  ss << "border-radius: 0px;\n";
  ss << fontString;
  ss << "}\n\n";

  ss << "QPushButton:disabled { ";
  ss << "background-color: rgb(150, 150, 150);\n";
  ss << "color: rgb(190, 190, 190);\n";
  ss << "border-radius: 0px;\n";
  ss << fontString;
  ss << "}";

  return cssStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PipelineListWidget::getStartPipelineInProgressStyle(float percent)
{
  QFont font = SVStyle::Instance()->GetHumanLabelFont();
  QString fontString;
  QTextStream in(&fontString);
  in << "font: " << font.weight() << " " <<
#if defined(Q_OS_MAC)
      font.pointSize() - 2
#elif defined(Q_OS_WIN)
      font.pointSize() - 3
#else
      font.pointSize()
#endif
     << "pt \"" << font.family() << "\";"
     << "font-weight: bold;";

  QString cssStr;
  QTextStream ss(&cssStr);
  ss << "QPushButton { ";
  ss << QString("background: qlineargradient(x1:0, y1:0.5, x2:1, y2:0.5, stop:0 rgb(29, 168, 29), stop:%1 rgb(29, 168, 29), stop:%2 rgb(0, 118, 6), stop:1 rgb(0, 118, 6));\n")
            .arg(percent)
            .arg(percent + 0.001);
  ss << "color: white;\n";
  ss << "border-radius: 0px;\n";
  ss << fontString;

  ss << "}";

  return cssStr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SVPipelineView* PipelineListWidget::getPipelineView()
{
  return pipelineView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PipelineListWidget::setPipelineState(SVPipelineView::PipelineViewState state)
{
  switch(state)
  {
  case SVPipelineView::PipelineViewState::Idle:
    startPipelineBtn->setText("Start Pipeline");
    startPipelineBtn->setIcon(QIcon(":/SIMPL/icons/images/media_play_white.png"));
    startPipelineBtn->setStyleSheet(getStartPipelineIdleStyle());
    break;
  case SVPipelineView::PipelineViewState::Running:
    startPipelineBtn->setText("Cancel Pipeline");
    startPipelineBtn->setIcon(QIcon(":/SIMPL/icons/images/media_stop_white.png"));
    break;

  case SVPipelineView::PipelineViewState::Cancelling:
    startPipelineBtn->setText("Canceling...");
    break;
  }
  pipelineView->setPipelineState(state);
}
