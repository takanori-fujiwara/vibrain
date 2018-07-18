#include "comparison_view.hpp"

ComparisonView::ComparisonView(Style *style, QWidget *parent)
{
    this->setMouseTracking(true);

    this->style = style;
    scale = 0.95;
    translateX = 0.0;
    translateY = 0.0;
    glyphSize = 0.2;
    isShowMdsErrors = false;
    isShowTimeVaryingLines = true;
    isShowColorMap = false;

    colorMapMinVal = -1.0;
    colorMapMaxVal = 1.0;

    mouseMode = "Lasso";
    rightButtonPressed = false;
    hoveredScannedDataId = -1;
    movingMatrixScannedDataId = -1;
    selectedScannedDataIdsInOrder.clear();

    numDivXForMdsError = 100;
    numDivYForMdsError = 100;
    ratioNeighborForMdsError = 0.05;
    mdsErrorColorMapMinVal = -1.0;
    mdsErrorColorMapMaxVal = -1.0;
    mdsErrorVbo.clear();
    mdsErrorCbo.clear();
}

ComparisonView::~ComparisonView()
{
    //delete style;
    //delete scannedDataset;
    //delete rois;
}

void ComparisonView::initializeGL()
{
    glClearColor(style->bgColor.redF(), style->bgColor.greenF(), style->bgColor.blueF(), style->bgColor.alphaF());

    glMatrixMode(GL_PROJECTION);
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat diffuse[]  = {0.7, 0.7, 0.7, 1.0};
    GLfloat ambient[]  = {0.2, 0.2, 0.2, 1.0};
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

void ComparisonView::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glLoadIdentity();
    update();
}

void ComparisonView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    {
        glScalef(scale, scale, 0.0);
        glTranslatef(translateX, translateY, 0.0);

        if(isShowMdsErrors) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            //drawMdsErrors();
            drawMdsErrors2();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        if(isShowTimeVaryingLines) {
            drawTimeVaryingLines();
        }

        drawPoints();
        drawLinesFromMatrixToPoint();
        drawMatrices(scannedDataset->getVisTargetMatrixKey());
        drawLasso();

        paintButtons();
        paintLegends();
        paintPopup();
    }
    glPopMatrix();
}

void ComparisonView::drawMdsErrors()
{
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible()) {
            //float size = 0.1 * qMax(0.0, (sd->getMdsError() - 0.0)) * 0.1;
            float size = 0.1 * qMax(0.0, (sd->getMdsError() - 0.0)) * qMax(0.0, (sd->getMdsError() - 0.0)) * 0.02 / scale;
            drawRadialColCircle(sd->getPos().x(), sd->getPos().y(), size, style->mdsErrorColorCenter, style->mdsErrorColorPeri);
        }
    }
}

void ComparisonView::drawMdsErrors2()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &mdsErrorVbo[0]);
    glColorPointer(4, GL_FLOAT, 0, &mdsErrorCbo[0]);
    glDrawArrays(GL_QUADS, 0, mdsErrorVbo.size() / 2);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ComparisonView::drawTimeVaryingLines()
{

}

void ComparisonView::drawPoints()
{
    // filltered out points
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getFilteredOut()) {
            QColor fillColor = style->pointColors.at(sd->getColorGroup());
            drawPoint(style->pointShapes.at(sd->getShapeGroup()),
                      sd->getPos().x(), sd->getPos().y(), sd->getSize() / scale,
                      fillColor.darker(300), style->strokeColor, 0.002 / scale);
        }
    }

    // normal points not selected
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && !sd->getFilteredOut() && !sd->getSelected()) {
            drawPoint(style->pointShapes.at(sd->getShapeGroup()),
                      sd->getPos().x(), sd->getPos().y(), sd->getSize() / scale,
                      style->pointColors.at(sd->getColorGroup()), style->strokeColor, 0.002 / scale);
        }
    }

    // selected points
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && !sd->getFilteredOut() && sd->getSelected()) {
            drawPoint(style->pointShapes.at(sd->getShapeGroup()),
                      sd->getPos().x(), sd->getPos().y(), sd->getSize() / scale,
                      style->pointColors.at(sd->getColorGroup()), style->selectedStrokeColor, 0.004 / scale);
        }
    }
}

void ComparisonView::drawLinesFromMatrixToPoint()
{
    // filltered out points
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getMatrixDisplayed() && sd->getFilteredOut()) {
            drawLineFromMatrixToPoint(sd->getMatrixPos(), sd->getPos(), 0.003 / scale, style->lineFromMatrixToPointColor);
        }
    }

    // normal points not selected
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getMatrixDisplayed() && !sd->getFilteredOut() && !sd->getSelected()) {
            drawLineFromMatrixToPoint(sd->getMatrixPos(), sd->getPos(), 0.003 / scale, style->lineFromMatrixToPointColor);
        }
    }

    // selected points
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getMatrixDisplayed() && !sd->getFilteredOut() && sd->getSelected()) {
            drawLineFromMatrixToPoint(sd->getMatrixPos(), sd->getPos(), 0.003 / scale, style->selectedStrokeColor);
        }
    }
}

void ComparisonView::drawMatrices(QString matrixKey)
{
    isShowColorMap = false;

    QVector<int> order;
    if (matrixKey == "ROI-ROI Matrix") {
        order = rois->getOrder();
    }

    // filltered out matrices
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getMatrixDisplayed() && sd->getFilteredOut()) {
            //std::vector<std::vector<float>> mat = sd->getMatrixAt(matrixKey);
            // TODO: here should apply darker color
            drawMatrix(matrixKey, sd->getMatrixAt(matrixKey),
                       sd->getMatrixPos().x(), sd->getMatrixPos().y(), glyphSize / scale,
                       order, style->colormap, style->strokeColor);
            isShowColorMap = true;
        }
    }

    // normal matrices not selected
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getMatrixDisplayed() && !sd->getFilteredOut() && !sd->getSelected()) {
            //std::vector<std::vector<float>> mat = sd->getMatrixAt(matrixKey);
            drawMatrix(matrixKey, sd->getMatrixAt(matrixKey),
                       sd->getMatrixPos().x(), sd->getMatrixPos().y(), glyphSize / scale,
                       order, style->colormap, style->strokeColor);
            isShowColorMap = true;
        }
    }

    // selected matrices
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible() && sd->getMatrixDisplayed() && !sd->getFilteredOut() && sd->getSelected()) {
            //std::vector<std::vector<float>> mat = sd->getMatrixAt(matrixKey);
            drawMatrix(matrixKey, sd->getMatrixAt(matrixKey),
                       sd->getMatrixPos().x(), sd->getMatrixPos().y(), glyphSize / scale,
                       order, style->colormap, style->selectedStrokeColor, 3.0);
            isShowColorMap = true;
        }
    }
}

void ComparisonView::drawLasso()
{
    glColor4f(style->selectedStrokeColor.redF(), style->selectedStrokeColor.greenF(), style->selectedStrokeColor.blueF(), style->selectedStrokeColor.alphaF());
    glLineWidth(1.0 * (float)devicePixelRatio());

    glLineStipple(4, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glVertexPointer(2, GL_FLOAT, 0, &lassoPoints[0]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, lassoPoints.size() / 2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_LINE_STIPPLE);
}

void ComparisonView::paintButtons()
{

}

void ComparisonView::paintLegends()
{
    QPainter painter(this);
    painter.setFont(QFont("San Francisco", 9));
    int fontW = painter.fontMetrics().ascent();
    int fontH = painter.fontMetrics().height();

    int legendStartX = 5;
    int legendStartY = this->height() - 5;

    ///
    /// color legends
    ///
    int colorLegendsWidth = 0;
    if (scannedDataset->getCategoryForColorGroups() != "none") {
        QVector<QString> labels = scannedDataset->getValuesOfCategories()[scannedDataset->getCategoryForColorGroups()];
        legendStartY -= fontH * labels.size();
        for (int i = 0; i < labels.size(); ++i) {
            int labelW = labels.at(i).size() * fontW;
            painter.setBrush(style->pointColors.at(i));
            painter.setPen(style->pointColors.at(i));
            painter.drawRect(legendStartX, legendStartY + fontH / 4, fontH - 2, fontH / 2);
            painter.setPen(Qt::white);
            painter.drawText(legendStartX + fontH, legendStartY, labelW, fontH, Qt::AlignVCenter | Qt::AlignLeft, labels.at(i));
            legendStartY += fontH;
            if (labelW > colorLegendsWidth) colorLegendsWidth = labelW;
        }
        legendStartX += colorLegendsWidth;
    }

    ///
    /// shape legends
    ///
    if (scannedDataset->getCategoryForShapeGroups() != "none") {
        QVector<QString> labels = scannedDataset->getValuesOfCategories()[scannedDataset->getCategoryForShapeGroups()];
        legendStartY -= fontH * labels.size();
        for (int i = 0; i < labels.size(); ++i) {
            int labelW = labels.at(i).size() * fontW;
            painter.setBrush(Qt::white);
            painter.setPen(Qt::white);
            painter.drawPolygon(genLegendShapePoints(legendStartX, legendStartY + fontH / 4, fontH - 2, style->pointShapes.at(i)));
            painter.drawText(legendStartX + fontH, legendStartY, labelW, fontH, Qt::AlignVCenter | Qt::AlignLeft, labels.at(i));
            legendStartY += fontH;
        }
    }

    legendStartX = 5;
    ///
    /// color map
    ///
    if (isShowColorMap) {
        int w = fontW * 20;
        int h = fontH;
        int x = width() - legendStartX - w;
        int y = legendStartY - fontH;

        int numOfSplit = style->colormap.size();
        float wInc = (float)w / (float)numOfSplit;

        for (int i = 0; i < numOfSplit; ++i) {
            QColor color = style->colormap.at(i);
            painter.setBrush(color);
            painter.setPen(color);
            painter.drawRect(x + i * wInc, y, wInc, h);
        }

        painter.setBrush(Qt::white);
        painter.setPen(Qt::white);
        painter.drawText(x - fontW * 2, y - fontH, fontW * 4, fontH, Qt::AlignVCenter | Qt::AlignCenter, QString::number(colorMapMinVal, 'g', 3));
        painter.drawText(x + w - fontW * 2, y - fontH, fontW * 4, fontH, Qt::AlignVCenter | Qt::AlignCenter, QString::number(colorMapMaxVal, 'g', 3));
    }

    ///
    /// color map
    ///
    if (isShowMdsErrors) {
        int w = fontW * 20;
        int h = fontH;
        int x = width() - legendStartX - w;
        int y = legendStartY - fontH;
        if (isShowColorMap) y -= fontH * 2;
        int numOfSplit = style->uncertaintyColormap.size();
        float wInc = (float)w / (float)numOfSplit;

        for (int i = 0; i < numOfSplit; ++i) {
            QColor color = style->uncertaintyColormap.at(i);
            painter.setBrush(color);
            painter.setPen(color);
            painter.drawRect(x + i * wInc, y, wInc, h);
        }

        painter.setBrush(Qt::white);
        painter.setPen(Qt::white);
        painter.drawText(x - fontW * 2, y - fontH, fontW * 4, fontH, Qt::AlignVCenter | Qt::AlignCenter, QString::number(mdsErrorColorMapMinVal, 'g', 3));
        painter.drawText(x + w - fontW * 2, y - fontH, fontW * 4, fontH, Qt::AlignVCenter | Qt::AlignCenter, QString::number(mdsErrorColorMapMaxVal, 'g', 3));
    }

    painter.end();
}

void ComparisonView::paintPopup()
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(QColor(222,222,222,220));

    int x = mouseMovePos.x();
    int y = mouseMovePos.y();

    if (hoveredScannedDataId >= 0) {
        int fontSize = 10;
        painter.setFont(QFont("San Francisco", fontSize));

        // Text Info
        QString info;
        QString longestText = "";
        QStringList keys;
        keys = scannedDataset->getScannedDataAt(hoveredScannedDataId)->getSubIds().keys();
        foreach(QString key, keys) {
            QString text = key + ": " + QString::number(scannedDataset->getScannedDataAt(hoveredScannedDataId)->getSubIdAt(key));
            if (text.size() > longestText.size()) longestText = text;
            info += text + "\n";
        }
        keys = scannedDataset->getScannedDataAt(hoveredScannedDataId)->getCategories().keys();
        foreach(QString key, keys) {
            QString text = key + ": " + scannedDataset->getScannedDataAt(hoveredScannedDataId)->getCategoryAt(key);
            if (text.size() > longestText.size()) longestText = text;
            info += text + "\n";
        }
        keys = scannedDataset->getScannedDataAt(hoveredScannedDataId)->getNumbers().keys();
        foreach(QString key, keys) {
            QString text = key + ": " + QString::number(scannedDataset->getScannedDataAt(hoveredScannedDataId)->getNumberAt(key));
            if (text.size() > longestText.size()) longestText = text;
            info += text + "\n";
        }
        info.chop(1); // remove last \n

        int infoPixelsWide = painter.fontMetrics().width(longestText) + 5;
        int numOfInfo =
                scannedDataset->getScannedDataAt(hoveredScannedDataId)->getNumOfSubIds() +
                scannedDataset->getScannedDataAt(hoveredScannedDataId)->getNumOfCategories() +
                scannedDataset->getScannedDataAt(hoveredScannedDataId)->getNumOfNumbers();
        int infoPixelsHeight = painter.fontMetrics().height() * numOfInfo;

        QPolygon poly;
        int rightSpace  = this->width()  - x;
        int bottomSpace = this->height() - y;

        // Draw Baloon. Position is changed based on spaces
        if(rightSpace  > 10 + infoPixelsWide && bottomSpace > 15 + infoPixelsHeight) {
            poly << QPoint(x + 0,                      y + 0)
                 << QPoint(x + 8,                      y + 3)
                 << QPoint(x + 18 + infoPixelsWide, y + 3)
                 << QPoint(x + 18 + infoPixelsWide, y + 13 + infoPixelsHeight)
                 << QPoint(x + 3,                      y + 13 + infoPixelsHeight)
                 << QPoint(x + 3,                      y + 8);
        }
        else if(rightSpace <= 10 + infoPixelsWide && bottomSpace > 15 + infoPixelsHeight) {
            poly << QPoint(x - 0,                      y + 0)
                 << QPoint(x - 8,                      y + 3)
                 << QPoint(x - 18 - infoPixelsWide, y + 3)
                 << QPoint(x - 18 - infoPixelsWide, y + 13 + infoPixelsHeight)
                 << QPoint(x - 3,                      y + 13 + infoPixelsHeight)
                 << QPoint(x - 3,                      y + 8);
        }
        else if(rightSpace > 10 + infoPixelsWide && bottomSpace <= 15 + infoPixelsHeight) {
            poly << QPoint(x + 0,                      y - 0)
                 << QPoint(x + 8,                      y - 3)
                 << QPoint(x + 18 + infoPixelsWide, y - 3)
                 << QPoint(x + 18 + infoPixelsWide, y - 13 - infoPixelsHeight)
                 << QPoint(x + 3,                      y - 13 - infoPixelsHeight)
                 << QPoint(x + 3,                      y - 8);
        }
        else {
            poly << QPoint(x - 0,                      y - 0)
                 << QPoint(x - 8,                      y - 3)
                 << QPoint(x - 18 - infoPixelsWide, y - 3)
                 << QPoint(x - 18 - infoPixelsWide, y - 13 - infoPixelsHeight)
                 << QPoint(x - 3,                      y - 13 - infoPixelsHeight)
                 << QPoint(x - 3,                      y - 8);
        }
        QPainterPath path;
        path.addPolygon(poly);
        painter.drawPolygon(poly);

        // Draw Text. Changing popup position based on spaces
        if(rightSpace  > 10 + infoPixelsWide && bottomSpace > 15 + infoPixelsHeight) {
            painter.drawText(x + 10, y + 10, infoPixelsWide, infoPixelsHeight,
                             Qt::AlignVCenter | Qt::AlignLeft, info);
        }
        else if(rightSpace <= 10 + infoPixelsWide && bottomSpace > 15 + infoPixelsHeight) {
            painter.drawText(x - 10 - infoPixelsWide, y + 10, infoPixelsWide, infoPixelsHeight,
                             Qt::AlignVCenter | Qt::AlignLeft, info);
        }
        else if(rightSpace > 10 + infoPixelsWide && bottomSpace <= 15 + infoPixelsHeight) {
            painter.drawText(x + 10, y - 10 - infoPixelsHeight, infoPixelsWide, infoPixelsHeight,
                             Qt::AlignVCenter | Qt::AlignLeft, info);
        }
        else {
            painter.drawText(x - 10 - infoPixelsWide, y - 10 - infoPixelsHeight, infoPixelsWide, infoPixelsHeight,
                             Qt::AlignVCenter | Qt::AlignLeft, info);
        }
    }
    painter.end();
}

QPolygonF ComparisonView::genLegendShapePoints(int x, int y, int size, int shapeType)
{
    QVector<QPointF> points;

    int numOfSides = shapeType;
    if (shapeType == 0) {
        numOfSides = 30;
    }

    float twicePi = 2.0 * M_PI;
    float halfPi = 0.5 * M_PI;

    for(int i = 0; i <= numOfSides;i++) {
        points.push_back(QPointF((float)x + (float)size * 0.5 + ((float)size * 0.5 * cos(-halfPi + (float)i * twicePi / (float)numOfSides)),
                                 (float)y + (float)size * 0.5 + ((float)size * 0.5 * sin(-halfPi + (float)i * twicePi / (float)numOfSides))));
    }

    return QPolygonF(points);
}

void ComparisonView::drawRadialColCircle(float x, float y, float radius, QColor colorCenter, QColor colorPeri)
{
    int numOfSides = 30; //# of vertex of triangles used to draw circle
    GLfloat twicePi = 2.0f *  M_PI;

    glBegin(GL_TRIANGLE_FAN);
        glColor4d(colorCenter.redF(), colorCenter.greenF(), colorCenter.blueF(), colorCenter.alphaF());
        glVertex2f(x, y);

        glColor4d(colorPeri.redF(), colorPeri.greenF(), colorPeri.blueF(), colorPeri.alphaF());
        for(int i = 0; i <= numOfSides;i++) {
            glVertex2f(
                x + (radius * cos((float)i * twicePi / (float)numOfSides)),
                y + (radius * sin((float)i * twicePi / (float)numOfSides))
            );
        }
    glEnd();
}

void ComparisonView::drawPoint(int shapeType, float x, float y, float size, QColor fillColor, QColor strokeColor, float strokeWidth)
{
    int numOfSides = shapeType; //# of vertex of triangles used to draw circle
    if (shapeType == 0) {
        numOfSides = 30;
    }

    GLfloat twicePi = 2.0 * M_PI;
    GLfloat halfPi = 0.5 * M_PI;

    glColor4d(strokeColor.redF(), strokeColor.greenF(), strokeColor.blueF(), strokeColor.alphaF());
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for(int i = 0; i <= numOfSides;i++) {
            glVertex2f(
                x + ((size + strokeWidth) * cos(halfPi + (float)i * twicePi / (float)numOfSides)),
                y + ((size + strokeWidth) * sin(halfPi + (float)i * twicePi / (float)numOfSides))
            );
        }
    glEnd();

    glColor4d(fillColor.redF(), fillColor.greenF(), fillColor.blueF(), fillColor.alphaF());
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for(int i = 0; i <= numOfSides; i++) {
            glVertex2f(
                x + (size * cos(halfPi + (float)i * twicePi / (float)numOfSides)),
                y + (size * sin(halfPi + (float)i * twicePi / (float)numOfSides))
            );
        }
    glEnd();
}

void ComparisonView::drawQuad(float x, float y, float w, float h, QColor color)
{
    glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());

    glBegin(GL_QUADS);
    glVertex2d(x,     y);
    glVertex2d(x + w, y);
    glVertex2d(x + w, y + h);
    glVertex2d(x,     y + h);
    glEnd();
}

void ComparisonView::drawLineQuad(float x, float y, float w, float h, float width, QColor color)
{
    glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    glLineWidth(width);
    glBegin(GL_LINE_LOOP);
    glVertex2d(x,     y);
    glVertex2d(x + w, y);
    glVertex2d(x + w, y + h);
    glVertex2d(x,     y + h);
    glEnd();
    // follows needed to paint corner correctly
    glPointSize(width);
    glBegin(GL_POINTS);
    glVertex2d(x,     y);
    glVertex2d(x + w, y);
    glVertex2d(x + w, y + h);
    glVertex2d(x,     y + h);
    glEnd();
}

void ComparisonView::drawMatrix(QString matrixKey, std::vector<std::vector<float>> mat, float x, float y, float size, QVector<int> orderOfMatrix, QVector<QColor> colormap, QColor strokeColor, float strokeWidth)
{
    glPushMatrix();
    {
        glTranslatef(- size / 2.0, size / 2.0, 0.0); // move to origin
        glTranslatef(x, y, 0.0);

        int displayedNumCol = rois->getNumOfNonNegativeRanks();
        if (matrixKey != "ROI-ROI Matrix") displayedNumCol = (int)mat.size();
        float unitSize = size / (float)displayedNumCol;

        for (int i = 0; i < displayedNumCol; ++i) {
            for (int j = 0; j < displayedNumCol; ++j) {
                int i_ = i;
                int j_ = j ;

                if (orderOfMatrix.size() > 0) {
                    i_ = orderOfMatrix.at(i);
                    j_ = orderOfMatrix.at(j);
                }

                if (i_ >= 0 && j_ >= 0) {
                    float val = mat.at(i_).at(j_);
                    float scaledVal = fmin(1.0, fmax(0.0, (val - colorMapMinVal) / (colorMapMaxVal - colorMapMinVal)));

                    QColor color = colormap.at((int)(scaledVal * (colormap.size() - 1)));
                    drawQuad(i * unitSize, - j * unitSize, unitSize, -unitSize, color);
                    drawQuad(j * unitSize, - i * unitSize, unitSize, -unitSize, color);
                }
            }
        }

        drawLineQuad(0, 0, size, - size, strokeWidth, strokeColor);
    }
    glPopMatrix();
}

void ComparisonView::drawLineFromMatrixToPoint(QPointF startPos, QPointF endPos, float width, QColor color)
{
    QLineF vec(startPos, endPos);
    QLineF normVec = vec.normalVector().unitVector();

    glColor4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    glBegin(GL_TRIANGLES);
        glVertex2f(startPos.x() + normVec.dx() * width, startPos.y() + normVec.dy() * width);
        glVertex2f(startPos.x() - normVec.dx() * width, startPos.y() - normVec.dy() * width);
        glVertex2f(endPos.x(), endPos.y());
    glEnd();
}

QPointF ComparisonView::qtPosToGlPos(QPoint qtPos)
{
    float resultX =   (-1.0 + 2.0 * (float)qtPos.x() / (float)this->width()) / scale - translateX;
    float resultY = - (-1.0 + 2.0 * (float)qtPos.y() / (float)this->height()) / scale - translateY;
    return QPointF(resultX, resultY);
}

bool ComparisonView::isInsidePolygon(QPointF point, std::vector<GLfloat> selectionAreaGlBuffer)
{
    bool isInside = false;

    float x = point.x();
    float y = point.y();

    int i;
    int j = selectionAreaGlBuffer.size() - 2;
    for (i = 0; i < (int)selectionAreaGlBuffer.size(); i += 2) {
          if ((selectionAreaGlBuffer.at(i + 1) < y && selectionAreaGlBuffer.at(j + 1) >= y || selectionAreaGlBuffer.at(j + 1) < y && selectionAreaGlBuffer.at(i + 1) >= y)
               && (selectionAreaGlBuffer.at(i) <= x || selectionAreaGlBuffer.at(j) <= x))
          {
              isInside ^=
                      (selectionAreaGlBuffer.at(i) + (y - selectionAreaGlBuffer.at(i + 1)) / (selectionAreaGlBuffer.at(j + 1) - selectionAreaGlBuffer.at(i + 1)) * (selectionAreaGlBuffer.at(j) - selectionAreaGlBuffer.at(i))) < x;
          }
          j = i;
    }

    return isInside;
}

std::vector<GLfloat> ComparisonView::genCirclePoints(float cx, float cy, float r, int numSplit)
{
    std::vector<GLfloat> result;

    int i;

    GLfloat twicePi = 2.0f * M_PI;

    for(i = 0; i <= numSplit; i++) {
        result.push_back(cx + r * cos(i * twicePi / (float)numSplit));
        result.push_back(cy + r * sin(i * twicePi / (float)numSplit));
    }

    return result;
}

void ComparisonView::selectElement(std::vector<GLfloat> selectionAreaGlBuffer)
{
    // reverse for, to select the point in front
    for (int i = scannedDataset->getScannedDataset().size() - 1; i >= 0; --i) {
        if (isInsidePolygon(scannedDataset->getScannedDataAt(i)->getPos(), selectionAreaGlBuffer) ||
                isInsidePolygon(scannedDataset->getScannedDataAt(i)->getMatrixPos(), selectionAreaGlBuffer)) {
            scannedDataset->getScannedDataAt(i)->switchSelected();

            // these are used for calc diff mat
            if (selectedScannedDataIdsInOrder.indexOf(i) >= 0) {
                selectedScannedDataIdsInOrder.removeAt(i);
            } else {
                selectedScannedDataIdsInOrder.push_back(i);
            }
            break;
        }
    }

    emit selectedScannedDataChanged();
}

void ComparisonView::hoverElement(std::vector<GLfloat> selectionAreaGlBuffer)
{
    // reverse for, to select the point in front
    for (int i = scannedDataset->getScannedDataset().size() - 1; i >= 0; --i) {
        if (isInsidePolygon(scannedDataset->getScannedDataAt(i)->getPos(), selectionAreaGlBuffer) ||
                isInsidePolygon(scannedDataset->getScannedDataAt(i)->getMatrixPos(), selectionAreaGlBuffer)) {
            hoveredScannedDataId = scannedDataset->getScannedDataAt(i)->getId();
            break;
        } else {
            hoveredScannedDataId = -1;
        }
    }
}

void ComparisonView::selectMovingMatrixScannedDataId(std::vector<GLfloat> selectionAreaGlBuffer)
{
    // reverse for, to select the point in front
    for (int i = scannedDataset->getScannedDataset().size() - 1; i >= 0; --i) {
        if (scannedDataset->getScannedDataAt(i)->getMatrixDisplayed() && isInsidePolygon(scannedDataset->getScannedDataAt(i)->getMatrixPos(), selectionAreaGlBuffer)) {
            movingMatrixScannedDataId = i;
            hoveredScannedDataId = -1;
            break;
        }
    }
}

void ComparisonView::selectElementsInsideLasso()
{
    for (int i = 0; i < scannedDataset->getScannedDataset().size(); ++i) {
        if (isInsidePolygon(scannedDataset->getScannedDataAt(i)->getPos(), lassoPoints) ||
                isInsidePolygon(scannedDataset->getScannedDataAt(i)->getMatrixPos(), lassoPoints)) {
            scannedDataset->getScannedDataAt(i)->setSelected(true);
            // these are used for calc diff mat
            if (!selectedScannedDataIdsInOrder.contains(i)) selectedScannedDataIdsInOrder.push_back(i);
        }
    }

    emit selectedScannedDataChanged();
}

void ComparisonView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(lassoPoints.size() > 0) {
            lassoPoints.clear();
        }
        QPointF eventPosF = qtPosToGlPos(event->pos());
        std::vector<GLfloat> circlePoints = genCirclePoints(eventPosF.x(), eventPosF.y(), 0.1 / scale);
        selectMovingMatrixScannedDataId(circlePoints);
    } else if (event->button() == Qt::RightButton) {
        rightButtonPressed = true;
    }

    mousePressPos = event->pos();
}

void ComparisonView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;

    if (event->buttons()) {
        if (movingMatrixScannedDataId >= 0) {
            scannedDataset->getScannedDataAt(movingMatrixScannedDataId)->setMatrixPos(qtPosToGlPos(event->pos()));
        } else {
            if (mouseMode == "Hand" || rightButtonPressed) {
                if (mousePosDelta.manhattanLength() > 5) {
                    QPointF glPosCurrent = qtPosToGlPos(event->pos());
                    QPointF glPosPre = qtPosToGlPos(mouseMovePos);
                    translateX += (glPosCurrent - glPosPre).x();
                    translateY += (glPosCurrent - glPosPre).y();
                }
            } else if (mouseMode == "Lasso") {
                QPointF glPos = qtPosToGlPos(event->pos());
                lassoPoints.push_back(glPos.x());
                lassoPoints.push_back(glPos.y());
            }
        }
    } else if (!event->buttons()) {
        // hover
        QPointF eventPosF = qtPosToGlPos(event->pos());
        std::vector<GLfloat> circlePoints = genCirclePoints(eventPosF.x(), eventPosF.y(), 0.02 / scale);
        hoverElement(circlePoints);
    }

    mouseMovePos = event->pos();

    update();
}

void ComparisonView::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint mousePosDelta = event->pos() - mousePressPos;
    QPointF eventPosF = qtPosToGlPos(event->pos());

    if(event->button() == Qt::LeftButton) {
        if (lassoPoints.size() > 1) {
            lassoPoints.push_back(lassoPoints.at(0));
            lassoPoints.push_back(lassoPoints.at(1));
            selectElementsInsideLasso();
        } else {
            if (mousePosDelta.manhattanLength() < 5 && movingMatrixScannedDataId < 0) {
                // TODO: need to change the selection method using vbo to make more accurate selection
                std::vector<GLfloat> circlePoints = genCirclePoints(eventPosF.x(), eventPosF.y(), 0.04 / scale);
                selectElement(circlePoints);
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (mousePosDelta.manhattanLength() < 5) {
            showContextMenu(event->pos());
        }
    }

    rightButtonPressed = false;
    movingMatrixScannedDataId = -1;
    mouseReleasePos = event->pos();

    update();
}

void ComparisonView::wheelEvent(QWheelEvent *event)
{
    float sensitiveRatio = 0.02;
    scale += (event->delta() / 8.0 / 15.0) * sensitiveRatio;
    if(scale < 0.1) {
        scale = 0.1;
    }

    mouseWheelPos = event->pos();

    update();
}

void ComparisonView::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction actClearSelection("Clear selection", this);
    QAction actHideAllMatrices("Hide all matrices", this);
    QAction actShowMatricesOfSelectedPoints("Show matrices on selected points", this);
    QAction actHideMatricesOfSelectedPoints("Hide selected matrices", this);
    QAction actShowMatrixInDetailViews("Show matrix in other views", this);
    QAction actShowAveMatrixInDetailViews("Show averaged matrix in other views", this);
    QAction actShowDiffMatrixInDetailViews("Show diff matrix in other views", this);
    QAction actShowSdMatrixInDetailViews("Show standard deviation matrix in other views", this);
    QAction actShowMaxMinMatrixInDetailViews("Show max - min matrix in other views", this);
    QAction actRecalcMds("Recalculate MDS with selected points", this);
    QAction actSwitchMdsError("", this);
    QAction actChangeMdsErrorSettings("Change MDS uncertainty settings", this);
    QAction actChangeColorMapMinMax("Change Correlation color map min and max values", this);

    if (isShowMdsErrors) actSwitchMdsError.setText("Hide MDS uncertainty");
    else actSwitchMdsError.setText("Show MDS uncertainty");

    connect(&actClearSelection, &QAction::triggered, this, &ComparisonView::clearSelection);
    connect(&actHideAllMatrices, &QAction::triggered, this, &ComparisonView::hideAllMatrices);
    connect(&actShowMatricesOfSelectedPoints, &QAction::triggered, this, &ComparisonView::showMatricesOfSelectedPoints);
    connect(&actHideMatricesOfSelectedPoints, &QAction::triggered, this, &ComparisonView::hideMatricesOfSelectedPoints);
    connect(&actShowMatrixInDetailViews, &QAction::triggered, this, &ComparisonView::showMatrixInDetailViews);
    connect(&actShowAveMatrixInDetailViews, &QAction::triggered, this, &ComparisonView::showAveMatrixInDetailViews);
    connect(&actShowDiffMatrixInDetailViews, &QAction::triggered, this, &ComparisonView::showDiffMatrixInDetailViews);
    connect(&actShowSdMatrixInDetailViews, &QAction::triggered, this, &ComparisonView::showSdMatrixInDetailViews);
    connect(&actShowMaxMinMatrixInDetailViews, &QAction::triggered, this, &ComparisonView::showMaxMinMatrixInDetailViews);
    connect(&actRecalcMds, &QAction::triggered, this, &ComparisonView::recalcMds);
    connect(&actSwitchMdsError, &QAction::triggered, this, &ComparisonView::switchShowMdsErrors);
    connect(&actChangeMdsErrorSettings, &QAction::triggered, this, &ComparisonView::changeMdsErrorSettings);
    connect(&actChangeColorMapMinMax, &QAction::triggered, this, &ComparisonView::changeColorMapMinMax);

    int numOfMatrixDisplayedIndices = scannedDataset->getMatrixDisplayedIndices().size();
    int numOfSelectedIndices = scannedDataset->getSelectedIndices().size();

    // clear selection
    contextMenu.addAction(&actClearSelection);

    // matrix showing in the view
    if (numOfMatrixDisplayedIndices > 0) {
        contextMenu.addAction(&actHideAllMatrices);
        if (numOfSelectedIndices > 0) {
            contextMenu.addAction(&actHideMatricesOfSelectedPoints);
        }
    }
    if (numOfSelectedIndices > 0) contextMenu.addAction(&actShowMatricesOfSelectedPoints);

    // matrix showing in other views
    if (numOfSelectedIndices == 1) {
        contextMenu.addAction(&actShowMatrixInDetailViews);
    } else if (numOfSelectedIndices == 2) {
        contextMenu.addAction(&actShowAveMatrixInDetailViews);
        contextMenu.addAction(&actShowDiffMatrixInDetailViews);
        contextMenu.addAction(&actShowMaxMinMatrixInDetailViews);
    } else if (numOfSelectedIndices > 2) {
        contextMenu.addAction(&actShowAveMatrixInDetailViews);
        contextMenu.addAction(&actShowSdMatrixInDetailViews);
        contextMenu.addAction(&actShowMaxMinMatrixInDetailViews);
    }

    if (numOfSelectedIndices > 2) contextMenu.addAction(&actRecalcMds);

    // visualization settings
    contextMenu.addAction(&actSwitchMdsError);

    if (numOfMatrixDisplayedIndices > 0) {
        contextMenu.addAction(&actChangeColorMapMinMax);
    }
    if (isShowMdsErrors) {
        contextMenu.addAction(&actChangeMdsErrorSettings);
    }

    contextMenu.exec(mapToGlobal(pos));
}

void ComparisonView::clearSelection()
{
    foreach(ScannedData* sd, scannedDataset->getScannedDataset()) {
        sd->setSelected(false);
    }
    lassoPoints.clear();
    selectedScannedDataIdsInOrder.clear();
    update();
}

void ComparisonView::switchShowMdsErrors()
{
    this->isShowMdsErrors = !this->isShowMdsErrors;
    update();
}

void ComparisonView::hideAllMatrices()
{
    foreach(ScannedData* sd, scannedDataset->getScannedDataset()) {
        sd->setMatrixDisplayed(false);
        sd->setMatrixPos(sd->getPos());
    }
    update();
}

void ComparisonView::showMatricesOfSelectedPoints()
{
    foreach(ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getSelected()) sd->setMatrixDisplayed(true);
    }
    update();
}

void ComparisonView::hideMatricesOfSelectedPoints()
{
    foreach(ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getSelected()) {
            sd->setMatrixDisplayed(false);
            sd->setMatrixPos(sd->getPos());
        }
    }
    update();
}

void ComparisonView::showMatrixInDetailViews()
{
    emit showMatrixInDetailViewsSelected();
}

void ComparisonView::showAveMatrixInDetailViews()
{   
    emit showAveMatrixInDetailViewsSelected();
}

void ComparisonView::showDiffMatrixInDetailViews()
{
    int n = selectedScannedDataIdsInOrder.size();
    if (n >= 2) {
        int index1 = selectedScannedDataIdsInOrder.at(n - 2);
        int index2 = selectedScannedDataIdsInOrder.at(n - 1);
        emit showDiffMatrixInDetailViewsSelected(index1, index2);
    }
}

void ComparisonView::showSdMatrixInDetailViews()
{
    emit showSdMatrixInDetailViewsSelected();
}

void ComparisonView::showMaxMinMatrixInDetailViews()
{
    emit showMaxMinMatrixInDetailViewsSelected();
}

void ComparisonView::recalcMds()
{
    lassoPoints.clear();
    emit recalcMdsSelected();
}

void ComparisonView::changeColorMapMinMax()
{
    colorMapMinMaxDialog = new QDialog;
    colorMapMinMaxDialog->setWindowTitle("Correlation Colormap Min and Max Value Setting for Comparison View");
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QHBoxLayout *hBoxLayoutMain = new QHBoxLayout;
    QHBoxLayout *hBoxLayoutCancelOk = new QHBoxLayout;
    colorMapMinMaxDialog->setLayout(vBoxLayout);
    vBoxLayout->addLayout(hBoxLayoutMain);
    vBoxLayout->addLayout(hBoxLayoutCancelOk);

    colorMapMinValLineEdit = new QLineEdit;
    colorMapMinValLineEdit->setText(QString::number(colorMapMinVal));
    colorMapMaxValLineEdit = new QLineEdit;
    colorMapMaxValLineEdit->setText(QString::number(colorMapMaxVal));
    hBoxLayoutMain->addWidget(new QLabel("min"));
    hBoxLayoutMain->addWidget(colorMapMinValLineEdit);
    hBoxLayoutMain->addWidget(new QLabel("max"));
    hBoxLayoutMain->addWidget(colorMapMaxValLineEdit);

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    QPushButton *okBtn = new QPushButton("OK");
    hBoxLayoutCancelOk->addSpacerItem(hSpacer);
    hBoxLayoutCancelOk->addWidget(cancelBtn);
    hBoxLayoutCancelOk->addWidget(okBtn);

    connect(cancelBtn, &QPushButton::clicked, this, &ComparisonView::handleColorMapMinMaxDialogCancelBtn);
    connect(okBtn, &QPushButton::clicked, this, &ComparisonView::handleColorMapMinMaxDialogOkBtn);

    colorMapMinMaxDialog->show();
}

void ComparisonView::changeMdsErrorSettings()
{
    mdsErrorSettingDialog = new QDialog;
    mdsErrorSettingDialog->setWindowTitle("MDS Uncertainty Settings");
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QHBoxLayout *hBoxLayoutMain = new QHBoxLayout;
    QHBoxLayout *hBoxLayoutCancelOk = new QHBoxLayout;
    mdsErrorSettingDialog->setLayout(vBoxLayout);
    vBoxLayout->addLayout(hBoxLayoutMain);
    vBoxLayout->addLayout(hBoxLayoutCancelOk);

    mdsErrorColorMapMinValLineEdit = new QLineEdit;
    mdsErrorColorMapMinValLineEdit->setText(QString::number(mdsErrorColorMapMinVal));
    mdsErrorColorMapMaxValLineEdit = new QLineEdit;
    mdsErrorColorMapMaxValLineEdit->setText(QString::number(mdsErrorColorMapMaxVal));
    ratioNeighborForMdsErrorLineEdit = new QLineEdit;
    ratioNeighborForMdsErrorLineEdit->setText(QString::number(ratioNeighborForMdsError));
    hBoxLayoutMain->addWidget(new QLabel("Color map min"));
    hBoxLayoutMain->addWidget(mdsErrorColorMapMinValLineEdit);
    hBoxLayoutMain->addWidget(new QLabel("Color map max"));
    hBoxLayoutMain->addWidget(mdsErrorColorMapMaxValLineEdit);
    hBoxLayoutMain->addWidget(new QLabel("Ratio of neighbors to be considered"));
    hBoxLayoutMain->addWidget(ratioNeighborForMdsErrorLineEdit);

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    QPushButton *okBtn = new QPushButton("OK");
    hBoxLayoutCancelOk->addSpacerItem(hSpacer);
    hBoxLayoutCancelOk->addWidget(cancelBtn);
    hBoxLayoutCancelOk->addWidget(okBtn);

    connect(cancelBtn, &QPushButton::clicked, this, &ComparisonView::handleMdsErrorSettingDialogCancelBtn);
    connect(okBtn, &QPushButton::clicked, this, &ComparisonView::handleMdsErrorSettingDialogOkBtn);

    mdsErrorSettingDialog->show();
}

void ComparisonView::handleColorMapMinMaxDialogCancelBtn()
{
    colorMapMinMaxDialog->close();
}

void ComparisonView::handleColorMapMinMaxDialogOkBtn()
{
    colorMapMinVal = colorMapMinValLineEdit->text().toFloat();
    colorMapMaxVal = colorMapMaxValLineEdit->text().toFloat();

    colorMapMinMaxDialog->close();

    update();
}

void ComparisonView::handleMdsErrorSettingDialogCancelBtn()
{
    mdsErrorSettingDialog->close();
}

void ComparisonView::handleMdsErrorSettingDialogOkBtn()
{
    mdsErrorColorMapMinVal = mdsErrorColorMapMinValLineEdit->text().toFloat();
    mdsErrorColorMapMaxVal = mdsErrorColorMapMaxValLineEdit->text().toFloat();
    ratioNeighborForMdsError = ratioNeighborForMdsErrorLineEdit->text().toFloat();

    mdsErrorSettingDialog->close();

    updateMdsErrorVboAndCbo();
    update();
}

void ComparisonView::updateMdsErrorVboAndCbo()
{
    mdsErrorVbo.clear();
    mdsErrorCbo.clear();

    QVector<QVector<QPointF>> positions;
    QVector<QVector<QColor>> colors;

    QVector<float> mdsErrors;
    QVector<QPointF> mdsPositions;
    float minMdsError = 1.0 * scannedDataset->getNumOfScannedData();
    float maxMdsError = 0.0;
    foreach (ScannedData* sd, scannedDataset->getScannedDataset()) {
        if (sd->getVisible()) {
            float mdsError = sd->getMdsError();
            if (mdsError < minMdsError) minMdsError = mdsError;
            if (mdsError > maxMdsError) maxMdsError = mdsError;
            mdsErrors.push_back(mdsError);
            mdsPositions.push_back(sd->getPos());
        }
    }

    if (mdsErrorColorMapMinVal < 0) mdsErrorColorMapMinVal = minMdsError;
    if (mdsErrorColorMapMaxVal < 0) mdsErrorColorMapMaxVal = maxMdsError;

    if (mdsErrors.size() != 0 && mdsPositions.size() != 0) {
        ValueInterpolation vi;

        float dx = 2.2 / (float)numDivXForMdsError;
        float dy = 2.2 / (float)numDivYForMdsError;

        // calc interpolated color
        for (int i = 0; i <= numDivXForMdsError; ++i) {
            QVector<QPointF> yPositions;
            QVector<QColor> yColors;

            for (int j = 0; j <= numDivYForMdsError; ++j) {
                QPointF pos(-1.1 + (float)i * dx, -1.1 + (float)j * dy);

                float interpolatedVal = vi.calcWeightedAverageFromPos(mdsErrors, mdsPositions, pos, ratioNeighborForMdsError * mdsErrors.size());

                float scaledVal = 0.0;
                if (mdsErrorColorMapMaxVal - mdsErrorColorMapMinVal != 0) {
                    scaledVal = qMax(0.0, qMin(1.0, (interpolatedVal - mdsErrorColorMapMinVal) / (mdsErrorColorMapMaxVal - mdsErrorColorMapMinVal)));
                }

                QColor color = style->uncertaintyColormap.at((int)(scaledVal * (style->uncertaintyColormap.size() - 1)));

                yPositions.push_back(pos);
                yColors.push_back(color);
            }

            positions.push_back(yPositions);
            colors.push_back(yColors);
        }

        // store vertex info
        for (int i = 0; i < numDivXForMdsError; ++i) {
            for (int j = 0; j < numDivYForMdsError; ++j) {
                mdsErrorVbo.push_back(positions.at(i).at(j).x());
                mdsErrorVbo.push_back(positions.at(i).at(j).y());
                mdsErrorVbo.push_back(positions.at(i+1).at(j).x());
                mdsErrorVbo.push_back(positions.at(i+1).at(j).y());
                mdsErrorVbo.push_back(positions.at(i+1).at(j+1).x());
                mdsErrorVbo.push_back(positions.at(i+1).at(j+1).y());
                mdsErrorVbo.push_back(positions.at(i).at(j+1).x());
                mdsErrorVbo.push_back(positions.at(i).at(j+1).y());

                mdsErrorCbo.push_back(colors.at(i).at(j).redF());
                mdsErrorCbo.push_back(colors.at(i).at(j).greenF());
                mdsErrorCbo.push_back(colors.at(i).at(j).blueF());
                mdsErrorCbo.push_back(colors.at(i).at(j).alphaF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j).redF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j).greenF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j).blueF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j).alphaF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j+1).redF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j+1).greenF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j+1).blueF());
                mdsErrorCbo.push_back(colors.at(i+1).at(j+1).alphaF());
                mdsErrorCbo.push_back(colors.at(i).at(j+1).redF());
                mdsErrorCbo.push_back(colors.at(i).at(j+1).greenF());
                mdsErrorCbo.push_back(colors.at(i).at(j+1).blueF());
                mdsErrorCbo.push_back(colors.at(i).at(j+1).alphaF());
            }
        }
    }
}

void ComparisonView::setScanneDataset(ScannedDataset *scannedDataset) { this->scannedDataset = scannedDataset; }
void ComparisonView::setRois(Rois *rois) { this->rois = rois; }
