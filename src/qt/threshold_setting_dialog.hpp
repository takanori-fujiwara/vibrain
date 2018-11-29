#ifndef THRESHOLDSETTINGDIALOG_H
#define THRESHOLDSETTINGDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPalette>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

#include "detailed_matrix.hpp"

class ThresholdSettingDialog : public QDialog {
  Q_OBJECT
public:
  explicit ThresholdSettingDialog(DetailedMatrix *detailedMatrix,
                                  QWidget *parent = 0);
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
