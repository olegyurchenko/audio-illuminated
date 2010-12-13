#include "editor_window.h"
#include "ui_editorwindow.h"
#include <QMessageBox>
#include <audio_controller.h>
#include <effect_controller.h>

#include <QFileDialog>
#include <QMap>
#include <QActionGroup>
/*----------------------------------------------------------------------------*/
EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow)
{
  ui->setupUi(this);
  ui->actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  ui->actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

  QActionGroup *effectGroup = new QActionGroup(this);
  effectGroup->setExclusive(true);

  selectEffectAction = new QAction(this);
  selectEffectAction->setIcon(QIcon(":/resources/cursor.png"));
  selectEffectAction->setCheckable(true);
  selectEffectAction->setChecked(true);
  effectGroup->addAction(selectEffectAction);

  connect(selectEffectAction, SIGNAL(triggered()), this, SLOT(onSelectEfectAction()));

  ui->EffectPanel->clear();
  //addEffectBar(tr("Led effects"));

  connect(audioController, SIGNAL(error(QString)), this, SLOT(onError(QString)));

  connect(ui->actionNewProject, SIGNAL(triggered()), this, SLOT(onFileNew()));
  connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onFileOpen()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onFileSave()));
  connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(onFileSaveAs()));
  connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(onPlayerPlay()));
  connect(ui->actionStop, SIGNAL(triggered()), this, SLOT(onPlayerStop()));

  //Load demo panels
  {
    QBoxLayout *vbLayout = new QVBoxLayout(ui->demoPanel);

    EffectController::ControllerMap::iterator end = effectController->controllers().end();
    for(EffectController::ControllerMap::iterator it = effectController->controllers().begin(); it != end; it++)
    {
      EffectController::Controller *ctrl = &it.value();
      ctrl->demoPanel = ctrl->iface->createDemoPanel(ui->demoPanel);
      vbLayout->addWidget(ctrl->demoPanel);
    }
    vbLayout->addSpacing(100);
    ui->demoPanel->setLayout(vbLayout);
  }

  //Load effects
  {
    QBoxLayout *vbLayout = new QVBoxLayout(ui->propertyPanel);
    typedef QMap<QString, QToolBar*> ToolBarMap;
    ToolBarMap toolBarMap;

    EffectController::EffectMap::iterator end = effectController->effects().end();
    for(EffectController::EffectMap::iterator it = effectController->effects().begin(); it != end; it++)
    {
      EffectController::Effect *eff = &it.value();
      eff->propertyPanel = eff->iface->createPropertyPanel(ui->propertyPanel);
      eff->propertyPanel->setVisible(false);
      vbLayout->addWidget(eff->propertyPanel);

      QString name = eff->iface->effectName();
      QString cathegory = eff->iface->effectCathegory();
      QPixmap pixmap = eff->iface->effectIcon();
      QString description = eff->iface->effectDescription();

      QToolBar* bar;
      ToolBarMap::iterator ti = toolBarMap.find(cathegory);
      if(ti == toolBarMap.end())
      {
        bar = addEffectBar(cathegory);
        toolBarMap[cathegory] = bar;
      }
      else
        bar = ti.value();

      QAction *effectAction = new QAction(this);
      effectAction->setIcon(QIcon(pixmap));
      effectAction->setText(name);
      effectAction->setToolTip(description);
      effectAction->setCheckable(true);
      effectAction->setChecked(false);
      effectAction->setProperty("effectId", eff->iface->effectId());
      effectGroup->addAction(effectAction);
      bar->addAction(effectAction);
    }
    vbLayout->addSpacing(100);
    ui->propertyPanel->setLayout(vbLayout);
  }
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
