#include "scanned_data.hpp"

ScannedData::ScannedData() {
  this->id = -1;
  this->fileName = "";
}

ScannedData::ScannedData(int id, const QString &scannedFilePath) {
  this->id = id;
  loadScannedDataFile(scannedFilePath);

  visible = true;
  pos = QPointF(0.0, 0.0);
  size = 0.02;
  colorGroup = 0;
  shapeGroup = 0;
  selected = false;
  filteredOut = false;
  matrixDisplayed = false;
  matrixPos = QPointF(0.0, 0.0);
}

bool ScannedData::loadScannedDataFile(const QString &filePath) {
  isLoadCompleted = false;
  fileName = "";
  subIds.clear();
  categories.clear();
  numbers.clear();
  matrices.clear();

  QFile file(filePath);
  QFileInfo fileInfo(file);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Couldn't open scanned data file. file path:" << filePath;
    return false;
  }

  fileName = fileInfo.fileName();

  QTextStream ts(&file);
  ts.readLine(); // ignore header

  int lineNum = 2;
  while (!ts.atEnd()) {
    QString line = ts.readLine();
    QStringList elements = line.split(",");

    if (elements.size() < 3) {
      qWarning() << "there are incorrect number of values for loading spatial "
                    "correction (need 3 elments) in line: "
                 << lineNum;
      return false;
    }

    QString attrName = elements.at(0);
    QString dataType = elements.at(1);

    if (dataType == "id") {
      subIds.insert(attrName, elements.at(2).toInt());
    } else if (dataType == "category") {
      categories.insert(attrName, elements.at(2));
    } else if (dataType == "number") {
      numbers.insert(attrName, elements.at(2).toFloat());
    } else if (dataType == "matrix") {
      std::vector<std::vector<float>> mat;
      while (!ts.atEnd()) {
        line = ts.readLine();
        QStringList values = line.split(",");
        if (values.size() == 1 && values.first() == "")
          break; // blank line is used for matrix data separation
        std::vector<float> row;
        foreach (QString v, values) { row.push_back(v.toFloat()); }

        mat.push_back(row);
      }

      matrices.insert(attrName, mat);
    } else {
      qWarning() << "there are unknown data type (you can use "
                    "id,category,number, or matrix) in line: "
                 << lineNum;
      return false;
    }

    lineNum++;
  }

  isLoadCompleted = true;
  return true;
}

int ScannedData::getId() { return id; }
QString ScannedData::getFileName() { return fileName; }
QMap<QString, int> ScannedData::getSubIds() { return subIds; }
QMap<QString, QString> ScannedData::getCategories() { return categories; }
QMap<QString, float> ScannedData::getNumbers() { return numbers; }
QMap<QString, std::vector<std::vector<float>>> ScannedData::getMatrices() {
  return matrices;
}
bool ScannedData::getIsLoadCompleted() { return isLoadCompleted; }
int ScannedData::getSubIdAt(QString key) { return subIds[key]; }
QString ScannedData::getCategoryAt(QString key) { return categories[key]; }
float ScannedData::getNumberAt(QString key) { return numbers[key]; }
std::vector<std::vector<float>> ScannedData::getMatrixAt(QString key) {
  return matrices[key];
}
float ScannedData::getMatrixElementAt(QString key, int row, int col) {
  return matrices[key].at(row).at(col);
}
int ScannedData::getNumOfSubIds() { return subIds.size(); }
int ScannedData::getNumOfCategories() { return categories.size(); }
int ScannedData::getNumOfNumbers() { return numbers.size(); }
int ScannedData::getNumOfMatrices() { return matrices.size(); }
bool ScannedData::getVisible() { return visible; }
QPointF ScannedData::getPos() { return pos; }
float ScannedData::getSize() { return size; }
uint ScannedData::getColorGroup() { return colorGroup; }
uint ScannedData::getShapeGroup() { return shapeGroup; }
bool ScannedData::getSelected() { return selected; }
bool ScannedData::getFilteredOut() { return filteredOut; }
bool ScannedData::getMatrixDisplayed() { return matrixDisplayed; }
QPointF ScannedData::getMatrixPos() { return matrixPos; }
float ScannedData::getMdsError() { return mdsError; }

void ScannedData::setVisible(bool visible) { this->visible = visible; }
void ScannedData::setPos(QPointF pos) { this->pos = pos; }
void ScannedData::setSize(float size) { this->size = size; }
void ScannedData::setColorGroup(uint colorGroup) {
  this->colorGroup = colorGroup;
}
void ScannedData::setShapeGroup(uint shapeGroup) {
  this->shapeGroup = shapeGroup;
}
void ScannedData::setSelected(bool selected) { this->selected = selected; }
void ScannedData::switchSelected() { this->selected = !this->selected; }
void ScannedData::setFilteredOut(bool filteredOut) {
  this->filteredOut = filteredOut;
}
void ScannedData::setMatrixDisplayed(bool matrixDisplayed) {
  this->matrixDisplayed = matrixDisplayed;
}
void ScannedData::setMatrixPos(QPointF matrixPos) {
  this->matrixPos = matrixPos;
}
void ScannedData::setMdsError(float mdsError) { this->mdsError = mdsError; }
