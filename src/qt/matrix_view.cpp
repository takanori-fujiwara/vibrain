#include "matrix_view.hpp"

MatrixView::MatrixView(Style *style, QWidget *parent) {
  this->setMouseTracking(true);
  this->setCursor(Qt::CrossCursor);

  this->style = style;

  matrixDrawingStartPos = QPointF(0.0, 0.0);
  matrixDrawingSize = 1.0;

  isDragging = false;
}

MatrixView::~MatrixView() {
  // delete style;
  // delete detailedMatrix;
}

void MatrixView::initializeGL() {
  glClearColor(style->bgColor.redF(), style->bgColor.greenF(),
               style->bgColor.blueF(), style->bgColor.alphaF());
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void MatrixView::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  this->update();
}

void MatrixView::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  drawDetailedMatrix(matrixDrawingStartPos.x(), matrixDrawingStartPos.y(),
                     matrixDrawingSize, style->colormap);
  drawDraggedArea();
  drawGroupLines(matrixDrawingStartPos.x(), matrixDrawingStartPos.y(),
                 matrixDrawingSize, style->roiColors);
  drawHoveredRoiLines(matrixDrawingStartPos.x(), matrixDrawingStartPos.y(),
                      matrixDrawingSize);
  paintPopup();
}

void MatrixView::drawDetailedMatrix(float x, float y, float size,
                                    QVector<QColor> colormap,
                                    QColor strokeColor, float strokeWidth) {
  if (detailedMatrix->getNumOfCols() == 0 ||
      detailedMatrix->getNumOfRows() == 0) {
    return;
  }

  glPushMatrix();
  {
    glTranslatef(x, -y, 0.0);

    int displayedNumCol = rois->getNumOfNonNegativeRanks();

    if (detailedMatrix->getNumOfCols() < displayedNumCol ||
        detailedMatrix->getNumOfRows() < displayedNumCol) {
      qWarning() << "detailed matrix row or col size < non-negaive ranks";
      return;
    }

    if (scannedDataset->getVisTargetMatrixKey() != "ROI-ROI Matrix")
      displayedNumCol = detailedMatrix->getNumOfCols();
    float unitSize = size / (float)displayedNumCol;

    for (int i = 0; i < displayedNumCol; ++i) {
      for (int j = 0; j < displayedNumCol; ++j) {
        int i_ = i;
        int j_ = j;

        if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
          i_ = rois->getIndexOnOrder(i);
          j_ = rois->getIndexOnOrder(j);
        }

        if (i_ >= 0 && j_ >= 0) {
          if (detailedMatrix->getVisibleAt(i_, j_) &&
              detailedMatrix->getUsedInDimReductionAt(i_, j_)) {
            float val = detailedMatrix->getValueAt(i_, j_);
            float scaledVal =
                fmin(1.0, fmax(0.0, (val - detailedMatrix->getMinThresVal()) /
                                        (detailedMatrix->getMaxThresVal() -
                                         detailedMatrix->getMinThresVal())));

            QColor color =
                colormap.at((int)(scaledVal * (colormap.size() - 1)));
            if (detailedMatrix->getFilteredOutAt(i_, j_))
              color = color.darker(200);

            drawQuad(i * unitSize, 1.0 - j * unitSize, unitSize, -unitSize,
                     color);
            drawQuad(j * unitSize, 1.0 - i * unitSize, unitSize, -unitSize,
                     color);

            if (detailedMatrix->getSelectedAt(i_, j_)) {
              drawQuad(i * unitSize, 1.0 - j * unitSize, unitSize, -unitSize,
                       style->selectedFillColor);
              drawQuad(j * unitSize, 1.0 - i * unitSize, unitSize, -unitSize,
                       style->selectedFillColor);
              // float lineWidth = 1.0 * devicePixelRatioF();
              // drawLineQuad(i_ * unitSize, 1.0 - j_ * unitSize, unitSize,
              // -unitSize, lineWidth, style->selectedStrokeColor);
              // drawLineQuad(j_ * unitSize, 1.0 - i_ * unitSize, unitSize,
              // -unitSize, lineWidth, style->selectedStrokeColor);
            }
          }
        }
      }
    }

    drawLineQuad(0, 0, size, -size, strokeWidth, strokeColor);
  }
  glPopMatrix();
}

void MatrixView::drawGroupLines(float x, float y, float size,
                                QVector<QColor> groupColors) {
  if (detailedMatrix->getNumOfCols() == 0 ||
      detailedMatrix->getNumOfRows() == 0) {
    return;
  }

  if (scannedDataset->getVisTargetMatrixKey() != "ROI-ROI Matrix")
    return;

  glPushMatrix();
  {
    glTranslatef(x, -y, 0.0);

    int displayedNumCol = rois->getNumOfNonNegativeRanks();

    if (detailedMatrix->getNumOfCols() < displayedNumCol ||
        detailedMatrix->getNumOfRows() < displayedNumCol) {
      qWarning() << "detailed matrix row or col size < non-negaive ranks";
      return;
    }

    float unitSize = size / (float)displayedNumCol;

    QVector<int> moduleSizes;
    QVector<QColor> moduleColors;

    int currentGroup = -1;

    for (int i = 0; i < displayedNumCol; ++i) {
      int i_ = i;
      i_ = rois->getIndexOnOrder(i);

      if (i_ >= 0) {
        int group = rois->getRoiAt(i_)->getGroup();
        if (group != currentGroup) {
          currentGroup = group;
          moduleSizes.push_back(1);
          moduleColors.push_back(groupColors.at(group % groupColors.size()));
        } else {
          moduleSizes.last()++;
        }
      }
    }

    int tmpTotal1 = 0;
    for (int i = 0; i < moduleSizes.size(); ++i) {
      tmpTotal1 += moduleSizes.at(i);

      int tmpTotal2 = 0;
      for (int j = 0; j < moduleSizes.size(); ++j) {
        tmpTotal2 += moduleSizes.at(j);
        QColor color = moduleColors.at(j);

        glLineWidth(2.0 * (float)devicePixelRatio());
        glBegin(GL_LINES);
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
        glVertex2f(tmpTotal1 * unitSize, 1.0 - tmpTotal2 * unitSize);
        glVertex2f(tmpTotal1 * unitSize,
                   1.0 - tmpTotal2 * unitSize + moduleSizes.at(j) * unitSize);
        glVertex2f(tmpTotal2 * unitSize, 1.0 - tmpTotal1 * unitSize);
        glVertex2f(tmpTotal2 * unitSize - moduleSizes[j] * unitSize,
                   1.0 - tmpTotal1 * unitSize);
        glEnd();
      }
    }
  }
  glPopMatrix();
}

void MatrixView::drawDraggedArea() {
  if (isDragging) {
    QPointF startPos = qtPosToGlPos(mousePressPos);
    QPointF endPos = qtPosToGlPos(mouseMovePos);
    float x = fmin(startPos.x(), endPos.x());
    float y = fmin(startPos.y(), endPos.y());
    float w = fabs(startPos.x() - endPos.x());
    float h = fabs(startPos.y() - endPos.y());
    drawLineQuad(x, y, w, h, 1.0 * (float)devicePixelRatio(),
                 style->selectedStrokeColor);
  }
}

void MatrixView::drawHoveredRoiLines(float matrixStartX, float matrixStartY,
                                     float matrixSize) {
  int hoveredRoiIndex = rois->getHoveredRoiIndex();

  if (hoveredRoiIndex >= 0) {
    int rank = rois->getRoiAt(hoveredRoiIndex)->getRank();
    int displayedNumCol = rois->getNumOfNonNegativeRanks();
    if (scannedDataset->getVisTargetMatrixKey() != "ROI-ROI Matrix") {
      displayedNumCol = detailedMatrix->getNumOfCols();
    }

    float unitSize = matrixSize / (float)displayedNumCol;

    float lineWidth = 0.5 * (float)devicePixelRatio();
    glPushMatrix();
    {
      glTranslatef(matrixStartX, -matrixStartY, 0.0);
      drawLineQuad(rank * unitSize, 1.0, unitSize, -1.0, lineWidth,
                   style->selectedStrokeColor);
      drawLineQuad(0.0, 1.0 - rank * unitSize, 1.0, -unitSize, lineWidth,
                   style->selectedStrokeColor);
    }
    glPopMatrix();
  }
}

void MatrixView::paintPopup() {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::black);
  painter.setBrush(QColor(222, 222, 222, 220));

  int x = mouseMovePos.x();
  int y = mouseMovePos.y();
  QPoint hoveredRowAndColIndex = detailedMatrix->getHoveredRowAndColIndex();
  int source = hoveredRowAndColIndex.x();
  int target = hoveredRowAndColIndex.y();

  if (source >= 0 && target >= 0 && detailedMatrix->getNumOfRows() > source &&
      detailedMatrix->getNumOfCols() > target) {
    int fontSize = 10;
    painter.setFont(QFont("San Francisco", fontSize));

    // Text Info
    QString info;
    QString longestText = "";
    QString sourceInfo;
    QString targetInfo;
    if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
      sourceInfo = rois->getRoiAt(source)->getName();
      info += sourceInfo + "\n";
    }
    if (sourceInfo >= longestText)
      longestText = sourceInfo;
    if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
      targetInfo = rois->getRoiAt(target)->getName();
      info += targetInfo + "\n";
    }
    if (targetInfo >= longestText)
      longestText = targetInfo;
    QString valueInfo =
        QString::number(detailedMatrix->getValueAt(source, target));
    info += valueInfo + "\n";
    if (valueInfo >= longestText)
      longestText = valueInfo;
    info.chop(1);

    int infoPixelsWide = painter.fontMetrics().width(longestText) + 10;
    int numOfInfo = 3;
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

void MatrixView::drawQuad(float x, float y, float w, float h, QColor color) {
  glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());

  glBegin(GL_QUADS);
  glVertex2d(x, y);
  glVertex2d(x + w, y);
  glVertex2d(x + w, y + h);
  glVertex2d(x, y + h);
  glEnd();
}

void MatrixView::drawLineQuad(float x, float y, float w, float h, float width,
                              QColor color) {
  glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  glLineWidth(width);
  glBegin(GL_LINE_LOOP);
  glVertex2d(x, y);
  glVertex2d(x + w, y);
  glVertex2d(x + w, y + h);
  glVertex2d(x, y + h);
  glEnd();
  // follows needed to paint corner correctly
  glPointSize(width);
  glBegin(GL_POINTS);
  glVertex2d(x, y);
  glVertex2d(x + w, y);
  glVertex2d(x + w, y - h);
  glVertex2d(x, y - h);
  glEnd();
}

void MatrixView::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {

  } else if (event->button() == Qt::RightButton) {
  }

  mousePressPos = event->pos();
  update();
}

void MatrixView::mouseMoveEvent(QMouseEvent *event) {
  QPoint mousePosDelta = event->pos() - mousePressPos;

  if (event->buttons()) {
    detailedMatrix->setHoveredRowAndColIndex(QPoint(-1, -1));
    if (mousePosDelta.manhattanLength() > 5)
      isDragging = true;
  } else {
    detailedMatrix->setHoveredRowAndColIndex(
        mousePosToRowAndColIndices(event->pos()));
  }

  mouseMovePos = event->pos();
  emit hoveredRowAndColIndexChanged();
  update();
}

void MatrixView::mouseReleaseEvent(QMouseEvent *event) {
  QPoint mousePosDelta = event->pos() - mousePressPos;

  if (event->button() == Qt::LeftButton) {
    if (mousePosDelta.manhattanLength() < 5) {
      selectElement(event->pos());
    } else if (isDragging) {
      selectElementWithRect(mousePressPos, event->pos());
    }
  } else if (event->button() == Qt::RightButton) {
    if (mousePosDelta.manhattanLength() < 5) {
      showContextMenu(event->pos());
    }
  }

  isDragging = false;
  mouseReleasePos = event->pos();
  update();
}

void MatrixView::leaveEvent(QEvent *event) {
  detailedMatrix->setHoveredRowAndColIndex(QPoint(-1, -1));
  emit hoveredRowAndColIndexChanged();
  update();
}

QPointF MatrixView::qtPosToGlPos(QPoint qtPos) {
  float resultX = ((float)qtPos.x() / (float)this->width());
  float resultY = (1.0 - (float)qtPos.y() / (float)this->height());
  return QPointF(resultX, resultY);
}

QPoint MatrixView::mousePosToRowAndColPos(QPoint mousePos) {
  QPoint result;
  float posX = mousePos.x() / (float)width();
  float posY = mousePos.y() / (float)height();

  if (posX < matrixDrawingStartPos.x() || posY < matrixDrawingStartPos.y()) {
    result = QPoint(-1, -1);
  } else if (posX > matrixDrawingStartPos.x() + matrixDrawingSize ||
             posY > matrixDrawingStartPos.y() + matrixDrawingSize) {
    result = QPoint(-1, -1);
  } else {
    float numOfDisplayedRows = rois->getNumOfNonNegativeRanks();
    float numOfDisplayedCols = rois->getNumOfNonNegativeRanks();
    if (scannedDataset->getVisTargetMatrixKey() != "ROI-ROI Matrix") {
      numOfDisplayedRows = detailedMatrix->getNumOfRows();
      numOfDisplayedCols = detailedMatrix->getNumOfCols();
    }
    result = QPoint(numOfDisplayedRows * (posX - matrixDrawingStartPos.x()) /
                        matrixDrawingSize,
                    numOfDisplayedCols * (posY - matrixDrawingStartPos.y()) /
                        matrixDrawingSize);
  }

  return result;
}

QPoint MatrixView::mousePosToRowAndColIndices(QPoint mousePos) {
  QPoint rowAndColPos = mousePosToRowAndColPos(mousePos);
  return QPoint(rois->getIndexOnOrder(rowAndColPos.x()),
                rois->getIndexOnOrder(rowAndColPos.y()));
}

void MatrixView::selectElement(QPoint mousePos) {
  QPoint rowAndColIndices = mousePosToRowAndColIndices(mousePos);
  detailedMatrix->switchSelectedAt(rowAndColIndices.x(), rowAndColIndices.y());
  detailedMatrix->switchSelectedAt(rowAndColIndices.y(), rowAndColIndices.x());
}

void MatrixView::selectElementWithRect(QPoint pos1, QPoint pos2) {
  QPoint rowAndColPos1 = mousePosToRowAndColPos(pos1);
  QPoint rowAndColPos2 = mousePosToRowAndColPos(pos2);

  int startRow = qMin(rowAndColPos1.y(), rowAndColPos2.y());
  int endRow = qMax(rowAndColPos1.y(), rowAndColPos2.y());
  int startCol = qMin(rowAndColPos1.x(), rowAndColPos2.x());
  int endCol = qMax(rowAndColPos1.x(), rowAndColPos2.x());

  for (int i = startCol; i < endCol; ++i) {
    for (int j = startRow; j < endRow; ++j) {
      QPoint rowAndColIndices =
          QPoint(rois->getIndexOnOrder(i), rois->getIndexOnOrder(j));
      detailedMatrix->setSelectedAt(rowAndColIndices.x(), rowAndColIndices.y(),
                                    true);
      detailedMatrix->setSelectedAt(rowAndColIndices.y(), rowAndColIndices.x(),
                                    true);
    }
  }
}

void MatrixView::showContextMenu(const QPoint &pos) {
  QMenu contextMenu(tr("Context menu"), this);
  QAction actClearSelection("Clear selection", this);
  QAction actClearFilteredOut("Reset elements shown in brain graph view", this);
  QAction actClearElementsUsedInDimReduction("Reset elements used in MDS",
                                             this);
  QAction actShowSelectedElementsInBrainGraphView(
      "Show selected elements in brain graph view", this);
  QAction actRecalcMdsWithSelectedElements("Recalc MDS with selected elements",
                                           this);
  QAction actArrangeByHClust("Arrange with Hierarchical Clustering", this);
  QAction actArrangeByModularity("Arrange with Modularity (spends long time)",
                                 this);
  QAction actChangeSubsetRois("Use selected ROIs in analysis", this);
  QAction actResetArrange("Reset arrange", this);
  QAction actSaveFiles("Save matrix files", this);

  connect(&actClearSelection, &QAction::triggered, this,
          &MatrixView::clearSelection);
  connect(&actClearFilteredOut, &QAction::triggered, this,
          &MatrixView::clearFilteredOut);
  connect(&actClearElementsUsedInDimReduction, &QAction::triggered, this,
          &MatrixView::clearElementsUsedInDimReduction);
  connect(&actShowSelectedElementsInBrainGraphView, &QAction::triggered, this,
          &MatrixView::showSelectedElementsInBrainGraphView);
  connect(&actRecalcMdsWithSelectedElements, &QAction::triggered, this,
          &MatrixView::recalcMdsWithSelectedElements);
  connect(&actArrangeByHClust, &QAction::triggered, this,
          &MatrixView::arrangeByHClust);
  connect(&actArrangeByModularity, &QAction::triggered, this,
          &MatrixView::arrangeByModularity);
  connect(&actChangeSubsetRois, &QAction::triggered, this,
          &MatrixView::changeSubsetRois);
  connect(&actResetArrange, &QAction::triggered, this,
          &MatrixView::resetArrange);
  connect(&actSaveFiles, &QAction::triggered, this,
          &MatrixView::saveMatrixFiles);

  int numOfSelected = detailedMatrix->getNumOfSelected();
  int numOfFilteredOut = detailedMatrix->getNumOfFilteredOut();
  int numOfUnusedInDimReduction =
      detailedMatrix->getNumOfUnusedInDimReduction();

  if (numOfUnusedInDimReduction > 0) {
    contextMenu.addAction(&actClearElementsUsedInDimReduction);
  }
  if (numOfFilteredOut > 0) {
    if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
      contextMenu.addAction(&actClearFilteredOut);
    }
  }
  if (numOfSelected > 0) {
    contextMenu.addAction(&actClearSelection);
    if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
      contextMenu.addAction(&actShowSelectedElementsInBrainGraphView);
    }
    contextMenu.addAction(&actRecalcMdsWithSelectedElements);
  }
  if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
    contextMenu.addAction(&actArrangeByHClust);
    contextMenu.addAction(&actArrangeByModularity);
    contextMenu.addAction(&actChangeSubsetRois);
    contextMenu.addAction(&actResetArrange);
  }
  contextMenu.addAction(&actSaveFiles);

  contextMenu.exec(mapToGlobal(pos));
}

void MatrixView::clearSelection() {
  detailedMatrix->setFalseEntireSelectedMat();
  update();
}

void MatrixView::clearFilteredOut() {
  detailedMatrix->setFalseEntireFilteredOutMat();
  update();
  emit detailedMatrixFilteredOutChanged();
}

void MatrixView::clearElementsUsedInDimReduction() {
  detailedMatrix->setTrueEntireUsedInDimReductionMat();
  update();
  emit recalcMdsSelected();
}

void MatrixView::showSelectedElementsInBrainGraphView() {
  detailedMatrix->setFilteredOutForUnselectedElements();
  update();
  emit detailedMatrixFilteredOutChanged();
}

void MatrixView::recalcMdsWithSelectedElements() {
  detailedMatrix->setUnusedInDimReductionForUnselectedElements();
  // rois->setOrdersForSelectedRois(detailedMatrix->getRowsOrColsIncludesSelectedElements());
  update();

  emit recalcMdsSelected();
}

void MatrixView::arrangeByHClust() {
  HierarchicalClustering hc;
  std::vector<std::vector<float>> targetMat =
      detailedMatrix->getSubMatrixBasedOnRoiOrder();

  int numOfCommunities = 1;
  bool ok;
  int val = QInputDialog::getInt(this, tr("QInputDialog::getInt()"),
                                 tr("Number of Clusters:"), 1, 1, 8, 1, &ok);
  if (ok)
    numOfCommunities = val;

  hc.hclust(targetMat, "complete", numOfCommunities);

  // TODO: orders in R and orders in this software mean different things. Need
  // to change the name. QVector<int> orders = hc.orders; QVector<float> heights
  // = hc.heights; QVector<QVector<int>> mergeHistory = hc.mergeHistory;
  QVector<int> clustOrder = hc.order;
  QVector<int> communityNumbers = hc.communityNumbers;
  QVector<int> newOrder;
  foreach (int i, clustOrder) { newOrder.push_back(rois->getOrder().at(i)); }

  // first change group and then order (must be this procedure)
  rois->setGroups(communityNumbers);
  rois->setOrder(newOrder);

  emit matrixOrderChanged();
}

void MatrixView::arrangeByModularity() {
  CommunityDetection cd;
  std::vector<std::vector<float>> targetMat =
      detailedMatrix->getSubMatrixBasedOnRoiOrder();
  cd.louvain(targetMat);
  // int numOfCommunities = cd.numOfCommunities;
  // float modularity = cd.modularity;
  QVector<int> communityNumbers = cd.communityNumbers;
  // TODO: orders in R and orders in this software mean different things. Need
  // to change the name.
  QVector<int> clustOrder = cd.order;
  //    int numOfCommunities = 5;
  //    float modularity = 0.07;
  //    QVector<int> communityNumbers = QVector<int>({1, 0, 1, 1, 1, 0, 1, 0, 1,
  //    1, 1, 1, 1, 1, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1,
  //    1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 2, 1, 1, 2, 2, 0, 0, 2, 3,
  //    3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1,
  //    1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 3, 3, 3, 4, 4, 4,
  //    3, 3, 3, 1, 1, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3,
  //    4, 1, 4, 2, 2, 2, 2}); QVector<int> indicesToOrders = QVector<int>({5,
  //    7, 60, 54, 53, 1, 59, 77, 79, 80, 76, 78, 38, 39, 75, 49, 81, 67, 50,
  //    82, 119, 120, 37, 121, 123, 130, 122, 118, 83, 84, 117, 85, 86, 116,
  //    108, 109, 68, 12, 13, 29, 24, 27, 28, 26, 11, 0, 2, 10, 3, 4, 9, 6, 8,
  //    36, 35, 34, 33, 32, 31, 30, 20, 19, 21, 48, 23, 22, 16, 134, 133, 18,
  //    132, 40, 17, 41, 25, 52, 135, 51, 55, 113, 45, 115, 114, 112, 111, 74,
  //    44, 107, 46, 110, 47, 42, 127, 43, 128, 57, 56, 58, 126, 105, 125, 63,
  //    61, 124, 62, 106, 65, 90, 92, 91, 89, 64, 66, 15, 87, 14, 69, 99, 101,
  //    88, 73, 100, 72, 71, 70, 131, 129, 103, 102, 98, 104, 97, 96, 93, 95,
  //    94});

  QVector<int> newOrder;
  foreach (int i, clustOrder) { newOrder.push_back(rois->getOrder().at(i)); }

  // first change group and then order (must be this procedure)
  rois->setGroups(communityNumbers);
  rois->setOrder(newOrder);

  update();
  emit matrixOrderChanged();
}

void MatrixView::changeSubsetRois() {
  QVector<int> selectedRowsOrCols =
      detailedMatrix->getRowsOrColsIncludesSelectedElements();
  QVector<int> currentOrder = rois->getOrder();
  QVector<int> newOrder;
  for (int i = 0; i < currentOrder.size(); ++i) {
    if (selectedRowsOrCols.contains(currentOrder.at(i)))
      newOrder.push_back(currentOrder.at(i));
  }
  rois->setOrder(newOrder);
  update();
  emit roisSubsetChanged();
}

void MatrixView::resetArrange() {
  rois->resetOrderWithInitialOrder();
  update();
  // emit matrixOrderChanged();
  emit roisSubsetChanged();
}

void MatrixView::saveMatrixFiles() {
  QString dir =
      QFileDialog::getExistingDirectory(this, tr("Select Directory"), "",
                                        QFileDialog::ShowDirsOnly |
                                            QFileDialog::DontResolveSymlinks) +
      "/";
  QString roisCsvFilePath = dir + "/rois.csv";
  QString rowNamesTxtFilePath = dir + "/row_names.txt";
  QString subMatCsvFilePath = dir + "/matrix.csv";

  rois->outputRoisCsv(roisCsvFilePath);
  rois->outputOrderAsNames(rowNamesTxtFilePath);
  detailedMatrix->outputSubMatrix(subMatCsvFilePath);
}

void MatrixView::setDetailedMatrix(DetailedMatrix *detailedMatrix) {
  this->detailedMatrix = detailedMatrix;
}
void MatrixView::setRois(Rois *rois) { this->rois = rois; }
void MatrixView::setScannedDataset(ScannedDataset *scannedDataset) {
  this->scannedDataset = scannedDataset;
}
