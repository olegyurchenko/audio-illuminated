#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QPainter>
#include <QTimer>
#include <led.h>
#include <led_effects.h>
#include <led_project.h>

namespace Ui
{
    class MainWindowClass;
}

class DrawWidget : public QWidget
{
  Q_OBJECT
protected:
  LedVector *leds;
public:
  DrawWidget(QWidget *parent) : QWidget(parent){leds = NULL;}
  virtual ~DrawWidget() {}

  void setLeds(LedVector *_leds){leds = _leds;}
protected:
  virtual void paintEvent ( QPaintEvent * event );
  virtual void mouseMoveEvent ( QMouseEvent * event );
  virtual void paint(QPainter *painter, const QRect &r = QRect());
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
    Ui::MainWindowClass *ui;
    LedVector leds;
    LedEffects effects;
    LedProject project;
    QTimer timer;
    int effectIndex;
    int selectedLed;
    enum
    {
      TrainingMode,
      RecordMode,
      PlayMode
    } mode;
protected slots:
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onSaveAsProject();
    void onStartRecord();
    void onStopRecord();
    void onStartPlay();
    void onStopPlay();
    void onTimer();
    void onEffectSelected();

    void onProjectStop();
protected:
  //virtual void mouseMoveEvent ( QMouseEvent * event );
};

#endif // MAINWINDOW_H
