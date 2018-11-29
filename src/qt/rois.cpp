#include "rois.hpp"

Rois::Rois(const QString &roiFilePath) {
  loadRoisFile(roiFilePath);
  setOrderBasedOnRanks();
  setInitialOrder(order);
  initNamesToIndices();
  hoveredRoiIndex = -1;
}

Rois::~Rois() {
  foreach (Roi *roi, rois) { delete roi; }
}

void Rois::resetRois(const QString &roiFilePath) {
  foreach (Roi *roi, rois) { delete roi; }

  loadRoisFile(roiFilePath);
  setOrderBasedOnRanks();
  setInitialOrder(order);
  initNamesToIndices();
  hoveredRoiIndex = -1;
}

bool Rois::loadRoisFile(const QString &filePath) {
  rois.clear();

  QFile file(filePath);

  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Couldn't open ROIs file. file path:" << filePath;
    return false;
  }

  QTextStream ts(&file);
  ts.readLine(); // ignore header

  int lineNum = 2;
  while (!ts.atEnd()) {
    QString line = ts.readLine();
    QStringList values = line.split(",");

    if (values.size() < 5) {
      qWarning() << "there are incorrect number of values for loading spatial "
                    "correction (need 5 elments) in line: "
                 << lineNum;
      return false;
    }

    QString name = values.at(0);
    float x = values.at(1).toFloat();
    float y = values.at(2).toFloat();
    float z = values.at(3).toFloat();
    uint group = values.at(4).toUInt();
    int rank = values.at(5).toInt();

    Roi *roi = new Roi(name, x, y, z, group, rank);
    rois.push_back(roi);

    lineNum++;
  }

  return true;
}

bool Rois::qPairSecondLessThan(QPair<int, int> pair1, QPair<int, int> pair2) {
  return pair1.second < pair2.second;
}

int Rois::getNumOfNonNegativeRanks() {
  int result = 0;
  foreach (Roi *roi, rois) {
    if (roi->getRank() >= 0)
      result++;
  }
  return result;
}

void Rois::setOrderBasedOnRanks() {
  QVector<int> order = QVector<int>(getNumOfNonNegativeRanks());
  for (int i = 0; i < rois.size(); ++i) {
    if (rois.at(i)->getRank() >= 0) {
      order[rois.at(i)->getRank()] = i;
    }
  }
  setOrder(order);
  // initOrdersToIndices();
}

void Rois::resetOrderWithInitialOrder() { setOrder(initialOrder); }

// void Rois::initOrdersToIndices()
//{
//    ordersToIndices.clear();

//    for (int i = 0; i < orders.size(); ++i) {
//        if (orders.at(i) >= 0) {
//            ordersToIndices.insert(i, orders.at(i));
//        }
//    }
//}

void Rois::initNamesToIndices() {
  namesToIndices.clear();

  for (int i = 0; i < rois.size(); ++i) {
    namesToIndices.insert(rois.at(i)->getName(), i);
  }
}

QVector<int> Rois::getGroups() {
  QVector<int> groups;
  foreach (Roi *roi, rois) {
    if (!groups.contains(roi->getGroup()))
      groups.push_back(roi->getGroup());
  }
  return groups;
}

QVector<Roi *> Rois::getRois() { return rois; }
Roi *Rois::getRoiAt(int pos) { return rois.at(pos); }
int Rois::getNumOfRois() { return rois.size(); }
QVector<int> Rois::getOrder() { return order; }
int Rois::getHoveredRoiIndex() { return hoveredRoiIndex; }
int Rois::getIndexOnOrder(int pos) { return order.at(pos); }
// int Rois::getIndexFromOrder(int order) { return ordersToIndices[order]; }
int Rois::getIndexFromName(QString name) { return namesToIndices[name]; }
// int Rois::getNumOfNonNegativeOrders()
//{
//    int count = 0;
//    foreach (int order, orders) {
//        if (order >= 0) count++;
//    }
//    return count;
//}

void Rois::setOrder(QVector<int> order) {
  this->order = order;

  // change ranks
  foreach (Roi *roi, rois) { roi->setRank(-1); }
  for (int i = 0; i < order.size(); ++i) {
    int roiIndex = order.at(i);
    rois[roiIndex]->setRank(i);
  }
}

void Rois::setInitialOrder(QVector<int> initialOrder) {
  this->initialOrder = initialOrder;
}

void Rois::setGroups(QVector<int> groups) {
  for (int i = 0; i < groups.size(); ++i) {
    rois.at(order.at(i))->setGroup(groups.at(i));
  }
}

void Rois::setHoveredRoiIndex(int hoveredRoiIndex) {
  this->hoveredRoiIndex = hoveredRoiIndex;
}
// void Rois::setOrdersForSelectedRois(QVector<int> selectedRois)
//{
//    QVector<int> ordersForSelectedRois;
//    foreach (int index, selectedRois) {
//        ordersForSelectedRois.push_back(orders.at(index));
//    }

//    orders = QVector<int>(orders.size(), -1);

//    // qpair is for obtaining the index order after sort
//    QVector<QPair<int,int>> indicesAndOrders;
//    for (int i = 0; i < ordersForSelectedRois.size(); ++i) {
//        indicesAndOrders.push_back(qMakePair(i, ordersForSelectedRois.at(i)));
//    }

//    qSort(indicesAndOrders.begin(), indicesAndOrders.end(),
//    qPairSecondLessThan);

//    for (int i = 0; i < indicesAndOrders.size(); ++i) {
//        orders[selectedRois.at(indicesAndOrders.at(i).first)] = i;
//    }

//    initOrdersToIndices();
//}

void Rois::outputOrderAsNames(QString filePath) {
  QVector<QString> names;

  foreach (int roiIndex, order) {
    names.push_back(rois.at(roiIndex)->getName());
  }

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "cannot open file: " << filePath;
    return;
  }

  QTextStream out(&file);
  foreach (QString name, names) { out << name << "\n"; }

  file.close();
}

void Rois::outputRoisCsv(QString filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "cannot open file: " << filePath;
    return;
  }

  QTextStream out(&file);
  foreach (Roi *roi, rois) {
    out << roi->getName() << ",";
    out << roi->getX() << ",";
    out << roi->getY() << ",";
    out << roi->getZ() << ",";
    out << roi->getGroup() << ",";
    out << roi->getRank() << "\n";
  }

  file.close();
}
