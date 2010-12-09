#include <QtGui/QApplication>
#include "editor_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EditorWindow w;
    w.show();

    return a.exec();
}
