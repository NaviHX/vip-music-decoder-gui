#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QtWidgets/QMainWindow>
#include <QtWidgets/qaction.h>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/qdesktopservices.h>


#include <string>
#include <iostream>

#include "src-ncm/decoderNCM.hpp"
#include "src-qmc/decoderQMC.hpp"

class mainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    mainWindow(QWidget* parent=0);
    ~mainWindow();

private:
    QAction* dumpNCM;
    QAction* dumpQMC;
    QAction* readme;

    QLabel* label;

private slots:
    void dumpNCMFile();
    void dumpQMCFile();
    void openReadme();
};

#endif