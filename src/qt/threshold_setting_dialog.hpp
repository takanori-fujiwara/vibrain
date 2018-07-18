#ifndef THRESHOLDSETTINGDIALOG_H
#define THRESHOLDSETTINGDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QPalette>

#include "detailed_matrix.hpp"

class ThresholdSettingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ThresholdSettingDialog(DetailedMatrix *detailedMatrix, QWidget *parent = 0);
    ~ThresholdSettingDialog();

private:
    DetailedMatrix *detailedMatrix;
    QLineEdit *thresMinLineEdit;
    QLineEdit *lowerThresLineEdit;
    QLineEdit *upperThresLineEdit;
    QLineEdit *thresMaxLineEdit;
    QComboBox *rangeComboBox;
    QLabel *errorMsgLabel;

private slots:
    void handleCancelBtn();
    void handleOkBtn();

signals:
    void thresholdsChanged();
};

#endif // THRESHOLDSETTINGDIALOG_H
