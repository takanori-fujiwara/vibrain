#include "data_directory_selection_dialog.hpp"

DataDirectorySelectionDialog::DataDirectorySelectionDialog(QWidget *parent)
{
    baseDataDir = "";
    brainMeshDir = "";
    roiDir = "";
    scannedDataDir = "";
    styleDir = "";

    baseDataDirLineEdit = new QLineEdit;
    brainMeshDirLineEdit = new QLineEdit;
    roiDirLineEdit = new QLineEdit;
    scannedDataDirLineEdit = new QLineEdit;
    styleDirLineEdit = new QLineEdit;

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QHBoxLayout *baseDataDirHBoxLayout = new QHBoxLayout;
    QGroupBox *detailDirGroupBox = new QGroupBox;
    QVBoxLayout *detailDirVBoxLayout = new QVBoxLayout;
    QHBoxLayout *brainMeshDirHBoxLayout = new QHBoxLayout;
    QHBoxLayout *roiDirHBoxLayout = new QHBoxLayout;
    QHBoxLayout *scannedDataDirHBoxLayout = new QHBoxLayout;
    QHBoxLayout *styleDirHBoxLayout = new QHBoxLayout;
    QHBoxLayout *cancelOkHBoxLayout = new QHBoxLayout;

    QLabel *explanationLabel = new QLabel;
    QLabel *detailDirExplanationLabel = new QLabel;
    QLabel *baseDataDirLabel = new QLabel;
    QLabel *brainMeshDirLabel = new QLabel;
    QLabel *roiDirLabel = new QLabel;
    QLabel *scannedDataDirLabel = new QLabel;
    QLabel *styleDirLabel = new QLabel;

    QPushButton *baseDataDirPushBtn = new QPushButton;
    QPushButton *brainMeshDirPushBtn = new QPushButton;
    QPushButton *roiDirPushBtn = new QPushButton;
    QPushButton *scannedDataDirPushBtn = new QPushButton;
    QPushButton *styleDirPushBtn = new QPushButton;
    QPushButton *cancelBtn = new QPushButton;
    QPushButton *okBtn = new QPushButton;

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

    this->setWindowTitle("Select Data Directory");
    explanationLabel->setText("Select directories of necessary data");
    explanationLabel->setFont(QFont("QFont::SansSeri", 13, QFont::Bold));
    detailDirExplanationLabel->setText("Detail directory settings");
    baseDataDirLabel->setText("Base data directory");
    brainMeshDirLabel->setText("Brain mesh data directory");
    roiDirLabel->setText("ROI data directory");
    scannedDataDirLabel->setText("Scanned data directory");
    styleDirLabel->setText("Style data directory");

    baseDataDirLineEdit->setText(baseDataDir);
    baseDataDirLineEdit->setReadOnly(true);
    baseDataDirPushBtn->setText("Select");
    brainMeshDirPushBtn->setText("Select");
    roiDirPushBtn->setText("Select");
    scannedDataDirPushBtn->setText("Select");
    styleDirPushBtn->setText("Select");
    cancelBtn->setText("Cancel");
    okBtn->setText("OK");

    brainMeshDirLabel->setMinimumWidth(200);
    brainMeshDirLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    roiDirLabel->setMinimumWidth(200);
    roiDirLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    scannedDataDirLabel->setMinimumWidth(200);
    scannedDataDirLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    styleDirLabel->setMinimumWidth(200);
    styleDirLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    baseDataDirLineEdit->setMinimumWidth(400);
    brainMeshDirLineEdit->setMinimumWidth(400);
    roiDirLineEdit->setMinimumWidth(400);
    scannedDataDirLineEdit->setMinimumWidth(400);
    styleDirLineEdit->setMinimumWidth(400);

    baseDataDirLineEdit->setFocusPolicy(Qt::NoFocus);
    brainMeshDirLineEdit->setFocusPolicy(Qt::NoFocus);
    roiDirLineEdit->setFocusPolicy(Qt::NoFocus);
    scannedDataDirLineEdit->setFocusPolicy(Qt::NoFocus);
    styleDirLineEdit->setFocusPolicy(Qt::NoFocus);
    baseDataDirPushBtn->setFocusPolicy(Qt::NoFocus);
    brainMeshDirPushBtn->setFocusPolicy(Qt::NoFocus);
    roiDirPushBtn->setFocusPolicy(Qt::NoFocus);
    scannedDataDirPushBtn->setFocusPolicy(Qt::NoFocus);
    styleDirPushBtn->setFocusPolicy(Qt::NoFocus);
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    okBtn->setFocusPolicy(Qt::NoFocus);

    this->setLayout(vBoxLayout);
    vBoxLayout->addWidget(explanationLabel);
    vBoxLayout->addLayout(baseDataDirHBoxLayout);
    vBoxLayout->addWidget(detailDirGroupBox);
    detailDirGroupBox->setLayout(detailDirVBoxLayout);
    detailDirVBoxLayout->addWidget(detailDirExplanationLabel);
    detailDirVBoxLayout->addLayout(brainMeshDirHBoxLayout);
    detailDirVBoxLayout->addLayout(roiDirHBoxLayout);
    detailDirVBoxLayout->addLayout(scannedDataDirHBoxLayout);
    detailDirVBoxLayout->addLayout(styleDirHBoxLayout);
    vBoxLayout->addLayout(cancelOkHBoxLayout);

    baseDataDirHBoxLayout->addWidget(baseDataDirLabel);
    baseDataDirHBoxLayout->addWidget(baseDataDirLineEdit);
    baseDataDirHBoxLayout->addWidget(baseDataDirPushBtn);

    brainMeshDirHBoxLayout->addWidget(brainMeshDirLabel);
    brainMeshDirHBoxLayout->addWidget(brainMeshDirLineEdit);
    brainMeshDirHBoxLayout->addWidget(brainMeshDirPushBtn);

    roiDirHBoxLayout->addWidget(roiDirLabel);
    roiDirHBoxLayout->addWidget(roiDirLineEdit);
    roiDirHBoxLayout->addWidget(roiDirPushBtn);

    scannedDataDirHBoxLayout->addWidget(scannedDataDirLabel);
    scannedDataDirHBoxLayout->addWidget(scannedDataDirLineEdit);
    scannedDataDirHBoxLayout->addWidget(scannedDataDirPushBtn);

    styleDirHBoxLayout->addWidget(styleDirLabel);
    styleDirHBoxLayout->addWidget(styleDirLineEdit);
    styleDirHBoxLayout->addWidget(styleDirPushBtn);

    cancelOkHBoxLayout->addSpacerItem(hSpacer);
    //cancelOkHBoxLayout->addWidget(cancelBtn);
    cancelOkHBoxLayout->addWidget(okBtn);

    setCurrentSelectedDirecotries();

    connect(baseDataDirPushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setBaseDataDir);
    connect(brainMeshDirPushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setBrainMeshDir);
    connect(roiDirPushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setRoiDir);
    connect(scannedDataDirPushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setScannedDataDir);
    connect(styleDirPushBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::setStyleDir);
    connect(cancelBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::close);
    connect(okBtn, &QPushButton::clicked, this, &DataDirectorySelectionDialog::handleOk);
}

DataDirectorySelectionDialog::~DataDirectorySelectionDialog()
{

}

void DataDirectorySelectionDialog::setBaseDataDir()
{
    baseDataDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "",
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    brainMeshDir = baseDataDir + "brain_mesh/";
    roiDir = baseDataDir + "roi/";
    scannedDataDir = baseDataDir + "scanned_data/";
    styleDir = baseDataDir + "style/";

    baseDataDirLineEdit->setText(baseDataDir);
    brainMeshDirLineEdit->setText(brainMeshDir);
    roiDirLineEdit->setText(roiDir);
    scannedDataDirLineEdit->setText(scannedDataDir);
    styleDirLineEdit->setText(styleDir);
}

void DataDirectorySelectionDialog::setBrainMeshDir()
{
    brainMeshDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), baseDataDir,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    brainMeshDirLineEdit->setText(brainMeshDir);
}

void DataDirectorySelectionDialog::setRoiDir()
{
    roiDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), baseDataDir,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    roiDirLineEdit->setText(roiDir);
}

void DataDirectorySelectionDialog::setScannedDataDir()
{
    scannedDataDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), baseDataDir,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    scannedDataDirLineEdit->setText(scannedDataDir);
}

void DataDirectorySelectionDialog::setStyleDir()
{
    styleDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), baseDataDir,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks) + "/";
    styleDirLineEdit->setText(styleDir);
}

void DataDirectorySelectionDialog::handleOk()
{
    if (QDir(baseDataDir).exists() && QDir(brainMeshDir).exists() && QDir(roiDir).exists()
            && QDir(scannedDataDir).exists() && QDir(styleDir).exists()) {
        saveDirectories();
        emit dataDirecotriesChanged(baseDataDir, brainMeshDir, roiDir, scannedDataDir, styleDir);
        this->close();
    } else {
        QMessageBox msgBox;
        if (!QDir(baseDataDir).exists()) {
            msgBox.setText(baseDataDir + " does not exist.");
            msgBox.exec();
        } else if (!QDir(brainMeshDir).exists()) {
            msgBox.setText(brainMeshDir + " does not exist.");
            msgBox.exec();
        } else if (!QDir(roiDir).exists()) {
            msgBox.setText(roiDir + " does not exist.");
            msgBox.exec();
        } else if (!QDir(scannedDataDir).exists()) {
            msgBox.setText(scannedDataDir + " does not exist.");
            msgBox.exec();
        } else if (!QDir(styleDir).exists()) {
            msgBox.setText(styleDir + " does not exist.");
            msgBox.exec();
        }
    }
}

void DataDirectorySelectionDialog::saveDirectories()
{
    QSettings settings("VIDI", "ViBrain");

    settings.setValue("data_directory_selection_dialog/baseDataDir", baseDataDir);
    settings.setValue("data_directory_selection_dialog/brainMeshDir", brainMeshDir);
    settings.setValue("data_directory_selection_dialog/roiDir", roiDir);
    settings.setValue("data_directory_selection_dialog/scannedDataDir", scannedDataDir);
    settings.setValue("data_directory_selection_dialog/styleDir", styleDir);
}

void DataDirectorySelectionDialog::setCurrentSelectedDirecotries()
{
    QSettings settings("VIDI", "ViBrain");

    if (settings.value("data_directory_selection_dialog/baseDataDir").isValid()) {
        baseDataDirLineEdit->setText(settings.value("data_directory_selection_dialog/baseDataDir").toString());
        baseDataDir = baseDataDirLineEdit->text();
    }
    if (settings.value("data_directory_selection_dialog/brainMeshDir").isValid()) {
        brainMeshDirLineEdit->setText(settings.value("data_directory_selection_dialog/brainMeshDir").toString());
        brainMeshDir = brainMeshDirLineEdit->text();
    }
    if (settings.value("data_directory_selection_dialog/roiDir").isValid()) {
        roiDirLineEdit->setText(settings.value("data_directory_selection_dialog/roiDir").toString());
        roiDir = roiDirLineEdit->text();
    }
    if (settings.value("data_directory_selection_dialog/scannedDataDir").isValid()) {
        scannedDataDirLineEdit->setText(settings.value("data_directory_selection_dialog/scannedDataDir").toString());
        scannedDataDir = scannedDataDirLineEdit->text();
    }
    if (settings.value("data_directory_selection_dialog/styleDir").isValid()) {
        styleDirLineEdit->setText(settings.value("data_directory_selection_dialog/styleDir").toString());
        styleDir = styleDirLineEdit->text();
    }
}
