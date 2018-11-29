#include "information_view.hpp"

InformationView::InformationView(QComboBox *comboBox,
                                 ScannedDataset *scannedDataset, Rois *rois,
                                 DetailedMatrix *detailedMatrix,
                                 QWidget *parent) {
  setFont(QFont("QFont::SansSeri", 10));
  setSelectionMode(QAbstractItemView::ExtendedSelection);

  this->comboBox = comboBox;
  comboBox->setFont(QFont("QFont::SansSeri", 10));
  this->scannedDataset = scannedDataset;
  this->rois = rois;
  this->detailedMatrix = detailedMatrix;

  this->comboBox->clear();
  this->comboBox->addItem("Displayed Scanned Data");
  this->comboBox->addItem("Order of Matrix");
  this->comboBox->addItem("Displayed ROIs in Brain Graph View");
  this->comboBox->addItem("Displayed Edges in Brain Graph View");
  // this->comboBox->addItem("Selected Rows and Columns in Matrix");

  setLists(comboBox->currentText());
  setViewMode(QListWidget::ListMode);
  addItems(lists);
  setListsSelected(selectedLists);

  connect(this->comboBox, &QComboBox::currentTextChanged, this,
          &InformationView::updateItems);
  connect(this, &InformationView::itemSelectionChanged, this,
          &InformationView::handleSelectedItem);
  // connect(this->model(), &QAbstractItemModel::rowsMoved, this,
  // &InformationView::handleDropEvent);
}

void InformationView::setLists(QString comboBoxCurrentText) {
  lists.clear();
  selectedLists.clear();

  if (comboBoxCurrentText == "Displayed Scanned Data") {
    foreach (ScannedData *scannedData, scannedDataset->getScannedDataset()) {
      if (scannedData->getVisible()) {
        lists.push_back(scannedData->getFileName());
        if (scannedData->getSelected())
          selectedLists.push_back(true);
        else
          selectedLists.push_back(false);
      }
    }
  } else if (comboBoxCurrentText == "Order of Matrix") {
    if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
      foreach (int index, rois->getOrder()) {
        if (index >= 0)
          lists.push_back(rois->getRoiAt(index)->getName());
      }
    } else {
      // TODO: support later
      lists.push_back("selected mat is not supported");
    }
  } else if (comboBoxCurrentText == "Displayed ROIs in Brain Graph View") {
    if (scannedDataset->getVisTargetMatrixKey() == "ROI-ROI Matrix") {
      QVector<int> degrees = detailedMatrix->getNodeDegreeFromDisplayedInfo();
      int degreeThres = detailedMatrix->getDegreeThres();
      for (int i = 0; i < degrees.size(); ++i) {
        if (degrees.at(i) > degreeThres) {
          if (rois->getNumOfRois() > i)
            lists.push_back(rois->getRoiAt(i)->getName());
        }
      }
    }
  } else if (comboBoxCurrentText == "Displayed Links in Brain Graph View") {
    for (int i = 0; i < detailedMatrix->getNumOfRows(); ++i) {
      for (int j = i; j < detailedMatrix->getNumOfCols(); ++j) {
        if (detailedMatrix->getVisibleAt(i, j) &&
            !detailedMatrix->getFilteredOutAt(i, j) &&
            detailedMatrix->getUsedInDimReductionAt(i, j) &&
            rois->getRoiAt(i)->getRank() >= 0 &&
            rois->getRoiAt(j)->getRank() >= 0) {
          lists.push_back(rois->getRoiAt(i)->getName() + " - " +
                          rois->getRoiAt(j)->getName());
        }
      }
    }
  }
}

void InformationView::setListsSelected(QList<bool> selectedLists) {
  for (int i = 0; i < selectedLists.size(); ++i) {
    if (selectedLists.at(i))
      this->item(i)->setSelected(true);
    else
      this->item(i)->setSelected(false);
  }
}

void InformationView::updateItems() {
  if (comboBox->currentText() == "Order of Matrix")
    setDragDropMode(QAbstractItemView::InternalMove);
  else
    setDragDropMode(QAbstractItemView::NoDragDrop);

  setLists(comboBox->currentText());
  clear();
  addItems(lists);
  setListsSelected(selectedLists);
}

void InformationView::handleSelectedItem() {
  if (comboBox->currentText() == "Displayed Scanned Data") {
    for (int i = 0; i < this->count(); ++i) {
      QListWidgetItem *item = this->item(i);
      if (item->isSelected())
        scannedDataset->getScannedDataFromFileName(item->text())
            ->setSelected(true);
      else
        scannedDataset->getScannedDataFromFileName(item->text())
            ->setSelected(false);
    }
    emit selectedScannedDataChanged();
  } else if (comboBox->currentText() == "Order of Matrix" ||
             comboBox->currentText() == "Displayed ROIs in Brain Graph View") {
    detailedMatrix->setFalseEntireSelectedMat();
    for (int i = 0; i < this->count(); ++i) {
      QListWidgetItem *item = this->item(i);
      if (item->isSelected()) {
        int index = rois->getIndexFromName(item->text());
        detailedMatrix->setSelectedMatByRow(index, true);
        detailedMatrix->setSelectedMatByCol(index, true);
      }
    }
    emit selectedRoisChanged();
  }
  // else if (comboBox->currentText() == "Selected Rows and Columns in Matrix")
  // {
  //
  //}
}

void InformationView::handleDropEvent() {
  lists.clear();
  for (int i = 0; i < this->count(); ++i) {
    QListWidgetItem *item = this->item(i);
    lists.push_back(item->text());
  }

  if (comboBox->currentText() == "Order of Matrix") {
    QVector<int> newOrder;
    for (int i = 0; i < lists.size(); ++i) {
      newOrder.push_back(rois->getIndexFromName(lists.at(i)));
    }

    rois->setOrder(newOrder);

    emit matrixOrderChanged();
  }
}

void InformationView::dropEvent(QDropEvent *event) {
  // drag UI for multiple items
  QList<int> selectedIndices;
  foreach (QModelIndex modelIndex, selectedIndexes()) {
    selectedIndices.push_back(modelIndex.row());
  }
  qSort(selectedIndices.begin(), selectedIndices.end(), qGreater<int>());

  int dropIndex = indexAt(event->pos()).row();
  int insertPositionDelta = 0;
  if (dropIndicatorPosition() == 1) { // above
    insertPositionDelta = -1;
  } else if (dropIndicatorPosition() == 2) { // below
    insertPositionDelta = 1;
  }

  int insertPosition = dropIndex + insertPositionDelta;

  foreach (int index, selectedIndices) {
    if (index < insertPosition)
      insertPosition--;
  }
  if (insertPosition < 0)
    insertPosition = 0;

  QList<QListWidgetItem *> takenItems;
  foreach (int index, selectedIndices) {
    takenItems.push_back(takeItem(index));
  }
  foreach (QListWidgetItem *item, takenItems) {
    insertItem(insertPosition, item);
  }

  // update lists
  lists.clear();
  for (int i = 0; i < this->count(); ++i) {
    QListWidgetItem *item = this->item(i);
    lists.push_back(item->text());
  }

  if (comboBox->currentText() == "Order of Matrix") {
    QVector<int> newOrder;
    for (int i = 0; i < lists.size(); ++i) {
      newOrder.push_back(rois->getIndexFromName(lists.at(i)));
    }

    rois->setOrder(newOrder);

    emit matrixOrderChanged();
  }
}

void InformationView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    showContextMenu(event->pos());
  }

  mouseReleasePos = event->pos();
}

void InformationView::showContextMenu(const QPoint &pos) {
  QMenu contextMenu(tr("Context menu"), this);

  QAction actSettingSubsetRois("Setting a subset of ROIs", this);
  QAction actResetRoisOrder("Reset order of ROIs", this);

  connect(&actSettingSubsetRois, &QAction::triggered, this,
          &InformationView::showSettingSubsetRoisDialog);
  connect(&actResetRoisOrder, &QAction::triggered, this,
          &InformationView::resetRoisOrder);

  if (comboBox->currentText() == "Order of Matrix") {
    contextMenu.addAction(&actSettingSubsetRois);
    contextMenu.addAction(&actResetRoisOrder);
  }

  contextMenu.exec(mapToGlobal(pos));
}

void InformationView::showSettingSubsetRoisDialog() {
  SubsetRoisSettingDialog *subsetRoisSettingDialog =
      new SubsetRoisSettingDialog(rois);
  connect(subsetRoisSettingDialog, &SubsetRoisSettingDialog::roisOrderChanged,
          this, &InformationView::changeRoisOrderFromDialog);
  subsetRoisSettingDialog->show();
}

void InformationView::changeRoisOrderFromDialog() { emit roisSubsetChanged(); }

void InformationView::resetRoisOrder() {
  rois->resetOrderWithInitialOrder();
  emit roisSubsetChanged();
}

void InformationView::setScannedDataset(ScannedDataset *scannedDataset) {
  this->scannedDataset = scannedDataset;
}
void InformationView::setRois(Rois *rois) { this->rois = rois; }
