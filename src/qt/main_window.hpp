#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QListWidget>
#include <QFrame>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QMessageBox>
#include <QDebug>

#include "RInside.h"
#include "data_directory_selection_dialog.hpp"
#include "brain_mesh.hpp"
#include "rois.hpp"
#include "scanned_dataset.hpp"
#include "detailed_matrix.hpp"
#include "dim_reduction.hpp"
#include "style.hpp"
#include "comparison_view.hpp"
#include "matrix_view.hpp"
#include "brain_graph_view.hpp"
#include "information_view.hpp"
#include "threshold_control_view.hpp"
#include "setting_panel.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QString baseDataDir;
    QString brainMeshDir;
    QString roiDir;
    QString scannedDataDir;
    QString styleDir;

    DataDirectorySelectionDialog *dirSelectionDialog;
    QWidget *centralWidget;
    QHBoxLayout *hBox;
    QVBoxLayout *vBoxLeft;
    QVBoxLayout *vBoxCenter;
    QVBoxLayout *vBoxRight;
    QStackedWidget *stackedWidgetL;
    QStackedWidget *stackedWidgetM1;
    QStackedWidget *stackedWidgetM2;
    QStackedWidget *stackedWidgetS;
    QStackedWidget *stackedWidgetList;
    QComboBox *comboBoxForList;
    QGroupBox *largeViewSelection;
    QPushButton *comparisonViewBtn;
    QPushButton *matrixViewBtn;
    QPushButton *brainGraphViewBtn;

    ComparisonView *comparisonView;
    MatrixView *matrixView;
    BrainGraphView *brainGraphView;
    InformationView* informationView;
    ThresholdControlView  *thresholdControlView;
    SettingPanel *settingPanel;

    BrainMesh *brainMesh;
    Rois *rois;
    ScannedDataset *scannedDataset;
    DetailedMatrix *detailedMatrix;
    DimReduction *dimReduction;
    Style *style;

    void resizeEvent(QResizeEvent* event);

    void initWidgets();
    void setupMenuBar();
    void showDirSelectionDialog();
    void setViews();
    void setDetailUi();
    void setBrainMesh();
    void setRois();
    void setScannedDataset();
    void setAllData();
    void installRPackages();
    void showSettingPanel();
    QString appDirPathToSrcDirPath();

private slots:
    void handleOpen();
    void handleSaveMatrixData();
    void handleShowSettingPanel();
    void setScannedDatasetPositions(QVector<QPair<int,int>> targetLinks);
    void setScannedDatasetPositionsAndUpdate(QVector<QPair<int,int>> targetLinks);
    void setScannedDatasetPositionsWithSettings(QString distCalcMethod, QString matrixKey);
    void setScannedDatasetPositionsWithSelectedData();
    void setScannedDatasetPositionsWithSelectedMatrixElements();
    void setScannedDatasetFilteredOut(QMap<QString, QMap<QString, bool>> statusOfCategoryFilterCheckBoxes, QMap<QString, QPair<QString, QString>> statusOfNumberFilterOperatorsAndValues);
    void setScannedDatasetVisibleWithFiltering();
    void setScannedDatasetColorGroups(QString key);
    void setScannedDatasetShapeGroups(QString key);
    void setScannedDatasetSizes(QString key);
    void setMatrixAsDetailedMatrix();
    void setAveMatrixAsDetailedMatrix();
    void setDiffMatrixAsDetailedMatrix(int index1, int index2);
    void setSdMatrixAsDetailedMatrix();
    void setMaxMinMatrixAsDetailedMatrix();

    void updateDetailedMatrix();
    void updateMatrixView();
    void updateBrainGraphView();
    void updateComparisonView();
    void updateInformationView();
    void updateViewsRelatedWithMatrixOrder();
    void updateViewsRelatedWithMatrixOrderAndRoisSubset();
    void updateViewsRelatedWithMatrixFilteredOut();
    void updateDataDirectories(QString baseDataDir, QString brainMeshDir, QString roiDir, QString scannedDataDir, QString styleDir);

    void setComparisonViewInLargeWindow();
    void setMatrixViewInLargeWindow();
    void setBrainGraphViewInLargeWindow();
};

#endif // MAIN_WINDOW_HPP
