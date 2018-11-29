#include "value_interpolation.hpp"

ValueInterpolation::ValueInterpolation() {}

bool ValueInterpolation::qPairSecondLessThan(const QPair<int, float> &pair1,
                                             const QPair<int, float> &pair2) {
  return pair1.second < pair2.second;
}

QVector<int> ValueInterpolation::findKNearestFromPos(QVector<QPointF> &points,
                                                     QPointF pos, int k) {
  QVector<int> resultIndices;

  QVector<QPair<int, float>> indicesAndDists;

  int count = 0;
  foreach (QPointF point, points) {
    float dist =
        sqrt(pow(point.x() - pos.x(), 2) + pow(point.y() - pos.y(), 2));
    indicesAndDists.push_back(qMakePair(count, dist));
    ++count;
  }

  qSort(indicesAndDists.begin(), indicesAndDists.end(), qPairSecondLessThan);

  for (int i = 0; i < k; ++i) {
    resultIndices.push_back(indicesAndDists.takeFirst().first);
  }

  return resultIndices;
}

float ValueInterpolation::calcWeightedAverageFromPos(QVector<float> &values,
                                                     QVector<QPointF> &points,
                                                     QPointF pos, int k) {
  double sumWeightedVal = 0.0;
  double sumWeight = 0.0;

  QVector<int> indices = findKNearestFromPos(points, pos, k);

  foreach (int index, indices) {
    double dist = sqrt(pow(points.at(index).x() - pos.x(), 2) +
                       pow(points.at(index).y() - pos.y(), 2));
    double weight = 1.0 / (dist + 0.00000000001); // to avoid 0 div

    sumWeightedVal += weight * values.at(index);
    sumWeight += weight;
  }

  return sumWeightedVal / sumWeight;
}
