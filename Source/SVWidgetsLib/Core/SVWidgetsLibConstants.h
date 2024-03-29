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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtGui/QColor>

namespace SIMPLView
{

namespace Defaults
{
static const QColor BasicColor(215, 215, 215);
static const QColor AdvancedColor(200, 200, 212);
} // namespace Defaults

namespace DragAndDrop
{
const QString FilterListItem("Filter List Item");
const QString FilterPipelineItem("Filter Pipeline Item");
const QString BookmarkItem("Bookmark Item");
const QString Url("text/uri-list");
const QString DataArrayPath("DataArrayPath");
const QString SelectionWidget("Selection Widget");
} // namespace DragAndDrop

namespace GeometryIcons
{
const QString Image(":SIMPL/icons/images/ImageGeometry.png");
const QString Vertex(":SIMPL/icons/images/VertexGeometry.png");
const QString Edge(":SIMPL/icons/images/EdgeGeometry.png");
const QString Triangle(":SIMPL/icons/images/TriangleGeometry.png");
const QString Quad(":SIMPL/icons/images/QuadGeometry.png");
const QString Tetetrahedral(":SIMPL/icons/images/TetrahedralGeometry.png");
const QString Hexahedral(":SIMPL/icons/images/HexahedralGeometry.png");
const QString Rectilinear(":SIMPL/icons/images/RectilinearGeometry.png");
} // namespace GeometryIcons

namespace DataArrayPath
{
const QString BaseNewPathIcon(":SIMPL/icons/images/Asterisk.png");
}

namespace DockWidgetSettings
{
const QString GroupName("DockWidgetSettings");
const QString IssuesDockGroupName("Issues Dock Widget");
const QString StandardOutputGroupName("Standard Output Dock Widget");
const QString KeyName("Show / Hide On Error");
enum class HideDockSetting : int
{
  Ignore = 0,
  OnError = 1,
  OnStatusAndError = 2
};
} // namespace DockWidgetSettings
} // namespace SIMPLView
