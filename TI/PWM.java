package TI;

/**
 * Class to run a PWM in the background
 */
public class PWM
{
    private int pin;
    private int dutycycle;
    
    /**
     * Creates and starts the PWM
     * @param pin the BoeBot pin number
     * @param dutycycle the dutycycle, range of 0 to 255
     */
    public PWM(int pin, int dutycycle)
    {
        this.pin = BoeBot.pinMap[pin];
        this.dutycycle = dutycycle;
        PiGpio.pwm(this.pin, this.dutycycle);
    }
    
    /**
     * Starts or resumes the PWM
     */
    public void start()
    {
        PiGpio.pwm(this.pin, this.dutycycle);
    }
    
    /**
     * Stops the PWM
     */
    public void stop()
    {
        PiGpio.pwm(this.pin, 0);
    }

    /**
     * updates and starts the PWM
     * @param dutycycle the dutycycle, range of 0 to 255
     */
    public void update(int dutycycle)
    {
        this.dutycycle = dutycycle;
        PiGpio.pwm(this.pin, this.dutycycle);
    }
    
    /**
     * Returns the current dutycycle
     * @return the dutycycle, in milliseconds
     */
    public int getDutyCycle()
    {
        return this.dutycycle;
    }
    
}