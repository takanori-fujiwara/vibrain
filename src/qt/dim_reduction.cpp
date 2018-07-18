#include "dim_reduction.hpp"

DimReduction::DimReduction(QString dimReductionMethod, QString distCalcMethod, QString targetMatrixKey)
{
    this->dimReductionMethod = dimReductionMethod;
    this->distCalcMethod = distCalcMethod;
    this->targetMatrixKey = targetMatrixKey;
}

void DimReduction::resetDimReduction(QString dimReductionMethod, QString distCalcMethod, QString targetMatrixKey)
{
    this->dimReductionMethod = dimReductionMethod;
    this->distCalcMethod = distCalcMethod;
    this->targetMatrixKey = targetMatrixKey;
}

float DimReduction::calcEuclidDist(std::vector<float> &vec1, std::vector<float> &vec2)
{
    std::vector<float> diffVec;
    for (int i = 0; i < (int)vec1.size(); ++i) {
        diffVec.push_back(vec1.at(i) - vec2.at(i));
    }
    return std::sqrt(inner_product(diffVec.begin(), diffVec.end(), diffVec.begin(), 0.0));
}

float DimReduction::calcCosDist(std::vector<float> &vec1, std::vector<float> &vec2)
{
    float inner = std::inner_product(vec1.begin(), vec1.end(), vec2.begin(), 0.0);
    float magVec1 = std::sqrt(inner_product(vec1.begin(), vec1.end(), vec1.begin(), 0.0));
    float magVec2 = std::sqrt(inner_product(vec2.begin(), vec2.end(), vec2.begin(), 0.0));
    float cosDist = 1.0 - (inner / magVec1 / magVec2);
    if(cosDist < 0.0) cosDist = 0.0; // to modify minus value caused by precision

    return cosDist; // return 0 to 2 value;
}

float DimReduction::calcCorDist(std::vector<float> &vec1, std::vector<float> &vec2)
{
    float meanX = std::accumulate(vec1.begin(), vec1.end(), 0.0) / vec1.size();
    float meanY = std::accumulate(vec2.begin(), vec2.end(), 0.0) / vec2.size();

    float xy = 0.0;
    float xx = 0.0;
    float yy = 0.0;

    for (int i = 0; i < (int)vec1.size(); ++i) {
        float x_ = vec1.at(i) - meanX;
        float y_ = vec2.at(i) - meanY;
        xy += x_ * y_;
        xx += x_ * x_;
        yy += y_ * y_;
    }

    float corDist = 1.0 - (xy / std::sqrt(xx) / std::sqrt(yy));
    if (corDist < 0.0) corDist = 0.0;

    return corDist;
}

float DimReduction::calcDist(std::vector<float> &vec1, std::vector<float> &vec2, QString distCalcMethod)
{
    float result = 0.0;
    if (distCalcMethod == "euclid") { result = calcEuclidDist(vec1, vec2); }
    else if (distCalcMethod == "cosine") { result = calcCosDist(vec1, vec2); }
    else if (distCalcMethod == "cor") { result = calcCorDist(vec1, vec2); }
    else {
        qWarning() << "(in calcDist) indicated distance method is not supported: " << distCalcMethod;
    }

    return result;
}

std::vector<std::vector<float>> DimReduction::calcDistMatBetweenScannedDataset(ScannedDataset* scannedDataset, QVector<int> targetScannedDataIndices, QString targetMatrixKey, QVector<QPair<int,int>> targetLinks, QString distCalcMethod)
{
    std::vector<std::vector<float>> distMat(targetScannedDataIndices.size(), std::vector<float>(targetScannedDataIndices.size(), 0.0));
    std::vector<std::vector<float>> targetVectors;

    foreach (int sdIndex, targetScannedDataIndices) {
        std::vector<float> targetVec;

        if (targetLinks.size() != 0) {
            for (int i = 0; i < targetLinks.size(); ++i) { // QPair cannot use foreach
                QPair<int,int> srcTarget = targetLinks.at(i);
                float val = scannedDataset->getScannedDataAt(sdIndex)->getMatrixElementAt(targetMatrixKey, srcTarget.first, srcTarget.second);
                targetVec.push_back(val);
            }
        } else {
            std::vector<std::vector<float>> matrix = scannedDataset->getScannedDataAt(sdIndex)->getMatrixAt(targetMatrixKey);
            for (int i = 0; i < (int)matrix.size(); ++i) {
                for (int j = 0; j < (int)matrix.at(i).size(); ++j) {
                    targetVec.push_back(matrix.at(i).at(j));
                }
            }
        }

        targetVectors.push_back(targetVec);
    }

    for (int i = 0; i < (int)targetVectors.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            float dist = calcDist(targetVectors.at(i), targetVectors.at(j), distCalcMethod);
            distMat[i][j] = dist;
            distMat[j][i] = dist;
        }
    }

    return distMat;
}

void DimReduction::calcMdsPositions(ScannedDataset* scannedDataset, QVector<int> targetScannedDataIndices, QString targetMatrixKey, QVector<QPair<int,int>> targetLinks, QString distCalcMethod)
{
    QVector<QPointF> mdsPositions;
    QVector<float> mdsErrors;

    std::vector<std::vector<float>> distMat = calcDistMatBetweenScannedDataset(scannedDataset, targetScannedDataIndices, targetMatrixKey, targetLinks, distCalcMethod);
    int numCol = distMat.size();

    if (distMat.size() < 2) {
        qWarning() << "to clac mds, at least 2 points needed to be selected.";
        return;
    }

    std::string txt;
    RInside &m_R = RInside::instance();
    m_R.parseEval("library(data.table)");
    m_R.parseEval("library(lsa)");

    // TODO: this causes unnecessary copy. find another way (use c++ library for mds, etc)
    // make distance matrix for R numeric matrix
    bool validDistMat = false;
    Rcpp::NumericMatrix rDistMat(numCol, numCol);
    for (int i = 0; i < numCol; i++) {
        for (int j = 0; j < numCol; j++) {
            rDistMat(i, j) = distMat.at(i).at(j);
            if (distMat.at(i).at(j) != 0) validDistMat = true; // to calc MDS at least one dist > 0
        }
    }
    if (!validDistMat) return;

    // calc mds
    m_R["distMat"] = rDistMat;
    m_R.parseEval("mdsResult <- cmdscale(distMat, k = 2)");

    // scaling pos
    m_R.parseEval("maxRange <- max( (max(mdsResult[,1]) - min(mdsResult[,1])) * 0.5, (max(mdsResult[,2]) - min(mdsResult[,2])) * 0.5)");
    m_R.parseEval("scaledMdsResult <- cbind((mdsResult[,1] - (min(mdsResult[,1]) + max(mdsResult[,1])) * 0.5) * 0.95 / maxRange, (mdsResult[,2] - (min(mdsResult[,2]) + max(mdsResult[,2])) * 0.5) * 0.95 / maxRange)");
    //m_R.parseEval("scaledMdsResult <- cbind(((mdsResult[,1] - min(mdsResult[,1])) * 0.95 / (max(mdsResult[,1]) - min(mdsResult[,1]) )) + 0.025, ((mdsResult[,2] - min(mdsResult[,2])) * 0.95 / (max(mdsResult[,2]) - min(mdsResult[,2]))) + 0.025)");

    QVector<float> xPos;
    QVector<float> yPos;
    xPos = xPos.fromStdVector( Rcpp::as<std::vector<float>>(m_R.parseEval("scaledMdsResult[,1]")) );
    yPos = xPos.fromStdVector( Rcpp::as<std::vector<float>>(m_R.parseEval("scaledMdsResult[,2]")) );

    for(int i = 0; i < xPos.size(); ++i) {
        mdsPositions.push_back(QPointF(xPos[i], yPos[i]));
    }

    // Calc mds distance error for each node
    m_R.parseEval("distMds <- as.matrix(dist(mdsResult))");
    m_R.parseEval("scaledDist <- distMat / max(distMat)");
    m_R.parseEval("scaledDistMds <- distMds / max(distMds)");
    m_R.parseEval("distInProduct <- as.matrix((scaledDist - scaledDistMds) * (scaledDist - scaledDistMds))");
    m_R.parseEval("distErrors <- apply(distInProduct, 1, function(x) {sqrt(sum(x))} )");
    m_R.parseEval("sumDistErrors <- sum(distErrors)");

    mdsErrors = mdsErrors.fromStdVector( Rcpp::as<std::vector<float>>(m_R.parseEval("distErrors")) );

    // set results in scanned dataset
    for (int i = 0; i < targetScannedDataIndices.size(); ++i) {
        int index = targetScannedDataIndices.at(i);
        scannedDataset->getScannedDataAt(index)->setPos(mdsPositions.at(i));
        scannedDataset->getScannedDataAt(index)->setMatrixPos(mdsPositions.at(i));
        scannedDataset->getScannedDataAt(index)->setMdsError(mdsErrors.at(i));
    }
}

void DimReduction::calcDimReductionPositions(ScannedDataset *scannedDataset, QVector<int> targetScannedDataIndices, QVector<QPair<int,int>> targetLinks)
{
    if (dimReductionMethod == "mds") {
        calcMdsPositions(scannedDataset, targetScannedDataIndices, targetMatrixKey, targetLinks, distCalcMethod);
    } else {
        qWarning() << "the indicated DR method is not supported.";
    }
}

QString DimReduction::getDimReductionMethod() { return dimReductionMethod; }
QString DimReduction::getDistCalcMethod() { return distCalcMethod; }
QString DimReduction::getTargetMatrixKey() { return targetMatrixKey; }
void DimReduction::setDimReductionMethod(QString dimReductionMethod) { this->dimReductionMethod = dimReductionMethod; }
void DimReduction::setDistCalcMethod(QString distCalcMethod) { this->distCalcMethod = distCalcMethod; }
void DimReduction::setTargetMatrixKey(QString targetMatrixKey) { this->targetMatrixKey = targetMatrixKey; }
