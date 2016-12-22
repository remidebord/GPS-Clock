#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>
#include "timezones.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TimeZones timezones;

    QString error;
    int result = 1;

    QPixmap pixmap(":/resources/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();

    a.processEvents();

    splash->showMessage("Loading tz_world.geojson...", Qt::AlignBottom);
    result = timezones.LoadGeoJsonFile((QString)"tz_world.geojson", &error);

    if(result == 0)
    {
        qDebug() << error;
        return 0;
    }

    a.processEvents();

    splash->showMessage("Loading tz.xml...", Qt::AlignBottom);
    result = timezones.LoadXMLFile("tz.xml", &error);
    QThread::msleep(500);

    if(result == 0)
    {
        qDebug() << error;
        return 0;
    }

    a.processEvents();

    splash->showMessage("Loading polygons in RAM...", Qt::AlignBottom);
    result = timezones.LoadPolygons();
    QThread::msleep(500);

    if(result == 0)
    {
        qDebug() << "No polygons";
        return 0;
    }

    a.processEvents();

    splash->showMessage("Loading timezones in RAM...", Qt::AlignBottom);
    result = timezones.LoadTimezones();
    QThread::msleep(500);

    // Debug
    timezones.CheckMissingTZID();

    if(result == 0)
    {
        qDebug() << "No timezones";
        return 0;
    }

    splash->showMessage("Loading polygons fast in RAM...", Qt::AlignBottom);
    timezones.LoadPolygonsFast();
    QThread::msleep(500);

    a.processEvents();

    splash->hide();

    a.processEvents();

    MainWindow w(&timezones);
    w.show();
    
    return a.exec();
}
