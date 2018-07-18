#ifndef DETAILEDMATRIX_H
#define DETAILEDMATRIX_H

#include <vector>
#include <QVector>
#include <QMap>
#include <QPoint>

#include "rois.hpp"

class DetailedMatrix
{
public:
    DetailedMatrix(Rois *rois);
    std::vector<std::vector<float>> getMatrix();
    float getValueAt(int row, int col);
    float getMinThresVal();
    float getMaxThresVal();
    float getLowerThresVal();
    float getUpperThresVal();
    QString getThresRangeType();
    int getDegreeThres();
    int getNumOfCols();
    int getNumOfRows();
    QVector<QVector<bool>> getVisibleMat();
    QVector<QVector<float>> getSizeMat();
    QVector<QVector<bool>> getSelectedMat();
    QVector<QVector<bool>> getFilteredOutMat();
    QVector<QVector<bool>> getUsedInDimReductionMat();
    QPoint getHoveredRowAndColIndex();
    bool getVisibleAt(int row, int col);
    float getSizeAt(int row, int col);
    bool getSelectedAt(int row, int col);
    bool getFilteredOutAt(int row, int col);
    bool getUsedInDimReductionAt(int row, int col);
    int getNumOfSelected();
    int getNumOfFilteredOut();
    int getNumOfUnusedInDimReduction();
    QVector<int> getRowsOrColsIncludesSelectedElements();
    QVector<QPair<int,int>> getRowsAndColsOfSelectedElements();
    QVector<QPair<int,int>> getRowsAndColsUsedInDimReduction();
    std::vector<std::vector<float>> getSubMatrixBasedOnRoiOrder();
    QVector<int> getNodeDegreeFromDisplayedInfo();

    void setMatrix(std::vector<std::vector<float>> matrix);
    void setMinThresVal(float minThresVal);
    void setMaxThresVal(float maxThresVal);
    void setLowerThresVal(float lowerThresVal);
    void setUpperThresVal(float upperThresVal);
    void setThresRangeType(QString thresRangeType);
    void setDegreeThres(int degreeThres);
    void setVisibleMat(QVector<QVector<bool>> visibleMat);
    void setSizeMat(QVector<QVector<float>> sizeMat);
    void setSelectedMat(QVector<QVector<bool>> selectedMat);
    void setFilteredOutMat(QVector<QVector<bool>> filteredOutMat);
    void setUsedInDimReductionMat(QVector<QVector<bool>> usedInDimReductionMat);
    void setHoveredRowAndColIndex(QPoint hoveredRowAndColIndex);
    void setVisibleAt(int row, int col, bool visible);
    void setSizeAt(int row, int col, float size);
    void setSelectedAt(int row, int col, bool selected);
    void setFilteredOutAt(int row, int col, bool filteredOut);
    void setUsedInDimReductionMatAt(int row, int col, bool usedInDimReductionMat);
    void setSelectedMatByRow(int row, bool selected);
    void setSelectedMatByCol(int col, bool selected);
    void setFalseEntireSelectedMat();
    void setFalseEntireFilteredOutMat();
    void setTrueEntireUsedInDimReductionMat();
    void switchSelectedAt(int row, int col);
    void setInvisibleForUnselectedElements();
    void setFilteredOutForUnselectedElements();
    void setUnusedInDimReductionForUnselectedElements();

    void initMatrix(std::vector<std::vector<float>> matrix, float minThresVal, float maxThresVal, float lowerThresVal, float upperThresVal);
    void updateVisibleMat();
    void updateUsedInDimReductionMatWithSubsetRoisInfo();

    void outputSubMatrix(QString filePath);

private:
    Rois *rois;
    std::vector<std::vector<float>> matrix;
    float minThresVal;
    float maxThresVal;
    float lowerThresVal;
    float upperThresVal;
    QString thresRangeType;
    int degreeThres;

    // TODO: maybe these should be separated from this class
    QVector<QVector<bool>> visibleMat;
    QVector<QVector<float>> sizeMat;
    QVector<QVector<bool>> selectedMat;
    QVector<QVector<bool>> filteredOutMat;
    QVector<QVector<bool>> usedInDimReductionMat;

    QPoint hoveredRowAndColIndex;
};

#endif // DETAILEDMATRIX_H
