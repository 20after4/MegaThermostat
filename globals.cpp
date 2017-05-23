#include <math.h>
float fixed_decimal(float val, int dp) {
    float multiplier = powf( 10.0f, dp);
    float result = roundf(val * multiplier ) / multiplier;
    if (isnan(result)) {
        return val;
    } else {
        return result;
    }
}
