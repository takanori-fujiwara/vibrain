#include "detailed_matrix.hpp"

DetailedMatrix::DetailedMatrix(Rois *rois) {
  this->rois = rois;
  matrix.clear();
  minThresVal = maxThresVal = lowerThresVal = upperThresVal = 0.0;
  thresRangeType = "between";
  degreeThres = 1;
  hoveredRowAndColIndex = QPoint(-1, -1);
  usedInDimReductionMat = QVector<QVector<bool>>(
      rois->getRois().size(),
      QVector<bool>(rois->getRois().size(), true)); // test
}

void DetailedMatrix::updateVisibleMat() {
  QVector<QVector<bool>> visibleMat;
  for (int i = 0; i < (int)matrix.size(); ++i) {
    QVector<bool> visibleRow;
    for (int j = 0; j < (int)matrix.at(i).size(); ++j) {
      if (thresRangeType == "between") {
        if (matrix.at(i).at(j) >= lowerThresVal &&
            matrix.at(i).at(j) <= upperThresVal)
          visibleRow.push_back(true);
        else
          visibleRow.push_back(false);
      } else if (thresRangeType == "outside") {
        if (matrix.at(i).at(j) <= lowerThresVal ||
            matrix.at(i).at(j) >= upperThresVal)
          visibleRow.push_back(true);
        else
          visibleRow.push_back(false);
      }
    }
    visibleMat.push_back(visibleRow);
  }
  this->visibleMat = visibleMat;
}

void DetailedMatrix::updateUsedInDimReductionMatWithSubsetRoisInfo() {
  for (int i = 0; i < (int)usedInDimReductionMat.size(); ++i) {
    for (int j = 0; j < (int)usedInDimReductionMat.at(i).size(); ++j) {
      if (rois->getRoiAt(i)->getRank() < 0 ||
          rois->getRoiAt(j)->getRank() < 0) {
        usedInDimReductionMat[i][j] = false;
      } else {
        usedInDimReductionMat[i][j] = true;
      }
    }
  }
}

std::vector<std::vector<float>> DetailedMatrix::getMatrix() { return matrix; }
float DetailedMatrix::getValueAt(int row, int col) {
  return matrix.at(row).at(col);
}
float DetailedMatrix::getMinThresVal() { return minThresVal; }
float DetailedMatrix::getMaxThresVal() { return maxThresVal; }
float DetailedMatrix::getLowerThresVal() { return lowerThresVal; }
float DetailedMatrix::getUpperThresVal() { return upperThresVal; }
QString DetailedMatrix::getThresRangeType() { return thresRangeType; }
int DetailedMatrix::getDegreeThres() { return degreeThres; }
int DetailedMatrix::getNumOfRows() { return (int)matrix.size(); }
int DetailedMatrix::getNumOfCols() {
  if ((int)matrix.size() == 0)
    return 0;
  else
    return matrix.at(0).size();
}
QVector<QVector<bool>> DetailedMatrix::getVisibleMat() { return visibleMat; }
QVector<QVector<float>> DetailedMatrix::getSizeMat() { return sizeMat; }
QVector<QVector<bool>> DetailedMatrix::getSelectedMat() { return selectedMat; }
QVector<QVector<bool>> DetailedMatrix::getFilteredOutMat() {
  return filteredOutMat;
}
QVector<QVector<bool>> DetailedMatrix::getUsedInDimReductionMat() {
  return usedInDimReductionMat;
}
QPoint DetailedMatrix::getHoveredRowAndColIndex() {
  return hoveredRowAndColIndex;
}
bool DetailedMatrix::getVisibleAt(int row, int col) {
  return visibleMat.at(row).at(col);
}
float DetailedMatrix::getSizeAt(int row, int col) {
  return sizeMat.at(row).at(col);
}
bool DetailedMatrix::getSelectedAt(int row, int col) {
  return selectedMat.at(row).at(col);
}
bool DetailedMatrix::getFilteredOutAt(int row, int col) {
  return filteredOutMat.at(row).at(col);
}
bool DetailedMatrix::getUsedInDimReductionAt(int row, int col) {
  return usedInDimReductionMat.at(row).at(col);
}
void DetailedMatrix::setMatrix(std::vector<std::vector<float>> matrix) {
  this->matrix = matrix;
}
void DetailedMatrix::setMinThresVal(float minThresVal) {
  this->minThresVal = minThresVal;
}
void DetailedMatrix::setMaxThresVal(float maxThresVal) {
  this->maxThresVal = maxThresVal;
}
void DetailedMatrix::setLowerThresVal(float lowerThresVal) {
  this->lowerThresVal = lowerThresVal;
}
void DetailedMatrix::setUpperThresVal(float upperThresVal) {
  this->upperThresVal = upperThresVal;
}
void DetailedMatrix::setThresRangeType(QString thresRangeType) {
  this->thresRangeType = thresRangeType;
}
void DetailedMatrix::setDegreeThres(int degreeThres) {
  this->degreeThres = degreeThres;
}
void DetailedMatrix::setVisibleMat(QVector<QVector<bool>> visibleMat) {
  this->visibleMat = visibleMat;
}
void DetailedMatrix::setSizeMat(QVector<QVector<float>> sizeMat) {
  this->sizeMat = sizeMat;
}
void DetailedMatrix::setSelectedMat(QVector<QVector<bool>> selectedMat) {
  this->selectedMat = selectedMat;
}
void DetailedMatrix::setFilteredOutMat(QVector<QVector<bool>> filteredOutMat) {
  this->filteredOutMat = filteredOutMat;
}
void DetailedMatrix::setUsedInDimReductionMat(
    QVector<QVector<bool>> usedInDimReductionMat) {
  this->usedInDimReductionMat = usedInDimReductionMat;
}
void DetailedMatrix::setHoveredRowAndColIndex(QPoint hoveredRowAndColIndex) {
  this->hoveredRowAndColIndex = hoveredRowAndColIndex;
}
void DetailedMatrix::setVisibleAt(int row, int col, bool visible) {
  visibleMat[row][col] = visible;
}
void DetailedMatrix::setSizeAt(int row, int col, float size) {
  sizeMat[row][col] = size;
}
void DetailedMatrix::setSelectedAt(int row, int col, bool selected) {
  selectedMat[row][col] = selected;
}
void DetailedMatrix::setFilteredOutAt(int row, int col, bool filteredOut) {
  filteredOutMat[row][col] = filteredOut;
}
void DetailedMatrix::setUsedInDimReductionMatAt(int row, int col,
                                                bool usedInDimReduction) {
  usedInDimReductionMat[row][col] = usedInDimReduction;
}
void DetailedMatrix::switchSelectedAt(int row, int col) {
  selectedMat[row][col] = !selectedMat.at(row).at(col);
}

int DetailedMatrix::getNumOfSelected() {
  int result = 0;
  foreach (QVector<bool> col, selectedMat) {
    foreach (bool selected, col) {
      if (selected)
        result++;
    }
  }
  return result;
}

int DetailedMatrix::getNumOfFilteredOut() {
  int result = 0;
  foreach (QVector<bool> col, filteredOutMat) {
    foreach (bool filteredOut, col) {
      if (filteredOut)
        result++;
    }
  }
  return result;
}

int DetailedMatrix::getNumOfUnusedInDimReduction() {
  int result = 0;
  foreach (QVector<bool> col, usedInDimReductionMat) {
    foreach (bool usedInDimReduction, col) {
      if (!usedInDimReduction)
        result++;
    }
  }
  return result;
}

QVector<int> DetailedMatrix::getRowsOrColsIncludesSelectedElements() {
  QSet<int> rowsOrColsIncludesSelectedElements;

  for (int i = 0; i < selectedMat.size(); ++i) {
    for (int j = 0; j < selectedMat.at(i).size(); ++j) {
      if (selectedMat.at(i).at(j)) {
        rowsOrColsIncludesSelectedElements.insert(i);
        rowsOrColsIncludesSelectedElements.insert(j);
      }
    }
  }

  return rowsOrColsIncludesSelectedElements.toList().toVector();
}

QVector<QPair<int, int>> DetailedMatrix::getRowsAndColsOfSelectedElements() {
  QVector<QPair<int, int>> result;

  for (int i = 0; i < selectedMat.size(); ++i) {
    for (int j = 0; j < selectedMat.at(i).size(); ++j) {
      if (selectedMat.at(i).at(j)) {
        result.push_back(qMakePair(i, j));
      }
    }
  }

  return result;
}

QVector<QPair<int, int>> DetailedMatrix::getRowsAndColsUsedInDimReduction() {
  QVector<QPair<int, int>> result;

  for (int i = 0; i < usedInDimReductionMat.size(); ++i) {
    for (int j = 0; j < usedInDimReductionMat.at(i).size(); ++j) {
      if (usedInDimReductionMat.at(i).at(j)) {
        result.push_back(qMakePair(i, j));
      }
    }
  }

  return result;
}

std::vector<std::vector<float>> DetailedMatrix::getSubMatrixBasedOnRoiOrder() {
  std::vector<std::vector<float>> result;

  foreach (int i, rois->getOrder()) {
    std::vector<float> row;
    foreach (int j, rois->getOrder()) { row.push_back(matrix.at(i).at(j)); }
    result.push_back(row);
  }

  return result;
}

QVector<int> DetailedMatrix::getNodeDegreeFromDisplayedInfo() {
  QVector<int> result;

  for (int i = 0; i < visibleMat.size(); ++i) {
    int degree = 0;
    for (int j = 0; j < visibleMat.at(i).size(); ++j) {
      degree += (int)visibleMat.at(i).at(j) *
                (int)usedInDimReductionMat.at(i).at(j) *
                (1 - (int)filteredOutMat.at(i).at(j));
    }
    result.push_back(degree);
  }

  return result;
}

void DetailedMatrix::setSelectedMatByRow(int row, bool selected) {
  if (selectedMat.size() > row) {
    for (int i = 0; i < selectedMat.at(row).size(); ++i) {
      selectedMat[row][i] = selected;
    }
  }
}

void DetailedMatrix::setSelectedMatByCol(int col, bool selected) {
  for (int i = 0; i < selectedMat.size(); ++i) {
    if (selectedMat.at(i).size() > col)
      selectedMat[i][col] = selected;
  }
}

void DetailedMatrix::setFalseEntireSelectedMat() {
  int row = selectedMat.size();
  int col = 0;
  if (row > 0)
    col = selectedMat.at(0).size();

  selectedMat = QVector<QVector<bool>>(row, QVector<bool>(col, false));
}

void DetailedMatrix::setFalseEntireFilteredOutMat() {
  int row = filteredOutMat.size();
  int col = 0;
  if (row > 0)
    col = filteredOutMat.at(0).size();

  filteredOutMat = QVector<QVector<bool>>(row, QVector<bool>(col, false));
}

void DetailedMatrix::setTrueEntireUsedInDimReductionMat() {
  int row = usedInDimReductionMat.size();
  int col = 0;
  if (row > 0)
    col = usedInDimReductionMat.at(0).size();

  usedInDimReductionMat = QVector<QVector<bool>>(row, QVector<bool>(col, true));
}

void DetailedMatrix::setFilteredOutForUnselectedElements() {
  for (int i = 0; i < selectedMat.size(); ++i) {
    for (int j = 0; j < selectedMat.at(i).size(); ++j) {
      if (selectedMat.at(i).at(j))
        filteredOutMat[i][j] = false;
      else
        filteredOutMat[i][j] = true;
    }
  }
}

void DetailedMatrix::setInvisibleForUnselectedElements() {
  for (int i = 0; i < selectedMat.size(); ++i) {
    for (int j = 0; j < selectedMat.at(i).size(); ++j) {
      if (selectedMat.at(i).at(j))
        visibleMat[i][j] = true;
      else
        visibleMat[i][j] = false;
    }
  }
}

void DetailedMatrix::setUnusedInDimReductionForUnselectedElements() {
  for (int i = 0; i < selectedMat.size(); ++i) {
    for (int j = 0; j < selectedMat.at(i).size(); ++j) {
      if (selectedMat.at(i).at(j))
        usedInDimReductionMat[i][j] = true;
      else
        usedInDimReductionMat[i][j] = false;
    }
  }
}

void DetailedMatrix::initMatrix(std::vector<std::vector<float>> matrix,
                                float minThresVal, float maxThresVal,
                                float lowerThresVal, float upperThresVal) {
  setMatrix(matrix);
  int numOfRow = (int)matrix.size();
  if (numOfRow > 0) {
    int numOfCol = (int)matrix.at(0).size();
    visibleMat =
        QVector<QVector<bool>>(numOfRow, QVector<bool>(numOfCol, true));
    selectedMat =
        QVector<QVector<bool>>(numOfRow, QVector<bool>(numOfCol, false));
    sizeMat = QVector<QVector<float>>(numOfRow, QVector<float>(numOfCol, 2.0));
    filteredOutMat =
        QVector<QVector<bool>>(numOfRow, QVector<bool>(numOfCol, false));
    usedInDimReductionMat =
        QVector<QVector<bool>>(numOfRow, QVector<bool>(numOfCol, true));
  }

  this->minThresVal = minThresVal;
  this->maxThresVal = maxThresVal;
  this->lowerThresVal = lowerThresVal;
  this->upperThresVal = upperThresVal;

  updateVisibleMat();
}

void DetailedMatrix::outputSubMatrix(QString filePath) {
  std::vector<std::vector<float>> subMat = getSubMatrixBasedOnRoiOrder();

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "cannot open file: " << filePath;
    return;
  }

  QTextStream out(&file);
  foreach (std::vector<float> row, subMat) {
    QString outputLine;
    foreach (float val, row) { outputLine += QString::number(val) + ","; }
    outputLine.chop(1); // remove last comma
    outputLine += "\n";
    out << outputLine;
  }
  file.close();
}
