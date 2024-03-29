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

//-- Qt Includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsPixmapItem>

#include "SVWidgetsLib/SVWidgetsLib.h"

// --- Forward declarations of Classes ----
class QMainWindow;
class QGraphicsItem;
class QGraphicsView;
class QGraphicsScene;

/**
 * @class QtSImageGraphicsDelegate QtSImageGraphicsDelegate.h QtSImageGraphicsDelegate.h
 * @brief A Delegate class that renders HDF5 Data to various outputs
 * @author Mike Jackson
 * @date May 29, 2007
 * @version $Revision: 1.5 $
 */
class SVWidgetsLib_EXPORT QtSImageGraphicsDelegate : public QObject
{
  Q_OBJECT

public:
  QtSImageGraphicsDelegate(QObject* parent = nullptr);
  ~QtSImageGraphicsDelegate() override;

  /**
   * @brief Setter property for MainWindow
   */
  void setMainWindow(QMainWindow* value);
  /**
   * @brief Getter property for MainWindow
   * @return Value of MainWindow
   */
  QMainWindow* getMainWindow() const;

  /**
   * @brief Setter property for GraphicsView
   */
  void setGraphicsView(QGraphicsView* value);
  /**
   * @brief Getter property for GraphicsView
   * @return Value of GraphicsView
   */
  QGraphicsView* getGraphicsView() const;

  /**
   * @brief Setter property for GraphicsScene
   */
  void setGraphicsScene(QGraphicsScene* value);
  /**
   * @brief Getter property for GraphicsScene
   * @return Value of GraphicsScene
   */
  QGraphicsScene* getGraphicsScene() const;

  /**
   * @brief Setter property for CachedImage
   */
  void setCachedImage(const QImage& value);
  /**
   * @brief Getter property for CachedImage
   * @return Value of CachedImage
   */
  QImage getCachedImage() const;

  /**
   * @brief Setter property for OverlayImage
   */
  void setOverlayImage(const QImage& value);
  /**
   * @brief Getter property for OverlayImage
   * @return Value of OverlayImage
   */
  QImage getOverlayImage() const;

  /**
   * @brief Setter property for CompositeImages
   */
  void setCompositeImages(bool value);
  /**
   * @brief Getter property for CompositeImages
   * @return Value of CompositeImages
   */
  bool getCompositeImages() const;

  /**
   * @brief Displays a Text message in the graphics view. This is typically used
   * when there is an error. You should NOT use this to display general String
   * data. Use an QHDFStringDataWindow instead
   * @param message The message to display
   */
  void displayTextMessage(QString message);

  /**
   * @brief sets all cached values to nullptr or empty
   */
  void resetCaches();

public Q_SLOTS:
  void on_parentResized();

  /**
   * @brief Sets the zoom value. This is expressed in a floating point number
   * where 1.0 is 100% or NO ZOOM and -1.0 represents fit to current window.
   * @param zoomFactor The value of the zoom Factor
   */
  void setZoomFactor(double zoomFactor);

  void increaseZoom();

  void decreaseZoom();

  void fitToWindow();

  void updateGraphicsScene(bool update = true);

  //  void compositeWithImage(QImage* image, bool update = true);

public Q_SLOTS:
  void setClearMode()
  {
    m_composition_mode = QPainter::CompositionMode_Clear;
  }
  void setSourceMode()
  {
    m_composition_mode = QPainter::CompositionMode_Source;
  }
  void setDestinationMode()
  {
    m_composition_mode = QPainter::CompositionMode_Destination;
  }
  void setSourceOverMode()
  {
    m_composition_mode = QPainter::CompositionMode_SourceOver;
  }
  void setDestinationOverMode()
  {
    m_composition_mode = QPainter::CompositionMode_DestinationOver;
  }
  void setSourceInMode()
  {
    m_composition_mode = QPainter::CompositionMode_SourceIn;
  }
  void setDestInMode()
  {
    m_composition_mode = QPainter::CompositionMode_DestinationIn;
  }
  void setSourceOutMode()
  {
    m_composition_mode = QPainter::CompositionMode_SourceOut;
  }
  void setDestOutMode()
  {
    m_composition_mode = QPainter::CompositionMode_DestinationOut;
  }
  void setSourceAtopMode()
  {
    m_composition_mode = QPainter::CompositionMode_SourceAtop;
  }
  void setDestAtopMode()
  {
    m_composition_mode = QPainter::CompositionMode_DestinationAtop;
  }
  void setXorMode()
  {
    m_composition_mode = QPainter::CompositionMode_Xor;
  }
  void setPlusMode()
  {
    m_composition_mode = QPainter::CompositionMode_Plus;
  }
  void setMultiplyMode()
  {
    m_composition_mode = QPainter::CompositionMode_Multiply;
  }
  void setScreenMode()
  {
    m_composition_mode = QPainter::CompositionMode_Screen;
  }
  void setOverlayMode()
  {
    m_composition_mode = QPainter::CompositionMode_Overlay;
  }
  void setDarkenMode()
  {
    m_composition_mode = QPainter::CompositionMode_Darken;
  }
  void setLightenMode()
  {
    m_composition_mode = QPainter::CompositionMode_Lighten;
  }
  void setColorDodgeMode()
  {
    m_composition_mode = QPainter::CompositionMode_ColorDodge;
  }
  void setColorBurnMode()
  {
    m_composition_mode = QPainter::CompositionMode_ColorBurn;
  }
  void setHardLightMode()
  {
    m_composition_mode = QPainter::CompositionMode_HardLight;
  }
  void setSoftLightMode()
  {
    m_composition_mode = QPainter::CompositionMode_SoftLight;
  }
  void setDifferenceMode()
  {
    m_composition_mode = QPainter::CompositionMode_Difference;
  }
  void setExclusionMode()
  {
    m_composition_mode = QPainter::CompositionMode_Exclusion;
  }

private:
  QMainWindow* m_MainWindow = nullptr;
  QGraphicsView* m_GraphicsView = nullptr;
  QGraphicsScene* m_GraphicsScene = nullptr;

  QImage m_CachedImage = {};
  QImage m_OverlayImage = {};
  bool m_CompositeImages = {};

  QGraphicsPixmapItem* m_CurrentGraphicsItem;

  double _zoomFactor;
  double _zoomFactors[10];
  int _zoomIndex;
  bool _shouldFitToWindow;
  QPainter::CompositionMode m_composition_mode;

  /**
   * @brief Displays a Text message in the QGraphicsScene
   * @param message The message to display
   */
  void _displayTextMessage(QString message);

  /**
   * @brief Scales the image by a _zoomFactor
   * @return Returns a QImage that is scaled copy of the cached image
   */
  QImage _scaleImage();
  QImage _scaleImage(QImage image);

  QtSImageGraphicsDelegate(const QtSImageGraphicsDelegate&); // Copy Constructor Not Implemented
  void operator=(const QtSImageGraphicsDelegate&);           // Copy Assignment Not Implemented
};
