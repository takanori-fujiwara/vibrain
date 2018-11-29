#include "main_window.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  QDialog rPackageInstallMsgBox;
  QVBoxLayout *rPackageInstallMsgBoxLayout = new QVBoxLayout;
  QLabel *rPackageInstallMsg = new QLabel;
  rPackageInstallMsgBox.setWindowFlags(Qt::WindowTitleHint | Qt::Dialog |
                                       Qt::CustomizeWindowHint |
                                       Qt::FramelessWindowHint);
  rPackageInstallMsgBox.setLayout(rPackageInstallMsgBoxLayout);
  rPackageInstallMsgBoxLayout->addWidget(rPackageInstallMsg);
  rPackageInstallMsgBox.setSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::MinimumExpanding);
  rPackageInstallMsg->setText("Installing R packages...");
  QFutureWatcher<void> rPackageInstallWatcher;
  connect(&rPackageInstallWatcher, SIGNAL(finished()), &rPackageInstallMsgBox,
          SLOT(close()));
  QFuture<void> rPackageInstallFuture =
      QtConcurrent::run(this, &MainWindow::installRPackages);
  rPackageInstallWatcher.setFuture(rPackageInstallFuture);
  rPackageInstallMsgBox.exec();

  // ui->setupUi(this);
  baseDataDir = "";
  brainMeshDir = "";
  roiDir = "";
  scannedDataDir = "";
  styleDir = "";

  setupMenuBar();
  initWidgets();
  showDirSelectionDialog();

  QDialog msgBox;
  QVBoxLayout *msgBoxLayout = new QVBoxLayout;
  QLabel *msg = new QLabel;
  msgBox.setWindowFlags(Qt::WindowTitleHint | Qt::Dialog |
                        Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
  msgBox.setLayout(msgBoxLayout);
  msgBoxLayout->addWidget(msg);
  msgBox.setSizePolicy(QSizePolicy::MinimumExpanding,
                       QSizePolicy::MinimumExpanding);
  msg->setText("Loading data....");
  QFutureWatcher<void> watcher;
  connect(&watcher, SIGNAL(finished()), &msgBox, SLOT(close()));
  QFuture<void> future = QtConcurrent::run(this, &MainWindow::setAllData);
  watcher.setFuture(future);
  msgBox.exec();

  setDetailUi();
  showSettingPanel();
}

MainWindow::~MainWindow() {
  //    delete brainMesh;
  //    delete rois;
  //    delete scannedDataset;
  //    delete detailedMatrix;
  //    delete style;

  delete ui;
}

void MainWindow::installRPackages() {
  RInside &m_R = RInside::instance();
  m_R.parseEval("if (!require(Rcpp)) { install.packages(\"Rcpp\", "
                "repos='http://cran.us.r-project.org') }");
  m_R.parseEval("if (!require(data.table)) { install.packages(\"data.table\", "
                "repos='http://cran.us.r-project.org') }");
  m_R.parseEval("if (!require(SnowballC)) { install.packages(\"SnowballC\", "
                "repos='http://cran.us.r-project.org') }");
  m_R.parseEval("if (!require(lsa)) { install.packages(\"lsa\", "
                "repos='http://cran.us.r-project.org') }");
  m_R.parseEval("if (!require(modMax)) { install.packages(\"modMax\", "
                "repos='http://cran.us.r-project.org') }");
}

QString MainWindow::appDirPathToSrcDirPath() {
  QString appDirPath = qApp->applicationDirPath();
  QString srcDirName = "/src/";
  return appDirPath.mid(0, appDirPath.indexOf(srcDirName) + srcDirName.size());
}

void MainWindow::initWidgets() {
  centralWidget = new QWidget;

  hBox = new QHBoxLayout;
  vBoxLeft = new QVBoxLayout;
  vBoxCenter = new QVBoxLayout;
  vBoxRight = new QVBoxLayout;
  stackedWidgetL = new QStackedWidget;
  stackedWidgetM1 = new QStackedWidget;
  stackedWidgetM2 = new QStackedWidget;
  stackedWidgetS = new QStackedWidget;
  stackedWidgetList = new QStackedWidget;
  comboBoxForList = new QComboBox;
  largeViewSelection = new QGroupBox;
  comparisonViewBtn = new QPushButton("Comparison View");
  matrixViewBtn = new QPushButton("Matrix View");
  brainGraphViewBtn = new QPushButton("Brain Graph View");

  stackedWidgetL->setSizePolicy(
      QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  stackedWidgetM1->setSizePolicy(
      QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  stackedWidgetM2->setSizePolicy(
      QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  stackedWidgetS->setSizePolicy(
      QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  comboBoxForList->setSizePolicy(
      QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));
  stackedWidgetList->setSizePolicy(
      QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  stackedWidgetL->setMinimumSize(this->height() * 0.9, this->height() * 0.9);
  stackedWidgetM1->setMinimumSize(this->height() * 0.44, this->height() * 0.44);
  stackedWidgetM2->setMinimumSize(this->height() * 0.44, this->height() * 0.44);
  stackedWidgetS->setMinimumSize(this->height() * 0.44, this->height() * 0.05);
  comboBoxForList->setMinimumWidth(this->height() * 0.2);
  stackedWidgetList->setMinimumSize(this->height() * 0.2, this->height() * 0.9);

  QFrame *vLine1 = new QFrame();
  vLine1->setGeometry(QRect());
  vLine1->setFrameShape(QFrame::VLine);
  vLine1->setFrameShadow(QFrame::Sunken);
  QFrame *vLine2 = new QFrame();
  vLine2->setGeometry(QRect());
  vLine2->setFrameShape(QFrame::VLine);
  vLine2->setFrameShadow(QFrame::Sunken);

  hBox->addLayout(vBoxLeft);
  vBoxLeft->setMargin(0);
  vBoxLeft->setSpacing(0);
  vBoxLeft->addWidget(largeViewSelection);
  QHBoxLayout *hBoxLeft = new QHBoxLayout;
  hBoxLeft->setMargin(0);
  hBoxLeft->setSpacing(0);
  largeViewSelection->setFlat(true);
  largeViewSelection->setLayout(hBoxLeft);
  hBoxLeft->addWidget(comparisonViewBtn);
  hBoxLeft->addWidget(matrixViewBtn);
  hBoxLeft->addWidget(brainGraphViewBtn);
  comparisonViewBtn->setCheckable(true);
  comparisonViewBtn->setAutoExclusive(true);
  matrixViewBtn->setCheckable(true);
  matrixViewBtn->setAutoExclusive(true);
  brainGraphViewBtn->setCheckable(true);
  brainGraphViewBtn->setAutoExclusive(true);
  comparisonViewBtn->setChecked(true);
  vBoxLeft->addWidget(stackedWidgetL);
  hBox->addWidget(vLine1);

  hBox->addLayout(vBoxCenter);
  vBoxCenter->addWidget(stackedWidgetM1);
  vBoxCenter->addWidget(stackedWidgetS);
  vBoxCenter->addWidget(stackedWidgetM2);
  hBox->addWidget(vLine2);

  hBox->addLayout(vBoxRight);
  vBoxRight->addWidget(comboBoxForList);
  vBoxRight->addWidget(stackedWidgetList);

  centralWidget->setLayout(hBox);
  setCentralWidget(centralWidget);
}

void MainWindow::showDirSelectionDialog() {
  dirSelectionDialog = new DataDirectorySelectionDialog;
  connect(dirSelectionDialog,
          &DataDirectorySelectionDialog::dataDirecotriesChanged, this,
          &MainWindow::updateDataDirectories);

  dirSelectionDialog->exec();
}

void MainWindow::setupMenuBar() {
  QMenu *fileMenu = new QMenu(tr("&File"));
  menuBar()->addMenu(fileMenu);
  QAction *actOpen = new QAction(tr("&Open"), this);
  fileMenu->addAction(actOpen);
  connect(actOpen, &QAction::triggered, this, &MainWindow::handleOpen);
  QAction *actSaveMatrixData =
      new QAction(tr("&Save matrix data displayed in Matrix View"), this);
  fileMenu->addAction(actSaveMatrixData);
  connect(actSaveMatrixData, &QAction::triggered, this,
          &MainWindow::handleSaveMatrixData);

  QMenu *viewMenu = new QMenu(tr("&View"));
  menuBar()->addMenu(viewMenu);
  QAction *actShowSettingPanel = new QAction(tr("&Show settting panel"), this);
  viewMenu->addAction(actShowSettingPanel);
  connect(actShowSettingPanel, &QAction::triggered, this,
          &MainWindow::handleShowSettingPanel);
}

void MainWindow::handleOpen() {
  showDirSelectionDialog();

  const QString brainMeshFilePath = brainMeshDir + "brain_mesh.txt";
  const QString spatialCorrectionFilePath =
      brainMeshDir + "spatial_correction.csv";
  brainMesh->resetBrainMesh(brainMeshFilePath, spatialCorrectionFilePath);
  ;

  const QString roisFilePath = roiDir + "rois.csv";
  rois->resetRois(roisFilePath);

  scannedDataset->resetScannedDataset(scannedDataDir);
  scannedDataset->setVisTargetMatrixKey("ROI-ROI Matrix");

  dimReduction->resetDimReduction("mds", "euclid", "ROI-ROI Matrix");

  setScannedDatasetPositions(
      detailedMatrix->getRowsAndColsUsedInDimReduction());

  style->resetStyle(styleDir);

  settingPanel->close();
  showSettingPanel();
}

void MainWindow::handleSaveMatrixData() { matrixView->saveMatrixFiles(); }

void MainWindow::handleShowSettingPanel() {
  settingPanel->close();
  showSettingPanel();
}

void MainWindow::updateDataDirectories(QString baseDataDir,
                                       QString brainMeshDir, QString roiDir,
                                       QString scannedDataDir,
                                       QString styleDir) {
  this->baseDataDir = baseDataDir;
  this->brainMeshDir = brainMeshDir;
  this->roiDir = roiDir;
  this->scannedDataDir = scannedDataDir;
  qDebug() << scannedDataDir;
  this->styleDir = styleDir;
}

void MainWindow::setAllData() {
  setBrainMesh();
  setRois();
  setScannedDataset();
  detailedMatrix = new DetailedMatrix(rois);
  dimReduction = new DimReduction("mds", "euclid", "ROI-ROI Matrix");
  setScannedDatasetPositions(
      detailedMatrix->getRowsAndColsUsedInDimReduction());
}

void MainWindow::setViews() {
  style = new Style(styleDir);
  comparisonView = new ComparisonView(style);
  comparisonView->setScanneDataset(scannedDataset);
  comparisonView->setRois(rois);
  comparisonView->updateMdsErrorVboAndCbo();
  matrixView = new MatrixView(style);
  matrixView->setDetailedMatrix(detailedMatrix);
  matrixView->setRois(rois);
  matrixView->setScannedDataset(scannedDataset);
  brainGraphView = new BrainGraphView(style);
  brainGraphView->setDetailedMatrix(detailedMatrix);
  brainGraphView->setRois(rois);
  brainGraphView->setScannedDataset(scannedDataset);
  brainGraphView->setBrainMesh(brainMesh);
  informationView = new InformationView(comboBoxForList, scannedDataset, rois,
                                        detailedMatrix);
  thresholdControlView = new ThresholdControlView(detailedMatrix, style);

  connect(comparisonView, &ComparisonView::showMatrixInDetailViewsSelected,
          this, &MainWindow::setMatrixAsDetailedMatrix);
  connect(comparisonView, &ComparisonView::showAveMatrixInDetailViewsSelected,
          this, &MainWindow::setAveMatrixAsDetailedMatrix);
  connect(comparisonView, &ComparisonView::showDiffMatrixInDetailViewsSelected,
          this, &MainWindow::setDiffMatrixAsDetailedMatrix);
  connect(comparisonView, &ComparisonView::showSdMatrixInDetailViewsSelected,
          this, &MainWindow::setSdMatrixAsDetailedMatrix);
  connect(comparisonView,
          &ComparisonView::showMaxMinMatrixInDetailViewsSelected, this,
          &MainWindow::setMaxMinMatrixAsDetailedMatrix);
  connect(comparisonView, &ComparisonView::recalcMdsSelected, this,
          &MainWindow::setScannedDatasetPositionsWithSelectedData);
  connect(comparisonView, &ComparisonView::selectedScannedDataChanged, this,
          &MainWindow::updateInformationView);
  connect(thresholdControlView, &ThresholdControlView::thresholdsChanged, this,
          &MainWindow::updateDetailedMatrix);
  connect(matrixView, &MatrixView::detailedMatrixFilteredOutChanged, this,
          &MainWindow::updateViewsRelatedWithMatrixFilteredOut);
  connect(matrixView, &MatrixView::recalcMdsSelected, this,
          &MainWindow::setScannedDatasetPositionsWithSelectedMatrixElements);
  connect(matrixView, &MatrixView::matrixOrderChanged, this,
          &MainWindow::updateViewsRelatedWithMatrixOrder);
  connect(matrixView, &MatrixView::roisSubsetChanged, this,
          &MainWindow::updateViewsRelatedWithMatrixOrderAndRoisSubset);
  connect(matrixView, &MatrixView::hoveredRowAndColIndexChanged, this,
          &MainWindow::updateBrainGraphView);
  connect(brainGraphView, &BrainGraphView::hoveredRoiIndexChanged, this,
          &MainWindow::updateMatrixView);
  connect(informationView, &InformationView::selectedScannedDataChanged, this,
          &MainWindow::updateComparisonView);
  connect(informationView, &InformationView::selectedRoisChanged, this,
          &MainWindow::updateMatrixView);
  connect(informationView, &InformationView::matrixOrderChanged, this,
          &MainWindow::updateViewsRelatedWithMatrixOrder);
  connect(informationView, &InformationView::roisSubsetChanged, this,
          &MainWindow::updateViewsRelatedWithMatrixOrderAndRoisSubset);
  connect(comparisonViewBtn, &QPushButton::clicked, this,
          &MainWindow::setComparisonViewInLargeWindow);
  connect(matrixViewBtn, &QPushButton::clicked, this,
          &MainWindow::setMatrixViewInLargeWindow);
  connect(brainGraphViewBtn, &QPushButton::clicked, this,
          &MainWindow::setBrainGraphViewInLargeWindow);
}

void MainWindow::setDetailUi() {
  QDesktopWidget desktop;
  this->setMinimumSize(500 * 1.75, 500);
  int height = (desktop.geometry().height() - 50) * 0.8;
  this->resize(height * 1.75, height);
  this->move(desktop.geometry().width() - height * 1.75, 0);

  setViews();

  stackedWidgetL->addWidget(comparisonView);
  stackedWidgetM1->addWidget(matrixView);
  stackedWidgetM2->addWidget(brainGraphView);
  stackedWidgetS->addWidget(thresholdControlView);
  stackedWidgetList->addWidget(informationView);
}

void MainWindow::setBrainMesh() {
  // const QString brainMeshDir = appDirPathToSrcDirPath() +
  // "../data/brain_mesh/";
  const QString brainMeshFilePath = brainMeshDir + "brain_mesh.txt";
  const QString spatialCorrectionFilePath =
      brainMeshDir + "spatial_correction.csv";
  brainMesh = new BrainMesh(brainMeshFilePath, spatialCorrectionFilePath);
}

void MainWindow::setRois() {
  // const QString roisFilePath = appDirPathToSrcDirPath() +
  // "../data/roi/rois.csv";
  const QString roisFilePath = roiDir + "rois.csv";
  rois = new Rois(roisFilePath);
}

void MainWindow::setScannedDataset() {
  // const QString scannedDataDir = appDirPathToSrcDirPath() +
  // "../data/scanned_data/";
  scannedDataset = new ScannedDataset(scannedDataDir);
  scannedDataset->setVisTargetMatrixKey("ROI-ROI Matrix");
}

void MainWindow::setScannedDatasetPositions(
    QVector<QPair<int, int>> targetLinks) {
  dimReduction->calcDimReductionPositions(
      scannedDataset, scannedDataset->getVisibleIndices(), targetLinks);
}

void MainWindow::setScannedDatasetPositionsAndUpdate(
    QVector<QPair<int, int>> targetLinks) {
  dimReduction->calcDimReductionPositions(
      scannedDataset, scannedDataset->getVisibleIndices(), targetLinks);
  informationView->updateItems();
  comparisonView->updateMdsErrorVboAndCbo();
  comparisonView->update();
}

void MainWindow::setScannedDatasetPositionsWithSettings(QString distCalcMethod,
                                                        QString matrixKey) {
  scannedDataset->setVisibleWithFiltering();
  scannedDataset->setVisTargetMatrixKey(matrixKey);
  dimReduction->setDistCalcMethod(distCalcMethod);
  dimReduction->setTargetMatrixKey(matrixKey);
  setScannedDatasetPositionsAndUpdate(
      detailedMatrix->getRowsAndColsUsedInDimReduction());
}

void MainWindow::setScannedDatasetPositionsWithSelectedData() {
  scannedDataset->setVisibleFromSelected();
  setScannedDatasetPositionsAndUpdate(
      detailedMatrix->getRowsAndColsUsedInDimReduction());
}

void MainWindow::setScannedDatasetPositionsWithSelectedMatrixElements() {
  setScannedDatasetPositionsAndUpdate(
      detailedMatrix->getRowsAndColsUsedInDimReduction());
  comparisonView->update();
  brainGraphView->update();
}

void MainWindow::setScannedDatasetFilteredOut(
    QMap<QString, QMap<QString, bool>> statusOfCategoryFilterCheckBoxes,
    QMap<QString, QPair<QString, QString>>
        statusOfNumberFilterOperatorsAndValues) {
  scannedDataset->setFilteredOutWithFilterStatus(
      statusOfCategoryFilterCheckBoxes, statusOfNumberFilterOperatorsAndValues);
  comparisonView->update();
}

void MainWindow::setScannedDatasetVisibleWithFiltering() {
  scannedDataset->setVisibleWithFiltering();
  informationView->updateItems();
  comparisonView->update();
}

void MainWindow::setScannedDatasetColorGroups(QString key) {
  scannedDataset->setScannedDataColorGroupsFromCategoryKey(key);
  comparisonView->update();
}

void MainWindow::setScannedDatasetShapeGroups(QString key) {
  scannedDataset->setScannedDataShapeGroupsFromCategoryKey(key);
  comparisonView->update();
}

void MainWindow::setScannedDatasetSizes(QString key) {
  scannedDataset->setScannedDataSizesFromNumberKey(key);
  comparisonView->update();
}

void MainWindow::setMatrixAsDetailedMatrix() {
  QVector<int> selectedIndices = scannedDataset->getSelectedIndices();
  if (selectedIndices.size() == 1) {
    int index = selectedIndices.at(0);
    detailedMatrix->initMatrix(
        scannedDataset->getScannedDataAt(index)->getMatrixAt(
            scannedDataset->getVisTargetMatrixKey()),
        -1.0, 1.0, -1.0, 1.0);
  }

  matrixView->update();
  thresholdControlView->update();
  brainGraphView->update();
  informationView->updateItems();
}

void MainWindow::setAveMatrixAsDetailedMatrix() {
  detailedMatrix->initMatrix(scannedDataset->genAveMatFromSelectedData(), -1.0,
                             1.0, -1.0, 1.0);

  matrixView->update();
  thresholdControlView->update();
  brainGraphView->update();
  informationView->updateItems();
}

void MainWindow::setDiffMatrixAsDetailedMatrix(int index1, int index2) {
  detailedMatrix->initMatrix(
      scannedDataset->genDiffMatFromSelectedData(index1, index2), -1.0, 1.0,
      -1.0, 1.0);

  matrixView->update();
  thresholdControlView->update();
  brainGraphView->update();
  informationView->updateItems();
}

void MainWindow::setSdMatrixAsDetailedMatrix() {
  detailedMatrix->initMatrix(scannedDataset->genSdMatFromSelectedData(), 0.0,
                             0.3, 0.0, 0.3);

  matrixView->update();
  thresholdControlView->update();
  brainGraphView->update();
  informationView->updateItems();
}

void MainWindow::setMaxMinMatrixAsDetailedMatrix() {
  detailedMatrix->initMatrix(scannedDataset->genMaxMinMatFromSelectedData(),
                             0.0, 2.0, 0.0, 2.0);

  matrixView->update();
  thresholdControlView->update();
  brainGraphView->update();
  informationView->updateItems();
}

void MainWindow::updateComparisonView() { comparisonView->update(); }

void MainWindow::updateDetailedMatrix() {
  detailedMatrix->updateVisibleMat();
  thresholdControlView->update();
  matrixView->update();
  brainGraphView->update();
  informationView->updateItems();
}

void MainWindow::updateMatrixView() { matrixView->update(); }

void MainWindow::updateBrainGraphView() { brainGraphView->update(); }

void MainWindow::updateInformationView() { informationView->updateItems(); }

void MainWindow::updateViewsRelatedWithMatrixOrder() {
  matrixView->update();
  brainGraphView->update();
  informationView->updateItems();
  comparisonView->update();
}

void MainWindow::updateViewsRelatedWithMatrixOrderAndRoisSubset() {
  detailedMatrix->updateUsedInDimReductionMatWithSubsetRoisInfo();
  setScannedDatasetPositionsAndUpdate(
      detailedMatrix->getRowsAndColsUsedInDimReduction());
  updateViewsRelatedWithMatrixOrder();
}

void MainWindow::updateViewsRelatedWithMatrixFilteredOut() {
  informationView->updateItems();
  brainGraphView->update();
}

void MainWindow::showSettingPanel() {
  settingPanel = new SettingPanel(scannedDataset, this);

  settingPanel->move(0, 0);
  settingPanel->show();
  settingPanel->raise();
  settingPanel->activateWindow();

  connect(settingPanel, &SettingPanel::mdsCalcMethodChanged, this,
          &MainWindow::setScannedDatasetPositionsWithSettings);
  connect(settingPanel, &SettingPanel::filterChanged, this,
          &MainWindow::setScannedDatasetFilteredOut);
  connect(settingPanel, &SettingPanel::applyFilteringClicked, this,
          &MainWindow::setScannedDatasetVisibleWithFiltering);
  connect(settingPanel, &SettingPanel::recalcMdsClicked, this,
          &MainWindow::setScannedDatasetPositionsWithSettings);
  connect(settingPanel, &SettingPanel::pointColorChanged, this,
          &MainWindow::setScannedDatasetColorGroups);
  connect(settingPanel, &SettingPanel::pointShapeChanged, this,
          &MainWindow::setScannedDatasetShapeGroups);
  connect(settingPanel, &SettingPanel::pointSizeChanged, this,
          &MainWindow::setScannedDatasetSizes);
}

void MainWindow::setComparisonViewInLargeWindow() {
  stackedWidgetL->removeWidget(stackedWidgetL->currentWidget());
  stackedWidgetM1->removeWidget(stackedWidgetM1->currentWidget());
  stackedWidgetM2->removeWidget(stackedWidgetM2->currentWidget());
  stackedWidgetL->addWidget(comparisonView);
  stackedWidgetM1->addWidget(matrixView);
  stackedWidgetM2->addWidget(brainGraphView);

  update();
}

void MainWindow::setMatrixViewInLargeWindow() {
  stackedWidgetL->removeWidget(stackedWidgetL->currentWidget());
  stackedWidgetM1->removeWidget(stackedWidgetM1->currentWidget());
  stackedWidgetM2->removeWidget(stackedWidgetM2->currentWidget());
  stackedWidgetL->addWidget(matrixView);
  stackedWidgetM1->addWidget(comparisonView);
  stackedWidgetM2->addWidget(brainGraphView);

  update();
}

void MainWindow::setBrainGraphViewInLargeWindow() {
  stackedWidgetL->removeWidget(stackedWidgetL->currentWidget());
  stackedWidgetM1->removeWidget(stackedWidgetM1->currentWidget());
  stackedWidgetM2->removeWidget(stackedWidgetM2->currentWidget());
  stackedWidgetL->addWidget(brainGraphView);
  stackedWidgetM1->addWidget(matrixView);
  stackedWidgetM2->addWidget(comparisonView);

  update();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  this->resize(this->height() * 1.75, this->height());
  stackedWidgetL->setMinimumSize(this->height() * 0.9, this->height() * 0.9);
  stackedWidgetM1->setMinimumSize(this->height() * 0.44, this->height() * 0.44);
  stackedWidgetM2->setMinimumSize(this->height() * 0.44, this->height() * 0.44);
  stackedWidgetS->setMinimumSize(this->height() * 0.44, this->height() * 0.05);
  comboBoxForList->setMinimumWidth(this->height() * 0.2);
  stackedWidgetList->setMinimumSize(this->height() * 0.2, this->height() * 0.9);
}
