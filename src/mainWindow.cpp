#include "mainWindow.h"

std::string fileNameWithoutExt(std::string filename)
{
    std::string ret = filename.substr(0, filename.rfind('.'));
    return ret;
}

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent)
{
    QMenu *file = menuBar()->addMenu(tr("&File"));
    dumpNCM = new QAction(tr("dump .&NCM File"), this);
    dumpQMC = new QAction(tr("dump .&QMC File"), this);
    file->addAction(dumpNCM);
    file->addAction(dumpQMC);

    QMenu* help=menuBar()->addMenu(tr("&Help"));
    readme=new QAction(tr("README"),this);
    help->addAction(readme);

    label= new QLabel(this);
    label->setText(tr("Enjoy playing .mp3/.flac music\non your favourite media player / walkman\nClick Help -> README for more information"));
    setCentralWidget(label);

    connect(dumpNCM, SIGNAL(triggered()), this, SLOT(dumpNCMFile()));
    connect(dumpQMC, SIGNAL(triggered()), this, SLOT(dumpQMCFile()));
    connect(readme,SIGNAL(triggered()),this,SLOT(openReadme()));
}

mainWindow::~mainWindow()
{
}

void mainWindow::dumpNCMFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setViewMode(QFileDialog::List);
    QStringList filename;
    if (dialog.exec())
        filename = dialog.selectedFiles();
    else
    {
        QMessageBox::information(this, tr("Dump Failed"), tr("Cannot select files"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    for (int i = 0; i < filename.size(); i++)
    {
        QString temp = filename.at(i);
        std::string file = temp.toStdString();
        std::string fileWithoutExt = fileNameWithoutExt(file);
        try
        {
            dec::decoderNCM decoder(file);
            decoder.dump();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void mainWindow::dumpQMCFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setViewMode(QFileDialog::List);
    QStringList filename;
    if (dialog.exec())
        filename = dialog.selectedFiles();
    else
    {
        QMessageBox::information(this, tr("Dump Failed"), tr("Cannot select files"), QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    for (int i = 0; i < filename.size(); i++)
    {
        QString temp = filename.at(i);
        std::string file = temp.toStdString();
        std::string fileWithoutExt = fileNameWithoutExt(file);
        try
        {
            dec::decoderQMC decoder(file, fileWithoutExt + ".mp3");
            decoder.get();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

void mainWindow::openReadme()
{
    QDesktopServices::openUrl(QUrl("https://github.com/NaviHX/vip-music-decoder-gui"));
}
