def upwards_cross(fastEMA, slowEMA, fastEMAold, slowEMAold):
    """
    Checks if the fast ema crosses the slow ema from down to up
    """
    return fastEMA > slowEMA and fastEMAold < slowEMAold

def downwards_cross(fastEMA, slowEMA, fastEMAold, slowEMAold):
    """
    Checks if the fast ema crosses the slow ema from up to down
    """
    return fastEMA < slowEMA and fastEMAold > slowEMAold

def simple_ema_cross(fastEMA, slowEMA, fastEMAold, slowEMAold):
    """
    Inputs:
    - fastEMA (The EMA from the fast Period)
    - slowEMA (The EMA from the slow Period)
    - fastEMAold (The EMA that is one timestamp before)
    - slowEMAold (The EMA that is one timestamp before)

    Returns 0, 1 or 2:
    - 0 - do nothing
    - 1 - enter long trade
    - 2 - enter short trade
    """
    if upwards_cross(fastEMA, slowEMA, fastEMAold, slowEMAold):
        return 1
    elif downwards_cross(fastEMA, slowEMA, fastEMAold, slowEMAold):
        return 2
    else:
        return 0
    