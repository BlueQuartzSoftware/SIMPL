/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Softwae, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#pragma once

#include "SIMPLib/Common/PipelineMessage.h"

#include <QtWidgets/QWidget>

#include "SVWidgetsLib/SVWidgetsLib.h"

#include "SVWidgetsLib/ui_PopUpWidget.h"

/**
 * @brief The PopUpWidget class
 */
class SVWidgetsLib_EXPORT PopUpWidget : public QWidget, private Ui::PopUpWidget
{
    Q_OBJECT

  public:
    PopUpWidget(QWidget *parent = Q_NULLPTR);
    ~PopUpWidget();

    using EnumType = unsigned int;

    enum class ArrowOrientation : EnumType
    {
      Top,
      Bottom,
      Left,
      Right,
      None
    };

    SIMPL_INSTANCE_PROPERTY(int, ArrowHeight)
    SIMPL_INSTANCE_PROPERTY(int, ArrowWidth)
    SIMPL_INSTANCE_PROPERTY(int, ArrowOffset)

    /**
     * @brief setupGui
     */
    void setupGui();

    /**
     * @brief setWidget
     * @param widget
     */
    void setWidget(QWidget* widget);

    /**
     * @brief setArrowOrientation
     * @param orientation
     */
    void setArrowOrientation(ArrowOrientation orientation);

  protected:
    void paintEvent(QPaintEvent* event);

  private:
    ArrowOrientation                    m_ArrowOrientation;

    PopUpWidget(const PopUpWidget&); // Copy Constructor Not Implemented
    void operator=(const PopUpWidget&); // Move assignment Not Implemented
};

