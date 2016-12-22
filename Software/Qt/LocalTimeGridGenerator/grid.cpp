#include "grid.h"
#include <QDebug>
#include <math.h>
#include <fstream>
#include <QTime>

using namespace std;

Grid::Grid()
{
    m_parameters.earthRadius = 0;
    m_parameters.earthSurface = 0;
    m_parameters.precision = 0;
    m_parameters.tiles = 0;
    m_parameters.tilesX = 0;
    m_parameters.tilesY = 0;
}

char Grid::update(gridParameters *p)
{
    char result = 0;

    // Prevent division by zero
    if((p->earthRadius > 0) && (p->tiles > 0))
    {
        m_parameters.earthRadius = p->earthRadius;
        m_parameters.tiles = p->tiles;

        // Compute earth surface
        m_parameters.earthSurface = Grid::SphereSurface(m_parameters.earthRadius);

        // Compute tiles in X and Y
        Grid::TilesXY(&m_parameters);

        // Compute precision
        Grid::Precision(&m_parameters);

        memcpy(p, &m_parameters, sizeof(gridParameters));

        result = 1;
    }

    return result;
}

void Grid::Precision(gridParameters *p)
{
    // Precision = sqrt(earth surface/tiles)
    p->precision = sqrt((double)((double)p->earthSurface / p->tiles));
}

void Grid::TilesXY(gridParameters *p)
{
    int X = 0;
    int Y = 0;

    // Equations:
    //  X * Y = tiles
    //  X = 2Y

    // Compute tiles on X (X = sqrt(2*tiles))
    X = sqrt((double)(2 * p->tiles));

    // Compute tiles on Y (Y = tiles/X)
    Y = p->tiles / X;

    p->tilesX = X;
    p->tilesY = Y;
}

int Grid::SphereSurface(int radius)
{
    int surface = 0;

    surface = 4 * M_PI * (radius * radius);

    return surface;
}

GridThread::GridThread()
{
    m_run = 0;
}

void GridThread::run()
{
    ofstream file;
    timezone tz;
    char offset = 0;
    int counter = 0;
    int elapsed = 0;
    QTime time(0, 0, 0);

    // Start timer
    time.start();

    // Create ouput file
    file.open("grid.dat", ios::trunc | ios::out);

    if(file)
    {
        qDebug() << "Generating grid file...";

        int x = 0;
        int y = 0;

        while(m_run)
        {
            // Debug
            counter++;

            // Compute x and y to lon and lat
            double lat = LATITUDE_MAX - (((2 * y) + 1) * (LATITUDE_MAX / m_parameters.tilesY));
            double lon = LONGITUDE_MIN + (((2 * x) + 1) * (LONGITUDE_MAX / m_parameters.tilesX));

            // Get UTC offset from coordinates (lat and lon)
            //m_timezones->FindTimezone(lat, lon, &tz);
            m_timezones->FindTimezoneFast(lat, lon, &tz);

            // Concatenate UTC offset into one byte
            offset = (tz.offset < 0) ? (0x01 << 7) : 0; // Sign
            offset |= (((abs(tz.offset) / HOUR_SEC) & 0x0F) << 3); // Hour(s)
            offset |= (((abs(tz.offset) % HOUR_SEC) / QUARTER_HOURS_SEC) & 0x07); // Quarter hour [0 to 3]

            // Store offset (1byte)  into grid.dat
            file << (unsigned char) offset;

            //qDebug("x: %.2f (%d/%d) - y: %.2f (%d/%d)", lon, (x+1), m_parameters.tilesX, lat, (y+1), m_parameters.tilesY);
            //qDebug("TZID: %s Offset: %d (0x%02X)", tz.tzid.toLocal8Bit().constData(), tz.offset, (unsigned char)offset);

            if(x < (m_parameters.tilesX - 1))
            {
                x++;
            }
            else
            {
                x = 0;
                y++;
            }

            if(y > (m_parameters.tilesY - 1))
            {
                m_run = 0;
            }

            // Refresh value
            if(time.elapsed() >= REFRESH_TIME)
            {
                time.start();
                elapsed++;

                //qDebug("Counter: %d", counter);

                // emit signal
                emit Processing(counter, (m_parameters.tilesX * m_parameters.tilesY), REFRESH_TIME);
            }
        }

        // Close file
        file.close();

        // emit signal
        emit Processed(elapsed, counter, (m_parameters.tilesX * m_parameters.tilesY), REFRESH_TIME);

        qDebug() << "Grid file generated !";
    }
    else
    {
        qDebug() << "Cannot create file !";
    }
}

void GridThread::startc(gridParameters* p, TimeZones* tz)
{
    if(!this->isRunning())
    {
        // Copy grid parameters
        memcpy(&m_parameters, p, sizeof(gridParameters));

        // Pointer to TimeZones object
        m_timezones = tz;

        m_run = 1;
    }
    this->start();
}

void GridThread::stop(void)
{
    m_run = 0;
}
