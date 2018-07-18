#ifndef STYLE_H
#define STYLE_H

#include <QVector>
#include <QColor>
#include <QFile>
#include <QDir>
#include <QDebug>

class Style
{
public:
    Style(const QString &dirPath);
    void resetStyle(const QString &dirPath);
    QColor bgColor;
    QColor mdsErrorColorCenter;
    QColor mdsErrorColorPeri;
    QColor strokeColor;
    QColor selectedFillColor;
    QColor selectedStrokeColor;
    QColor timeLineColor;
    QColor lineFromMatrixToPointColor;
    QColor brainMeshColor;
    QColor bgColorForControls;

    QVector<QColor> colormap;
    QVector<QColor> uncertaintyColormap;
    QVector<QColor> pointColors;
    QVector<QColor> roiColors;
    QVector<int> pointShapes;

private:
    bool loadStyle(const QString &dirPath);
    bool loadColors(QVector<QColor> &targetColors, const QString &filePath);
    bool loadShapes(QVector<int> &targetShapes, const QString &filePath);
};

#endif // STYLE_H
