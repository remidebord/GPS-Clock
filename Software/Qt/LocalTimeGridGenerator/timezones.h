#ifndef TIMEZONES
#define TIMEZONES

#include <QApplication>
#include <QtGUI>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QtXml/QtXml>
#include <QFile>

#include "global.h"

typedef struct{
    int hour;
    int minute;
    int second;
}s_time;

typedef struct{
    QString tzid;
    QPolygon polygon;
}polygon;

typedef struct{
    int offset;
    QString tzid;
}timezone;

class TimeZones
{
public:
    TimeZones();
    char LoadGeoJsonFile(QString filename, QString* error);
    char LoadXMLFile(QString filename, QString* error);
    int LoadPolygons(void);
    int LoadTimezones(void);
    void LoadPolygonsFast(void);
    int CheckMissingTZID(void);
    char FindTimezone(double latitude, double longitude, timezone *tz);
    char FindTimezoneFast(double latitude, double longitude, timezone *tz);
    int PolygonsCount(void);
    int TimezonesCount(void);
    int MissingTimezonesCount(void);

private:
    QJsonDocument* m_jsonDoc;
    QDomDocument* m_domDoc;

    QList<timezone> m_timezoneList;
    QList<polygon> m_missingList;
    QList<polygon> m_polygonList;
    QList<polygon> m_polygonFastList[ZONES];
};

#endif // TIMEZONES

