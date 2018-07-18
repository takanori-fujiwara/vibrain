#ifndef VALUEINTERPOLATION_H
#define VALUEINTERPOLATION_H

#include <math.h>

#include <QVector>
#include <QPointF>
#include <QPair>

class ValueInterpolation
{
public:
    ValueInterpolation();
    static bool qPairSecondLessThan(const QPair<int,float> &pair1, const QPair<int,float> &pair2);
    QVector<int> findKNearestFromPos(QVector<QPointF> &points, QPointF pos, int k);
    float calcWeightedAverageFromPos(QVector<float> &values, QVector<QPointF> &points, QPointF pos, int k);
};

#endif // VALUEINTERPOLATION_H
