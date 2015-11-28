package TI;

public class SerialConnection
{
    int handle;
    /**
     * Opens a connection to the serial port
     * @param baudrate the baudrate
     */
    public SerialConnection(int baudrate)
    {
        handle = PiGpio.serOpen("/dev/ttyAMA0", baudrate, 0);
    }
    /**
     * Opens a connection to the serial port at the default 115200 baud
     */
    public SerialConnection()
    {
        this(115200);
    }
    
    /**
     * Reads and returns a byte if available
     * @return the byte read
     */
    public int readByte()
    {
        return PiGpio.serReadByte(handle);
    }
    
    /**
     * Writes a byte to the serial buffer
     * @param data the data to write
     */
    public void writeByte(int data)
    {
        PiGpio.serWriteByte(handle, data);
    }
    
    /**
     * Checks if there is data available
     * @return the amount of bytes in the buffer.
     */
    public int available()
    {
        return PiGpio.serDataAvailable(handle);
    }
}