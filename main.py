from data_handler import Data, Timeframe
from order_handler import Orders, Order
from strat import simple_ema_cross

def Backtest(data: Data, fast_period, slow_period, candles, ratio, risk):
    """
    Simulates old Data to the Strat and gives an good overview if the Strat is Profitable
    """
    order_handler = Orders()

    # Caclulates EMAs
    data.calc_ema(fast_period)
    data.calc_ema(slow_period)

    # Insures that only data is used were the Calculation of the EMA and Candle low/high is right
    skipped_candles = slow_period
    if candles > slow_period:
        skipped_candles = candles

    # Loops threw all the Data
    for index in range(len(data.data)):
        if index < skipped_candles:
            continue
        # Checks all open Orders if the Order is complete
        order_handler.check_all(data.data[index]["timestamp"], data.data[index]["high"], data.data[index]["low"])

        # Makes the Strat test
        strat_answer = simple_ema_cross(data.data[index][f"ema_{fast_period}"], data.data[index][f"ema_{slow_period}"], data.data[index - 1][f"ema_{fast_period}"], data.data[index - 1][f"ema_{slow_period}"])
        # Open a Long Order
        if strat_answer == 1:
            stoploss = data.candle_low(index, candles)                                              
            takeprofit = (data.data[index]["close"] - stoploss) * ratio + data.data[index]["close"]
            order = Order(True, data.data[index]["timestamp"], data.data[index]["close"], takeprofit, stoploss, risk)
            order_handler.add_order(order)
        # Open a Short Order
        elif strat_answer == 2:
            stoploss = data.candle_high(index, candles)
            takeprofit = data.data[index]["close"] - (stoploss - data.data[index]["close"]) * ratio
            order = Order(False, data.data[index]["timestamp"], data.data[index]["close"], takeprofit, stoploss, risk)

    print(f"Successful Orders: {order_handler.successful} Failed Orders: {order_handler.failed} Commulative Win Ratio: {order_handler.successful / order_handler.failed * ratio}")
    return order_handler.successful / order_handler.failed * ratio

data = Data("ETHUSDT", Timeframe.h4)
Backtest(data, 12, 20, 20, 1, 1)