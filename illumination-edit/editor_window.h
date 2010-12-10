#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>

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

  public slots:
    void onSetWindwoDuration(const QString& txt);
};
/*----------------------------------------------------------------------------*/

#endif // EDITOR_WINDOW_H
