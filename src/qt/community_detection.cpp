#include "community_detection.hpp"

CommunityDetection::CommunityDetection()
{
    numOfCommunities = 0;
    modularity = 0.0;
    order.clear();
    communityNumbers.clear();
}

void CommunityDetection::louvain(std::vector<std::vector<float>> &targetMat)
{
    numOfCommunities = 0;
    modularity = 0.0;
    communityNumbers.clear();
    order.clear();

    RInside &m_R = RInside::instance();

    // TODO: this causes unnecessary copy. find another way (use c++ library for mds, etc)
    // make distance matrix for R numeric matrix
    int numCol = targetMat.size();
    Rcpp::NumericMatrix rMat(numCol, numCol);
    for (int i = 0; i < numCol; i++) {
        for (int j = 0; j < numCol; j++) {
            rMat(i, j) = targetMat.at(i).at(j);
        }
    }

    // community detection with louvain
    // negative weight will be considered as 0.0 weighted adjacency
    m_R.parseEval("library(modMax)");
    m_R["mat"] = rMat;
    m_R.parseEval("minVal = min(mat)");
    m_R.parseEval("mat.nonnegative <- mat - minVal");
    m_R.parseEval("louvainResult <- louvain(mat.nonnegative)");

    numOfCommunities = Rcpp::as<int>(m_R.parseEval("louvainResult$`number of communities`"));
    modularity = Rcpp::as<float>(m_R.parseEval("louvainResult$modularity"));
    communityNumbers = communityNumbers.fromStdVector(Rcpp::as<std::vector<int>>(m_R.parseEval("louvainResult$`community structure`")));

    for (int i = 0; i < communityNumbers.size(); ++i) {
        communityNumbers[i] -= 1; // R index starts from 1
    }

    QVector<QPair<int,int>> indicesAndCommunityNumbers;
    for (int i = 0; i < communityNumbers.size(); ++i) {
        indicesAndCommunityNumbers.push_back(qMakePair(i, communityNumbers.at(i)));
    }
    qSort(indicesAndCommunityNumbers.begin(), indicesAndCommunityNumbers.end(), qPairSecondLessThan);

    for (int i = 0; i < indicesAndCommunityNumbers.size(); ++i) {
        order.push_back(indicesAndCommunityNumbers.at(i).first);
    }

    //for (int i = 0; i < indicesAndCommunityNumbers.size(); ++i) {
    //    indicesToOrders.push_back(indicesAndCommunityNumbers.at(i).first);
    //}

//    qDebug() << numOfCommunities;
//    qDebug() << modularity;
//    qDebug() << communityNumbers;
//    qDebug() << indicesToOrders;
}

bool CommunityDetection::qPairSecondLessThan(QPair<int, int> pair1, QPair<int, int> pair2) { return pair1.second < pair2.second; }
