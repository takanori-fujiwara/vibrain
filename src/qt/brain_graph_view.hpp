#ifndef BRAINGRAPHVIEW_H
#define BRAINGRAPHVIEW_H

#include <math.h>

#include <QMenu>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "brain_mesh.hpp"
#include "detailed_matrix.hpp"
#include "edge_bundle_connections.hpp"
#include "rois.hpp"
#include "scanned_dataset.hpp"
#include "style.hpp"

#define SELECT_BUFFER_SIZE 4048 // used in mouse picking
#define NODE_LOAD_NAME 1        // used in mouse picking
#define LINK_LOAD_NAME 2        // used in mouse picking
#define SPHERE_DIVIDE 20        // used in drawing sphere

class BrainGraphView : public QOpenGLWidget {
  Q_OBJECT

public:
  explicit BrainGraphView(Style *style, QWidget *parent = 0);
  ~BrainGraphView();
  void setDetailedMatrix(DetailedMatrix *detailedMatrix);
  void setRois(Rois *rois);
  void setScannedDataset(ScannedDataset *scannedDataset);
  void setBrainMesh(BrainMesh *brainMesh);

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:
  Style *style;
  DetailedMatrix *detailedMatrix;
  Rois *rois;
  ScannedDataset *scannedDataset;
  BrainMesh *brainMesh;

  bool isBundleEdge;
  bool isEdgeColoringByGroup;
  bool isShowBrainMesh;
  bool isShowFrame;

  int rotValue[3];
  float camDist;
  float camDelta[3];
  float lookAtDelta[3];

  QPoint mousePressPos;
  QPoint mouseMovePos;
  QPoint mouseReleasePos;
  QPoint mouseWheelPos;

  float roiPositionsScalingRatio;

  int selectedObjId[2];

  QList<EdgeBundleEdge *> bundledEdges;

  void setOrtho(int w, int h);
  void draw(GLenum mode, int pickType = 0);
  void drawBrainMesh();
  void drawFrame(float size);
  void drawGraph(GLenum mode);
  void drawNodes(GLenum mode);
  void drawLinks(GLenum mode, bool edgeColoringByGroup = false,
                 bool bundlingEdge = false);
  void paintLegend();
  void paintPopup();
  void drawCube(float centerX, float centerY, float centerZ, float size,
                QColor color, bool withFill = true);
  void drawSphere(float x, float y, float z, float size, QColor color);
  void showContextMenu(const QPoint &pos);
  void bundleEdges();

  int pick(int x, int y, int pickType);
  int *selectHits(GLint hits, const GLuint *buffer, int *hitName);

private slots:
  void switchBundleEdge();
  void switchEdgeColoringMethod();
  void switchShowBrainMesh();
  void switchShowFrame();

signals:
  void hoveredRoiIndexChanged();
};

#endif // BRAINGRAPHVIEW_H
