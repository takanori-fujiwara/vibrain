#ifndef ROIS_H
#define ROIS_H

#include <QDebug>
#include <QFile>
#include <QMap>
#include <QVector>
#include <QtAlgorithms>

#include "roi.hpp"

class Rois {
public:
  Rois(const QString &roiFilePath);
  void resetRois(const QString &roiFilePath);
  ~Rois();

  QVector<Roi *> getRois();
  Roi *getRoiAt(int pos);
  int getNumOfRois();
  QVector<int> getOrder();
  QVector<int> getGroups();
  int getHoveredRoiIndex();
  int getIndexOnOrder(int pos);
  // int getIndexFromOrder(int order);
  int getIndexFromName(QString name);
  // int getNumOfNonNegativeOrders();
  void setOrder(QVector<int> order);
  void setGroups(QVector<int> groups);
  // void setOrdersForSelectedRois(QVector<int> selectedRois);
  void setHoveredRoiIndex(int hoveredRoiIndex);

  int getNumOfNonNegativeRanks();
  void setOrderBasedOnRanks();
  void resetOrderWithInitialOrder();
  void initNamesToIndices();

  void outputOrderAsNames(QString filePath);
  void outputRoisCsv(QString filePath);

private:
  QVector<Roi *> rois;
  QVector<int> initialOrder;
  QVector<int> order;
  // QMap<int, int> ordersToIndices;
  QMap<QString, int> namesToIndices;
  int hoveredRoiIndex;

  bool loadRoisFile(const QString &filePath);
  static bool qPairSecondLessThan(QPair<int, int> pair1, QPair<int, int> pair2);
  void setInitialOrder(QVector<int> initialOrder);
};

#endif // ROIS_H
