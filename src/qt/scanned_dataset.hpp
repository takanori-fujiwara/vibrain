#ifndef SCANNEDDATASET_H
#define SCANNEDDATASET_H

#include <cmath>

#include <QString>
#include <QDir>
#include <QFile>
#include <QDebug>

#include "scanned_data.hpp"

class ScannedDataset
{
public:
    ScannedDataset(const QString &scanndeDataDir);
    ~ScannedDataset();
    void resetScannedDataset(const QString &scanndeDataDir);
    QVector<ScannedData*> getScannedDataset();
    ScannedData* getScannedDataAt(int pos);
    ScannedData* getScannedDataFromFileName(QString fileName);
    int getNumOfScannedData();
    QVector<int> getVisibleIndices();
    QVector<int> getSelectedIndices();
    QVector<int> getMatrixDisplayedIndices();
    QMap<QString, QVector<QString>> getValuesOfCategories();
    QMap<QString, QVector<float>> getMinAndMaxOfNumbers();
    QString getCategoryForColorGroups();
    QString getCategoryForShapeGroups();
    QString getNumberForSizes();
    QString getVisTargetMatrixKey();
    void setFilteredOutWithFilterStatus(QMap<QString, QMap<QString, bool>> statusOfCategoryFilterCheckBoxes, QMap<QString, QPair<QString, QString>> statusOfNumberFilterOperatorsAndValues);
    void setVisibleWithFiltering();
    void setVisibleFromSelected();
    void setScannedDataColorGroupsFromCategoryKey(QString key);
    void setScannedDataShapeGroupsFromCategoryKey(QString key);
    void setScannedDataSizesFromNumberKey(QString key);
    void setVisTargetMatrixKey(QString visTargetMatrixKey);

    std::vector<std::vector<float>> genAveMatFromSelectedData();
    std::vector<std::vector<float>> genDiffMatFromSelectedData(int index1, int index2);
    std::vector<std::vector<float>> genSdMatFromSelectedData();
    std::vector<std::vector<float>> genMaxMinMatFromSelectedData();

private:
    QVector<ScannedData*> scannedDataset;
    QMap<QString, QVector<QString>> valuesOfCategories;
    QMap<QString, QVector<float>> minAndMaxOfNumbers;
    QString categoryForColorGroups;
    QString categoryForShapeGroups;
    QString numberForSizes;
    QString visTargetMatrixKey;

    bool loadScannedDataset(const QString &dirPath);
    int checkAllScannedDataHaveSameNumAttr();
    bool ifWithQStringOperator(float leftVal, QString ope, float rightVal);
};

#endif // SCANNEDDATASET_H
