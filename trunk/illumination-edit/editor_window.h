#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>
#include <QList>
#include <QToolBar>

namespace Ui {
    class EditorWindow;
}

/*----------------------------------------------------------------------------*/
class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow();

private:
    Ui::EditorWindow *ui;
    typedef QList<QToolBar *>EffectBars;
    EffectBars effectBars;

    QToolBar *addEffectBar(const QString& text);
    QAction *selectEffectAction;

  protected slots:
    void onSelectEfectAction();
    void onAddAction();
  public slots:
    void onSetWindowDuration(const QString& txt);
    void onError(const QString& txt);

    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onPlayerPlay();
    void onPlayerStop();
signals:
    void changeEditMode();
};
/*----------------------------------------------------------------------------*/

#endif // EDITOR_WINDOW_H
