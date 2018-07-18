#ifndef COMPARISONVIEW_H
#define COMPARISONVIEW_H

#include <math.h>
#include <vector>

#include <QOpenGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QDebug>

#include "style.hpp"
#include "scanned_dataset.hpp"
#include "rois.hpp"
#include "value_interpolation.hpp"

class ComparisonView: public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit ComparisonView(Style *style, QWidget *parent = 0);
    ~ComparisonView();
    void setScanneDataset(ScannedDataset *scannedDataset);
    void setRois(Rois *rois);
    void updateMdsErrorVboAndCbo();

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
    ScannedDataset *scannedDataset;
    Rois *rois;

    float scale;
    float translateX;
    float translateY;
    float glyphSize;
    bool isShowMdsErrors;
    bool isShowTimeVaryingLines;
    bool isShowColorMap;

    // TODO: these variables maight be separated from here
    float colorMapMinVal;
    float colorMapMaxVal;
    QDialog *colorMapMinMaxDialog;
    QLineEdit *colorMapMinValLineEdit;
    QLineEdit *colorMapMaxValLineEdit;

    QString mouseMode;
    bool rightButtonPressed;
    QPoint mousePressPos;
    QPoint mouseMovePos;
    QPoint mouseReleasePos;
    QPoint mouseWheelPos;

    std::vector<GLfloat> lassoPoints;
    int hoveredScannedDataId;
    int movingMatrixScannedDataId;
    // This is used for calc diff matrix (need to consider the order);
    QList<int> selectedScannedDataIdsInOrder;

    // TODO: add below things and vbo later to make rendering fast
    // QVector<int> visibleIndices;

    int numDivXForMdsError;
    int numDivYForMdsError;
    float ratioNeighborForMdsError;
    double mdsErrorColorMapMinVal;
    double mdsErrorColorMapMaxVal;
    QDialog *mdsErrorSettingDialog;
    QLineEdit *mdsErrorColorMapMinValLineEdit;
    QLineEdit *mdsErrorColorMapMaxValLineEdit;
    QLineEdit *ratioNeighborForMdsErrorLineEdit;

    std::vector<GLfloat> mdsErrorVbo;
    std::vector<GLfloat> mdsErrorCbo;

    void drawMdsErrors();
    void drawMdsErrors2();
    void drawTimeVaryingLines();
    void drawPoints();
    void drawLinesFromMatrixToPoint();
    void drawMatrices(QString matrixKey);
    void drawLasso();
    void paintButtons();
    void paintLegends();
    void paintPopup();

    void drawRadialColCircle(float x, float y, float radius, QColor colorCenter, QColor colorPeri);
    void drawPoint(int shapeType, float x, float y, float size, QColor fillColor, QColor strokeColor, float strokeWidth);
    void drawQuad(float x, float y, float w, float h, QColor color);
    void drawLineQuad(float x, float y, float w, float h, float width, QColor color);
    void drawMatrix(QString matrixKey, std::vector<std::vector<float>> mat, float x, float y, float size, QVector<int> orderOfMatrix, QVector<QColor> colormap, QColor strokeColor, float strokeWidth = 1.0);
    void drawLineFromMatrixToPoint(QPointF startPos, QPointF endPos, float width, QColor color);


    QPolygonF genLegendShapePoints(int x, int y, int size, int shapeType);
    QPointF qtPosToGlPos(QPoint qtPos);
    bool isInsidePolygon(QPointF point, std::vector<GLfloat> selectionAreaGlBuffer);

    std::vector<GLfloat> genCirclePoints(float cx, float cy, float r, int numSplit = 6);
    void selectElement(std::vector<GLfloat> selectionAreaGlBuffer);
    void hoverElement(std::vector<GLfloat> selectionAreaGlBuffer);
    void selectMovingMatrixScannedDataId(std::vector<GLfloat> selectionAreaGlBuffer);
    void selectElementsInsideLasso();

    void showContextMenu(const QPoint &pos);

private slots:
    void clearSelection();
    void switchShowMdsErrors();
    void hideAllMatrices();
    void showMatricesOfSelectedPoints();
    void hideMatricesOfSelectedPoints();
    void showMatrixInDetailViews();
    void showAveMatrixInDetailViews();
    void showDiffMatrixInDetailViews();
    void showSdMatrixInDetailViews();
    void showMaxMinMatrixInDetailViews();
    void recalcMds();
    void changeColorMapMinMax();
    void handleColorMapMinMaxDialogCancelBtn();
    void handleColorMapMinMaxDialogOkBtn();
    void changeMdsErrorSettings();
    void handleMdsErrorSettingDialogCancelBtn();
    void handleMdsErrorSettingDialogOkBtn();

signals:
    void showMatrixInDetailViewsSelected();
    void showAveMatrixInDetailViewsSelected();
    void showDiffMatrixInDetailViewsSelected(int index1, int index2);
    void showSdMatrixInDetailViewsSelected();
    void showMaxMinMatrixInDetailViewsSelected();
    void recalcMdsSelected();
    void selectedScannedDataChanged();
};

#endif // COMPARISONVIEW_H
