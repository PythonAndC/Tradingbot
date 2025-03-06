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

typedef struct {
    double profit;
    int fastPeriod;
    int slowPeriod;
    int candles;
    float ratio;
}StratParams;
/**
 * @brief Loads data from a file and stores it in an array of Data.
 * 
 * @param dataLength The length of the data array.
 * @param filename The name of the file to be loaded.
 * @return The array of Data.
 * 
 * @attention The first two elements of the fast and slowEMA are set to the close price of the first element.
 * 
 * @note SmallDataSet has a datalength of 11340.
 * @note MediumDataSet has a datalength of 88400.
 * @note LargeDataSet has a datalength of 527200.
 */
Data *loadData(int dataLength, char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        printf("Error: File not found\n");
        exit(1);
    }

    Data *data = (Data *)malloc(dataLength * sizeof(Data));
    if(data == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(1);
    }

    int count = 0;
    // Maps the data from the file to the data array with the right data types.
    while(count < dataLength && fscanf(file, "%lu %lf %lf %lf %lf %lf",
        &data[count].timestamp,
        &data[count].open,
        &data[count].high,
        &data[count].low,
        &data[count].close,
        &data[count].volume) != EOF) {
        if(count == 0) {
            data[count].delta = 0.0;
        }
        data[count].delta = data[count].close - data[count - 1].close;
        data[count].rsi = 0.0;
        count++;
    }

    fclose(file);

    // Declares the first element of the fast and slow EMA to the close price of the first element.
    data[0].fastEma = data[0].close;
    data[0].slowEma = data[0].close;
    return data;
}
/**
 * @brief Gives the lowest value of the last n candles.
 * 
 * @param index The index of the current candle.
 * @param candles The number of candles to look back.
 * @return The lowest value of the last n candles.
 */
double candleLow(Data* data, int index, int candles) {
    double lowest = 999999;
    for(int i = index - candles; i < index; i++) {
        if(lowest > data[i].low) {
            lowest = data[i].low;
        }
    }
    return lowest;
}
/**
 * @brief Gives the highest value of the last n candles.
 * 
 * @param index The index of the current candle.
 * @param candles The number of candles to look back.
 * @return The highest value of the last n candles.
 */
double candleHigh(Data* data, int index, int candles) {
    double highest = 0;
    for(int i = index - candles; i < index; i++) {
        if(highest < data[i].high) {
            highest = data[i].high;
        }
    }
    return highest;
}

void insertSorted(StratParams strats[], int *size, StratParams strat) {
    if(*size < 100) {
        strats[*size] = strat;
        (*size)++;
    }else if(strat.profit * strat.ratio > strats[0].profit * strats[0].ratio) {
        strats[0] = strat;
    }

    for(int i = 0; i < *size - 1; i++){
        for(int j = 0; j < *size - i - 1; j++) {
            if(strats[j].profit * strats[j].ratio > strats[j + 1].profit * strats[j + 1].ratio) {
                StratParams temp = strats[j];
                strats[j] = strats[j + 1];
                strats[j + 1] = temp;
            }
        }
    }
}