#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "utils.h"
#include "orderHandler.h"
#include "indicators.h"
#include "strat.h"

#define DATALENGTH 193928
#define NUM_THREADS 4
#define OUTPUT true
#define FILENAME "data/ETHUSDT_30m.txt"

#define RISK 1.0

typedef struct {
    Data* data;
    int dataLength;
    int minFastPeriod;
    int minSlowPeriod;
    int maxSlowPeriod;
    int minCandles;
    int maxCandles;
    float minRatio;
    float maxRatio;
    StratParams* strats;  // Gemeinsamer Speicher für die besten Strategien
    int* size;            // Anzahl der gespeicherten Strategien
    pthread_mutex_t* lock; // Mutex für Thread-Sicherheit
    int thread_id;        // ID des Threads
} OptimizerParams;

/**
 * @brief Backtests the Strategy and simulates the trades that would have been made.
 * 
 * @param fastPeriod The period for the fast EMA must be less than slowPeriod.
 * @param slowPeriod The period for the slow EMA must be greater than fastPeriod.
 * @param candles The number of candles to look back for the high and low (is used to set the stoploss).
 * @param ratio Determines the takeprofit based on the stoploss.
 * 
 * @return The sum of all profits made by the strategy.
 * 
 * @attention The Implemented Strategy is a simple Strategy and it is not expected to make profit.
 * @note This should only be used for testing and optimization purposes.
 */
double backtest(Data *data, int dataLength, int fastPeriod, int slowPeriod, int candles, float ratio) {
    // Initialize all the necessary variables
    Node *headOrder = NULL;
    Node *headComplete = NULL;
    Node *temp = NULL;
    Order order;

    // Would be exported to the optimizer to bring greater performance
    calcEma(data, dataLength, fastPeriod, slowPeriod);
    calcRSI(data, dataLength, 14);

    // Is important because otherwise the execuation of the orders are determined by false calculations
    int lowest;
    if(slowPeriod > candles) {
        lowest = slowPeriod;
    } else {
        lowest = candles;
    }

    // The main loop that goes through the data and checks for possible trades
    for(int i = lowest; i < dataLength; i++) {
        // Handling the orders and checking if they are complete
        headComplete = checkOrders(headOrder, headComplete, data[i].timestamp, data[i].high, data[i].low);
        headOrder = deleteWaste(headOrder);
        int answer = simpleStrat(data[i - 1].fastEma, data[i].fastEma, data[i - 1].slowEma, data[i].slowEma);
        // Read the output instructions of the simpleStrat function
        if(answer == 1) {
            double stoploss = candleLow(data, i, candles);
            double takeprofit = data[i].close + ((data[i].close - stoploss) * ratio);
            double amount = RISK / (data[i].close - stoploss);
            order = createOrder(true, data[i].timestamp, data[i].close, takeprofit, stoploss, amount);
            temp = createNode(order);
            headOrder = pushNode(headOrder, temp);
        }else if(answer == 2) {
            double stoploss = candleHigh(data, i, candles);
            double takeprofit = data[i].close - ((stoploss - data[i].close) * ratio);
            double amount = RISK / (stoploss - data[i].close);
            order = createOrder(false, data[i].timestamp, data[i].close, takeprofit, stoploss, amount);
            temp = createNode(order);
            headOrder = pushNode(headOrder, temp);
        }
        // For debugging purposes
        //printf("Timestamp: %lu Close: %lf\n", data[i].timestamp, data[i].close);
        //printf("Header List\n");
        //printList(headOrder);
    }

    double profit = 0;
    Node *tmp = headComplete;
    int counter = 0;
    int winning = 0;
    int losing = 0;

    int winningLast100 = 0;
    int losingLast100 = 0;

    int winningLast1000 = 0;
    int losingLast1000 = 0;

    while(tmp != NULL) {
        if(tmp->order.success) {
            winning ++;
        }else {
            losing ++;
        }
        profit += tmp->order.profit;
        tmp = tmp->next;
        counter ++;
    }
    float winRatio = (float)winning / (float)losing;
    float commulativeWinRatio = winRatio * ratio;

    if(OUTPUT) {
        char filename[100];
        sprintf(filename, "backtest/EMA_%d_%d_%d_%.2f", fastPeriod, slowPeriod, candles, ratio);
        FILE *file = fopen(filename, "w");
        if(file == NULL) {
            printf("Datei konnte nicht geöffnet werden.");
        }
    
        tmp = headComplete;
        while(tmp != NULL) {
            fprintf(file, "%s %ld %ld %.2lf %.2lf %.2lf %.2lf %.4lf %.4lf %s\n", 
                tmp->order.type ? "long" : "short", 
                tmp->order.opentimestamp, 
                tmp->order.closetimestamp,
                tmp->order.open, 
                tmp->order.close,
                tmp->order.takeprofit,
                tmp->order.stoploss,
                tmp->order.profit,
                tmp->order.amount,
                tmp->order.success ? "True" : "False");
            tmp = tmp->next;
        }
    
        fclose(file);

        printf("Number of trades: %d\n", counter);
        printf("Trade Ratio: %f W/L %d/%d\n", commulativeWinRatio, winning, losing);
        printf("Profit: %lf\n", profit);
    }

    // Free the memory
    deleteFullList(&headOrder);
    deleteFullList(&headComplete);
    return winRatio;
}

/**
 * @brief The function that is executed by the threads.
 * 
 * @param arg Is the hole data given over the Thread creator.
 * 
 * @note The Function is the main execution function but seperated beacuse Threads needs to execute a function with a specific signature.
 */
void* thread_optimizer(void* arg) {
    // Loading all Params from the given struct
    OptimizerParams* params = (OptimizerParams*)arg;
    int dataLength = params->dataLength;
    Data* data = params->data;
    int minFastPeriod = params->minFastPeriod;
    int minSlowPeriod = params->minSlowPeriod;
    int maxSlowPeriod = params->maxSlowPeriod;
    int minCandles = params->minCandles;
    int maxCandles = params->maxCandles;
    float minRatio = params->minRatio;
    float maxRatio = params->maxRatio;
    StratParams* strats = params->strats;
    int* size = params->size;
    pthread_mutex_t* lock = params->lock;
    int thread_id = params->thread_id;

    // Calculating the start based on the thread_id
    int start = minSlowPeriod + thread_id;

    // The main loop that goes through all combinations that should be executed by the thread this ensures that the threads are working on different tasks
    for(int slowPeriod = start; slowPeriod <= maxSlowPeriod; slowPeriod += NUM_THREADS) {
        for(int fastPeriod = minFastPeriod; fastPeriod < slowPeriod; fastPeriod ++) {
            for(int candles = minCandles; candles <= maxCandles; candles ++) {
                for(float ratio = minRatio; ratio <= maxRatio; ratio += 0.1) {
                    double winRatio = backtest(data, dataLength, fastPeriod, slowPeriod, candles, ratio);
                    if(winRatio > 0) {
                        StratParams strat = {winRatio, fastPeriod, slowPeriod, candles, ratio};
                
                        pthread_mutex_lock(lock);           // Insures that the threads dont access the shared memory at the same time
                        insertSorted(strats, size, strat); // Insures that only the 100 best strategies are saved
                        pthread_mutex_unlock(lock);       // Releases the memory for the next thread
                    }
                }
            }
        }
        printf("Thread %d: SlowPeriod: %d\n", thread_id, slowPeriod);
    }

    return NULL;
}

/**
 * @brief The optimizer function that is used to find the best strategy based on the given data.
 * 
 * @param minFastPeriod The minimum period that the fastPeriod would ever reach.
 * @param minSlowPeriod The minimum period that the slowPeriod would ever reach.
 * @param maxSlowPeriod The maximum period that the slowPeriod would ever reach.
 * 
 * @note The function is using multiple threads to optimize the used time for the calculations and insures that all availabe power is used.
 */
void optimizer(Data *data, int dataLength, int minFastPeriod, int minSlowPeriod, int maxSlowPeriod, int minCandles, int maxCandles, float minRatio, float maxRatio) {
    pthread_t threads[NUM_THREADS];
    OptimizerParams params[NUM_THREADS];
    StratParams strats[100] = {0};
    int size = 0;
    pthread_mutex_t lock;

    pthread_mutex_init(&lock, NULL);

    for(int i = 0; i < NUM_THREADS; i ++) {
        // Giving the data to the threads
        params[i] = (OptimizerParams){data, dataLength, minFastPeriod, minSlowPeriod, maxSlowPeriod, minCandles, maxCandles, minRatio, maxRatio, strats, &size, &lock, i};

        // Creating the threads
        if(pthread_create(&threads[i], NULL, thread_optimizer, &params[i]) != 0) {
            perror("Thread creation failed!!");
            exit(1);
        }
    }

    // Wait for the Threads to finish
    for(int i = 0; i < NUM_THREADS; i ++) {
        if(pthread_join(threads[i], NULL) != 0) {
            perror("Thread join failed!!");
            exit(1);
        }
    }

    pthread_mutex_destroy(&lock);

    // Print the best strategies
    for(int i = 0; i < size; i ++) {
        StratParams strat = strats[i];
        printf("Index: %d Winratio: %lf Fast: %d Slow: %d Candles: %d Ratio: %f\n", i, strat.profit, strat.fastPeriod, strat.slowPeriod, strat.candles, strat.ratio);
    }
}

int main() {
    Data *data = loadData(DATALENGTH, FILENAME);

    //optimizer(data, DATALENGTH, 2, 4, 40, 4, 100, 1.0, 2.0);
    backtest(data, DATALENGTH, 13, 39, 48, 1.1);

    return 0;
}