#ifndef SETTINGPANEL_H
#define SETTINGPANEL_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "scanned_dataset.hpp"

class SettingPanel : public QDialog {
  Q_OBJECT
public:
  explicit SettingPanel(ScannedDataset *scannedDataset, QWidget *parent = 0);
  ~SettingPanel();

private:
  QString matrixKeyUsedForMds;
  QString mdsDistMethod;
  QGroupBox *filteringCategoryGroupBox;
  QGroupBox *filteringNumberGroupBox;

private slots:
  void sendSignalMatrixUsedForMdsBtnChanged(QAbstractButton *btn);
  void sendSignalMdsDistMethodBtnChanged(QAbstractButton *btn);
  void sendSignalFilterChanged();
  void sendSignalApplyFilteringClicked();
  void sendSignalRecalcMdsClicked();
  void sendSignalPointColorBtnChanged(QAbstractButton *btn);
  void sendSignalPointShapeBtnChanged(QAbstractButton *btn);
  void sendSignalPointSizeBtnChanged(QAbstractButton *btn);

signals:
  void mdsCalcMethodChanged(QString mdsDistMethod, QString matrixKey);
  void filterChanged(
      QMap<QString, QMap<QString, bool>> statusOfCategoryFilterCheckBoxes,
      QMap<QString, QPair<QString, QString>>
          statusOfNumberFilterOperatorsAndValues);
  void applyFilteringClicked();
  void recalcMdsClicked(QString mdsDistMethod, QString matrixKey);
  void pointColorChanged(QString key);
  void pointShapeChanged(QString key);
  void pointSizeChanged(QString key);
};

#endif // SETTINGPANEL_H
