#ifndef EDGEBUNDLEEDGE_H
#define EDGEBUNDLEEDGE_H

/// [for edge bundling]
/// original source: https://github.com/NeuroanatomyAndConnectivity/brainbundler
///

#include <QList>
#include <QVector3D>
#include <QDebug>

class EdgeBundleEdge
{
public:
    EdgeBundleEdge(QVector3D fn, QVector3D tn);
    QVector3D fn, tn;
    QList<QVector3D> points;
    QList<QVector3D> forces;

    void subdivide(int newp);
    void attract();
    void applyForces();
    bool flip(EdgeBundleEdge* other);
    double length();
    double segLength(int n);
};

#endif // EDGEBUNDLEEDGE_H
