class Order:
    """
    Holds the important inforamtion of an order.\n
    Has the Ability to check the Orders
    """
    def __init__(self, type, open_timestamp, open, take_profit, stop_loss, risk):
        self.type = type
        self.open_timestamp = open_timestamp
        self.close_timestamp = 0
        self.open = open
        self.close = 0
        self.take_profit = take_profit
        self.stop_loss = stop_loss
        if self.type:
            self.amount = risk / (open - stop_loss)
        else:
            self.amount = risk / (stop_loss - open)
        self.open_fee = -open * self.amount * 0.0006
        self.close_fee = 0
        self.profit = 0
        self.overall_profit = 0

        self.success = False
        self.finished = False
    
    def check(self, timestamp, high, low):
        """
        Checks the Orders based on the given high and low.
        ### Important Note
        Checks first if the Order reached the Stoploss before Takeprofit.
        """
        if self.type:
            if low <= self.stop_loss:
                self.close_timestamp = timestamp
                self.close = self.stop_loss
                self.profit = (self.close - self.open) * self.amount
                self.close_fee = -self.close * self.amount * 0.0006
                self.overall_profit = self.open_fee + self.profit + self.close_fee
                self.success = False
                self.finished = True

                return True
            elif high >= self.take_profit:
                self.close_timestamp = timestamp
                self.close = self.take_profit
                self.profit = (self.close - self.open) * self.amount
                self.close_fee = -self.close * self.amount * 0.0006
                self.overall_profit = self.open_fee + self.profit + self.close_fee
                self.success = True
                self.finished = True

                return True
        else:
            if high >= self.stop_loss:
                self.close_timestamp = timestamp
                self.close = self.stop_loss
                self.profit = (self.open - self.close) * self.amount
                self.close_fee = -self.close * self.amount * 0.0006
                self.overall_profit = self.open_fee + self.profit + self.close_fee
                self.success = False
                self.finished = True

                return True
            elif low <= self.take_profit:
                self.close_timestamp = timestamp
                self.close = self.take_profit
                self.profit = (self.open - self.close) * self.amount
                self.close_fee = -self.close * self.amount * 0.0006
                self.overall_profit = self.open_fee + self.profit + self.close_fee
                self.success = True
                self.finished = True

                return True
        
        return False

    def __str__(self):
        if self.type:
            return f"Long  Open/Close Timestamp: {self.open_timestamp} {self.close_timestamp} Open: {self.open} TP: {self.take_profit} SL: {self.stop_loss} Profit: {self.overall_profit}"
        else:
            return f"Short Open/Close Timestamp: {self.open_timestamp} {self.close_timestamp} Open: {self.open} TP: {self.take_profit} SL: {self.stop_loss} Profit: {self.overall_profit}"
        
class Orders:
    """
    Holds all relevant data to all Orders.\n
    Manages the Orders and keeps track of successful and failed orders.
    """
    def __init__(self):
        self.open_orders: list[Order] = []
        self.closed_orders: list[Order] = []

        self.opened_orders = 0
        self.finished_orders = 0
        self.successful = 0
        self.failed = 0

    def add_order(self, order: Order):
        self.opened_orders += 1
        self.open_orders.append(order)

    def check_all(self, timestamp, high, low):
        for order in self.open_orders:
            if order.check(timestamp, high, low):
                if order.success:
                    self.successful += 1
                else:
                    self.failed += 1
                self.finished_orders += 1

                self.closed_orders.append(order)
        
        deleted = 0
        for i in range(len(self.open_orders)):
            if self.open_orders[i - deleted].finished:
                self.open_orders.pop(i - deleted)
                deleted += 1
                
    
    def print_open_orders(self):
        print("Open Order List:")
        for order in self.open_orders:
            print(order)
        print("-------------------------------------------------------")

    def print_closed_orders(self):
        print("Closed Order List:")
        for order in self.closed_orders:
            print(order)
        print("-------------------------------------------------------")

    def __str__(self):
        return f"Successful Orders: {self.successful}\nFailed Orders: {self.failed}\nOpened Orders: {self.opened_orders}\nClosed Orders: {self.finished_orders}\n"
