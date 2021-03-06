package TI;
import java.awt.Color;


/**
 * The BoeBot class is the main interface to the BoeBot board.
 * It contains methods to do basic GPIO
 */
public class BoeBot
{  
    /** Mapping of BoeBot Pin number to Raspberry Pi Pin number */
    static int[] pinMap = { 17, 27, 22, 5, 6, 13, 19, 26, 18, 23, 24, 25, 12, 16, 20, 21};
    
    
    /**
     * Will put a pin in output mode, and set the state high if value is true, and low if value is false
     * If this pin is in input mode, will switch to output mode
     * @param pin the BoeBot pin number
     * @param value the value to write to this pin. true will set the pin high, low will set the pin low
     */
    public static void digitalWrite(int pin, boolean value)
    {
        PiGpio.write(pinMap[pin], value);
    }

    /**
     * Read the state of a pin. If the pin is high, will return true, otherwise will return false
     * @param pin the BoeBot pin number
     * @return the state of the pin, true for high, false for low
     */
    public static boolean digitalRead(int pin)
    {
        return PiGpio.read(pinMap[pin]) == PiGpio.PI_HIGH;
    }

    /**
     * Waits for a specific time
     * Warning: this delay is not very accurate
     * @param delay the time in milliseconds
     */
    public static void wait(int delay)
    {
        try
        {
            Thread.sleep(delay);
        }
        catch(InterruptedException e){
            e.printStackTrace();
        }
    }
    
    /**
     * Waits for a specific time
     * @param seconds the number of seconds
     * Warning: if the delay is too short, will use busy waiting
     * @param micros the number of microseconds
     */
    public static void wait(int seconds, int micros)
    {
        PiGpio.delay(seconds, micros);
    }
    /**
     * Waits for a specific time
     * Warning: if the delay is too short, will use busy waiting
     * @param micros the number of microseconds
     */
    public static void uwait(int micros)
    {
        PiGpio.delay(0, micros);
    }
    
   /**
    * Waits for a pulse on a pin
    * @param pin the BoeBot pin number
    * @param value the state to wait for. true to wait for a high pulse, false to wait for a low pulse
    * @param timeout the timeout to wait for, in microseconds
    * @return Returns the length of the measured pulse, -1 if there was a timeout, and 0 if there was a timeout during the pulse
    */
    public static int pulseIn(int pin, boolean value, int timeout)
    {
        return PiGpio.pulseIn(pinMap[pin], value, timeout);
    }
    

    /**
     * Stores data on the internal memory of the Raspberry Pi
     * @param slot the dataslot to store information on
     * @param value the value to store
     */
    public static void dataStore(int slot, int value)
    {
    }

    /**
     * Reads data from the internal memory of the Raspberry Pi
     * @param slot the dataslot to store information on
     * @return the information in memory
     */
    public static int dataRead(int slot)
    {
        return 0;
    }
    
    /**
     * Writes a frequency to a pin, for a certain amount of time
     * @param pin the BoeBot pin number
     * @param frequency the frequency to write, in Hz
     * @param time the amount of time to write, in milliseconds
     */
    public static void freqOut(int pin, int frequency, int time)
    {
        if(frequency < 0 || time < 0)
            return;

        long endTime = System.currentTimeMillis() + time;
        boolean value = false;
        while(endTime > System.currentTimeMillis())
        {
            digitalWrite(pin, value);
            value = !value;
            PiGpio.delay(0, 1000000 / (frequency*2));
        }
    }
    
    /**
     * Better version of writing a frequency to a pin, for a certain amount of time
     * @param pin the BoeBot pin number
     * @param frequency the frequency to write, in Hz
     * @param time the amount of time to write, in milliseconds
     */
    public static void freqOut(int pin, float frequency, int time)
    {
        if(frequency < 0 || time < 0)
            return;
        PiGpio.freqOut(pinMap[pin], frequency, time);
    }

    /**
     * Reads an analog value from the 12bit ADC. This method uses single-shot mode, and opens the SPI bus every time it is called.
     * @param pin the ADC pin
     * @return the value returned from the ADC (0-4096)
     */
    public static int analogRead(int pin)
    {
		return PiGpio.analogRead(pin);
    }

    /**
     * Shows the colors set to the NeoPixels on the board
     */
	public static void rgbShow()
	{
		PiGpio.sendTiny(2);
	}
	
	/**
	 * Sets the colors of a NeoPixel, but does not show this color. After calling this method for the LEDs you want to change color, call the BoeBot.show() method
	 * @param led the LED you want to change (0-5)
	 * @param r the value for red (0-255), 0 is off, 255 is completely on
	 * @param g the value for green (0-255), 0 is off, 255 is completely on
	 * @param b the value for blue (0-255), 0 is off, 255 is completely on
	 */
	public static void rgbSet(int led, int r, int g, int b)
	{
		PiGpio.sendTiny(0x1 | led << 3);
		PiGpio.sendTiny(r);
		PiGpio.sendTiny(g);
		PiGpio.sendTiny(b);
	}

	/**
	 * Sets the colors of a NeoPixel, but does not show this color. After calling this method for the LEDs you want to change color, call the BoeBot.show() method
	 * @param led the LED you want to change (0-5)
	 * @param color the color of the LED
	 */
	public static void rgbSet(int led, Color color)
	{
		rgbSet(led, color.getRed(), color.getGreen(), color.getBlue());
	}

	/**
	 * Turns the tiny status LED on or off
	 * @param enabled true to turn the LED on, false to turn the LED off
	 */
	public static void setStatusLed(boolean enabled)
	{
		PiGpio.sendTiny(enabled ? 4 : 5);
	}
}
