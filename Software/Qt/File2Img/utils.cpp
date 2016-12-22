#include "utils.h"
#include "math.h"

void GetDefaultWidthHeight(int size, int* width, int* height)
{
    int i = 0;

    // Compute the greatest divisor < sqrt(size)and > sqrt(size)/2
    for(i = (sqrt((double)size)); i > ((sqrt((double)size))/2); i--)
    {
        // Remainder == 0 ?
        if((size % i) == 0)
        {
            *height = i;
            *width = size/(*height);

            break;
        }
    }

    // No divisor found ?
    if(*height == 0)
    {
        *height = sqrt((double)size);
        *width = *height;
    }
}
