#ifndef DATADIRECTORYSELECTIONDIALOG_H
#define DATADIRECTORYSELECTIONDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSettings>
#include <QDebug>

class DataDirectorySelectionDialog : public QDialog
{
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
    void dataDirecotriesChanged(QString baseDataDir, QString brainMeshDir, QString roiDir, QString scannedDataDir, QString styleDir);
};

#endif // DATADIRECTORYSELECTIONDIALOG_H
