import pandas as pd
import datetime

def readData(filename):
    with open(filename, "r") as file:
        contains = file.read()
        lines = contains.split("\n")

        data = []
        for line in lines:
            objetcs = line.split(" ")
            if objetcs[0] == "":
                continue
            openTime = datetime.datetime.fromtimestamp(int(objetcs[1]) / 1000)
            closeTime = datetime.datetime.fromtimestamp(int(objetcs[2]) / 1000)
            objetcs = [str(objetcs[0]), 
                       openTime.strftime("%d.%m.%Y %H:%M"), 
                       closeTime.strftime("%d.%m.%Y %H:%M"), 
                       float(objetcs[3]), 
                       float(objetcs[4]), 
                       float(objetcs[5]), 
                       float(objetcs[6]), 
                       float(objetcs[7]), 
                       float(objetcs[8]), 
                       str(objetcs[9])]
            
            data.append(objetcs)
        
        return data

data = readData("backtest/EMA_13_39_48_1.10")

df = pd.DataFrame(columns=["Type", "Open Time", "Close Time", "Open", "Close", "Takeprofit", "Stoploss", "Profit", "Amount", "Success"])
for i in range(len(data)):
    df.loc[i] = data[i]
df["Open Time"] = pd.to_datetime(df["Open Time"])
df["Close Time"] = pd.to_datetime(df["Close Time"])
df["Week Day"] = df["Open Time"].dt.day_name()
df["Week End"] = df["Open Time"].dt.weekday.isin([5, 6]).replace({True: "true", False: "false"})

df.to_excel("output.xlsx")