# Results
Vorher muss erwähnt werden das ich bei dem Backtest bei weitem nicht die besten Parameter benutzt habe dazu wird im folgenden noch eie Analyse dieser Ergebnisse kommen.

## First
Nach einiger Zeit habe ich festgestellt das ein Problem ist das die Strategie sehr schlecht bei Seitwärts Trends funktioniert.
Um das zu verhindern oder es vielmehr zu lösen ist meine Idee ein Alogrithmus einzubauen der sowas probiert möglichst akorrat zu erkennen.
Problematisch könnte jedoch sein das die Strategie oftmals sehr gut darin ist zu erkennen ob wenn ein Trend wechselt man müsste aufpassen das wir das dadruch nicht verhindern.

### Wochenende
Ebefalls ist mir aufgfallen das das Wochenende anscheinend hier viele Probleme verursacht denn heir haben wir meistens ein Seitwärtstrend der hier meist zu vielen Falsch aussagen kommt und dadruch auch zu falschen Trades

|                              | Wochenende | Wochende Aus | Alles    | Prozentuale Verbesserung |
|------------------------------|------------|--------------|----------|--------------------------|
| Profit per Trade (All)       | -0.2220$   | -0.1435$     | -0.1670$ | 54%                      |
| Profit per Trade (Last 1000) | -0.2021$   | -0.0922$     | -0.1279$ | 119%                     |
| Profit per Trade (Last 100)  | -0.3645$   | -0.1574$     | -0.2402$ | 131%                     |

Bei der letzten Reihe muss erwähnt werden das diese nciht Repräsentativ ist da es um zu wenig Trades handelt.

Was jedoch Ziemlich klar wird ist das Wir hier eine klare Verbesserung hätten in jedem einzelnen Fall.

### Unter der Woche Seitwärts
Natrülich bleibt das Problem auch bei einem Seitwärtstrend unter der Woche ein  Problem mit fehlerhaften entscheidungen vorallem in diesen fällen ist auch das Problem das der Order Amount deutlich höher ist als Normal was ebenfalls das Problem erzeugt das die Gebühren anfangen mit rein zu spielen.

Im folgenden werden Lösungs Ideen gebracht die dieses Problem zumindest lösen könnten.
#### MACD
Die einfachste lösung wäre der MACD

TODO: Eine Strategie mit dem MACD einbauen und gucken inwiefern es einen einfluss auf diese Trades hat.

#### RSI
Wöre ebefalls ein einfacher ansatz der helfen könnte.
Nach meiner ersten einbindung des RSI habe ich die folgenden ergebnisse bekommen jedoch muss hierbei erwähnt werden dass ich glaube die inplmentierung falsch ist.
|                              | Old    | New    | Prozentuale Verbersserung |
|------------------------------|--------|--------|---------------------------|
|Profit per Trade (All)        | -0.17$ | -0.22$ | -29%                      |
| Profit per Trade (Last 1000) | -0.13$ | -0.27$ | -107%                     |
| Profit per Trade (Last 100)  | -0.24$ | -0.13$ | 84%                       |

Trades bei der Alten: 24995
Trades bei der Neuen: 3404

Auch hier ist wieder wichtig zu wiessen das die letzten 100 Trades nicht wirklich aussage stark sind da diese einfach zu wenig sind um eine klare aussage tätigen zu können.

Wir können anhand dieser Daten realtiv genau erkennen das wir unser Ziel nicht erreicht haben diese Strategie ist sogar schlechter als die Davor.

#### Steigung des EMAs
Hier ist die Idee das es eine untere Grenze bei der mindest steigung des EMAs gibt sodass wir eventuell dies kleinen ausschließen können

TODO: testen

#### Volume
Eine Sache die auch auffält das gerade in diesen bereichen das Volumen sehr gering ist.

TODO: testen

## Weiterre Tests ergebnisse
FastEMA: 12
SlowEMA: 20
Candles: 20
Ratio: 1.5

**5m Timeframe**
|                  | Profit per Trade (All) | Profit per Trade (last 1000) | Profit per Trade (last 100) |
|------------------|------------------------|------------------------------|-----------------------------|
| EMA_12_20_20_1.5 |     -0.17$ (24.995)    |            -0.13$            |            -0.24$           |
| EMA_24_38_39_1.9 |     -0.10$ (12.698)    |            -0.11$            |            -0.06$           |


**15m Timeframe**
|                  | Profit per Trade (All) | Profit per Trade (last 1000) | Profit per Trade (last 100) |
|------------------|------------------------|------------------------------|-----------------------------|
| EMA_14_39_93_1.5 |      0.01$ (5131)      |            -0.02$            |            -0.18$           |
