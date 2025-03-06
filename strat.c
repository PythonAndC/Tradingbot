#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Checks if a cross over has occured.
 * 
 * @attention checks only if the first price has crossed over the second price.
 */
bool crossOver(double price1Old, double price1New, double price2Old, double price2New) {
    return price1Old < price2Old && price1New > price2New;
}
/**
 * @brief Checks if a cross under has occured.
 * 
 * @attention checks only if the first price has crossed under the second price.
 */
bool crossUnder(double price1Old, double price1New, double price2Old, double price2New) {
    return price1Old > price2Old && price1New < price2New;
}
/**
 * @brief The simple strategy function.
 * @return 1 if a cross over has occured and a long position should be taken.
 * @return 2 if a cross under has occured and a short position should be taken.
 * @return 0 if no action should be taken.
 * 
 * @note The simple strategy checks only if a cross from the emas has occured.
 * @note It is not expected to be profitable.
 * @note The return structure should be used in future strategies.
 * @note The return structure is not final and will be changed in the future.
 * @attention Should only be used for testing purposes and performance evaluation.
 * @attention Do not use in a live trading environment.
 */
int simpleStrat(double ema1Old, double ema1New, double ema2Old, double ema2New) {
    if (crossOver(ema1Old, ema1New, ema2Old, ema2New)) {
        return 1;
    } else if (crossUnder(ema1Old, ema1New, ema2Old, ema2New)) {
        return 2;
    } else {
        return 0;
    }
}