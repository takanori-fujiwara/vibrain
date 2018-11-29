#include "threshold_control_view.hpp"

ThresholdControlView::ThresholdControlView(DetailedMatrix *detailedMatrix,
                                           Style *style, QWidget *parent) {
  this->detailedMatrix = detailedMatrix;
  this->style = style;

  sliderStartX = 0.02;
  sliderWidth = 0.96;

  selectedSliderPointIndex = -1;
}

ThresholdControlView::~ThresholdControlView() {}

void ThresholdControlView::initializeGL() {
  glClearColor(
      style->bgColorForControls.redF(), style->bgColorForControls.greenF(),
      style->bgColorForControls.blueF(), style->bgColorForControls.alphaF());
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ThresholdControlView::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  this->update();
}

void ThresholdControlView::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawScale(sliderStartX, 0.25, sliderWidth, 0.5, 3);
  drawColorMap(sliderStartX, 0.33, sliderWidth, 0.34);
  drawSlider(sliderStartX, 0.05, sliderWidth, 0.3);
  paintValueLabels(0, 0, this->width(), 10, 3);
}

void ThresholdControlView::drawColorMap(float x, float y, float w, float h) {
  int numOfSplit = style->colormap.size();
  float wInc = w / (float)numOfSplit;

  for (int i = 0; i < numOfSplit; ++i) {
    QColor color = style->colormap.at(i);
    glBegin(GL_TRIANGLE_FAN);
    glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    glVertex2d(x + i * wInc, y);
    glVertex2d(x + (i + 1) * wInc, y);
    glVertex2d(x + (i + 1) * wInc, y + h);
    glVertex2d(x + i * wInc, y + h);
    glEnd();
  }
}

void ThresholdControlView::drawScale(float x, float y, float w, float h,
                                     int numLabelsBetweenMinMax) {
  float wInc = w / (float)(numLabelsBetweenMinMax + 1);

  glLineWidth(1.0 * (float)devicePixelRatio());
  glBegin(GL_LINES);
  glColor4d(0.0, 0.0, 0.0, 1.0);
  // draw start line
  glVertex2d(x, y);
  glVertex2d(x, y + h);
  // draw end line
  glVertex2d(x + w, y);
  glVertex2d(x + w, y + h);
  glEnd();

  // draw lines between start and end
  for (int i = 0; i < numLabelsBetweenMinMax; ++i) {
    glBegin(GL_LINES);
    glVertex2d(x + wInc * (float)(i + 1), y);
    glVertex2d(x + wInc * (float)(i + 1), y + h);
    glEnd();
  }
}

void ThresholdControlView::drawSlider(float x, float y, float w, float h) {
  sliderPointVbos.clear();
  float minVal = detailedMatrix->getMinThresVal();
  float maxVal = detailedMatrix->getMaxThresVal();
  float lowerThresVal = detailedMatrix->getLowerThresVal();
  float upperThresVal = detailedMatrix->getUpperThresVal();

  float lowerThresValPos = x + w * (lowerThresVal - minVal) / (maxVal - minVal);
  float upperThresValPos = x + w * (upperThresVal - minVal) / (maxVal - minVal);
  float sliderPointSizeX = h * (float)this->height() / (float)this->width();

  // draw line showing displayed range
  if (detailedMatrix->getThresRangeType() == "between") {
    glLineWidth(3.0 * (float)devicePixelRatio());
    glBegin(GL_LINES);
    glColor4d(0.6875, 0.6875, 0.6875, 1.0);
    glVertex2d(x, y + h / 2.0);
    glVertex2d(x + w, y + h / 2.0);
    glColor4d(0.2539, 0.6055, 0.9727, 1.0);
    glVertex2d(lowerThresValPos, y + h / 2.0);
    glVertex2d(upperThresValPos, y + h / 2.0);
    glEnd();
  } else if (detailedMatrix->getThresRangeType() == "outside") {
    glLineWidth(3.0 * (float)devicePixelRatio());
    glBegin(GL_LINES);
    glColor4d(0.2539, 0.6055, 0.9727, 1.0);
    glVertex2d(x, y + h / 2.0);
    glVertex2d(x + w, y + h / 2.0);
    glColor4d(0.6875, 0.6875, 0.6875, 1.0);
    glVertex2d(lowerThresValPos, y + h / 2.0);
    glVertex2d(upperThresValPos, y + h / 2.0);
    glEnd();
  }

  // draw slider points
  foreach (float basePos,
           QVector<float>({lowerThresValPos, upperThresValPos})) {
    std::vector<GLfloat> pos = std::vector<GLfloat>(
        {basePos, y + h, (float)(basePos - 0.5 * sliderPointSizeX),
         (float)(y + h - 0.5 * h), (float)(basePos - 0.5 * sliderPointSizeX),
         (float)(y + h - 1.0 * h), (float)(basePos + 0.5 * sliderPointSizeX),
         (float)(y + h - 1.0 * h), (float)(basePos + 0.5 * sliderPointSizeX),
         (float)(y + h - 0.5 * h)});
    sliderPointVbos.push_back(pos);

    // inner area
    glColor4d(1.0, 1.0, 1.0, 1.0);
    glVertexPointer(2, GL_FLOAT, 0, &pos[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, pos.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);
    // outline
    glLineWidth(1.0 * (float)devicePixelRatio());
    glColor4d(0.71875, 0.71875, 0.71875, 1.0);
    glVertexPointer(2, GL_FLOAT, 0, &pos[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, pos.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);
  }
}

void ThresholdControlView::paintValueLabels(int startX, int startY, int width,
                                            int fontSize,
                                            int numLabelsBetweenMinMax) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  float minVal = detailedMatrix->getMinThresVal();
  float maxVal = detailedMatrix->getMaxThresVal();

  int fontW;
  int fontH;

  painter.setPen(Qt::black);
  painter.setFont(QFont("San Francisco", fontSize));
  fontW = painter.fontMetrics().ascent();
  fontH = painter.fontMetrics().height();
  // label for min
  painter.drawText(startX, startY, fontW * QString::number(minVal).size(),
                   fontH, Qt::AlignTop | Qt::AlignLeft,
                   QString::number(minVal));
  // label for max
  painter.drawText(startX + width - fontW * QString::number(maxVal).size(),
                   startY, fontW * QString::number(maxVal).size(), fontH,
                   Qt::AlignTop | Qt::AlignRight, QString::number(maxVal));

  // label for betwee min and max
  float wInc = (float)width / (float)(numLabelsBetweenMinMax + 1);
  float valInc = (maxVal - minVal) / (float)(numLabelsBetweenMinMax + 1);
  for (int i = 0; i < numLabelsBetweenMinMax; ++i) {
    float val = minVal + valInc * (float)(i + 1);
    painter.drawText(startX + wInc * (i + 1), startY,
                     fontW * QString::number(val).size(), fontH,
                     Qt::AlignTop | Qt::AlignLeft, QString::number(val));
  }
  painter.end();
}

QPointF ThresholdControlView::qtPosToGlPos(QPoint qtPos) {
  float resultX = ((float)qtPos.x() / (float)this->width());
  float resultY = (1.0 - (float)qtPos.y() / (float)this->height());
  return QPointF(resultX, resultY);
}

bool ThresholdControlView::isInsidePolygon(
    QPointF point, std::vector<GLfloat> selectionAreaGlBuffer) {
  bool isInside = false;

  float x = point.x();
  float y = point.y();

  int i;
  int j = selectionAreaGlBuffer.size() - 2;
  for (i = 0; i < (int)selectionAreaGlBuffer.size(); i += 2) {
    if ((selectionAreaGlBuffer.at(i + 1) < y &&
             selectionAreaGlBuffer.at(j + 1) >= y ||
         selectionAreaGlBuffer.at(j + 1) < y &&
             selectionAreaGlBuffer.at(i + 1) >= y) &&
        (selectionAreaGlBuffer.at(i) <= x ||
         selectionAreaGlBuffer.at(j) <= x)) {
      isInside ^=
          (selectionAreaGlBuffer.at(i) +
           (y - selectionAreaGlBuffer.at(i + 1)) /
               (selectionAreaGlBuffer.at(j + 1) -
                selectionAreaGlBuffer.at(i + 1)) *
               (selectionAreaGlBuffer.at(j) - selectionAreaGlBuffer.at(i))) < x;
    }
    j = i;
  }

  return isInside;
}

void ThresholdControlView::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    selectSliderPoints(qtPosToGlPos(event->pos()));
  } else if (event->button() == Qt::RightButton) {
  }

  mousePressPos = event->pos();
}

void ThresholdControlView::mouseMoveEvent(QMouseEvent *event) {
  if (selectedSliderPointIndex == 0) {
    changeLowerThresVal(qtPosToGlPos(event->pos()));
  } else if (selectedSliderPointIndex == 1) {
    changeUpperThresVal(qtPosToGlPos(event->pos()));
  }

  mouseMovePos = event->pos();
}

void ThresholdControlView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {

  } else if (event->button() == Qt::RightButton) {
    showContextMenu(event->pos());
  }

  mouseReleasePos = event->pos();
}

void ThresholdControlView::selectSliderPoints(QPointF mousePosF) {
  for (int i = 0; i < sliderPointVbos.size(); ++i) {
    if (isInsidePolygon(mousePosF, sliderPointVbos.at(i))) {
      selectedSliderPointIndex = i;
      break;
    }
  }
}

void ThresholdControlView::changeLowerThresVal(QPointF mousePosF) {
  float minVal = detailedMatrix->getMinThresVal();
  float maxVal = detailedMatrix->getMaxThresVal();
  float lowerThresVal = detailedMatrix->getLowerThresVal();
  float upperThresVal = detailedMatrix->getUpperThresVal();

  if (mousePosF.x() < sliderStartX) {
    lowerThresVal = minVal;
  } else if (mousePosF.x() > sliderStartX + sliderWidth) {
    lowerThresVal = maxVal;
  } else {
    lowerThresVal =
        ((mousePosF.x() - sliderStartX) / sliderWidth) * (maxVal - minVal) +
        minVal;
  }

  if (lowerThresVal > upperThresVal)
    lowerThresVal = upperThresVal;

  detailedMatrix->setLowerThresVal(lowerThresVal);
  emit thresholdsChanged();
  // update();
}

void ThresholdControlView::changeUpperThresVal(QPointF mousePosF) {
  float minVal = detailedMatrix->getMinThresVal();
  float maxVal = detailedMatrix->getMaxThresVal();
  float lowerThresVal = detailedMatrix->getLowerThresVal();
  float upperThresVal = detailedMatrix->getUpperThresVal();

  if (mousePosF.x() < sliderStartX) {
    upperThresVal = minVal;
  } else if (mousePosF.x() > sliderStartX + sliderWidth) {
    upperThresVal = maxVal;
  } else {
    upperThresVal =
        ((mousePosF.x() - sliderStartX) / sliderWidth) * (maxVal - minVal) +
        minVal;
  }

  if (upperThresVal < lowerThresVal)
    upperThresVal = lowerThresVal;

  detailedMatrix->setUpperThresVal(upperThresVal);
  emit thresholdsChanged();
  // update();
}

void ThresholdControlView::showContextMenu(const QPoint &pos) {
  QMenu contextMenu(tr("Context menu"), this);

  QAction actShowThresholdSettingDialog("Show threshold setting dialog", this);
  connect(&actShowThresholdSettingDialog, &QAction::triggered, this,
          &ThresholdControlView::showThresholdSettingDialog);
  contextMenu.addAction(&actShowThresholdSettingDialog);

  contextMenu.exec(mapToGlobal(pos));
}

void ThresholdControlView::showThresholdSettingDialog() {
  ThresholdSettingDialog *thresholdSettingDialog =
      new ThresholdSettingDialog(detailedMatrix);
  connect(thresholdSettingDialog, &ThresholdSettingDialog::thresholdsChanged,
          this, &ThresholdControlView::changeThresholdsFromDialog);
  thresholdSettingDialog->show();
}

void ThresholdControlView::changeThresholdsFromDialog() {
  emit thresholdsChanged();
}
