#include "style.hpp"

Style::Style(const QString &dirPath)
{
    bgColor = QColor(24,26,31,255);
    mdsErrorColorCenter = QColor(100,100,100,20);
    mdsErrorColorPeri = QColor(100,100,100,4);
    strokeColor = QColor(0,0,0,255);
    selectedFillColor = QColor(150,200,150,50);
    selectedStrokeColor = QColor(80,80,255);
    timeLineColor = QColor(229,196,148,80);
    lineFromMatrixToPointColor = QColor(180,184,193,255);
    brainMeshColor = QColor(255,255,255,8);
    bgColorForControls = QColor(237,237,237,255);
    loadStyle(dirPath);
}

void Style::resetStyle(const QString &dirPath)
{
    bgColor = QColor(24,26,31,255);
    mdsErrorColorCenter = QColor(100,100,100,20);
    mdsErrorColorPeri = QColor(100,100,100,4);
    strokeColor = QColor(0,0,0,255);
    selectedFillColor = QColor(150,200,150,50);
    selectedStrokeColor = QColor(80,80,255);
    timeLineColor = QColor(229,196,148,80);
    lineFromMatrixToPointColor = QColor(180,184,193,255);
    brainMeshColor = QColor(255,255,255,8);
    bgColorForControls = QColor(237,237,237,255);
    loadStyle(dirPath);
}

bool Style::loadStyle(const QString &dirPath)
{
    QString corColormapFilePath = dirPath + "correlation_colormap.csv";
    QString uncColormapFilePath = dirPath + "uncertainty_colormap.csv";
    QString pointColorsFilePath = dirPath + "point_colors.csv";
    QString pointShapesFilePath = dirPath + "point_shapes.csv";
    QString roiColorsFilePath = dirPath + "group_colors.csv";

    if (!loadColors(colormap, corColormapFilePath)) return false;
    if (!loadColors(uncertaintyColormap, uncColormapFilePath)) return false;
    if (!loadColors(pointColors, pointColorsFilePath)) return false;
    if (!loadColors(roiColors, roiColorsFilePath)) return false;
    if (!loadShapes(pointShapes, pointShapesFilePath)) return false;

    return true;
}

bool Style::loadColors(QVector<QColor> &targetColors, const QString &filePath)
{
    targetColors.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open color file. file path:" << filePath;
        return false;
    }

    QTextStream ts(&file);
    ts.readLine(); // ignore header

    int lineNum = 2;
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        QStringList elements = line.split(",");

        if (elements.size() < 4) {
            qWarning() << "there are incorrect number of values in color file (need 4 numbers) in line: " << lineNum;
            return false;
        }

        targetColors.push_back(QColor(elements.at(0).toInt(), elements.at(1).toInt(), elements.at(2).toInt(), elements.at(3).toInt()));

        lineNum++;
    }

    return true;
}

bool Style::loadShapes(QVector<int> &targetShapes, const QString &filePath)
{
    targetShapes.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open color file. file path:" << filePath;
        return false;
    }

    QTextStream ts(&file);
    ts.readLine(); // ignore header

    int lineNum = 2;
    while (!ts.atEnd()) {
        QString line = ts.readLine();
        QStringList elements = line.split(",");

        if (elements.size() < 1) {
            qWarning() << "there are incorrect number of values in shape file (need 1 number) in line: " << lineNum;
            return false;
        }

        targetShapes.push_back(elements.at(0).toInt());

        lineNum++;
    }

    return true;
}
