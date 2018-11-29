#ifndef BRAINMESH_H
#define BRAINMESH_H

#include <QDebug>
#include <QFile>
#include <QOpenGLWidget>
#include <QString>
#include <vector>

class BrainMesh {
public:
  BrainMesh(const QString &brainMeshFilePath,
            const QString &spatialCorrectonFilePath);
  void resetBrainMesh(const QString &brainMeshFilePath,
                      const QString &spatialCorrectonFilePath);

  std::vector<GLfloat> getVertices();
  std::vector<GLint> getIndices();
  std::vector<GLfloat> getBrainMeshPositions();
  float getTranslateX();
  float getTranslateY();
  float getTranslateZ();
  float getScaleX();
  float getScaleY();
  float getScaleZ();

private:
  std::vector<GLfloat> vertices;
  std::vector<GLint> indices;
  // TODO: should be able to remove brainMeshPositions
  std::vector<GLfloat> brainMeshPositions;
  float translateX;
  float translateY;
  float translateZ;
  float scaleX;
  float scaleY;
  float scaleZ;

  bool loadBrainMesh(const QString &filePath);
  bool loadSpatialCorrection(const QString &filePath);
};

#endif // BRAINMESH_H
