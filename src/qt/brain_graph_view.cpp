#include "brain_graph_view.hpp"

BrainGraphView::BrainGraphView(Style *style, QWidget *parent) {
  this->setMouseTracking(true);

  this->style = style;

  isBundleEdge = false;
  isEdgeColoringByGroup = false;
  isShowBrainMesh = true;
  isShowFrame = true;
  rotValue[0] = 0;
  rotValue[1] = 30;
  rotValue[2] = -40;
  camDist = 1.05;
  camDelta[0] = 0.0;
  camDelta[1] = 0.0;
  camDelta[2] = 0.0;
  lookAtDelta[0] = 0.0;
  lookAtDelta[1] = 0.0;
  lookAtDelta[2] = 0.0;

  roiPositionsScalingRatio = 0.005;

  bundledEdges.clear();
}

BrainGraphView::~BrainGraphView() {
  // delete style;
  // delete detailedMatrix;
}

void BrainGraphView::initializeGL() {
  glClearColor(style->bgColor.redF(), style->bgColor.greenF(),
               style->bgColor.blueF(), style->bgColor.alphaF());
  GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat diffuse[] = {0.7, 0.7, 0.7, 1.0};
  GLfloat ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat position[] = {3.0, 4.0, 10.0, 1.0};
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glEnable(GL_NORMALIZE);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
}

void BrainGraphView::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  setOrtho(width, height);

  glLoadIdentity();
  this->update();
}

void BrainGraphView::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // below glEnable required to recall to use opengl and paintEvent together
  glEnable(GL_DEPTH_TEST);
  // glCullFace(GL_BACK);
  // glEnable(GL_CULL_FACE); // problem for cylinder
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_ALPHA_TEST);
  // glAlphaFunc(GL_GREATER, 0.01);

  draw(GL_RENDER);
  paintLegend();
}

void BrainGraphView::draw(GLenum mode, int pickType) {
  glLoadIdentity();

  gluPerspective(45.0,                               //  view angle
                 (double)width() / (double)height(), // aspect ratio
                 0.1, 100.0);                        // view range (near, far)

  gluLookAt(camDist + camDelta[0], 0.0 + camDelta[1],
            0.0 + camDelta[2], //  eye pos
            0.0 + lookAtDelta[0], 0.0 + lookAtDelta[1],
            0.0 + lookAtDelta[2], //  target pos
            0.0, 0.0, 1.0);       //  upper direction

  glPushMatrix();
  {
    glRotatef(rotValue[0], 1.0f, 0.0f, 0.0f);
    glRotatef(rotValue[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rotValue[2], 0.0f, 0.0f, 1.0f);

    if (mode == GL_RENDER) {
      // draw graph
      glPushMatrix();
      glTranslatef(0.0, 0.1, 0.0);
      drawGraph(mode);
      glPopMatrix();

      // draw brain
      if (isShowBrainMesh) {
        glPushMatrix();
        {
          glTranslatef(brainMesh->getTranslateX(), brainMesh->getTranslateY(),
                       brainMesh->getTranslateZ());
          glScalef(brainMesh->getScaleX(), brainMesh->getScaleY(),
                   brainMesh->getScaleZ());
          drawBrainMesh();
        }
        glPopMatrix();
      }

      if (isShowFrame) {
        drawFrame(0.95);
      }

      paintPopup();
    }
    if (mode == GL_SELECT) {
      // draw graph for mouse picking
      glPushMatrix();
      glTranslatef(0.0, 0.1, 0.0);
      drawGraph(mode);
      glPopMatrix();
    }
  }
  glPopMatrix();

  if (mode == GL_RENDER) {
    frameSwapped();
  }
}

void BrainGraphView::setOrtho(int w, int h) {
  float aspect;

  if (w > h) {
    aspect = (float)w / h;
    gluOrtho2D(-aspect, aspect, -1.0, 1.0);
  } else {
    aspect = (float)h / w;
    gluOrtho2D(-1.0, 1.0, -aspect, aspect);
  }
}

void BrainGraphView::drawBrainMesh() {
  glColor4f(style->brainMeshColor.redF(), style->brainMeshColor.greenF(),
            style->brainMeshColor.blueF(), style->brainMeshColor.alphaF());

  // TODO: here should use drawelement with indices
  std::vector<float> brainMeshPositions = brainMesh->getBrainMeshPositions();
  glVertexPointer(3, GL_FLOAT, 0, &brainMeshPositions[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDrawArrays(GL_TRIANGLES, 0, brainMeshPositions.size() / 3);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void BrainGraphView::drawFrame(float size) {
  glLineWidth(1.0);
  drawCube(0.0, 0.0, 0.0, size, QColor(255, 255, 255, 255), false);
}

void BrainGraphView::drawGraph(GLenum mode) {
  if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
    drawNodes(mode);
    drawLinks(mode, isEdgeColoringByGroup, isBundleEdge);
  }
}

void BrainGraphView::drawNodes(GLenum mode) {
  QVector<int> degrees = detailedMatrix->getNodeDegreeFromDisplayedInfo();
  int degreeThres = detailedMatrix->getDegreeThres();

  if (degrees.size() < rois->getNumOfRois()) {
    degrees = QVector<int>(rois->getNumOfRois(), 0);
  }

  float unitSize = 1.0 * roiPositionsScalingRatio;
  float sizeEmpsize;

  if (mode == GL_SELECT)
    glLoadName(NODE_LOAD_NAME);

  for (int i = 0; i < rois->getNumOfRois(); ++i) {
    if (rois->getRoiAt(i)->getVisible() && rois->getRoiAt(i)->getRank() >= 0 &&
        degrees.at(i) >= degreeThres) {
      if (mode == GL_SELECT)
        glPushName(i);

      QColor color = style->roiColors.at(rois->getRoiAt(i)->getGroup());

      if (rois->getRoiAt(i)->getSelected()) {
        color = color.lighter(200);
        sizeEmpsize = 2.0;
      } else if (rois->getRoiAt(i)->getFilteredOut()) {
        color = color.darker(200);
      }

      QPoint hoveredRowAndColIndex = detailedMatrix->getHoveredRowAndColIndex();

      if (hoveredRowAndColIndex.x() == i || hoveredRowAndColIndex.y() == i) {
        sizeEmpsize = 2.0;
      } else {
        sizeEmpsize = 1.0;
      }

      drawSphere(rois->getRoiAt(i)->getX() * unitSize,
                 rois->getRoiAt(i)->getY() * unitSize,
                 rois->getRoiAt(i)->getZ() * unitSize,
                 rois->getRoiAt(i)->getSize() * sizeEmpsize, color);

      if (mode == GL_SELECT)
        glPopName();
    }
  }
}

void BrainGraphView::drawLinks(GLenum mode, bool edgeColoringByGroup,
                               bool bundlingEdge) {
  float unitSize = 1.0 * roiPositionsScalingRatio;
  float scaling = 1.0;

  if (mode == GL_SELECT)
    glLoadName(LINK_LOAD_NAME);

  int visibleEdgeCount = 0;
  for (int i = 0; i < detailedMatrix->getNumOfRows(); ++i) {
    for (int j = 0; j < detailedMatrix->getNumOfCols(); ++j) {
      if (detailedMatrix->getVisibleAt(i, j) &&
          !detailedMatrix->getFilteredOutAt(i, j) &&
          detailedMatrix->getUsedInDimReductionAt(i, j) &&
          rois->getRoiAt(i)->getRank() >= 0 &&
          rois->getRoiAt(j)->getRank() >= 0) {
        visibleEdgeCount++;
      }
    }
  }

  // for handling when threshold is changed after selecting bundle mode
  if (isBundleEdge && bundledEdges.size() != visibleEdgeCount) {
    bundleEdges();
  }

  int countForBundledEdges = 0;

  for (int i = 0; i < detailedMatrix->getNumOfRows(); ++i) {
    for (int j = i; j < detailedMatrix->getNumOfCols(); ++j) {
      if (detailedMatrix->getVisibleAt(i, j) &&
          !detailedMatrix->getFilteredOutAt(i, j) &&
          detailedMatrix->getUsedInDimReductionAt(i, j) &&
          rois->getRoiAt(i)->getRank() >= 0 &&
          rois->getRoiAt(j)->getRank() >= 0) {
        QColor color;
        if (!edgeColoringByGroup) {
          // TODO: modify here later (scaling value and color calculation. color
          // calc should be separated from here)
          float val = detailedMatrix->getValueAt(i, j);
          float scaledVal =
              fmin(1.0, fmax(0.0, (val - detailedMatrix->getMinThresVal()) /
                                      (detailedMatrix->getMaxThresVal() -
                                       detailedMatrix->getMinThresVal())));
          color = style->colormap.at(
              (int)(scaledVal * (style->colormap.size() - 1)));
        } else {
          color = QColor(
              style->roiColors.at(rois->getRoiAt(i)->getGroup()).red() * 0.5 +
                  style->roiColors.at(rois->getRoiAt(j)->getGroup()).red() *
                      0.5,
              style->roiColors.at(rois->getRoiAt(i)->getGroup()).green() * 0.5 +
                  style->roiColors.at(rois->getRoiAt(j)->getGroup()).green() *
                      0.5,
              style->roiColors.at(rois->getRoiAt(i)->getGroup()).blue() * 0.5 +
                  style->roiColors.at(rois->getRoiAt(j)->getGroup()).blue() *
                      0.5,
              style->roiColors.at(rois->getRoiAt(i)->getGroup()).alpha() * 0.5 +
                  style->roiColors.at(rois->getRoiAt(j)->getGroup()).alpha() *
                      0.5);
        }
        color.setAlpha(150);

        if (detailedMatrix->getSelectedAt(i, j)) {
          // color = color.lighter(200);
          // scaling = 2.0;
        } else if (detailedMatrix->getFilteredOutAt(i, j)) {
          color = color.darker(200);
        }

        float sizeEmphasize = 1.0;
        QPoint hoveredRowAndColIndex =
            detailedMatrix->getHoveredRowAndColIndex();
        if ((hoveredRowAndColIndex.x() == i &&
             hoveredRowAndColIndex.y() == j) ||
            (hoveredRowAndColIndex.x() == j &&
             hoveredRowAndColIndex.y() == i)) {
          sizeEmphasize = 5.0;
          color = color.lighter(150);
        }

        glLineWidth(detailedMatrix->getSizeAt(i, j) *
                    (float)devicePixelRatio() * sizeEmphasize);
        glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());

        if (!bundlingEdge) {
          glBegin(GL_LINES);
          glVertex3f(rois->getRoiAt(i)->getX() * unitSize,
                     rois->getRoiAt(i)->getY() * unitSize,
                     rois->getRoiAt(i)->getZ() * unitSize);
          glVertex3f(rois->getRoiAt(j)->getX() * unitSize,
                     rois->getRoiAt(j)->getY() * unitSize,
                     rois->getRoiAt(j)->getZ() * unitSize);
          glEnd();
        } else {
          QList<QVector3D> edgePoints =
              bundledEdges[countForBundledEdges]->points;

          glBegin(GL_LINE_STRIP);
          for (int k = 0; k < edgePoints.size(); ++k) {
            QVector3D point = edgePoints[k];
            glVertex3f(point.x() * unitSize, point.y() * unitSize,
                       point.z() * unitSize);
          }
          glEnd();

          countForBundledEdges++;
        }

        if (mode == GL_SELECT)
          glPopName();
      }
    }
  }
}

void BrainGraphView::paintLegend() {
  QPainter painter(this);
  painter.setFont(QFont("San Francisco", 9));
  int fontW = painter.fontMetrics().ascent();
  int fontH = painter.fontMetrics().height();

  int legendStartX = 5;
  int legendStartY = this->height() - 5;

  ///
  /// color legends
  ///
  QVector<int> labels = rois->getGroups();
  legendStartY -= fontH * labels.size();
  if (labels.size() > 1) {
    for (int i = 0; i < labels.size(); ++i) {
      int labelW = fontW * 5;
      painter.setBrush(style->roiColors.at(i));
      painter.setPen(style->roiColors.at(i));
      painter.drawEllipse(legendStartX, legendStartY, fontH - 2, fontH - 2);
      painter.setPen(Qt::white);
      painter.drawText(legendStartX + fontH, legendStartY, labelW, fontH,
                       Qt::AlignVCenter | Qt::AlignLeft,
                       "Group " + QString::number(labels.at(i)));
      legendStartY += fontH;
    }
  }

  painter.end();
}

void BrainGraphView::paintPopup() {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::black);
  painter.setBrush(QColor(222, 222, 222, 220));

  int x = mouseMovePos.x();
  int y = mouseMovePos.y();
  int hoveredRoiIndex = rois->getHoveredRoiIndex();
  if (hoveredRoiIndex >= 0) {
    int fontSize = 10;
    painter.setFont(QFont("San Francisco", fontSize));

    // Text Info
    QString info = rois->getRoiAt(hoveredRoiIndex)->getName();
    int infoPixelsWide = painter.fontMetrics().width(info);
    int numOfInfo = 1;
    int infoPixelsHeight = painter.fontMetrics().height() * numOfInfo;

    QPolygon poly;
    int rightSpace = this->width() - x;
    int bottomSpace = this->height() - y;

    // Draw Baloon. Position is changed based on spaces
    if (rightSpace > 10 + infoPixelsWide &&
        bottomSpace > 15 + infoPixelsHeight) {
      poly << QPoint(x + 0, y + 0) << QPoint(x + 8, y + 3)
           << QPoint(x + 18 + infoPixelsWide, y + 3)
           << QPoint(x + 18 + infoPixelsWide, y + 13 + infoPixelsHeight)
           << QPoint(x + 3, y + 13 + infoPixelsHeight) << QPoint(x + 3, y + 8);
    } else if (rightSpace <= 10 + infoPixelsWide &&
               bottomSpace > 15 + infoPixelsHeight) {
      poly << QPoint(x - 0, y + 0) << QPoint(x - 8, y + 3)
           << QPoint(x - 18 - infoPixelsWide, y + 3)
           << QPoint(x - 18 - infoPixelsWide, y + 13 + infoPixelsHeight)
           << QPoint(x - 3, y + 13 + infoPixelsHeight) << QPoint(x - 3, y + 8);
    } else if (rightSpace > 10 + infoPixelsWide &&
               bottomSpace <= 15 + infoPixelsHeight) {
      poly << QPoint(x + 0, y - 0) << QPoint(x + 8, y - 3)
           << QPoint(x + 18 + infoPixelsWide, y - 3)
           << QPoint(x + 18 + infoPixelsWide, y - 13 - infoPixelsHeight)
           << QPoint(x + 3, y - 13 - infoPixelsHeight) << QPoint(x + 3, y - 8);
    } else {
      poly << QPoint(x - 0, y - 0) << QPoint(x - 8, y - 3)
           << QPoint(x - 18 - infoPixelsWide, y - 3)
           << QPoint(x - 18 - infoPixelsWide, y - 13 - infoPixelsHeight)
           << QPoint(x - 3, y - 13 - infoPixelsHeight) << QPoint(x - 3, y - 8);
    }
    QPainterPath path;
    path.addPolygon(poly);
    painter.drawPolygon(poly);

    // Draw Text. Changing popup position based on spaces
    if (rightSpace > 10 + infoPixelsWide &&
        bottomSpace > 15 + infoPixelsHeight) {
      painter.drawText(x + 10, y + 10, infoPixelsWide, infoPixelsHeight,
                       Qt::AlignVCenter | Qt::AlignLeft, info);
    } else if (rightSpace <= 10 + infoPixelsWide &&
               bottomSpace > 15 + infoPixelsHeight) {
      painter.drawText(x - 10 - infoPixelsWide, y + 10, infoPixelsWide,
                       infoPixelsHeight, Qt::AlignVCenter | Qt::AlignLeft,
                       info);
    } else if (rightSpace > 10 + infoPixelsWide &&
               bottomSpace <= 15 + infoPixelsHeight) {
      painter.drawText(x + 10, y - 10 - infoPixelsHeight, infoPixelsWide,
                       infoPixelsHeight, Qt::AlignVCenter | Qt::AlignLeft,
                       info);
    } else {
      painter.drawText(x - 10 - infoPixelsWide, y - 10 - infoPixelsHeight,
                       infoPixelsWide, infoPixelsHeight,
                       Qt::AlignVCenter | Qt::AlignLeft, info);
    }
  }
  painter.end();
}

void BrainGraphView::drawCube(float centerX, float centerY, float centerZ,
                              float size, QColor color, bool withFill) {
  enum {
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
  };

  int face[][4] = {
      {A, B, C, D}, {A, E, F, B}, {B, F, G, C},
      {C, G, H, D}, {D, H, E, A}, {E, F, G, H},
  };

  float half = size / 2.0;
  GLdouble vertexCube[][3] = {
      {-1.0 * half + centerX, -1.0 * half + centerY,
       -1.0 * half + centerZ},                                              // A
      {1.0 * half + centerX, -1.0 * half + centerY, -1.0 * half + centerZ}, // B
      {1.0 * half + centerX, 1.0 * half + centerY, -1.0 * half + centerZ},  // C
      {-1.0 * half + centerX, 1.0 * half + centerY, -1.0 * half + centerZ}, // D
      {-1.0 * half + centerX, -1.0 * half + centerY, 1.0 * half + centerZ}, // E
      {1.0 * half + centerX, -1.0 * half + centerY, 1.0 * half + centerZ},  // F
      {1.0 * half + centerX, 1.0 * half + centerY, 1.0 * half + centerZ},   // G
      {-1.0 * half + centerX, 1.0 * half + centerY, 1.0 * half + centerZ}   // H
  };

  int edge[][2] = {
      {A, B}, {B, C}, {C, D}, {D, A}, {E, F}, {F, G},
      {G, H}, {H, E}, {A, E}, {B, F}, {C, G}, {D, H},
  };

  GLdouble normalCube[][3] = {{0.0, 0.0, -1.0}, {1.0, 0.0, 0.0},
                              {0.0, 0.0, 1.0},  {-1.0, 0.0, 0.0},
                              {0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}};

  if (withFill) {
    glBegin(GL_QUADS);
    for (int i = 0; i < 6; ++i) {
      glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
      glNormal3dv(normalCube[i]);
      glVertex3dv(vertexCube[face[i][0]]);
      glVertex3dv(vertexCube[face[i][1]]);
      glVertex3dv(vertexCube[face[i][2]]);
      glVertex3dv(vertexCube[face[i][3]]);
    }
    glEnd();
  } else {
    glBegin(GL_LINES);
    for (int i = 0; i < 12; ++i) {
      glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
      glVertex3dv(vertexCube[edge[i][0]]);
      glVertex3dv(vertexCube[edge[i][1]]);
    }
    glEnd();
  }
}

void BrainGraphView::drawSphere(float x, float y, float z, float size,
                                QColor color) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glBegin(GL_POLYGON);
    glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    GLUquadricObj *obj = gluNewQuadric();
    gluSphere(obj, size, SPHERE_DIVIDE, SPHERE_DIVIDE);
    glEnd();
    gluDeleteQuadric(obj);
  }
  glPopMatrix();
}

void BrainGraphView::bundleEdges() {
  EdgeBundleConnections *cons;
  cons = new EdgeBundleConnections(rois, detailedMatrix);
  cons->fullAttract();
  bundledEdges = cons->getEdges();
}

void BrainGraphView::mousePressEvent(QMouseEvent *event) {
  mousePressPos = event->pos();
  mouseMovePos = event->pos();
}

void BrainGraphView::mouseMoveEvent(QMouseEvent *event) {
  rois->setHoveredRoiIndex(-1);

  // for rotation
  float sensitiveRatio = 0.5;
  int dx = event->x() - mouseMovePos.x();
  int dy = event->y() - mouseMovePos.y();

  // if (dx + dy > 2) flagPopup = false;

  if (event->buttons() & Qt::LeftButton) {
    rotValue[1] += sensitiveRatio * dy;
    rotValue[2] += sensitiveRatio * dx;
    rois->setHoveredRoiIndex(-1);
  } else if (event->buttons() & Qt::RightButton) {
    rotValue[1] += sensitiveRatio * dy;
    rotValue[0] += sensitiveRatio * dx;
    rois->setHoveredRoiIndex(-1);
  } else if (!event->buttons()) {
    // Hover: mouse picking node
    int selectHits = pick(event->pos().x(), event->pos().y(),
                          1); // 1: pick a node, 2: pick a edge
    if (selectHits != 0) {
      if (selectedObjId[0] == 1) { // 1: name for Nodes
        rois->setHoveredRoiIndex(selectedObjId[1]);
      }
    }
  }

  mouseMovePos = event->pos();
  emit hoveredRoiIndexChanged();
  update();
}

void BrainGraphView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {

  } else if (event->button() == Qt::RightButton) {
    showContextMenu(event->pos());
  }
  mouseReleasePos = event->pos();
}

void BrainGraphView::wheelEvent(QWheelEvent *event) {
  // flagPopup = false;

  float sensitiveRatio = 0.02;

  camDelta[0] += (event->delta() / 8.0 / 15.0) * sensitiveRatio;
  lookAtDelta[0] += (event->delta() / 8.0 / 15.0) * sensitiveRatio;

  mouseWheelPos = event->pos();
  update();
}

int BrainGraphView::pick(int x, int y, int pickType) {
  GLuint selectBuffer[SELECT_BUFFER_SIZE];
  GLint hits;
  GLint vp[4];
  float capOfErr =
      5.0; // capacity of mouse position error, need to change by windowsize

  // glGetIntegerv(GL_VIEWPORT, vp); // this does not work well with qt
  // follows as altenative glGetIntergerv
  vp[0] = 0.0;
  vp[1] = 0.0;
  vp[2] = this->width();
  vp[3] = this->height();

  glSelectBuffer(SELECT_BUFFER_SIZE, selectBuffer);
  glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glLoadIdentity();

  gluPickMatrix(x, vp[3] - y, capOfErr, capOfErr, vp);

  setOrtho(vp[2], vp[3]);

  glMatrixMode(GL_MODELVIEW);
  draw(GL_SELECT, pickType); // pickType: 1 for node, 2 for links

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);

  hits = glRenderMode(GL_RENDER);

  selectHits(hits, selectBuffer, selectedObjId); // obtain selectedObjId
  return hits;
}

int *BrainGraphView::selectHits(GLint hits, const GLuint *buffer,
                                int *hitName) {
  hitName[0] = -1;
  hitName[1] = -1;
  GLuint depth1;
  GLuint depth2;
  GLuint depth = -1;
  GLuint depthName;
  GLuint *p;

  if (hits == 0)
    return hitName;

  p = (GLuint *)buffer;

  for (int i = 0; i < hits; i++) {
    depthName = *p;
    p++;
    depth1 = *p;
    p++;
    depth2 = *p;
    p++;

    if (depth > depth1) {
      depth = depth1;
      for (int j = 0; j < depthName; j++) {
        hitName[j] = *p;
        p++;
      }
    } else {
      for (int j = 0; j < depthName; j++) {
        p++;
      }
    }
  }
  // qDebug() << "p:" << p;
  // qDebug() << "hit name:"<< hitName[0] << "-" << hitName[1];

  return hitName;
}

void BrainGraphView::showContextMenu(const QPoint &pos) {
  QMenu contextMenu(tr("Context menu"), this);
  QAction actSwitchBundleEdge("", this);
  QAction actSwitchColoringMethod("", this);
  QAction actSwitchShowBrainMesh("", this);
  QAction actSwitchShowFrame("", this);

  if (isBundleEdge)
    actSwitchBundleEdge.setText("Unbundle edges");
  else
    actSwitchBundleEdge.setText("Bundle edges");

  if (isEdgeColoringByGroup)
    actSwitchColoringMethod.setText("Color edges based on the value");
  else
    actSwitchColoringMethod.setText("Color edges based on the group");

  if (isShowBrainMesh)
    actSwitchShowBrainMesh.setText("Hide brain image");
  else
    actSwitchShowBrainMesh.setText("Show brain image");

  if (isShowFrame)
    actSwitchShowFrame.setText("Hide frame");
  else
    actSwitchShowFrame.setText("Show frame");

  connect(&actSwitchBundleEdge, &QAction::triggered, this,
          &BrainGraphView::switchBundleEdge);
  connect(&actSwitchColoringMethod, &QAction::triggered, this,
          &BrainGraphView::switchEdgeColoringMethod);
  connect(&actSwitchShowBrainMesh, &QAction::triggered, this,
          &BrainGraphView::switchShowBrainMesh);
  connect(&actSwitchShowFrame, &QAction::triggered, this,
          &BrainGraphView::switchShowFrame);

  contextMenu.addAction(&actSwitchBundleEdge);
  contextMenu.addAction(&actSwitchColoringMethod);
  contextMenu.addAction(&actSwitchShowBrainMesh);
  contextMenu.addAction(&actSwitchShowFrame);

  contextMenu.exec(mapToGlobal(pos));
}

void BrainGraphView::switchBundleEdge() {
  isBundleEdge = !isBundleEdge;
  update();
}

void BrainGraphView::switchEdgeColoringMethod() {
  isEdgeColoringByGroup = !isEdgeColoringByGroup;
  update();
}

void BrainGraphView::switchShowBrainMesh() {
  isShowBrainMesh = !isShowBrainMesh;
  update();
}

void BrainGraphView::switchShowFrame() {
  isShowFrame = !isShowFrame;
  update();
}

void BrainGraphView::setDetailedMatrix(DetailedMatrix *detailedMatrix) {
  this->detailedMatrix = detailedMatrix;
}
void BrainGraphView::setRois(Rois *rois) { this->rois = rois; }
void BrainGraphView::setScannedDataset(ScannedDataset *scannedDataset) {
  this->scannedDataset = scannedDataset;
}
void BrainGraphView::setBrainMesh(BrainMesh *brainMesh) {
  this->brainMesh = brainMesh;
}
