#ifndef HIERARCHICALCLUSTERING_H
#define HIERARCHICALCLUSTERING_H

#include <vector>
#include <QVector>
#include <QDebug>

#include "RInside.h"

class HierarchicalClustering
{
public:
    HierarchicalClustering();

    QVector<int> order;
    QVector<float> heights;
    QVector<QVector<int>> mergeHistory;
    QVector<int> ranks;
    QVector<int> communityNumbers;

    void hclust(std::vector<std::vector<float>> &targetMat, QString hclustMethod = "complete", int numOfCommunities = 1);
    static bool qPairSecondLessThan(QPair<int, int> pair1, QPair<int, int> pair2);
};

#endif // HIERARCHICALCLUSTERING_H
