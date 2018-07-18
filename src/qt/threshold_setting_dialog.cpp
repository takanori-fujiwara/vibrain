#include "threshold_setting_dialog.hpp"

ThresholdSettingDialog::ThresholdSettingDialog(DetailedMatrix *detailedMatrix, QWidget *parent)
{
    setWindowTitle("Threshold Setting");

    this->detailedMatrix = detailedMatrix;
    thresMinLineEdit = new QLineEdit;
    lowerThresLineEdit = new QLineEdit;
    upperThresLineEdit = new QLineEdit;
    thresMaxLineEdit = new QLineEdit;
    rangeComboBox = new QComboBox;
    errorMsgLabel = new QLabel;

    thresMinLineEdit->setText(QString::number(detailedMatrix->getMinThresVal()));
    lowerThresLineEdit->setText(QString::number(detailedMatrix->getLowerThresVal()));
    upperThresLineEdit->setText(QString::number(detailedMatrix->getUpperThresVal()));
    thresMaxLineEdit->setText(QString::number(detailedMatrix->getMaxThresVal()));
    rangeComboBox->addItem("between thresholds");
    rangeComboBox->addItem("outside thresholds");
    if (detailedMatrix->getThresRangeType() == "outside") {
        rangeComboBox->setCurrentIndex(1);
    }

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    QHBoxLayout *hBoxLayoutMain = new QHBoxLayout;
    QHBoxLayout *hBoxLayoutRangeSelection = new QHBoxLayout;
    QHBoxLayout *hBoxLayoutCancelOk = new QHBoxLayout;
    setLayout(vBoxLayout);
    vBoxLayout->addLayout(hBoxLayoutMain);
    vBoxLayout->addLayout(hBoxLayoutRangeSelection);
    vBoxLayout->addLayout(hBoxLayoutCancelOk);

    QVBoxLayout *vBoxLayoutThresMin = new QVBoxLayout;
    QVBoxLayout *vBoxLayoutLowerThres = new QVBoxLayout;
    QVBoxLayout *vBoxLayoutUpperThres = new QVBoxLayout;
    QVBoxLayout *vBoxLayoutThresMax = new QVBoxLayout;
    hBoxLayoutMain->addLayout(vBoxLayoutThresMin);
    hBoxLayoutMain->addLayout(vBoxLayoutLowerThres);
    hBoxLayoutMain->addLayout(vBoxLayoutUpperThres);
    hBoxLayoutMain->addLayout(vBoxLayoutThresMax);

    QLabel *thresMinLabel = new QLabel;
    thresMinLabel->setText("Thres Min");
    vBoxLayoutThresMin->addWidget(thresMinLabel);
    vBoxLayoutThresMin->addWidget(thresMinLineEdit);

    QLabel *lowerThresLabel = new QLabel;
    lowerThresLabel->setText("Lower Thres");
    vBoxLayoutLowerThres->addWidget(lowerThresLabel);
    vBoxLayoutLowerThres->addWidget(lowerThresLineEdit);

    QLabel *upperThresLabel = new QLabel;
    upperThresLabel->setText("Upper Thres");
    vBoxLayoutUpperThres->addWidget(upperThresLabel);
    vBoxLayoutUpperThres->addWidget(upperThresLineEdit);

    QLabel *thresMaxLabel = new QLabel;
    thresMaxLabel->setText("Thres Max");
    vBoxLayoutThresMax->addWidget(thresMaxLabel);
    vBoxLayoutThresMax->addWidget(thresMaxLineEdit);

    QLabel *rangeMethodLabel = new QLabel;
    rangeMethodLabel->setText("Threshold Range Type");
    hBoxLayoutRangeSelection->addWidget(rangeMethodLabel);
    hBoxLayoutRangeSelection->addWidget(rangeComboBox);

    QSpacerItem *hSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QPalette* palette = new QPalette();
    palette->setColor(QPalette::WindowText,Qt::red);
    errorMsgLabel->setPalette(*palette);
    errorMsgLabel->setText("");
    QPushButton *cancelBtn = new QPushButton;
    cancelBtn->setText("Cancel");
    cancelBtn->setFocusPolicy(Qt::NoFocus);
    QPushButton *okBtn = new QPushButton;
    okBtn->setText("OK");
    hBoxLayoutCancelOk->addSpacerItem(hSpacer);
    hBoxLayoutCancelOk->addWidget(errorMsgLabel);
    hBoxLayoutCancelOk->addWidget(cancelBtn);
    hBoxLayoutCancelOk->addWidget(okBtn);

//    connect(toOutRangeSubsetBtn, &QPushButton::clicked, this, &SubsetRoisSettingDialog::handleToOutOfRangeSubsetBtn);
//    connect(toInRangeSubsetBtn, &QPushButton::clicked, this, &SubsetRoisSettingDialog::handleToInRangeSubsetBtn);
    connect(cancelBtn, &QPushButton::clicked, this, &ThresholdSettingDialog::handleCancelBtn);
    connect(okBtn, &QPushButton::clicked, this, &ThresholdSettingDialog::handleOkBtn);

//    initSubsetLists();
}

ThresholdSettingDialog::~ThresholdSettingDialog()
{

}

void ThresholdSettingDialog::handleCancelBtn()
{
    close();
}

void ThresholdSettingDialog::handleOkBtn()
{
    bool isFloatString;

    float thresMin = thresMinLineEdit->text().toFloat(&isFloatString);
    if (!isFloatString) {
        errorMsgLabel->setText("Thres min is invalid format");
        return;
    }

    float lowerThres = lowerThresLineEdit->text().toFloat(&isFloatString);
    if (!isFloatString) {
        errorMsgLabel->setText("Lower thres is invalid format");
        return;
    }

    float upperThres = upperThresLineEdit->text().toFloat(&isFloatString);
    if (!isFloatString) {
        errorMsgLabel->setText("Upper thres is invalid format");
        return;
    }

    float thresMax = thresMaxLineEdit->text().toFloat(&isFloatString);
    if (!isFloatString) {
        errorMsgLabel->setText("Thres max is invalid format");
        return;
    }

    if (!(thresMin <= lowerThres)) {
        errorMsgLabel->setText("Lower thres must be equal or larger than thres min");
        return;
    }

    if (!(lowerThres <= upperThres)) {
        errorMsgLabel->setText("Upper thres must be equal or larger than lower thres");
        return;
    }

    if (!(upperThres <= thresMax)) {
        errorMsgLabel->setText("Thres max must be equal or larger than upper thres");
        return;
    }

    detailedMatrix->setMinThresVal(thresMin);
    detailedMatrix->setLowerThresVal(lowerThres);
    detailedMatrix->setUpperThresVal(upperThres);
    detailedMatrix->setMaxThresVal(thresMax);

    if (rangeComboBox->currentText() == "within sliders") {
        detailedMatrix->setThresRangeType("between");
    } else if (rangeComboBox->currentText() == "outside sliders") {
        detailedMatrix->setThresRangeType("outside");
    }

    emit thresholdsChanged();

    close();
}

