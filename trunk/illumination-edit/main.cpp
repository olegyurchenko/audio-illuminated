#include <QtGui/QApplication>
#include "editor_window.h"
#include <audio_controller.h>
#include <effect_controller.h>
#include <qscript_wrap.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    audioController = new AudioController(&a);
    audioController->setObjectName("audioController");
    effectController = new EffectController(&a);
    effectController->setObjectName("effectController");

    //a.setStyle("Plastique");
    a.setStyle("Cleanlooks");

    mainWindow = new EditorWindow();

    loadScripts(&a);

    mainWindow->show();
    return a.exec();
}
