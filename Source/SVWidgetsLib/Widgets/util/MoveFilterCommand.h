/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _movefiltercommand_h_
#define _movefiltercommand_h_

#include <QtCore/QVariant>

#include <QtWidgets/QUndoCommand>

class PipelineFilterObject;
class PipelineView;

class MoveFilterCommand : public QUndoCommand
{
  public:
    MoveFilterCommand(PipelineFilterObject* filterWidget, QVariant origin, QVariant destination, PipelineView* pipelineView, QUndoCommand* parent = 0);
    virtual ~MoveFilterCommand();

    virtual void undo();

    virtual void redo();

  private:
    PipelineView*                           m_PipelineView;
    PipelineFilterObject*                   m_FilterWidget;
    QString                                 m_JsonString;
    QVariant                                m_Origin;
    QVariant                                m_Destination;
    bool                                    m_WindowIsModified;
    bool                                    m_FirstRun;

    void moveFilter(QVariant origin, QVariant destination);

    MoveFilterCommand(const MoveFilterCommand&); // Copy Constructor Not Implemented
    void operator=(const MoveFilterCommand&); // Operator '=' Not Implemented
};

#endif /* _movefiltercommand_h_ */

