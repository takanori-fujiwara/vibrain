#ifndef DIMREDUCTION_H
#define DIMREDUCTION_H

#include <string>
#include <vector>

#include <QDebug>
#include <QPoint>
#include <QVector>

#include "RInside.h"

#include "scanned_dataset.hpp"

class DimReduction {
public:
  DimReduction(QString dimReductionMethod, QString distCalcMethod,
               QString targetMatrixKey);
  void resetDimReduction(QString dimReductionMethod, QString distCalcMethod,
                         QString targetMatrixKey);
  QString getDimReductionMethod();
  QString getDistCalcMethod();
  QString getTargetMatrixKey();
  void setDimReductionMethod(QString dimReductionMethod);
  void setDistCalcMethod(QString distCalcMethod);
  void setTargetMatrixKey(QString targetMatrixKey);
  float calcEuclidDist(std::vector<float> &vec1, std::vector<float> &vec2);
  float calcCosDist(std::vector<float> &vec1, std::vector<float> &vec2);
  float calcCorDist(std::vector<float> &vec1, std::vector<float> &vec2);
  float calcDist(std::vector<float> &vec1, std::vector<float> &vec2,
                 QString distCalcMethod = "euclid");
  std::vector<std::vector<float>> calcDistMatBetweenScannedDataset(
      ScannedDataset *scannedDataset, QVector<int> targetScannedDataIndices,
      QString targetMatrixKey,
      QVector<QPair<int, int>> targetLinks = QVector<QPair<int, int>>(),
      QString distCalcMethod = "euclid");
  void calcMdsPositions(
      ScannedDataset *scannedDataset, QVector<int> targetScannedDataIndices,
      QString targetMatrixKey,
      QVector<QPair<int, int>> targetLinks = QVector<QPair<int, int>>(),
      QString distCalcMethod = "euclid");
  void calcDimReductionPositions(
      ScannedDataset *scannedDataset, QVector<int> targetScannedDataIndices,
      QVector<QPair<int, int>> targetLinks = QVector<QPair<int, int>>());

private:
  QString dimReductionMethod;
  QString distCalcMethod;
  QString targetMatrixKey;
};

#endif // DIMREDUCTION_H
