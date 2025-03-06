#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    long unsigned int timestamp;
    double open;
    double high;
    double low;
    double close;
    double volume;
    double fastEma;
    double slowEma;

    double delta;
    double rsi;
} Data;
/**
 * @brief Calculates the EMA value for an entire data set.
 * 
 * @param data Data set to calculate EMA for.
 * @param dataLength Length of data set.
 * @param fastperiod Length of fast EMA period.
 * @param slowperiod Length of slow EMA period.
 * 
 * @attention Because of the Pointer to the Data struct, the function does not return anything.
 */
void calcEma(Data *data, int dataLength, int fastperiod, int slowperiod) {
    double k = 2 / (fastperiod + 1.0);
    double a = 2 / (slowperiod + 1.0);
    for(int i = 1; i < dataLength; i++) {
        data[i].fastEma = k * data[i].close + (1 - k) * data[i - 1].fastEma;
        data[i].slowEma = a * data[i].close + (1 - a) * data[i - 1].slowEma;
    }
}