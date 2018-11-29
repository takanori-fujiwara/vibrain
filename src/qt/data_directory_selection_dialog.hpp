#ifndef DATADIRECTORYSELECTIONDIALOG_H
#define DATADIRECTORYSELECTIONDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSpacerItem>
#include <QVBoxLayout>

class DataDirectorySelectionDialog : public QDialog {
  Q_OBJECT
public:
  explicit DataDirectorySelectionDialog(QWidget *parent = 0);
  ~DataDirectorySelectionDialog();

private:
  QString baseDataDir;
  QString brainMeshDir;
  QString roiDir;
  QString scannedDataDir;
  QString styleDir;
  QLineEdit *baseDataDirLineEdit;
  QLineEdit *brainMeshDirLineEdit;
  QLineEdit *roiDirLineEdit;
  QLineEdit *scannedDataDirLineEdit;
  QLineEdit *styleDirLineEdit;
  void setCurrentSelectedDirecotries();

private slots:
  void setBaseDataDir();
  void setBrainMeshDir();
  void setRoiDir();
  void setScannedDataDir();
  void setStyleDir();
  void handleOk();
  void saveDirectories();

signals:
  void dataDirecotriesChanged(QString baseDataDir, QString brainMeshDir,
                              QString roiDir, QString scannedDataDir,
                              QString styleDir);
};

#endif // DATADIRECTORYSELECTIONDIALOG_H
