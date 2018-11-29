#include "subset_rois_setting_dialog.hpp"

SubsetRoisSettingDialog::SubsetRoisSettingDialog(Rois *rois, QWidget *parent) {
  setWindowTitle("Subset of ROIs Setting");

  this->rois = rois;
  inRangeSubsetList = new QListWidget;
  outOfRangeSubsetList = new QListWidget;
  inRangeSubsetList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  outOfRangeSubsetList->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QVBoxLayout *vBoxLayout = new QVBoxLayout;
  QHBoxLayout *hBoxLayoutMain = new QHBoxLayout;
  QHBoxLayout *hBoxLayoutCancelOk = new QHBoxLayout;
  setLayout(vBoxLayout);
  vBoxLayout->addLayout(hBoxLayoutMain);
  vBoxLayout->addLayout(hBoxLayoutCancelOk);
  QVBoxLayout *vBoxLayoutLeft = new QVBoxLayout;
  QVBoxLayout *vBoxLayoutCenter = new QVBoxLayout;
  QVBoxLayout *vBoxLayoutRight = new QVBoxLayout;
  hBoxLayoutMain->addLayout(vBoxLayoutLeft);
  hBoxLayoutMain->addLayout(vBoxLayoutCenter);
  hBoxLayoutMain->addLayout(vBoxLayoutRight);

  QLabel *inRangeSubsetLabel = new QLabel;
  inRangeSubsetLabel->setText("Rois used in analysis");
  vBoxLayoutLeft->addWidget(inRangeSubsetLabel);
  vBoxLayoutLeft->addWidget(inRangeSubsetList);

  QPushButton *toOutRangeSubsetBtn = new QPushButton;
  toOutRangeSubsetBtn->setText(">");
  toOutRangeSubsetBtn->setFocusPolicy(Qt::NoFocus);
  QPushButton *toInRangeSubsetBtn = new QPushButton;
  toInRangeSubsetBtn->setText("<");
  toInRangeSubsetBtn->setFocusPolicy(Qt::NoFocus);
  vBoxLayoutCenter->addWidget(toOutRangeSubsetBtn);
  vBoxLayoutCenter->addWidget(toInRangeSubsetBtn);

  QLabel *outRangeSubsetLabel = new QLabel;
  outRangeSubsetLabel->setText("Rois not used in analysis");
  vBoxLayoutRight->addWidget(outRangeSubsetLabel);
  vBoxLayoutRight->addWidget(outOfRangeSubsetList);

  QSpacerItem *hSpacer =
      new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QPushButton *cancelBtn = new QPushButton;
  cancelBtn->setText("Cancel");
  cancelBtn->setFocusPolicy(Qt::NoFocus);
  QPushButton *okBtn = new QPushButton;
  okBtn->setText("OK");
  hBoxLayoutCancelOk->addSpacerItem(hSpacer);
  hBoxLayoutCancelOk->addWidget(cancelBtn);
  hBoxLayoutCancelOk->addWidget(okBtn);

  connect(toOutRangeSubsetBtn, &QPushButton::clicked, this,
          &SubsetRoisSettingDialog::handleToOutOfRangeSubsetBtn);
  connect(toInRangeSubsetBtn, &QPushButton::clicked, this,
          &SubsetRoisSettingDialog::handleToInRangeSubsetBtn);
  connect(cancelBtn, &QPushButton::clicked, this,
          &SubsetRoisSettingDialog::handleCancelBtn);
  connect(okBtn, &QPushButton::clicked, this,
          &SubsetRoisSettingDialog::handleOkBtn);

  initSubsetLists();
}

SubsetRoisSettingDialog::~SubsetRoisSettingDialog() {}

void SubsetRoisSettingDialog::initSubsetLists() {
  inRangeSubsetList->clear();
  outOfRangeSubsetList->clear();

  foreach (int roiIndex, rois->getOrder()) {
    inRangeSubsetList->addItem(rois->getRoiAt(roiIndex)->getName());
  }

  for (int i = 0; i < rois->getNumOfRois(); ++i) {
    if (rois->getRoiAt(i)->getRank() < 0) {
      outOfRangeSubsetList->addItem(rois->getRoiAt(i)->getName());
    }
  }
}

void SubsetRoisSettingDialog::handleToOutOfRangeSubsetBtn() {
  // remove items from inRangeSubsetList
  QList<int> selectedIndices;
  foreach (QListWidgetItem *item, inRangeSubsetList->selectedItems()) {
    int index = inRangeSubsetList->row(item);
    selectedIndices.push_back(index);
  }
  qSort(selectedIndices.begin(), selectedIndices.end(), qGreater<int>());

  QList<QListWidgetItem *> takenItems;
  foreach (int index, selectedIndices) {
    takenItems.push_back(inRangeSubsetList->takeItem(index));
  }

  // add items to outOfRangeSubsetList
  foreach (QListWidgetItem *item, takenItems) {
    outOfRangeSubsetList->insertItem(0, item);
  }
}

void SubsetRoisSettingDialog::handleToInRangeSubsetBtn() {
  // remove items from inRangeSubsetList
  QList<int> selectedIndices;
  foreach (QListWidgetItem *item, outOfRangeSubsetList->selectedItems()) {
    int index = outOfRangeSubsetList->row(item);
    selectedIndices.push_back(index);
  }
  qSort(selectedIndices.begin(), selectedIndices.end(), qGreater<int>());

  QList<QListWidgetItem *> takenItems;
  foreach (int index, selectedIndices) {
    takenItems.push_back(outOfRangeSubsetList->takeItem(index));
  }

  // add items to outOfRangeSubsetList
  foreach (QListWidgetItem *item, takenItems) {
    inRangeSubsetList->insertItem(0, item);
  }
}

void SubsetRoisSettingDialog::handleCancelBtn() { close(); }

void SubsetRoisSettingDialog::handleOkBtn() {
  QVector<int> order;

  for (int i = 0; i < inRangeSubsetList->count(); ++i) {
    int roiIndex = rois->getIndexFromName(inRangeSubsetList->item(i)->text());
    order.push_back(roiIndex);
  }

  rois->setOrder(order);

  emit roisOrderChanged();
  close();
}
