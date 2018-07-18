#include "main_window.hpp"

#include <stdlib.h>
#include <iostream>

#include <QApplication>
#include <QProcess>

#include "RInside.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // after running "macdeployqt vibrain.app", R.framework will be in Frameworks
    QString rHomeBeforeDeploy = RHOME; // RHOME was set in .pro file
    QString rHomeAfterDeploy = a.applicationDirPath() + "/../Frameworks/R.framework/Versions/3.3/Resources";

    if (QDir(rHomeAfterDeploy).exists()) {
        setenv("R_HOME", rHomeAfterDeploy.toStdString().c_str(), 1);
    } else {
        setenv("R_HOME", rHomeBeforeDeploy.toStdString().c_str(), 1);
    }

    RInside R(argc, argv); // create an embedded R instance
    MainWindow w;
    w.show();

    return a.exec();
}
