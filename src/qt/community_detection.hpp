#ifndef COMMUNITYDETECTION_H
#define COMMUNITYDETECTION_H

#include <QDebug>
#include <QVector>
#include <vector>

#include "RInside.h"

class CommunityDetection {
public:
  CommunityDetection();

  int numOfCommunities;
  float modularity;
  QVector<int> communityNumbers;
  // QVector<int> indicesToOrders;
  QVector<int> order;

  void louvain(std::vector<std::vector<float>> &targetMat);
  static bool qPairSecondLessThan(QPair<int, int> pair1, QPair<int, int> pair2);
};

#endif // COMMUNITYDETECTION_H
