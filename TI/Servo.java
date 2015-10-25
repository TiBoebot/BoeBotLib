package TI;

/**
 * Class to control servo motors, and keep them under control in the background
 */
public class Servo {
	private int pin;
	private int pulseWidth;
	
	/**
	 * Creates a servo on a specific pin, and starts it right away
	 * @param pin The BoeBot Pin number
	 */
	public Servo(int pin)
	{
		this.pin = BoeBot.pinMap[pin];
		this.pulseWidth = 1499;
		PiGpio.servo(this.pin, pulseWidth);
	}
	
	/**
	 * Stops the servo
	 */
	public void stop()
	{
		PiGpio.servo(pin, 0);
	}
	
	/**
	 * Starts (or resumes) the servo
	 */
	public void start()
	{
		PiGpio.servo(this.pin, pulseWidth);
	}
	
	/**
	 * Changes the servo's speed
	 * @param pulseWidth the width of the pulse, in milliseconds
	 */
	public void update(int pulseWidth)
	{
		this.pulseWidth = pulseWidth;
		start();
	}
	
	
}
