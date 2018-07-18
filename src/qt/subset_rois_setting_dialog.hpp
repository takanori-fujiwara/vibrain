#ifndef SUBSETROISSETTINGDIALOG_H
#define SUBSETROISSETTINGDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include "rois.hpp"

class SubsetRoisSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SubsetRoisSettingDialog(Rois *rois, QWidget *parent = 0);
    ~SubsetRoisSettingDialog();

private:
    Rois *rois;
    QListWidget *inRangeSubsetList;
    QListWidget *outOfRangeSubsetList;

    void initSubsetLists();

private slots:
    void handleToOutOfRangeSubsetBtn();
    void handleToInRangeSubsetBtn();
    void handleCancelBtn();
    void handleOkBtn();

signals:
    void roisOrderChanged();
};

#endif // SUBSETROISSETTINGDIALOG_H
