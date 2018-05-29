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

#ifndef _simplviewstyles_h_
#define _simplviewstyles_h_

#include <QtCore/QSet>
#include <QtCore/QString>

#include <QtGui/QFont>
#include <QtGui/QIcon>

#include "SVWidgetsLib/SVWidgetsLib.h"

class QLineEdit;
class QPushButton;



class SVWidgetsLib_EXPORT QtSStyles : public QObject
{
    Q_OBJECT
  public:
    ~QtSStyles() override;

    /**
     * @brief Instance
     * @return
     */
    static QtSStyles* Instance();

    /**
     * @brief GetUIFont
     * @return
     */
    static QString GetUIFont();

    /**
     * @brief GetHumanLabelFont
     * @return
     */
    static QFont GetHumanLabelFont();

    /**
     * @brief GetBrandingLabelFont
     * @return
     */
    static QFont GetBrandingLabelFont();

    /**
     * @brief GetCategoryFont
     * @return
     */
    static QFont GetCategoryFont();

    /**
     * @brief GetTitleFont
     * @return
     */
    static QFont GetTitleFont();

    /**
     * @brief getTreeViewFont
     * @return
     */
    QFont getTreeViewFont();

    /**
     * @brief getTreeViewItemFontColor
     * @return
     */
    QColor getTreeViewItemFontColor();

    /**
     * @brief getTreeViewItemErrorFontColor
     * @return
     */
    QColor getTreeViewItemErrorFontColor();

    /**
     * @brief getHoveredTreeViewItemFontColor
     * @return
     */
    QColor getHoveredTreeViewItemFontColor();

    /**
     * @brief getTreeViewItemBackgroundColor
     * @return
     */
    QColor getTreeViewItemBackgroundColor();

    /**
     * @brief getTreeViewItemErrorBackgroundColor
     * @return
     */
    QColor getTreeViewItemErrorBackgroundColor();

    /**
     * @brief getSelectedTreeViewItemBackgroundColor
     * @return
     */
    QColor getSelectedTreeViewItemBackgroundColor();

    /**
     * @brief getHoveredTreeViewItemBackgroundColor
     * @return
     */
    QColor getHoveredTreeViewItemBackgroundColor();

    /**
     * @brief GetFilterBackgroundColor
     * @return
     */
    static QColor GetFilterBackgroundColor();
    
    /**
     * @brief GetFilterSelectionColor
     * @return 
     */
    static QColor GetFilterSelectionColor();

    /**
     * @brief GetFilterFontColor
     * @return 
     */
    static QColor GetFilterFontColor();

    /**
     * @brief LineEditErrorStyle
     * @param lineEdit
     */
    static void LineEditErrorStyle(QLineEdit* lineEdit);

    /**
     * @brief LineEditClearStyle
     * @param lineEdit
     */
    static void LineEditClearStyle(QLineEdit* lineEdit);

    /**
     * @brief LineEditRedErrorStyle
     * @param lineEdit
     */
    static void LineEditRedErrorStyle(QLineEdit* lineEdit);

    /**
     * @brief DAPSelectionButtonStyle
     * @param exists
     * @return
     */
    static QString QToolSelectionButtonStyle(bool exists);

    /**
     * @brief QtSStyles::ColorForFilterGroup
     * @param grpName
     * @return
     */
    static QColor ColorForFilterGroup(const QString &grpName);

    /**
     * @brief QtSStyles::IconForGroup
     * @param grpName
     * @return
     */
    static QIcon IconForGroup(const QString &grpName);

    /**
     * @brief QtSStyles::IconForGroup
     * @param objectName
     * @param cssName
     * @param imageName
     * @return
     */
    static QString StyleSheetForButton(const QString &objectName, const QString &cssName, const QString &imageName);

    /**
     * @brief hasStyle
     * @return
     */
    bool hasStyle();

    /**
     * @brief setStyleFilePath
     * @param styleFilePath
     */
    void setStyleFilePath(const QString &styleFilePath);

  protected:
    QtSStyles();

    /**
     * @brief updateStyle
     */
    void updateStyle();

  private:
    static QtSStyles* m_Self;

    QString m_StyleFilePath;
    QFont m_TreeViewFont;
    QColor m_TreeViewItemFontColor;
    QColor m_TreeViewItemErrorFontColor;
    QColor m_HoveredTreeViewItemFontColor;
    QColor m_TreeViewItemBackgroundColor;
    QColor m_TreeViewItemErrorBackgroundColor;
    QColor m_SelectedTreeViewItemBackgroundColor;
    QColor m_HoveredTreeViewItemBackgroundColor;

    bool m_HasStyle = false;

    /**
     * @brief fromRGBString
     * @param rgbString
     * @return
     */
    QColor fromRGBString(const QString &rgbString, bool &success);

    void updateTreeViewFont(QJsonObject cssRepl, QJsonObject varMapping);
    void updateTreeViewItemFontColor(QJsonObject cssRepl, QJsonObject varMapping);
    void updateHoveredTreeViewItemFontColor(QJsonObject cssRepl, QJsonObject varMapping);
    void updateTreeViewItemBackgroundColor(QJsonObject cssRepl, QJsonObject varMapping);
    void updateSelectedTreeViewItemBackgroundColor(QJsonObject cssRepl, QJsonObject varMapping);
    void updateHoveredTreeViewItemBackgroundColor(QJsonObject cssRepl, QJsonObject varMapping);
    void updateTreeViewItemErrorFontColor(QJsonObject cssRepl, QJsonObject varMapping);
    void updateTreeViewItemErrorBackgroundColor(QJsonObject cssRepl, QJsonObject varMapping);
};

#endif /* _SIMPLViewStyles_H_ */
