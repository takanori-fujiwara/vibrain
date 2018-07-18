#ifndef INFORMATIONVIEW_H
#define INFORMATIONVIEW_H

#include <QListWidget>
#include <QMenu>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDropEvent>
#include <QMouseEvent>

#include "scanned_dataset.hpp"
#include "rois.hpp"
#include "detailed_matrix.hpp"
#include "subset_rois_setting_dialog.hpp"

class InformationView : public QListWidget
{
   Q_OBJECT

public:
    explicit InformationView(QComboBox *comboBox, ScannedDataset *scannedDataset, Rois* rois, DetailedMatrix *detailedMatrix, QWidget *parent = 0);
    void setScannedDataset(ScannedDataset* scannedDataset);
    void setRois(Rois *rois);

public slots:
    void updateItems();

private:
    ScannedDataset *scannedDataset;
    Rois *rois;
    DetailedMatrix *detailedMatrix;

    QComboBox *comboBox;
    QStringList lists;
    QList<bool> selectedLists;

    QPoint mouseReleasePos;

    void setLists(QString comboBoxCurrentText);
    void setListsSelected(QList<bool> selectedLists);
    void showContextMenu(const QPoint &pos);

protected:
    void dropEvent(QDropEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void handleSelectedItem();
    void handleDropEvent();
    void showSettingSubsetRoisDialog();
    void resetRoisOrder();
    void changeRoisOrderFromDialog();

signals:
    void selectedScannedDataChanged();
    void selectedRoisChanged();
    void matrixOrderChanged();
    void roisSubsetChanged();
};

#endif // INFORMATIONVIEW_H
