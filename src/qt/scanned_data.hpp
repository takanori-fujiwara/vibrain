#ifndef SCANNEDDATA_H
#define SCANNEDDATA_H

#include <vector>

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QPointF>
#include <QString>

class ScannedData {
public:
  ScannedData();
  ScannedData(int id, const QString &scannedFilePath);
  int getId();
  QString getFileName();
  QMap<QString, int> getSubIds();
  QMap<QString, QString> getCategories();
  QMap<QString, float> getNumbers();
  QMap<QString, std::vector<std::vector<float>>> getMatrices();
  bool getIsLoadCompleted();
  int getSubIdAt(QString key);
  QString getCategoryAt(QString key);
  float getNumberAt(QString key);
  std::vector<std::vector<float>> getMatrixAt(QString key);
  float getMatrixElementAt(QString key, int row, int col);
  int getNumOfSubIds();
  int getNumOfCategories();
  int getNumOfNumbers();
  int getNumOfMatrices();

  bool getVisible();
  QPointF getPos();
  float getSize();
  uint getColorGroup();
  uint getShapeGroup();
  bool getSelected();
  bool getFilteredOut();
  bool getMatrixDisplayed();
  QPointF getMatrixPos();
  float getMdsError();

  void setVisible(bool visible);
  void setPos(QPointF pos);
  void setSize(float size);
  void setColorGroup(uint colorGroup);
  void setShapeGroup(uint shapeGroup);
  void setSelected(bool selected);
  void switchSelected();
  void setFilteredOut(bool filteredOut);
  void setMatrixDisplayed(bool matrixDisplayed);
  void setMatrixPos(QPointF matrixPos);
  void setMdsError(float mdsError);

private:
  int id;
  QString fileName;
  QMap<QString, int> subIds;
  QMap<QString, QString> categories;
  QMap<QString, float> numbers;
  QMap<QString, std::vector<std::vector<float>>> matrices;
  bool isLoadCompleted;

  // TODO: maybe these should be separated from this class
  bool visible;
  QPointF pos;
  float size;
  uint colorGroup;
  uint shapeGroup;
  bool selected;
  bool filteredOut;
  bool matrixDisplayed;
  QPointF matrixPos;
  float mdsError;

  bool loadScannedDataFile(const QString &filePath);
};

#endif // SCANNEDDATA_H
