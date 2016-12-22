#include "timezones.h"
#include "global.h"
#include <QDebug>

using namespace std;

TimeZones::TimeZones()
{
    m_jsonDoc = new QJsonDocument();
}

char TimeZones::LoadGeoJsonFile(QString filename, QString* error)
{
    char result = 0;

    QFile jsonFile(filename);
    QByteArray jsonData;
    QJsonParseError* jsonError = new QJsonParseError();

    // Success ?
    if(jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Store json file in RAM (jsonData)
        jsonData = jsonFile.readAll();

        // Data not null ?
        if(jsonData.count() != 0)
        {
            // Parse json
            *m_jsonDoc = QJsonDocument::fromJson(jsonData, jsonError);

            // No error ?
            if(jsonError->error == 0)
            {
                if(m_jsonDoc->isObject())
                {
                    result = 1;
                }
                else *error = (QString)"Invalid Json file";
            }
            else *error = (QString)"Json error ! (" + jsonError->errorString() + (QString)")";
        }
        else *error = (QString)"File empty !";
    }
    else *error = (QString)"Cannot open file !";

    // Close data file
    jsonFile.close();

    // Free RAM space
    jsonData.clear();

    return result;
}

char TimeZones::LoadXMLFile(QString filename, QString* error)
{
    char result = 0;

    QFile xmlFile(filename);
    m_domDoc = new QDomDocument;

    // Success ?
    if(xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Store XML data
        m_domDoc->setContent(&xmlFile);

        result = 1;

        // Close data file
        xmlFile.close();
    }

    return result;
}

int TimeZones::LoadPolygons(void)
{
    int count = 0;

    QJsonParseError* jsonError = new QJsonParseError();

    if(m_jsonDoc->isObject())
    {
        // Get data from json doc
        QJsonObject jsonObject = m_jsonDoc->object();

        // Get "features" content into array
        QJsonArray jsonArray = jsonObject["features"].toArray();

        // Get number of polygon(s)
        count = jsonArray.count();

        // Clear zone list
        m_polygonList.clear();

        // Copy data in RAM
        for(int i = 0; i < count; i++)
        {
            polygon p;

            QJsonObject object = jsonArray[i].toObject();
            QJsonObject properties = object["properties"].toObject();
            QJsonObject geometry = object["geometry"].toObject();

            //qDebug() << "Type:" << object["type"].toString();
            //qDebug() << "TZID:" << properties["TZID"].toString();
            //qDebug() << "type:" << geometry["type"].toString();

            // Store TZID
            p.tzid = properties["TZID"].toString();

            QJsonArray jsonCoordinates = geometry["coordinates"].toArray()[0].toArray();
            QJsonArray jsonPoint;

            // Store coordinates into a polygon
            for(int j = 0; j < jsonCoordinates.count(); j++)
            {
                jsonPoint = jsonCoordinates[j].toArray();

                //qDebug() << jsonPoint[0].toDouble() << ", "<< jsonPoint[1].toDouble();

                // x = longitude and y = latitude
                QPoint point(jsonPoint[1].toDouble()*1000, jsonPoint[0].toDouble()*1000);

                // Store point
                p.polygon << point;

                //qDebug() << z.polygon.at(j).x() << ", " << z.polygon.at(j).y() ;
            }

            // Add polygon to list
            m_polygonList.append(p);
        }

        // Free RAM space
        *m_jsonDoc = QJsonDocument::fromJson("{}", jsonError);
    }

    return count;
}

int TimeZones::LoadTimezones(void)
{
    int count = 0;

    // Get root element
    QDomElement root = m_domDoc->documentElement();

    // Get the first child of the root (timezone)
    QDomElement child1 = root.firstChild().toElement();

    // Loop while there is a child
    while(!child1.isNull())
    {
        // Check if the child tag name is "timezone"
        if (child1.tagName() == "timezone")
        {
            timezone tz;

            // Read and display the component ID
            tz.tzid = child1.attribute("id");

            // Get the first child of the component
            QDomElement child2 = child1.firstChild().toElement();

            if(!child2.isNull())
            {
                if (child2.tagName() == "utc_offset")
                {
                    QString offset = child2.firstChild().toText().data();

                    tz.offset = offset.toInt();

                    // Add timezone to list
                    m_timezoneList.append(tz);

                    count++;
                }
            }

            //qDebug() << tz.tzid << ";" << QString::number(tz.offset);
        }

        // Next timezone
        child1 = child1.nextSibling().toElement();
    }

    // Free RAM space
    m_domDoc->clear();

    return count;
}

void TimeZones::LoadPolygonsFast(void)
{
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int i = 0;
    int j = 0;

    for(j = 0; j < ZONES; j++)
    {
        // y1 and y2 value are constant
        y1 = LATITUDE_MAX;
        y2 = LATITUDE_MIN;

        // Compute x1 and x2
        x1 = LONGITUDE_MIN + ((LONGITUDE_RANGE / ZONES) * j);
        x2 = x1 + (LONGITUDE_RANGE / ZONES);

        QRect r;

        r.setCoords((y1*1000), (x1*1000), (y2*1000), (x2*1000));

        for(i = 0; i < m_polygonList.count(); i++)
        {
            QRect p = m_polygonList.at(i).polygon.boundingRect();

            // intersect between r and p ?
            if((r.intersects(p) == true) || (r.contains((QRect)p, false) == true))
            {
                m_polygonFastList[j].append(m_polygonList.at(i));

                //qDebug() << m_polygonList.at(i).tzid;
            }
        }

        //qDebug("Zone %d (%d %d %d %d) - polygons %d", j, x1, y1, x2, y2, m_polygonFastList[j].count());
    }
}

int TimeZones::CheckMissingTZID(void)
{
    int found = 0;
    int i = 0;
    int j = 0;
    int k = 0;

    // Scan polygon list
    for(i = 0; i < m_polygonList.count(); i++)
    {
        found = 0;

        // Scan timezone list
        for(j = 0; j < m_timezoneList.count(); j++)
        {
            if(m_polygonList.at(i).tzid == m_timezoneList.at(j).tzid)
            {
                found = 1;

                break;
            }
        }

        // No tzid found in timezone list ?
        if(found == 0)
        {
            // tzid not already present ?
            for(k = 0; k < m_missingList.count(); k++)
            {
                if(m_missingList.at(k).tzid == m_polygonList.at(i).tzid)
                {
                    break;
                }
            }

            // tzid not present
            if((k == m_missingList.count()) && (m_polygonList.at(i).tzid != "uninhabited"))
            {
                // Add it to the missing list
                m_missingList.append(m_polygonList.at(i));

                qDebug() << m_polygonList.at(i).tzid << " is missing !";
            }
        }
    }

    return m_missingList.count();
}

char TimeZones::FindTimezone(double latitude, double longitude, timezone *tz)
{
    int lat = latitude*1000;
    int lon = longitude*1000;
    int i = 0;
    int j = 0;

    char found = 0;

    // Clear values
    tz->offset = 0;
    tz->tzid = "";

    // Create a point
    QPoint coordinates(lat, lon);

    // Scan polygon list
    for(i = 0; i < m_polygonList.count(); i++)
    {
        // Point inside a polygon ?
        if(m_polygonList.at(i).polygon.containsPoint(coordinates, Qt::OddEvenFill))
        {
            //qDebug() << "TZID : " << m_polygonList.at(i).tzid;

            tz->tzid = m_polygonList.at(i).tzid;

            // Scan timezone list
            for(j = 0; j < m_timezoneList.count(); j++)
            {
                // TZID found ?
                if(m_polygonList.at(i).tzid == m_timezoneList.at(j).tzid)
                {
                    //qDebug() << "Offset : " << m_timezoneList.at(j).offset << " (sec)";

                    tz->offset = m_timezoneList.at(j).offset;

                    found = 1;

                    break;
                }
            }

            break;
        }
    }

    // Point not in polygon list ?
    if(found == 0)
    {
        // Compute default utc offset
        int offset = round((longitude / (ZONE_WIDTH/2)) / 2) * HOUR_SEC;

        tz->tzid = "Unknow";
        tz->offset = offset;
    }

    return found;
}

char TimeZones::FindTimezoneFast(double latitude, double longitude, timezone *tz)
{
    int lat = latitude*1000;
    int lon = longitude*1000;
    int i = 0;
    int j = 0;

    int listIndex = 0;
    char found = 0;

    // Clear values
    tz->offset = 0;
    tz->tzid = "";

    // Create a point
    QPoint coordinates(lat, lon);

    // Compute list index
    listIndex = (longitude + LONGITUDE_MAX) / (LONGITUDE_RANGE / ZONES);

    //qDebug("column: %d", listIndex);

    // Scan polygon list
    for(i = 0; i < m_polygonFastList[listIndex].count(); i++)
    {
        // Point inside a polygon ?
        if(m_polygonFastList[listIndex].at(i).polygon.containsPoint(coordinates, Qt::OddEvenFill))
        {
            //qDebug() << "TZID : " << m_polygonList.at(i).tzid;

            tz->tzid = m_polygonFastList[listIndex].at(i).tzid;

            // Scan timezone list
            for(j = 0; j < m_timezoneList.count(); j++)
            {
                // TZID found ?
                if(m_polygonFastList[listIndex].at(i).tzid == m_timezoneList.at(j).tzid)
                {
                    //qDebug() << "Offset : " << m_timezoneList.at(j).offset << " (sec)";

                    tz->offset = m_timezoneList.at(j).offset;

                    found = 1;

                    break;
                }
            }

            break;
        }
    }

    // Point not in polygon list ?
    if(found == 0)
    {
        // Compute default utc offset
        int offset = round((longitude / (ZONE_WIDTH/2)) / 2) * HOUR_SEC;

        tz->tzid = "Unknow";
        tz->offset = offset;
    }

    return found;
}

int TimeZones::PolygonsCount(void)
{
    return m_polygonList.count();
}

int TimeZones::TimezonesCount(void)
{
    return m_timezoneList.count();
}

int TimeZones::MissingTimezonesCount(void)
{
    return m_missingList.count();
}

