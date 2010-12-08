#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "led.h"
#include <QRect>
#include <QColor>
#include <QPaintEvent>
#include <QCursor>
#include <QFileDialog>
#include <QMessageBox>
#include <QActionGroup>
//--------------------------------------------------------------------------------------------------
void DrawWidget::paintEvent ( QPaintEvent * event )
{
  QPainter painter(this);
  QRect r = event->rect();

  paint(&painter, r);
  painter.end();

}
//--------------------------------------------------------------------------------------------------
void DrawWidget::paint(QPainter *painter, const QRect &rc)
{
  //draw grid
  QRect r = rect();
  double step = double(r.width()) / leds->size();

  int top = r.top() + (r.height() - step)/2;
  int bottom = r.top() + (r.height() + step)/2;

  painter->save();

  QPen pen(Qt::SolidLine);
  pen.setWidth(1);
  painter->setPen(pen);

  for(int i = 0; i < leds->size(); i++)
    if(leds->at(i).isOn())
      painter->fillRect(r.left() + i * step, top, step, bottom - top, Qt::darkMagenta);
    else
      painter->fillRect(r.left() + i * step, top, step, bottom - top, Qt::gray);

  for(int i = 0; i < leds->size(); i++)
    painter->drawLine(r.left() + i * step, r.top(), r.left() + i * step, r.bottom());

  pen.setColor(Qt::black);
  painter->drawLine(r.left(), r.top(), r.left(), r.bottom());
  painter->drawLine(r.right(), r.top(), r.right(), r.bottom());

  painter->drawLine(r.left(), r.top(), r.right(), r.top());
  painter->drawLine(r.left(), r.bottom(), r.right(), r.bottom());

  pen.setColor(Qt::darkMagenta);
  painter->drawLine(r.left(), top, r.right(), top);
  painter->drawLine(r.left(), bottom, r.right(), bottom);

  painter->restore();
}
//--------------------------------------------------------------------------------------------------
void DrawWidget::mouseMoveEvent ( QMouseEvent * event )
{
}
//--------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass), leds(32), timer(this), project(this)

{
  ui->setupUi(this);
  ui->drawWidget->setLeds(&leds);
  connect(ui->actionNewProject, SIGNAL(triggered()), this, SLOT(onNewProject()));
  connect(ui->actionOpenProject, SIGNAL(triggered()), this, SLOT(onOpenProject()));
  connect(ui->actionSaveProject, SIGNAL(triggered()), this, SLOT(onSaveProject()));
  connect(ui->actionSaveAsProject, SIGNAL(triggered()), this, SLOT(onSaveAsProject()));

  connect(ui->actionStartRecord, SIGNAL(triggered()), this, SLOT(onStartRecord()));
  connect(ui->actionStopRecord, SIGNAL(triggered()), this, SLOT(onStopRecord()));
  connect(ui->actionStartPlay, SIGNAL(triggered()), this, SLOT(onStartPlay()));
  connect(ui->actionStopPlay, SIGNAL(triggered()), this, SLOT(onStopPlay()));

  connect(&project, SIGNAL(stoped()), this, SLOT(onProjectStop()));

  leds[1].setOn();
  leds[3].setOn();
  timer.setSingleShot(false);
  timer.setInterval(10);
  connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
  timer.start();
  effectIndex = 0;
  selectedLed = -1;
  mode = TrainingMode;

  QActionGroup *effectGroup = new QActionGroup(this);
  effectGroup->setExclusive(true);

  QStringList lst = effects.getEffects();
  int size = lst.size();
  for(int i = 0; i < size; i++)
  {
    QAction *action = new QAction(lst[i], parent);
    effectGroup->addAction(action);
    action->setProperty("effectIndex", i);
    action->setCheckable(true);
    if(!i)
      action->setChecked(true);

    if(i >= 0 && i < 9)
      action->setShortcut(QString::number(i+1));
    else
    if(i <= 18)
      action->setShortcut(QString("Ctrl+%1").arg(QString::number(i - 8)));

    connect(action, SIGNAL(triggered()), this, SLOT(onEffectSelected()));
    ui->menu_Effect->addAction(action);
  }
}
//--------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onTimer()
{
  if(mode == PlayMode)
  {
    project.getRecord(&leds);
  }
  else
  {
    int selected = -1;
    QPoint pos = ui->drawWidget->mapFromGlobal(QCursor::pos());
    QRect r = ui->drawWidget->rect();
    double step = double(r.width()) / leds.size();
    if(r.contains(pos) && step > 1.)
    {
      selected = pos.x() / step;
    }

    for(; selected != -1 && selectedLed != -1 && selected > selectedLed; selectedLed ++)
      effects.execute(effectIndex, &leds, selectedLed);
    for(; selected != -1 && selectedLed != -1 && selected < selectedLed; selectedLed --)
      effects.execute(effectIndex, &leds, selectedLed);

    selectedLed = selected;

    effects.execute(effectIndex, &leds, selectedLed);
    if(mode == RecordMode)
      project.addRecord(&leds);
  }
  ui->drawWidget->update();
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onNewProject()
{
  project.clear();
  if(!project.isSoundAvailable())
    QMessageBox::warning(this, tr("Warning"), tr("Sond subsystem not available for this system"));
  else
  {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open WAV File"),
                                                  "",
                                                  tr("Wav files (*.wav)"));
    if(fileName.isNull())
      return;
    project.setWavFile(fileName);
  }

  ui->actionStartRecord->setEnabled(true);
  ui->actionStopRecord->setEnabled(false);
  ui->actionStartPlay->setEnabled(false);
  ui->actionStopPlay->setEnabled(false);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onOpenProject()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open project File"),
                                                  "",
                                                  tr("Project files (*.lpr);;All files (*.* *)"));
  if(fileName.isNull())
    return;

  if(!project.readFromFile(fileName))
  {
    QMessageBox::critical(this, tr("Error"), QString(tr("Error open file '%1'")).arg(fileName));
    return;
  }

  ui->actionStartRecord->setEnabled(true);
  ui->actionStopRecord->setEnabled(false);
  ui->actionStartPlay->setEnabled(true);
  ui->actionStopPlay->setEnabled(false);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onSaveProject()
{
  QString fileName;
  if(project.fileName().isNull())
  {
    fileName = QFileDialog::getSaveFileName(this, tr("Save project File"),
                                            "",
                                            tr("Project files (*.lpr);;All files (*.* *)"));
    if(fileName.isNull())
      return;
  }
  project.writeToFile(fileName);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onSaveAsProject()
{
  QString fileName;
  fileName = QFileDialog::getSaveFileName(this, tr("Save project File"),
                                          "",
                                          tr("Project files (*.lpr);;All files (*.* *)"));
  if(fileName.isNull())
    return;
  project.writeToFile(fileName);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onStartRecord()
{
  mode = RecordMode;
  ui->actionStartRecord->setEnabled(false);
  ui->actionStopRecord->setEnabled(true);
  ui->actionStartPlay->setEnabled(false);
  ui->actionStopPlay->setEnabled(false);
  project.clearRecords();
  project.start();
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onStopRecord()
{
  mode = TrainingMode;
  project.stop();
  ui->actionStartRecord->setEnabled(true);
  ui->actionStopRecord->setEnabled(false);
  ui->actionStartPlay->setEnabled(true);
  ui->actionStopPlay->setEnabled(false);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onStartPlay()
{
  mode = PlayMode;
  ui->actionStartRecord->setEnabled(false);
  ui->actionStopRecord->setEnabled(false);
  ui->actionStartPlay->setEnabled(false);
  ui->actionStopPlay->setEnabled(true);
  project.start();
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onStopPlay()
{
  mode = TrainingMode;
  project.stop();
  ui->actionStartRecord->setEnabled(true);
  ui->actionStopRecord->setEnabled(false);
  ui->actionStartPlay->setEnabled(true);
  ui->actionStopPlay->setEnabled(false);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onProjectStop()
{
  mode = TrainingMode;
  ui->actionStartRecord->setEnabled(true);
  ui->actionStopRecord->setEnabled(false);
  ui->actionStartPlay->setEnabled(true);
  ui->actionStopPlay->setEnabled(false);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::onEffectSelected()
{
  QVariant v = sender()->property("effectIndex");
  effectIndex = v.toInt();
}
//--------------------------------------------------------------------------------------------------
