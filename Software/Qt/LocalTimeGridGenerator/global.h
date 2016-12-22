#ifndef GLOBAL
#define GLOBAL

#define LATITUDE_MIN ((float)-90)
#define LATITUDE_MAX ((float)90)
#define LONGITUDE_MIN ((float)-180)
#define LONGITUDE_MAX ((float)180)

#define LATITUDE_RANGE ((float)LATITUDE_MAX-LATITUDE_MIN)
#define LONGITUDE_RANGE ((float)LONGITUDE_MAX-LONGITUDE_MIN)

#define HOUR_SEC 3600
#define QUARTER_HOURS_SEC (HOUR_SEC/4)

#define ZONES 360 //24
#define ZONE_WIDTH ((float)15) // 15°

#endif // GLOBAL

