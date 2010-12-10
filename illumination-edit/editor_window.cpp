#include "editor_window.h"
#include "ui_editorwindow.h"
/*----------------------------------------------------------------------------*/
EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow)
{
  ui->setupUi(this);
}
/*----------------------------------------------------------------------------*/
EditorWindow::~EditorWindow()
{
  delete ui;
}
/*----------------------------------------------------------------------------*/
void EditorWindow::onSetWindwoDuration(const QString& txt)
{

}
/*----------------------------------------------------------------------------*/
