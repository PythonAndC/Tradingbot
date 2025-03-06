#ifndef UTILS_H
#define UTILS_H

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

typedef struct {
    double profit;
    int fastPeriod;
    int slowPeriod;
    int candles;
    float ratio;
}StratParams;

Data *loadData(int dataLength, char *filename);
double candleLow(Data *data, int index, int candles);
double candleHigh(Data *data, int index, int candles);
void insertSorted(StratParams strats[], int *size, StratParams strat);

#endif