#ifndef MATRIXVIEW_H
#define MATRIXVIEW_H

#include <math.h>

#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>

#include "community_detection.hpp"
#include "detailed_matrix.hpp"
#include "hierarchical_clustering.hpp"
#include "rois.hpp"
#include "scanned_dataset.hpp"
#include "style.hpp"

class MatrixView : public QOpenGLWidget {
  Q_OBJECT

public:
  explicit MatrixView(Style *style, QWidget *parent = 0);
  ~MatrixView();
  void setDetailedMatrix(DetailedMatrix *detailedMatrix);
  void setRois(Rois *rois);
  void setScannedDataset(ScannedDataset *scannedDataset);

public slots:
  void saveMatrixFiles();

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void leaveEvent(QEvent *event);

private:
  Style *style;
  DetailedMatrix *detailedMatrix;
  Rois *rois;
  ScannedDataset *scannedDataset;

  QPointF matrixDrawingStartPos;
  float matrixDrawingSize;

  QPoint mousePressPos;
  QPoint mouseMovePos;
  QPoint mouseReleasePos;
  bool isDragging;

  void drawDetailedMatrix(float x, float y, float size,
                          QVector<QColor> colormap,
                          QColor strokeColor = QColor(0, 0, 0, 0),
                          float strokeWidth = 0.0);
  void drawGroupLines(float x, float y, float size,
                      QVector<QColor> groupColors);
  void drawQuad(float x, float y, float w, float h, QColor color);
  void drawLineQuad(float x, float y, float w, float h, float width,
                    QColor color);
  void drawDraggedArea();
  void drawHoveredRoiLines(float matrixStartX, float matrixStartY,
                           float matrixSize);
  void paintPopup();

  QPointF qtPosToGlPos(QPoint qtPos);
  QPoint mousePosToRowAndColPos(QPoint mousePos);
  QPoint mousePosToRowAndColIndices(QPoint mousePos);
  void selectElement(QPoint mousePos);
  void selectElementWithRect(QPoint pos1, QPoint pos2);
  void showContextMenu(const QPoint &pos);

private slots:
  void clearSelection();
  void clearFilteredOut();
  void clearElementsUsedInDimReduction();
  void showSelectedElementsInBrainGraphView();
  void recalcMdsWithSelectedElements();
  void arrangeByHClust();
  void arrangeByModularity();
  void changeSubsetRois();
  void resetArrange();

signals:
  void detailedMatrixFilteredOutChanged();
  void recalcMdsSelected();
  void matrixOrderChanged();
  void roisSubsetChanged();
  void hoveredRowAndColIndexChanged();
};

#endif // MATRIXVIEW_H
