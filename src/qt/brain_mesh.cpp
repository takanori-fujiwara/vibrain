#include "brain_mesh.hpp"

BrainMesh::BrainMesh(const QString &brainMeshFilePath, const QString &spatialCorrectonFilePath)
{
    loadBrainMesh(brainMeshFilePath);
    loadSpatialCorrection(spatialCorrectonFilePath);
}

void BrainMesh::resetBrainMesh(const QString &brainMeshFilePath, const QString &spatialCorrectonFilePath)
{
    loadBrainMesh(brainMeshFilePath);
    loadSpatialCorrection(spatialCorrectonFilePath);
}

bool BrainMesh::loadBrainMesh(const QString &filePath)
{
    vertices.clear();
    indices.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open brain mesh file. file path:" << filePath;
        return false;
    }

    QString data = file.readAll();
    file.close();

    QStringList lines = data.split('\n');

    int count = 0;

    // first line indicate # of vertices
    int numVertices = lines[0].toInt();
    count++;

    for(int i = 0; i < numVertices; ++i) {
        QStringList positions = lines[count].split(' ');
        vertices.emplace_back(positions.at(0).toFloat());
        vertices.emplace_back(positions.at(1).toFloat());
        vertices.emplace_back(positions.at(2).toFloat());
        count++;
    }

    // number of indices are indicated in a line between vertices and indices
    int numIndices = lines[count].toInt();
    count++;

    for(int i = 0; i < numIndices; ++i) {
        QStringList triIndices = lines[count].split(' ');
        indices.emplace_back(triIndices.at(0).toInt() - 1);
        indices.emplace_back(triIndices.at(1).toInt() - 1);
        indices.emplace_back(triIndices.at(2).toInt() - 1);
        count++;
    }

    foreach (GLint index, indices) {
        brainMeshPositions.push_back(vertices.at(index * 3));
        brainMeshPositions.push_back(vertices.at(index * 3 + 1));
        brainMeshPositions.push_back(vertices.at(index * 3 + 2));
    }

    return true;
}

bool BrainMesh::loadSpatialCorrection(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open spatial correction file. file path:" << filePath;
        return false;
    }

    QTextStream ts(&file);
    ts.readLine(); // ignore header
    QString line = ts.readLine();
    QStringList values = line.split(",");

    if (values.size() < 6) {
        qWarning() << "there are incorrect number of values for loading spatial correction (need 6 elments) now: " << values.size();
        return false;
    }

    translateX = values.at(0).toFloat();
    translateY = values.at(1).toFloat();
    translateZ = values.at(2).toFloat();
    scaleX = values.at(3).toFloat();
    scaleY = values.at(4).toFloat();
    scaleZ = values.at(5).toFloat();

    return true;
}

std::vector<GLfloat> BrainMesh::getVertices() { return vertices; }
std::vector<GLint> BrainMesh::getIndices() { return indices; }
std::vector<GLfloat> BrainMesh::getBrainMeshPositions() { return brainMeshPositions; }
float BrainMesh::getTranslateX() { return translateX; }
float BrainMesh::getTranslateY() { return translateY; }
float BrainMesh::getTranslateZ() { return translateZ; }
float BrainMesh::getScaleX() { return scaleX; }
float BrainMesh::getScaleY() { return scaleY; }
float BrainMesh::getScaleZ() { return scaleZ; }
