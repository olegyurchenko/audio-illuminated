#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <audio_controller.h>
#include <waveform.h>
#include <QFileDialog>
#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QScrollBar>

/*----------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onFileOpen()));
    connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(onStart()));
    connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onStop()));

    ui->actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    QVBoxLayout* vbLayout = new QVBoxLayout(ui->centralWidget);
    waveForm = new WaveFormWidget(ui->centralWidget);
    vbLayout->addWidget(waveForm);
    waveScroll = new QScrollBar(Qt::Horizontal);
    vbLayout->addWidget(waveScroll);

    ui->centralWidget->setLayout(vbLayout);

    audioController = new AudioController(this);
    connect(audioController, SIGNAL(wavOpen(WavFile*)), waveForm, SLOT(wavFileOpened(WavFile*)));
    connect(audioController, SIGNAL(wavClose()), waveForm, SLOT(wavFileClosed()));
    connect(audioController, SIGNAL(playPosition(qint64)), waveForm, SLOT(setFilePosition(qint64)));
    connect(audioController, SIGNAL(error(QString)), this, SLOT(onError(QString)));
    connect(audioController, SIGNAL(wavOpen(WavFile*)), this, SLOT(wavFileOpened()));
    connect(audioController, SIGNAL(wavClose()), this, SLOT(wavFileClosed()));
    connect(audioController, SIGNAL(playPosition(qint64)), this, SLOT(onPlayPositionChanged(qint64)));
}
/*----------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
    delete ui;
}
/*----------------------------------------------------------------------------*/
void MainWindow::onError(const QString& error)
{
  QMessageBox::critical(this, tr("Error"), error);
}
/*----------------------------------------------------------------------------*/
void MainWindow::onFileOpen()
{
  QString fileName;
  fileName = QFileDialog::getOpenFileName(
      this,
      "Select one or more files to open",
      "",
      "Wav files (*.wav)");

  if(fileName.isNull())
    return;

  if(!audioController->open(fileName))
    return;
}
/*----------------------------------------------------------------------------*/
void MainWindow::onScrollChanged(int pos)
{
  waveForm->setFilePosition((qint64)pos * waveForm->windowLength()/2);
}
/*----------------------------------------------------------------------------*/
void MainWindow::wavFileOpened()
{
  waveScroll->setMaximum(2 * (int)(audioController->length() / waveForm->windowLength()));
  waveScroll->setSingleStep(1);
  waveScroll->setPageStep(10);
  connect(waveScroll, SIGNAL(valueChanged(int)), this, SLOT(onScrollChanged(int)));
}
/*----------------------------------------------------------------------------*/
void MainWindow::wavFileClosed()
{
}
/*----------------------------------------------------------------------------*/
void MainWindow::onStart()
{
  if(!waveForm->selectionLength())
    audioController->play(waveForm->selectionStart());
  else
    audioController->play(waveForm->selectionStart(), waveForm->selectionLength());
}
/*----------------------------------------------------------------------------*/
void MainWindow::onStop()
{
  audioController->stop();
}
/*----------------------------------------------------------------------------*/
void MainWindow::onPlayPositionChanged(qint64 samples)
{
  waveScroll->setSliderPosition(2 * (int)(samples / waveForm->windowLength()));

}
/*----------------------------------------------------------------------------*/
