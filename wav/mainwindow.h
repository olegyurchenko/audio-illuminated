#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class WaveFormWidget;
class AudioController;
class QScrollBar;
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected slots:
  void onFileOpen();
  void onScrollChanged(int pos);
  void onError(const QString& error);
  void wavFileOpened();
  void wavFileClosed();
  void onStart();
  void onStop();
  void onPlayPositionChanged(qint64 samples);


private:
  Ui::MainWindow *ui;
  WaveFormWidget* waveForm;
  QScrollBar *waveScroll;
};

#endif // MAINWINDOW_H
