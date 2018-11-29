#ifndef EDGEBUNDLECONNECTIONS_H
#define EDGEBUNDLECONNECTIONS_H

/// [for edge bundling]
/// original source: https://github.com/NeuroanatomyAndConnectivity/brainbundler
///

#include <QDataStream> ///// FOR FIX BUG
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector3D>
#include <QtCore/QCoreApplication>
#include <QtMath>

#include "detailed_matrix.hpp" ///// FOR ADD NEW FUNC
#include "edge_bundle_edge.hpp"
#include "rois.hpp" ///// FOR ADD NEW FUNC

class EdgeBundleConnections {
public:
  EdgeBundleConnections(QString nname, QString ename);
  EdgeBundleConnections(QString fib);
  EdgeBundleConnections(
      Rois *rois,
      DetailedMatrix *detailedMatrix); // to add new constructor (takanori)

  void params();
  void subdivide();
  void subdivide(int newp);
  void attract();

  void fullAttract();
  void calcComps();
  float *comps;
  float comp(int i, int j);
  void writeVTK();
  void writeBinaryVTK();
  void writeBinaryVTK(QString name);
  void writeSegments();
  QString name();

  double c_thr, bell;
  int start_i, numcycles, smooth;
  QString prefix;

  QList<EdgeBundleEdge *> getEdges(); // added (takanori)

private:
  QList<QVector3D> nodes;
  QList<EdgeBundleEdge *> edges;
  double vis_c(EdgeBundleEdge *e1, EdgeBundleEdge *e2);
  QVector3D proj(QVector3D a, QVector3D b, QVector3D p);
};

#endif // EDGEBUNDLECONNECTIONS_H
