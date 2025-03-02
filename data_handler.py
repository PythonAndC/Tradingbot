import requests
from enum import Enum

class Timeframe(Enum):
    m1 = "1m"
    m5 = "5m"
    m15 = "15m"
    m30 = "30m"
    h1 = "1H"
    h4 = "4H"
    d1 = "1D"

class Data:
    '''
    Loads or Pulls exsisting Data by first use.\n
    Manages the Data and handles Data related stuff.
    '''
    def __init__(self, symbol: str, timeframe: Timeframe) -> None:
        self.symbol = symbol
        self.timeframe = timeframe
        self.data = []

        try:
            self.load()
        except:
            self.pull()

    def translate_server_responde(self, answer: requests.Response) -> list:
        '''
        Translates the Anser to a usable format.
        '''
        answer = answer.json()
        if(answer["code"] != "00000"):
            return None
        
        raw_data = answer["data"]

        data = []
        for row in raw_data:
            data.append({
                "timestamp": int(row[0]),
                "open": float(row[1]),
                "high": float(row[2]),
                "low": float(row[3]),
                "close": float(row[4]),
                "volume": float(row[5])
            })

        return data

    def pull(self):
        """
        Pulls all exsisting Data and Saves it.
        """
        print("Pulling data from API this could take a while...")
        answer = requests.get(f"https://api.bitget.com/api/v2/mix/market/candles?symbol={self.symbol}&granularity={self.timeframe.value}&limit=1000&productType=usdt-futures") 
        data = self.translate_server_responde(answer)
        self.data = data

        self.pull_historical()
        self.update()
        self.save()
        print("Data pulled and saved.")

    def pull_historical(self):
        print("Loading Historical Data.")
        if self.timeframe == Timeframe.d1:
            limit = 90
        else:
            limit = 200

        while True:
            end_time = self.data[0]["timestamp"]
            answer = requests.get(f"https://api.bitget.com/api/v2/mix/market/history-candles?symbol={self.symbol}&granularity={self.timeframe.value}&limit={limit}&productType=usdt-futures&endTime={end_time}")
            data = self.translate_server_responde(answer)
            self.data[:0] = data
            print(f"Timestamp: {self.data[0]['timestamp']}")

            if(len(data) != limit):
                print("Historical Data loaded.")
                break 

    def update(self):
        print("Update Data.")
        while True:
            start_time = self.data[len(self.data) - 1]["timestamp"]
            answer = requests.get(f"https://api.bitget.com/api/v2/mix/market/history-candles?symbol={self.symbol}&granularity={self.timeframe.value}&limit=200&productType=usdt-futures&startTime={start_time}")
            data = self.translate_server_responde(answer)
            self.data[len(self.data) - 1:] = data[1:] 

            if(len(data) != 200):
                print("Data is up do Date.")
                break       
    
    def load(self):
        """
        Loads data from the Data directory
        """
        with open(f"Data/{self.symbol}_{self.timeframe.value}", "r") as file:
            file_content = file.read()
            rows = file_content.split("\n")
            data = []
            for row in rows:
                row = row.split(" ")
                data.append({
                "timestamp": int(row[0]),
                "open": float(row[1]),
                "high": float(row[2]),
                "low": float(row[3]),
                "close": float(row[4]),
                "volume": float(row[5])
            })
        
        file.close()
        self.data = data
        self.update()
        print("Data loaded and updated.")

    def save(self):
        """
        Saves the Data to a .txt in the Data directory.\n
        The format is:\n
        Timestamp Open High Low Close Volume
        """
        with open(f"Data/{self.symbol}_{self.timeframe.value}", "w") as file:
            counter = 0
            for row in self.data:
                file.write(f'{row["timestamp"]} {row["open"]} {row["high"]} {row["low"]} {row["close"]} {row["volume"]}')
                if(counter < len(self.data) - 1):
                    file.write("\n")

                counter += 1

        file.close()
        print("Data Saved")