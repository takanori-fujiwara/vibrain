#include "scanned_dataset.hpp"

ScannedDataset::ScannedDataset(const QString &scanndeDataDir)
{
    loadScannedDataset(scanndeDataDir);
    categoryForColorGroups = "none";
    categoryForShapeGroups = "none";
    numberForSizes = "none";
    visTargetMatrixKey = "ROI-ROI Matrix";
}

ScannedDataset::~ScannedDataset()
{
    foreach (ScannedData *scannedData, scannedDataset) {
        delete scannedData;
    }
}

void ScannedDataset::resetScannedDataset(const QString &scanndeDataDir)
{
    foreach (ScannedData *scannedData, scannedDataset) {
        delete scannedData;
    }

    loadScannedDataset(scanndeDataDir);
    categoryForColorGroups = "none";
    categoryForShapeGroups = "none";
    numberForSizes = "none";
    visTargetMatrixKey = "ROI-ROI Matrix";
}

bool ScannedDataset::loadScannedDataset(const QString &dirPath)
{
    scannedDataset.clear();

    QDir dir(dirPath);
    dir.setNameFilters(QStringList()<<"*.csv");
    QFileInfoList fileInfos = dir.entryInfoList();

    for (int i = 0; i < fileInfos.size(); ++i) {
        QFileInfo fileInfo = fileInfos.at(i);

        ScannedData* scannedData = new ScannedData(i, fileInfo.absoluteFilePath());
        if (!scannedData->getIsLoadCompleted()) {
            qWarning() <<  "cannot load scanned data. file path: " << fileInfo.absoluteFilePath();
            return false;
        }

        scannedDataset.push_back(scannedData);
    }

    int lineNumOfDiffNumAttr = checkAllScannedDataHaveSameNumAttr();
    if (lineNumOfDiffNumAttr >= 0) {
        qWarning() << "some scanned data has different number of attributes. check scanned data file: "
                   << fileInfos.at(lineNumOfDiffNumAttr).absoluteFilePath();
        return false;
    }

    foreach (ScannedData* scannedData, scannedDataset) {
        QMap<QString, QString> categories = scannedData->getCategories();
        QMap<QString, float> numbers = scannedData->getNumbers();

        foreach(QString key, categories.keys()) {
            if (valuesOfCategories[key].size() == 0) {
                valuesOfCategories.insert(key, QVector<QString>({categories[key]}));
            } else {
                if (!valuesOfCategories[key].contains(categories[key])) {
                    QVector<QString> currentValues = valuesOfCategories[key];
                    currentValues.push_back(categories[key]);
                    valuesOfCategories.insert(key, currentValues);
                }
            }
        }
        foreach(QString key, numbers.keys()) {
            if (minAndMaxOfNumbers[key].size() == 0) {
                minAndMaxOfNumbers.insert(key, QVector<float>({numbers[key], numbers[key]}));
            } else {
                if (numbers[key] < minAndMaxOfNumbers[key].first()) {
                    QVector<float> currentMinMax = minAndMaxOfNumbers[key];
                    currentMinMax.first() = numbers[key];
                    minAndMaxOfNumbers.insert(key, currentMinMax);
                }
                if (numbers[key] > minAndMaxOfNumbers[key].last()) {
                    QVector<float> currentMinMax = minAndMaxOfNumbers[key];
                    currentMinMax.last() = numbers[key];
                    minAndMaxOfNumbers.insert(key, currentMinMax);
                }
            }
        }
    }

    return true;
}

int ScannedDataset::checkAllScannedDataHaveSameNumAttr()
{
    int numSubIds, numCategories, numNumbers, numMatrices;
    QVector<int> numMatricesRows, numMatricesCols;
    QStringList matrixKeys;

    if (scannedDataset.size() > 0) {
        numSubIds = scannedDataset[0]->getNumOfSubIds();
        numCategories = scannedDataset[0]->getNumOfCategories();
        numNumbers = scannedDataset[0]->getNumOfNumbers();
        numMatrices = scannedDataset[0]->getNumOfMatrices();

        matrixKeys = scannedDataset[0]->getMatrices().keys();
        foreach (QString key, matrixKeys) {
            int numRow = (int)scannedDataset[0]->getMatrixAt(key).size();
            numMatricesRows.push_back(numRow);
            if (numRow != 0) {
                numMatricesCols.push_back((int)scannedDataset[0]->getMatrixAt(key).at(0).size());
            } else {
                numMatricesCols.push_back(0);
            }
        }
    }

    int count = 0;
    foreach (ScannedData* sd, scannedDataset) {
        if (numSubIds != sd->getNumOfSubIds() ||
                numCategories != sd->getNumOfCategories() ||
                numNumbers != sd->getNumOfNumbers() ||
                numMatrices != sd->getNumOfMatrices()) {
            return count;
        } else {
            for (int i = 0; i < matrixKeys.size(); ++i) {
                int numRow = (int)sd->getMatrixAt(matrixKeys.at(i)).size();
                if (numRow != numMatricesRows.at(i)) {
                    qWarning() << "number of rows is not equal";
                    return count;
                } else {
                    for (int j = 0; j < numRow; ++j) {
                        if ((int)sd->getMatrixAt(matrixKeys.at(i)).at(j).size() != numMatricesCols.at(i)) {
                            qWarning() << "number of cols is not equal in " << matrixKeys.at(i);
                            return count;
                        }
                    }
                }
            }
        }
        count++;
    }

    return -1;
}

bool ScannedDataset::ifWithQStringOperator(float leftVal, QString ope, float rightVal)
{
    if(ope == "<") {
        return (leftVal < rightVal);
    } else if(ope == "<=") {
        return (leftVal <= rightVal);
    } else if(ope == ">") {
        return (leftVal > rightVal);
    } else if(ope == ">=") {
        return (leftVal >= rightVal);
    } else if(ope == "==") {
        return (leftVal == rightVal);
    } else if(ope == "<=") {
        return (leftVal != rightVal);
    }

    return false;
}

void ScannedDataset::setFilteredOutWithFilterStatus(QMap<QString, QMap<QString, bool>> statusOfCategoryFilterCheckBoxes, QMap<QString, QPair<QString, QString>> statusOfNumberFilterOperatorsAndValues)
{
    QStringList categoryKeys = statusOfCategoryFilterCheckBoxes.keys();
    QStringList numberKeys = statusOfNumberFilterOperatorsAndValues.keys();

    foreach (ScannedData* sd, scannedDataset) {
        sd->setFilteredOut(false);

        foreach (QString categoryKey, categoryKeys) {
            if (!statusOfCategoryFilterCheckBoxes[categoryKey][sd->getCategoryAt(categoryKey)]) {
                sd->setFilteredOut(true);
            }
        }

        foreach (QString numberKey, numberKeys) {
            QString ope = statusOfNumberFilterOperatorsAndValues[numberKey].first;
            QString val = statusOfNumberFilterOperatorsAndValues[numberKey].second;
            if (val != "") {
                if (!ifWithQStringOperator(sd->getNumberAt(numberKey), ope, val.toFloat())) {
                    sd->setFilteredOut(true);
                }
            }
        }
    }
}

void ScannedDataset::setVisibleWithFiltering()
{
    foreach (ScannedData* sd, scannedDataset) {
        if (sd->getFilteredOut()) sd->setVisible(false);
        else sd->setVisible(true);
    }
}

void ScannedDataset::setVisibleFromSelected()
{
    foreach (ScannedData* sd, scannedDataset) {
        if (sd->getSelected()) sd->setVisible(true);
        else sd->setVisible(false);
    }
}

void ScannedDataset::setScannedDataColorGroupsFromCategoryKey(QString key)
{
    if (!valuesOfCategories.contains(key)) { // to support key == "none"
        foreach (ScannedData* sd, scannedDataset) { sd->setColorGroup(0); }
    } else {
        foreach (ScannedData* sd, scannedDataset) {
            int index = valuesOfCategories[key].indexOf(sd->getCategoryAt(key));
            if (index >= 0) sd->setColorGroup(index);
            else qWarning() << "cannot find category in setPointColorsFromCategoryKey.";
        }
    }

    categoryForColorGroups = key;
}

void ScannedDataset::setScannedDataShapeGroupsFromCategoryKey(QString key)
{
    if (!valuesOfCategories.contains(key)) { // to support key == "none"
        foreach (ScannedData* sd, scannedDataset) { sd->setShapeGroup(0); }
    } else {
        foreach (ScannedData* sd, scannedDataset) {
            int index = valuesOfCategories[key].indexOf(sd->getCategoryAt(key));
            if (index >= 0) sd->setShapeGroup(index);
            else qWarning() << "cannot find category in setPointColorsFromCategoryKey.";
        }
    }

    categoryForShapeGroups = key;
}

void ScannedDataset::setScannedDataSizesFromNumberKey(QString key)
{
    if (!minAndMaxOfNumbers.contains(key)) { // to support key == "none"
        foreach (ScannedData* sd, scannedDataset) { sd->setSize(0.02); }
    } else {
        foreach (ScannedData* sd, scannedDataset) {
            QVector<float> minMax = minAndMaxOfNumbers[key];
            float size = 0.005 + 0.05 * (sd->getNumberAt(key) - minMax.first()) / (minMax.last() - minMax.first());
            sd->setSize(size);
        }
    }

    numberForSizes = key;
}


QVector<ScannedData*> ScannedDataset::getScannedDataset() { return scannedDataset; }
ScannedData* ScannedDataset::getScannedDataAt(int pos) { return scannedDataset.at(pos); }

ScannedData* ScannedDataset::getScannedDataFromFileName(QString fileName)
{
    foreach (ScannedData* sd, scannedDataset) {
        if (sd->getFileName() == fileName) return sd;
    }

    qWarning() << "cannot find scanned data whose file name is " << fileName;
    return scannedDataset.at(0);
}

int ScannedDataset::getNumOfScannedData() { return scannedDataset.size(); }

QVector<int> ScannedDataset::getVisibleIndices()
{
    QVector<int> result;
    for (int i = 0; i < scannedDataset.size(); ++i) {
        if (scannedDataset.at(i)->getVisible()) result.push_back(i);
    }

    return result;
}

QVector<int> ScannedDataset::getSelectedIndices()
{
    QVector<int> result;
    for (int i = 0; i < scannedDataset.size(); ++i) {
        if (scannedDataset.at(i)->getSelected()) result.push_back(i);
    }

    return result;
}

QVector<int> ScannedDataset::getMatrixDisplayedIndices()
{
    QVector<int> result;
    for (int i = 0; i < scannedDataset.size(); ++i) {
        if (scannedDataset.at(i)->getMatrixDisplayed()) result.push_back(i);
    }

    return result;
}

std::vector<std::vector<float>> ScannedDataset::genAveMatFromSelectedData()
{
    QVector<int> selectedIndices = getSelectedIndices();
    if (selectedIndices.empty()) return std::vector<std::vector<float>>();
    int row = scannedDataset.at(selectedIndices.at(0))->getMatrixAt(visTargetMatrixKey).size();
    if (row == 0) return std::vector<std::vector<float>>();
    int col = scannedDataset.at(selectedIndices.at(0))->getMatrixAt(visTargetMatrixKey).at(0).size();

    std::vector<std::vector<float>> result(row, std::vector<float>(col, 0.0));

    foreach (int index, selectedIndices) {
        std::vector<std::vector<float>> mat = scannedDataset.at(index)->getMatrixAt(visTargetMatrixKey);
        for (int i = 0; i < (int)mat.size(); ++i) {
            for (int j = 0; j < (int)mat.at(i).size(); ++j) {
                result[i][j] += mat.at(i).at(j);
            }
        }
    }

    for (int i = 0; i < (int)result.size(); ++i) {
        for (int j = 0; j < (int)result.at(i).size(); ++j) {
            result[i][j] /= (float)selectedIndices.size();
        }
    }

    return result;
}

std::vector<std::vector<float>> ScannedDataset::genDiffMatFromSelectedData(int index1, int index2)
{
    std::vector<std::vector<float>> result;

    std::vector<std::vector<float>> mat1 = scannedDataset.at(index1)->getMatrixAt(visTargetMatrixKey);
    std::vector<std::vector<float>> mat2 = scannedDataset.at(index2)->getMatrixAt(visTargetMatrixKey);

    if ((int)mat1.size() != (int)mat2.size()) {
        qWarning() << "matrix sizes are different in genDiffMatFromSelectedData";
        return result;
    }

    for (int i = 0; i < (int)mat1.size(); ++i) {
        if ((int)mat1.at(i).size() != (int)mat2.at(i).size()) {
            qWarning() << "matrix sizes are different in genDiffMatFromSelectedData";
            return result;
        }

        std::vector<float> rowResult;
        for (int j = 0; j < (int)mat1.at(i).size(); ++j) {
            rowResult.push_back(mat1.at(i).at(j) - mat2.at(i).at(j));
        }
        result.push_back(rowResult);
    }

    return result;
}

std::vector<std::vector<float>> ScannedDataset::genSdMatFromSelectedData()
{
    QVector<int> selectedIndices = getSelectedIndices();
    if (selectedIndices.empty()) return std::vector<std::vector<float>>();
    int row = scannedDataset.at(selectedIndices.at(0))->getMatrixAt(visTargetMatrixKey).size();
    if (row == 0) return std::vector<std::vector<float>>();
    int col = scannedDataset.at(selectedIndices.at(0))->getMatrixAt(visTargetMatrixKey).at(0).size();

    std::vector<std::vector<float>> result(row, std::vector<float>(col, 0.0));
    std::vector<std::vector<float>> sum(row, std::vector<float>(col, 0.0));
    std::vector<std::vector<float>> sumv(row, std::vector<float>(col, 0.0));

    foreach (int index, selectedIndices) {
        std::vector<std::vector<float>> mat = scannedDataset.at(index)->getMatrixAt(visTargetMatrixKey);
        for (int i = 0; i < (int)mat.size(); ++i) {
            for (int j = 0; j < (int)mat.at(i).size(); ++j) {
                float val = mat.at(i).at(j);
                sum[i][j] += val;
                sumv[i][j] += val * val;
            }
        }
    }

    for (int i = 0; i < (int)result.size(); ++i) {
        for (int j = 0; j < (int)result.at(i).size(); ++j) {
            double num = (double)selectedIndices.size();
            result[i][j] = sqrt(sumv[i][j] / num - pow(sum[i][j] / num, 2.0));
        }
    }

    return result;
}

std::vector<std::vector<float>> ScannedDataset::genMaxMinMatFromSelectedData()
{
    QVector<int> selectedIndices = getSelectedIndices();
    if (selectedIndices.empty()) return std::vector<std::vector<float>>();
    int row = scannedDataset.at(selectedIndices.at(0))->getMatrixAt(visTargetMatrixKey).size();
    if (row == 0) return std::vector<std::vector<float>>();
    int col = scannedDataset.at(selectedIndices.at(0))->getMatrixAt(visTargetMatrixKey).at(0).size();

    std::vector<std::vector<float>> result(row, std::vector<float>(col, 0.0));
    std::vector<std::vector<float>> mins(row, std::vector<float>(col, 0.0));
    std::vector<std::vector<float>> maxs(row, std::vector<float>(col, 0.0));

    foreach (int index, selectedIndices) {
        std::vector<std::vector<float>> mat = scannedDataset.at(index)->getMatrixAt(visTargetMatrixKey);
        for (int i = 0; i < (int)mat.size(); ++i) {
            for (int j = 0; j < (int)mat.at(i).size(); ++j) {
                float val = mat.at(i).at(j);
                if (val < mins.at(i).at(j)) mins[i][j] = val;
                if (val > maxs.at(i).at(j)) maxs[i][j] = val;
            }
        }
    }

    for (int i = 0; i < (int)result.size(); ++i) {
        for (int j = 0; j < (int)result.at(i).size(); ++j) {
            result[i][j] = maxs.at(i).at(j) - mins.at(i).at(j);
        }
    }

    return result;
}

QMap<QString, QVector<QString>> ScannedDataset::getValuesOfCategories() { return valuesOfCategories; }
QMap<QString, QVector<float>> ScannedDataset::getMinAndMaxOfNumbers() { return minAndMaxOfNumbers; }
QString ScannedDataset::getCategoryForColorGroups() { return categoryForColorGroups; }
QString ScannedDataset::getCategoryForShapeGroups() { return categoryForShapeGroups; }
QString ScannedDataset::getNumberForSizes() { return numberForSizes; }
QString ScannedDataset::getVisTargetMatrixKey() { return visTargetMatrixKey; }
void ScannedDataset::setVisTargetMatrixKey(QString visTargetMatrixKey) { this->visTargetMatrixKey = visTargetMatrixKey; }
