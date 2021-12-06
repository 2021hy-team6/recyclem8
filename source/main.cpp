#include "recyclem8.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    RecycleM8 w;
    w.show();
    w.perform_additional_prestart_actions();
    return a.exec();
}
