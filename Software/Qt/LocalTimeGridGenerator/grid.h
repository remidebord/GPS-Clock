#ifndef GRID
#define GRID

#include <QApplication>
#include <QtGUI>
#include <QThread>
#include <QString>
#include "timezones.h"
#include "global.h"

#define REFRESH_TIME 1000

typedef struct{
    int earthRadius;
    int earthSurface;
    int tiles;
    int tilesX;
    int tilesY;
    double precision;
}gridParameters;

class Grid
{
public:
    Grid();
    char update(gridParameters *p);
    static void Precision(gridParameters *p);
    static void TilesXY(gridParameters *p);
    static int SphereSurface(int radius);

private:
    gridParameters m_parameters;
};

class GridThread : public QThread
{
    Q_OBJECT

public:
    GridThread();

public slots :
    void run();
    void startc(gridParameters* p, TimeZones* tz);
    void stop();

signals :
    void Processing(int counter, int tiles, int refreshTime);
    void Processed(int elapsed, int counter, int tiles, int refreshTime);

private:
    gridParameters m_parameters;
    TimeZones* m_timezones;
    char m_run;
};

#endif // GRID

