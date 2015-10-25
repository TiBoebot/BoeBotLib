package TI;

/**
 * A simple timer class, to perform tasks at a certain frequency
 */
public class Timer
{
    private int interval;
    private long lasttick;
    
    /**
     * Creates the timer, and sets it to fire after the specified interval
     * @param interval The interval, in milliseconds
     */
    public Timer(int interval)
    {
        this.interval = interval;
        this.lasttick = System.currentTimeMillis();
    }
    
    /**
     * Used to indicate if this timer has passed the interval. If the interval has been passed twice, calling this method twice will return true multiple times
     */
    public boolean timeout()
    {
        long currentTick = System.currentTimeMillis();
        while(currentTick > lasttick + interval)
        {
            lasttick += interval;
            return true;
        }
        return false;
    }

    /**
     * Reset the timer
     */
    public void mark()
    {
        this.lasttick = System.currentTimeMillis();
    }
    
    /**
     * Sets the interval to another time, and reset the timer
     */
    public void setInterval(int interval)
    {
        this.interval = interval;
        this.lasttick = System.currentTimeMillis();
    }

}