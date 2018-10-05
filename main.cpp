#include <QApplication>
#include "moncontour.h"

int main(int argc,char *argv[])
{
    QApplication app(argc,argv);
    monContour fenetre;
    fenetre.show();
    return app.exec();
}
