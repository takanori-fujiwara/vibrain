#include "hierarchical_clustering.hpp"

HierarchicalClustering::HierarchicalClustering()
{
    order.clear();
    heights.clear();
    mergeHistory.clear();
}

void HierarchicalClustering::hclust(std::vector<std::vector<float>> &targetMat, QString hclustMethod, int numOfCommunities)
{
    order.clear();
    heights.clear();
    mergeHistory.clear();
    //indicesToOrders.clear();

    if (!QStringList({"single","complete","average","centroid","median","ward","mcquitty"}).contains(hclustMethod)) {
        qWarning() << "hclust does not have selected method:" << hclustMethod;
        return;
    }

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

    // hierarchy clustering
    m_R["mat"] = rMat;
    m_R.parseEval("hclustResult <- hclust(dist(mat), method=\"" + hclustMethod.toStdString() + "\")");

    order = order.fromStdVector(Rcpp::as<std::vector<int>>(m_R.parseEval("hclustResult$order")));
    heights = heights.fromStdVector(Rcpp::as<std::vector<float>>(m_R.parseEval("hclustResult$height")));
    QVector<int> mergeHistoryCol1 = mergeHistoryCol1.fromStdVector( Rcpp::as<std::vector<int>>(m_R.parseEval("hclustResult$merge[,1]")) );
    QVector<int> mergeHistoryCol2 = mergeHistoryCol2.fromStdVector( Rcpp::as<std::vector<int>>(m_R.parseEval("hclustResult$merge[,2]")) );
    mergeHistory.push_back(mergeHistoryCol1);
    mergeHistory.push_back(mergeHistoryCol2);

    // index modification. R index starts from 1 instead of 0
    for (int i = 0; i < order.size(); ++i) {
        order[i] -= 1;
    }

    // index modification. R index starts from 1 instead of 0
    // merge info includes both negative and positive numbers
    for (int i = 0; i < mergeHistory.size(); ++i) {
        for (int j = 0; j < mergeHistory.at(i).size(); ++j) {
            if (mergeHistory.at(i).at(j) >= 0) mergeHistory[i][j] -= 1;
            else mergeHistory[i][j] += 1;
        }
    }

    communityNumbers = communityNumbers.fromStdVector( Rcpp::as<std::vector<int>>(m_R.parseEval("cutree(hclustResult, k=" + std::to_string(numOfCommunities) + ")")));
    for (int i = 0; i < communityNumbers.size(); ++i) {
        communityNumbers[i] -= 1; // R index starts from 1
    }

//    QVector<QPair<int,int>> indicesAndOrders;
//    for (int i = 0; i < orders.size(); ++i) {
//        indicesAndOrders.push_back(qMakePair(i, orders.at(i)));
//    }
//    qSort(indicesAndOrders.begin(), indicesAndOrders.end(), qPairSecondLessThan);

//    for (int i = 0; i < indicesAndOrders.size(); ++i) {
//        indicesToOrders.push_back(indicesAndOrders.at(i).first);
//    }

//    qDebug() << orders;
//    qDebug() << heights;
//    qDebug() << mergeHistory;
//    qDebug() << indicesToOrders;
}

bool HierarchicalClustering::qPairSecondLessThan(QPair<int, int> pair1, QPair<int, int> pair2) { return pair1.second < pair2.second; }
