#include "setting_panel.hpp"

SettingPanel::SettingPanel(ScannedDataset *scannedDataset, QWidget *parent)
    : QDialog(parent) {
  setWindowTitle("Setting Panel");

  QList<QString> categoryKeys;
  QList<QString> numberKeys;
  QList<QString> matrixKeys;

  if (scannedDataset->getNumOfScannedData() > 0) {
    categoryKeys = scannedDataset->getScannedDataAt(0)->getCategories().keys();
    numberKeys = scannedDataset->getScannedDataAt(0)->getNumbers().keys();
    matrixKeys = scannedDataset->getScannedDataAt(0)->getMatrices().keys();
  }

  ///
  /// Main
  ///
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->setMargin(0);

  // scrollview so all items fit in window
  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Window);
  scrollArea->setFrameShadow(QFrame::Plain);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setWidgetResizable(true);
  mainLayout->addWidget(scrollArea);

  // vertical box that contains all the checkboxes for the filters
  QWidget *scrollableWidget = new QWidget;
  // mainWidget->setObjectName("main");
  scrollableWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::MinimumExpanding);
  scrollableWidget->setLayout(new QVBoxLayout(scrollArea));
  scrollArea->setWidget(scrollableWidget);

  ///
  /// Matrix Used for MDS
  ///
  QGroupBox *matrixUsedForMdsGroupBox = new QGroupBox("Matrix Used for MDS");
  QVBoxLayout *matrixUsedForMdsLayout = new QVBoxLayout;
  QButtonGroup *matrixUsedForMdsBtnGroup = new QButtonGroup;
  scrollableWidget->layout()->addWidget(matrixUsedForMdsGroupBox);
  matrixUsedForMdsGroupBox->setLayout(matrixUsedForMdsLayout);
  matrixUsedForMdsLayout->addStretch(1);
  matrixUsedForMdsLayout->setMargin(6);

  for (int i = 0; i < matrixKeys.size(); ++i) {
    QRadioButton *radio = new QRadioButton(matrixKeys.at(i));
    matrixUsedForMdsLayout->addWidget(radio);
    matrixUsedForMdsBtnGroup->addButton(radio);
    radio->setFont(QFont("QFont::SansSeri", 10));
    radio->setAutoExclusive(true);
    if (i == 0) {
      radio->setChecked(true);
      matrixKeyUsedForMds = matrixKeys.at(i);
    }
  }
  connect(matrixUsedForMdsBtnGroup, SIGNAL(buttonClicked(QAbstractButton *)),
          this, SLOT(sendSignalMatrixUsedForMdsBtnChanged(QAbstractButton *)));

  ///
  /// Distance Calculation Method
  ///
  QGroupBox *mdsDistMethodGroupBox =
      new QGroupBox("MDS Distance Calculation Method");
  QVBoxLayout *mdsDistMethodLayout = new QVBoxLayout;
  QButtonGroup *mdsDistMethodBtnGroup = new QButtonGroup;
  scrollableWidget->layout()->addWidget(mdsDistMethodGroupBox);
  mdsDistMethodGroupBox->setLayout(mdsDistMethodLayout);
  mdsDistMethodLayout->addStretch(1);
  mdsDistMethodLayout->setMargin(6);

  QStringList mdsDistMethods =
      QStringList({"Euclidean distance", "Cosine distance", "Correlation"});
  for (int i = 0; i < mdsDistMethods.size(); ++i) {
    QRadioButton *radio = new QRadioButton(mdsDistMethods.at(i));
    mdsDistMethodLayout->addWidget(radio);
    mdsDistMethodBtnGroup->addButton(radio);
    radio->setFont(QFont("QFont::SansSeri", 10));
    radio->setAutoExclusive(true);
    if (i == 0) {
      radio->setChecked(true);
      mdsDistMethod = "euclid";
    }
  }
  connect(mdsDistMethodBtnGroup, SIGNAL(buttonClicked(QAbstractButton *)), this,
          SLOT(sendSignalMdsDistMethodBtnChanged(QAbstractButton *)));

  ///
  /// Filtering
  ///
  QGroupBox *filteringGroupBox = new QGroupBox("Filtering");
  QVBoxLayout *filteringLayout = new QVBoxLayout;
  scrollableWidget->layout()->addWidget(filteringGroupBox);
  filteringGroupBox->setLayout(filteringLayout);
  filteringLayout->setMargin(6);

  ///
  /// Filtering for Categorical Data
  ///
  filteringCategoryGroupBox = new QGroupBox("Category");
  QVBoxLayout *filteringCategoryLayout = new QVBoxLayout;
  filteringLayout->addWidget(filteringCategoryGroupBox);
  filteringCategoryGroupBox->setObjectName("Category");
  filteringCategoryGroupBox->setLayout(filteringCategoryLayout);
  filteringCategoryGroupBox->setFont(QFont("QFont::SansSeri", 10));
  filteringCategoryLayout->addStretch(1);
  filteringCategoryLayout->setContentsMargins(6, 15, 6, 6);
  for (int i = 0; i < categoryKeys.size(); ++i) {
    QGroupBox *groupBox = new QGroupBox(categoryKeys.at(i));
    QVBoxLayout *layout = new QVBoxLayout;
    filteringCategoryLayout->addWidget(groupBox);
    groupBox->setObjectName(categoryKeys.at(i));
    groupBox->setLayout(layout);
    groupBox->setFont(QFont("QFont::SansSeri", 10));
    groupBox->setFlat(true);
    layout->addStretch(1);
    layout->setMargin(6);
    layout->setContentsMargins(6, 0, 6, 6);

    QVector<QString> valuesOfCategories =
        scannedDataset->getValuesOfCategories()[categoryKeys.at(i)];
    for (int j = 0; j < valuesOfCategories.size(); ++j) {
      QCheckBox *checkBox = new QCheckBox(valuesOfCategories.at(j));
      layout->addWidget(checkBox);
      checkBox->setFont(QFont("QFont::SansSeri", 10));
      checkBox->setChecked(true);
      connect(checkBox, SIGNAL(clicked(bool)), this,
              SLOT(sendSignalFilterChanged()));
    }
  }

  ///
  /// Filtering for Numeric Data
  ///
  filteringNumberGroupBox = new QGroupBox("Number");
  QVBoxLayout *filteringNumberLayout = new QVBoxLayout;
  filteringLayout->addWidget(filteringNumberGroupBox);
  filteringNumberGroupBox->setObjectName("Number");
  filteringNumberGroupBox->setLayout(filteringNumberLayout);
  filteringNumberGroupBox->setFont(QFont("QFont::SansSeri", 10));
  filteringNumberLayout->addStretch(1);
  filteringNumberLayout->setContentsMargins(6, 0, 6, 0);
  filteringNumberLayout->setSpacing(0);
  for (int i = 0; i < numberKeys.size(); ++i) {
    QGroupBox *groupBox = new QGroupBox;
    QHBoxLayout *layout = new QHBoxLayout;
    QLabel *label = new QLabel(numberKeys.at(i));
    QComboBox *comboBox = new QComboBox;
    QLineEdit *lineEdit = new QLineEdit;
    filteringNumberLayout->addWidget(groupBox);
    groupBox->setObjectName(numberKeys.at(i));
    groupBox->setLayout(layout);
    groupBox->setFont(QFont("QFont::SansSeri", 10));
    groupBox->setFlat(true);
    layout->addStretch(1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    layout->addWidget(comboBox);
    layout->addWidget(lineEdit);
    label->setFont(QFont("QFont::SansSeri", 10));
    comboBox->setStyleSheet(QString("width: 10px;"));
    comboBox->setFont(QFont("QFont::SansSeri", 10));
    comboBox->addItems((QStringList() << ">"
                                      << ">="
                                      << "<"
                                      << "<="
                                      << "=="
                                      << "!="));
    lineEdit->setStyleSheet(QString("width: 40px;"));
    lineEdit->setFont(QFont("QFont::SansSeri", 10));
    connect(comboBox, SIGNAL(currentTextChanged(QString)), this,
            SLOT(sendSignalFilterChanged()));
    connect(lineEdit, SIGNAL(textChanged(QString)), this,
            SLOT(sendSignalFilterChanged()));
  }

  ///
  /// Apply and Recalc MDS Buttons
  ///
  QHBoxLayout *filteringBtnLayout = new QHBoxLayout;
  QPushButton *applyFilteringBtn = new QPushButton("Apply");
  QPushButton *recalcMdsBtn = new QPushButton("Recalc MDS");
  applyFilteringBtn->setFocusPolicy(Qt::NoFocus);
  recalcMdsBtn->setFocusPolicy(Qt::NoFocus);
  applyFilteringBtn->setFont(QFont("QFont::SansSeri", 10));
  recalcMdsBtn->setFont(QFont("QFont::SansSeri", 10));
  filteringLayout->addLayout(filteringBtnLayout);
  filteringBtnLayout->addWidget(applyFilteringBtn);
  filteringBtnLayout->addWidget(recalcMdsBtn);
  filteringBtnLayout->setMargin(0);
  filteringBtnLayout->setSpacing(3);
  filteringBtnLayout->setContentsMargins(0, 0, 0, 0);
  connect(applyFilteringBtn, SIGNAL(clicked(bool)), this,
          SLOT(sendSignalApplyFilteringClicked()));
  connect(recalcMdsBtn, SIGNAL(clicked(bool)), this,
          SLOT(sendSignalRecalcMdsClicked()));

  ///
  /// Visual Encoding
  ///
  QGroupBox *visualEncodingGroupBox = new QGroupBox("Visual Encoding");
  QVBoxLayout *visualEncodingLayout = new QVBoxLayout;
  scrollableWidget->layout()->addWidget(visualEncodingGroupBox);
  visualEncodingGroupBox->setLayout(visualEncodingLayout);
  visualEncodingLayout->setMargin(6);

  ///
  /// Visual Encoding (Point Color)
  ///
  QVBoxLayout *pointColorLayout = new QVBoxLayout;
  QButtonGroup *pointColorBtnGroup = new QButtonGroup;
  QLabel *pointColorLabel = new QLabel("Point Color");
  visualEncodingLayout->addLayout(pointColorLayout);
  pointColorLayout->addStretch(1);
  pointColorLayout->addWidget(pointColorLabel);
  pointColorLabel->setFont(QFont("QFont::SansSeri", 10));

  QRadioButton *radioNonePointColor = new QRadioButton("none");
  pointColorLayout->addWidget(radioNonePointColor);
  radioNonePointColor->setFont(QFont("QFont::SansSeri", 10));
  radioNonePointColor->setChecked(true);
  pointColorBtnGroup->addButton(radioNonePointColor);
  for (int i = 0; i < categoryKeys.size(); ++i) {
    QRadioButton *radio = new QRadioButton(categoryKeys.at(i));
    pointColorLayout->addWidget(radio);
    radio->setFont(QFont("QFont::SansSeri", 10));
    pointColorBtnGroup->addButton(radio);
  }
  connect(pointColorBtnGroup, SIGNAL(buttonClicked(QAbstractButton *)), this,
          SLOT(sendSignalPointColorBtnChanged(QAbstractButton *)));

  ///
  /// Visual Encoding (Point Shape)
  ///
  QVBoxLayout *pointShapeLayout = new QVBoxLayout;
  QButtonGroup *pointShapeBtnGroup = new QButtonGroup;
  QLabel *pointShapeLabel = new QLabel("Point Shape");
  visualEncodingLayout->addLayout(pointShapeLayout);
  pointShapeLayout->addStretch(1);
  pointShapeLayout->addWidget(pointShapeLabel);
  pointShapeLabel->setFont(QFont("QFont::SansSeri", 10));

  QRadioButton *radioNonePointShape = new QRadioButton("none");
  pointShapeLayout->addWidget(radioNonePointShape);
  radioNonePointShape->setFont(QFont("QFont::SansSeri", 10));
  radioNonePointShape->setChecked(true);
  pointShapeBtnGroup->addButton(radioNonePointShape);

  for (int i = 0; i < categoryKeys.size(); ++i) {
    QRadioButton *radio = new QRadioButton(categoryKeys.at(i));
    pointShapeLayout->addWidget(radio);
    radio->setFont(QFont("QFont::SansSeri", 10));
    pointShapeBtnGroup->addButton(radio);
  }
  connect(pointShapeBtnGroup, SIGNAL(buttonClicked(QAbstractButton *)), this,
          SLOT(sendSignalPointShapeBtnChanged(QAbstractButton *)));

  ///
  /// Visual Encoding (Point Size)
  ///
  QVBoxLayout *pointSizeLayout = new QVBoxLayout;
  QButtonGroup *pointSizeBtnGroup = new QButtonGroup;
  QLabel *pointSizeLabel = new QLabel("Point Size");
  visualEncodingLayout->addLayout(pointSizeLayout);
  pointSizeLayout->addStretch(1);
  pointSizeLayout->addWidget(pointSizeLabel);
  pointSizeLabel->setFont(QFont("QFont::SansSeri", 10));

  QRadioButton *radioNonePointSize = new QRadioButton("none");
  pointSizeLayout->addWidget(radioNonePointSize);
  radioNonePointSize->setFont(QFont("QFont::SansSeri", 10));
  radioNonePointSize->setChecked(true);
  pointSizeBtnGroup->addButton(radioNonePointSize);
  for (int i = 0; i < numberKeys.size(); ++i) {
    // QVector<float> minMaxOfNumbers =
    // scannedDataset->getMinAndMaxOfNumbers()[numberKeys.at(i)];
    QRadioButton *radio = new QRadioButton(
        numberKeys.at(i)
        //+ "(min:" + QString::number(minMaxOfNumbers.first())
        //+ ",max:" + QString::number(minMaxOfNumbers.last()) + ")"
    );
    pointSizeLayout->addWidget(radio);
    pointSizeBtnGroup->addButton(radio);
    radio->setFont(QFont("QFont::SansSeri", 10));
  }
  connect(pointSizeBtnGroup, SIGNAL(buttonClicked(QAbstractButton *)), this,
          SLOT(sendSignalPointSizeBtnChanged(QAbstractButton *)));

  // resize
  QDesktopWidget desktop;
  resize(250, (desktop.geometry().height() - 50) * 0.8);
}

SettingPanel::~SettingPanel() {}

void SettingPanel::sendSignalMatrixUsedForMdsBtnChanged(QAbstractButton *btn) {
  matrixKeyUsedForMds = btn->text();
  emit mdsCalcMethodChanged(mdsDistMethod, matrixKeyUsedForMds);
}

void SettingPanel::sendSignalMdsDistMethodBtnChanged(QAbstractButton *btn) {
  if (btn->text() == "Euclidean distance") {
    mdsDistMethod = "euclid";
  } else if (btn->text() == "Cosine distance") {
    mdsDistMethod = "cosine";
  } else if (btn->text() == "Correlation") {
    mdsDistMethod = "cor";
  }

  emit mdsCalcMethodChanged(mdsDistMethod, matrixKeyUsedForMds);
}

void SettingPanel::sendSignalFilterChanged() {
  // get all check box status for category
  QMap<QString, QMap<QString, bool>> statusOfCategoryFilterCheckBoxes;
  foreach (QGroupBox *keyGroupBox,
           filteringCategoryGroupBox->findChildren<QGroupBox *>()) {
    QMap<QString, bool> categoryValuesAndChecks;
    foreach (QCheckBox *checkBox, keyGroupBox->findChildren<QCheckBox *>()) {
      categoryValuesAndChecks.insert(checkBox->text(), checkBox->isChecked());
    }
    statusOfCategoryFilterCheckBoxes.insert(keyGroupBox->objectName(),
                                            categoryValuesAndChecks);
  }

  // get all comparison operator and values for number
  QMap<QString, QPair<QString, QString>> statusOfNumberFilterOperatorsAndValues;
  foreach (QGroupBox *keyGroupBox,
           filteringNumberGroupBox->findChildren<QGroupBox *>()) {
    QString ope = keyGroupBox->findChild<QComboBox *>()->currentText();
    QString value = keyGroupBox->findChild<QLineEdit *>()->text();
    QPair<QString, QString> operatorAndValues = qMakePair(ope, value);
    statusOfNumberFilterOperatorsAndValues.insert(keyGroupBox->objectName(),
                                                  operatorAndValues);
  }

  emit filterChanged(statusOfCategoryFilterCheckBoxes,
                     statusOfNumberFilterOperatorsAndValues);
}

void SettingPanel::sendSignalApplyFilteringClicked() {
  emit applyFilteringClicked();
}

void SettingPanel::sendSignalRecalcMdsClicked() {
  emit recalcMdsClicked(mdsDistMethod, matrixKeyUsedForMds);
}

void SettingPanel::sendSignalPointColorBtnChanged(QAbstractButton *btn) {
  emit pointColorChanged(btn->text());
}

void SettingPanel::sendSignalPointShapeBtnChanged(QAbstractButton *btn) {
  emit pointShapeChanged(btn->text());
}

void SettingPanel::sendSignalPointSizeBtnChanged(QAbstractButton *btn) {
  emit pointSizeChanged(btn->text());
}
