#include "editor_window.h"
#include "ui_editorwindow.h"
#include <QMessageBox>
#include <audio_controller.h>
#include <effect_controller.h>

#include <QFileDialog>
#include <QMap>
#include <QActionGroup>
#include <project_file.h>
/*----------------------------------------------------------------------------*/
EditorWindow* mainWindow = NULL;
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
  connect(effectController, SIGNAL(modeChange()), this, SLOT(onEditModeChanged()));
  connect(effectController, SIGNAL(projectModify()), this, SLOT(onProjectMidify()));
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

      connect(effectAction, SIGNAL(triggered()), this, SLOT(onAddAction()));

      effectGroup->addAction(effectAction);
      bar->addAction(effectAction);
    }
    //vbLayout->addSpacing(100);
    ui->propertyPanel->setLayout(vbLayout);
  }
}
/*----------------------------------------------------------------------------*/
EditorWindow::~EditorWindow()
{
  delete ui;
  audioController->close();
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
  effectController->setMode(EffectController::SelectMode);
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onAddAction()
{
  effectController->setEffectToAdd(sender()->property("effectId").toInt());
  effectController->setMode(EffectController::AddMode);
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

  m_fileName = QString();
  audioController->close();
  effectController->clear();
  if(!audioController->open(fileName))
    return;
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onFileOpen()
{
  m_fileName = QFileDialog::getOpenFileName(
      this,
      "Select file to read project",
      "",
      "Project file (*.ipr)");

  if(m_fileName.isNull())
    return;

  audioController->close();
  effectController->clear();

  Project project;
  //project.wavFileName = audioController->wavFile()->fileName();
  project.props = &effectController->properties();

  if(!loadProject(m_fileName, &project))
    QMessageBox::critical(this, tr("Error"), tr("Error save file '%1'").arg(m_fileName));
  else
  {
    if(!audioController->open(project.wavFileName))
      return;
    effectController->rescan();
  }
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onFileSave()
{
  if(m_fileName.isNull())
    m_fileName = QFileDialog::getSaveFileName(
        this,
        "Select file to save project",
        "",
        "Project file (*.ipr)");

  if(m_fileName.isNull())
    return;

  Project project;
  project.wavFileName = audioController->wavFile()->fileName();
  project.props = &effectController->properties();
  if(!saveProject(m_fileName, &project))
    QMessageBox::critical(this, tr("Error"), tr("Error save file '%1'").arg(m_fileName));
  else
    ui->actionSave->setEnabled(false);

}
/*----------------------------------------------------------------------------*/
void EditorWindow::onFileSaveAs()
{
  m_fileName = QString();
  onFileSave();
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onPlayerPlay()
{
  effectController->selectEffect(-1);
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
void EditorWindow::onEditModeChanged()
{
  if(effectController->mode() == EffectController::SelectMode && !selectEffectAction->isChecked())
    selectEffectAction->setChecked(true);
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onSetChannel(int c)
{
  ui->editPanel->setChannel(c);
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onProjectMidify()
{
  ui->actionSave->setEnabled(true);
  ui->actionSaveAs->setEnabled(true);
}
/*----------------------------------------------------------------------------*/
