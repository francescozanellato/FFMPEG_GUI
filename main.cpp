/*
 * Author: Francesco Paolo Luca Zanellato
 */

#include "universal_gui.h"
#include <QApplication>


#include <QSettings>
#include <QCoreApplication>
#include <QStringList>
#include <QString>
#include <QClipboard>
#include <QTimer>
#include <QSharedMemory>
#include <QCoreApplication>
#include <QFileInfo>
#include <QMainWindow>

// All'interno del tuo main o del costruttore della MainWindow

int main(int argc, char *argv[])
{

    //QApplication a(argc, argv);
    QApplication::setStyle("Fusion"); // This is needed to correct the wrong cells highlight on Windows 11.
    QApplication ab(argc, argv);

    QSharedMemory sharedMemory;
    sharedMemory.setKey(QFileInfo(ab.applicationFilePath()).baseName() + "_QT_ID");
    if(sharedMemory.attach()) {
        // m_isRunning = true;
        return 2;
    }

    if (!sharedMemory.create(1)) {
        return 3; // Exit already a process running
    }


        //QApplication a(argc, argv);
        UniversalGUI w;
        //w.resize(1024,200);
        w.show();

        return ab.exec();

}
