#include "editor_window.h"
#include "ui_editorwindow.h"
#include <QMessageBox>
#include <audio_controller.h>

#include <QFileDialog>
/*----------------------------------------------------------------------------*/
EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow)
{
  ui->setupUi(this);
  ui->actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  ui->actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

  selectEffectAction = new QAction(this);
  selectEffectAction->setIcon(QIcon(":/resources/cursor.png"));
  selectEffectAction->setCheckable(true);
  selectEffectAction->setChecked(true);

  connect(selectEffectAction, SIGNAL(triggered()), this, SLOT(onSelectEfectAction()));

  ui->EffectPanel->clear();
  addEffectBar(tr("Led effects"));

  connect(audioController, SIGNAL(error(QString)), this, SLOT(onError(QString)));

  connect(ui->actionNewProject, SIGNAL(triggered()), this, SLOT(onFileNew()));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onFileOpen()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onFileSave()));
  connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(onFileSaveAs()));
  connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(onPlayerPlay()));
  connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onPlayerStop()));
}
/*----------------------------------------------------------------------------*/
EditorWindow::~EditorWindow()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
QToolBar *EditorWindow::addEffectBar(const QString& text)
{
  QToolBar *bar = new QToolBar(ui->EffectPanel);
  bar->addAction(selectEffectAction);

  ui->EffectPanel->addTab(bar, text);
  return bar;
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onSelectEfectAction()
{

}
/*----------------------------------------------------------------------------*/
void EditorWindow::onSetWindowDuration(const QString& txt)
{
  ui->editPanel->setWindowDuration((qint64)txt.toInt() * 1000);
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onError(const QString& txt)
{
  QMessageBox::critical(this, tr("Error"), txt);
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onFileNew()
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
void EditorWindow::onFileOpen()
{
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onFileSave()
{
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onFileSaveAs()
{
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onPlayerPlay()
{
  if(!ui->editPanel->waveForm()->selectionLength())
    audioController->play(ui->editPanel->waveForm()->selectionStart());
  else
    audioController->play(ui->editPanel->waveForm()->selectionStart(), ui->editPanel->waveForm()->selectionLength());
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onPlayerStop()
{
  audioController->stop();
}
/*----------------------------------------------------------------------------*/
