#ifndef THRESHOLDCONTROLVIEW_H
#define THRESHOLDCONTROLVIEW_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>

#include "style.hpp"
#include "detailed_matrix.hpp"
#include "threshold_setting_dialog.hpp"

class ThresholdControlView: public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit ThresholdControlView(DetailedMatrix* detailedMatrix, Style *style, QWidget *parent = 0);
    ~ThresholdControlView();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    DetailedMatrix* detailedMatrix;
    Style* style;

    float sliderStartX;
    float sliderWidth;

    QPoint mousePressPos;
    QPoint mouseMovePos;
    QPoint mouseReleasePos;

    int selectedSliderPointIndex;

    QVector<std::vector<GLfloat>> sliderPointVbos;
    void drawColorMap(float x, float y, float w, float h);
    void drawScale(float x, float y, float w, float h, int numLabelsBetweenMinMax);
    void drawSlider(float x, float y, float w, float h);
    void paintValueLabels(int startX, int startY, int width, int fontSize, int numLabelsBetweenMinMax);
    //void updateSliderPointVbos();

    QPointF qtPosToGlPos(QPoint qtPos);
    bool isInsidePolygon(QPointF point, std::vector<GLfloat> selectionAreaGlBuffer);
    void selectSliderPoints(QPointF mousePosF);
    void changeLowerThresVal(QPointF mousePosF);
    void changeUpperThresVal(QPointF mousePosF);

    void showContextMenu(const QPoint &pos);

private slots:
    void showThresholdSettingDialog();
    void changeThresholdsFromDialog();

signals:
    void thresholdsChanged();
};

#endif // THRESHOLDCONTROLVIEW_H
