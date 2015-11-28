package TI;
import java.net.*;

class PiGpio
{
    static
    {
        System.loadLibrary("piGpio");
        initialize();

		tinyHandle = i2cOpen(1, 0x26, 0);
        
        new Thread()
        {
            public void run()
            {
                try
                {
                    ServerSocket serverSocket = new ServerSocket(9999);
                    Socket s = serverSocket.accept();
                    System.out.println("Got request to quit, cleaning up");
					i2cClose(tinyHandle);
                    terminate();
                }
                catch(Exception e)
                {
                    e.printStackTrace();
                }
                System.exit(0);
            }
        }.start();
        
    }

	static private int tinyHandle;
        
    static int PI_MIN_GPIO = 0; 
    static int PI_MAX_GPIO = 53; 
    static int PI_OFF = 0; 
    static int PI_ON = 1; 
    
    static int PI_CLEAR = 0; 
    static int PI_SET = 1; 
    
    static int PI_LOW = 0; 
    static int PI_HIGH = 1; 
    static int PI_TIMEOUT = 2; 
    
        /* mode: 0-7 */
    
    static int PI_INPUT = 0; 
    static int PI_OUTPUT = 1; 
    static int PI_ALT0 = 4; 
    static int PI_ALT1 = 5; 
    static int PI_ALT2 = 6; 
    static int PI_ALT3 = 7; 
    static int PI_ALT4 = 3; 
    static int PI_ALT5 = 2; 
    static int PI_PUD_OFF = 0; 
    static int PI_PUD_DOWN = 1; 
    static int PI_PUD_UP = 2; 
    
        /* dutycycle: 0-range */
    
    static int PI_DEFAULT_DUTYCYCLE_RANGE = 255; 
    
        /* range: 25-40000 */
    
    static int PI_MIN_DUTYCYCLE_RANGE = 25; 
    static int PI_MAX_DUTYCYCLE_RANGE = 40000; 
    
        /* pulsewidth: 0, 500-2500 */
    
    static int PI_SERVO_OFF = 0; 
    static int PI_MIN_SERVO_PULSEWIDTH = 500; 
    static int PI_MAX_SERVO_PULSEWIDTH = 2500; 
    
    static int PI_INIT_FAILED = -1;  // gpioInitialise failed
    static int PI_BAD_USER_GPIO = -2;  // gpio not 0-31
    static int PI_BAD_GPIO = -3;  // gpio not 0-53
    static int PI_BAD_MODE = -4;  // mode not 0-7
    static int PI_BAD_LEVEL = -5;  // level not 0-1
    static int PI_BAD_PUD = -6;  // pud not 0-2
    static int PI_BAD_PULSEWIDTH = -7;  // pulsewidth not 0 or 500-2500
    static int PI_BAD_DUTYCYCLE = -8;  // dutycycle outside set range
    static int PI_BAD_TIMER = -9;  // timer not 0-9
    static int PI_BAD_MS = -10;  // ms not 10-60000
    static int PI_BAD_TIMETYPE = -11;  // timetype not 0-1
    static int PI_BAD_SECONDS = -12;  // seconds < 0
    static int PI_BAD_MICROS = -13;  // micros not 0-999999
    static int PI_TIMER_FAILED = -14;  // gpioSetTimerFunc failed
    static int PI_BAD_WDOG_TIMEOUT = -15;  // timeout not 0-60000
    static int PI_NO_ALERT_FUNC = -16;  // DEPRECATED
    static int PI_BAD_CLK_PERIPH = -17;  // clock peripheral not 0-1
    static int PI_BAD_CLK_SOURCE = -18;  // DEPRECATED
    static int PI_BAD_CLK_MICROS = -19;  // clock micros not 1, 2, 4, 5, 8, or 10
    static int PI_BAD_BUF_MILLIS = -20;  // buf millis not 100-10000
    static int PI_BAD_DUTYRANGE = -21;  // dutycycle range not 25-40000
    static int PI_BAD_DUTY_RANGE = -21;  // DEPRECATED (use PI_BAD_DUTYRANGE)
    static int PI_BAD_SIGNUM = -22;  // signum not 0-63
    static int PI_BAD_PATHNAME = -23;  // can't open pathname
    static int PI_NO_HANDLE = -24;  // no handle available
    static int PI_BAD_HANDLE = -25;  // unknown handle
    static int PI_BAD_IF_FLAGS = -26;  // ifFlags > 3
    static int PI_BAD_CHANNEL = -27;  // DMA channel not 0-14
    static int PI_BAD_PRIM_CHANNEL = -27;  // DMA primary channel not 0-14
    static int PI_BAD_SOCKET_PORT = -28;  // socket port not 1024-32000
    static int PI_BAD_FIFO_COMMAND = -29;  // unrecognized fifo command
    static int PI_BAD_SECO_CHANNEL = -30;  // DMA secondary channel not 0-6
    static int PI_NOT_INITIALISED = -31;  // function called before gpioInitialise
    static int PI_INITIALISED = -32;  // function called after gpioInitialise
    static int PI_BAD_WAVE_MODE = -33;  // waveform mode not 0-1
    static int PI_BAD_CFG_INTERNAL = -34;  // bad parameter in gpioCfgInternals call
    static int PI_BAD_WAVE_BAUD = -35;  // baud rate not 50-250K(RX)/50-1M(TX)
    static int PI_TOO_MANY_PULSES = -36;  // waveform has too many pulses
    static int PI_TOO_MANY_CHARS = -37;  // waveform has too many chars
    static int PI_NOT_SERIAL_GPIO = -38;  // no bit bang serial read in progress on gpio
    static int PI_BAD_SERIAL_STRUC = -39;  // bad (null) serial structure parameter
    static int PI_BAD_SERIAL_BUF = -40;  // bad (null) serial buf parameter
    static int PI_NOT_PERMITTED = -41;  // gpio operation not permitted
    static int PI_SOME_PERMITTED = -42;  // one or more gpios not permitted
    static int PI_BAD_WVSC_COMMND = -43;  // bad WVSC subcommand
    static int PI_BAD_WVSM_COMMND = -44;  // bad WVSM subcommand
    static int PI_BAD_WVSP_COMMND = -45;  // bad WVSP subcommand
    static int PI_BAD_PULSELEN = -46;  // trigger pulse length not 1-100
    static int PI_BAD_SCRIPT = -47;  // invalid script
    static int PI_BAD_SCRIPT_ID = -48;  // unknown script id
    static int PI_BAD_SER_OFFSET = -49;  // add serial data offset > 30 minutes
    static int PI_GPIO_IN_USE = -50;  // gpio already in use
    static int PI_BAD_SERIAL_COUNT = -51;  // must read at least a byte at a time
    static int PI_BAD_PARAM_NUM = -52;  // script parameter id not 0-9
    static int PI_DUP_TAG = -53;  // script has duplicate tag
    static int PI_TOO_MANY_TAGS = -54;  // script has too many tags
    static int PI_BAD_SCRIPT_CMD = -55;  // illegal script command
    static int PI_BAD_VAR_NUM = -56;  // script variable id not 0-149
    static int PI_NO_SCRIPT_ROOM = -57;  // no more room for scripts
    static int PI_NO_MEMORY = -58;  // can't allocate temporary memory
    static int PI_SOCK_READ_FAILED = -59;  // socket read failed
    static int PI_SOCK_WRIT_FAILED = -60;  // socket write failed
    static int PI_TOO_MANY_PARAM = -61;  // too many script parameters (> 10)
    static int PI_NOT_HALTED = -62;  // script already running or failed
    static int PI_BAD_TAG = -63;  // script has unresolved tag
    static int PI_BAD_MICS_DELAY = -64;  // bad MICS delay (too large)
    static int PI_BAD_MILS_DELAY = -65;  // bad MILS delay (too large)
    static int PI_BAD_WAVE_ID = -66;  // non existent wave id
    static int PI_TOO_MANY_CBS = -67;  // No more CBs for waveform
    static int PI_TOO_MANY_OOL = -68;  // No more OOL for waveform
    static int PI_EMPTY_WAVEFORM = -69;  // attempt to create an empty waveform
    static int PI_NO_WAVEFORM_ID = -70;  // no more waveforms
    static int PI_I2C_OPEN_FAILED = -71;  // can't open I2C device
    static int PI_SER_OPEN_FAILED = -72;  // can't open serial device
    static int PI_SPI_OPEN_FAILED = -73;  // can't open SPI device
    static int PI_BAD_I2C_BUS = -74;  // bad I2C bus
    static int PI_BAD_I2C_ADDR = -75;  // bad I2C address
    static int PI_BAD_SPI_CHANNEL = -76;  // bad SPI channel
    static int PI_BAD_FLAGS = -77;  // bad i2c/spi/ser open flags
    static int PI_BAD_SPI_SPEED = -78;  // bad SPI speed
    static int PI_BAD_SER_DEVICE = -79;  // bad serial device name
    static int PI_BAD_SER_SPEED = -80;  // bad serial baud rate
    static int PI_BAD_PARAM = -81;  // bad i2c/spi/ser parameter
    static int PI_I2C_WRITE_FAILED = -82;  // i2c write failed
    static int PI_I2C_READ_FAILED = -83;  // i2c read failed
    static int PI_BAD_SPI_COUNT = -84;  // bad SPI count
    static int PI_SER_WRITE_FAILED = -85;  // ser write failed
    static int PI_SER_READ_FAILED = -86;  // ser read failed
    static int PI_SER_READ_NO_DATA = -87;  // ser read no data available
    static int PI_UNKNOWN_COMMAND = -88;  // unknown command
    static int PI_SPI_XFER_FAILED = -89;  // spi xfer/read/write failed
    static int PI_BAD_POINTER = -90;  // bad (NULL) pointer
    static int PI_NO_AUX_SPI = -91;  // need a A+/B+/Pi2 for auxiliary SPI
    static int PI_NOT_PWM_GPIO = -92;  // gpio is not in use for PWM
    static int PI_NOT_SERVO_GPIO = -93;  // gpio is not in use for servo pulses
    static int PI_NOT_HCLK_GPIO = -94;  // gpio has no hardware clock
    static int PI_NOT_HPWM_GPIO = -95;  // gpio has no hardware PWM
    static int PI_BAD_HPWM_FREQ = -96;  // hardware PWM frequency not 1-125M
    static int PI_BAD_HPWM_DUTY = -97;  // hardware PWM dutycycle not 0-1M
    static int PI_BAD_HCLK_FREQ = -98;  // hardware clock frequency not 4689-250M
    static int PI_BAD_HCLK_PASS = -99;  // need password to use hardware clock 1
    static int PI_HPWM_ILLEGAL = -100;  // illegal, PWM in use for main clock
    static int PI_BAD_DATABITS = -101;  // serial data bits not 1-32
    static int PI_BAD_STOPBITS = -102;  // serial (half) stop bits not 2-8
    static int PI_MSG_TOOBIG = -103;  // socket/pipe message too big
    static int PI_BAD_MALLOC_MODE = -104;  // bad memory allocation mode
    static int PI_TOO_MANY_SEGS = -105;  // too many I2C transaction segments
    static int PI_BAD_I2C_SEG = -106;  // an I2C transaction segment failed
    static int PI_BAD_SMBUS_CMD = -107;  // SMBus command not supported by driver
    static int PI_NOT_I2C_GPIO = -108;  // no bit bang I2C in progress on gpio
    static int PI_BAD_I2C_WLEN = -109;  // bad I2C write length
    static int PI_BAD_I2C_RLEN = -110;  // bad I2C read length
    static int PI_BAD_I2C_CMD = -111;  // bad I2C command
    static int PI_BAD_I2C_BAUD = -112;  // bad I2C baud rate, not 50-500k
    static int PI_BAD_REPEAT_CNT = -113;  // bad repeat count, not 2-max
    static int PI_BAD_REPEAT_WID = -114;  // bad repeat wave id
    static int PI_TOO_MANY_COUNTS = -115;  // too many chain counters
    static int PI_BAD_CHAIN_CMD = -116;  // malformed chain command string
    static int PI_REUSED_WID = -117;  // wave already used in chain

    
    
    
    private native static void initialize();
    public native static void terminate();
    
    public native static int setMode(int pin, int mode);
    public native static int getMode(int pin);
    public native static int setPullUpDown(int pin, int pud);
    public native static int read(int pin);
    public native static int write(int pin, boolean level);
    
    public native static int pwm(int pin, int dutycycle);
    public native static int pwmGetDutyCycle(int pin);
    public native static int pwmSetRange(int pin, int range);
    public native static int pwmGetRange(int pin);
    public native static int pwmGetRealRange(int pin);
    public native static int pwmSetFreq(int pin, int freq);
    public native static int pwmGetFreq(int pin);
    
    public native static int servo(int pin, int pulseWidth);
    public native static int delay(int seconds, int micros);
    
    public native static int pulseIn(int pin, boolean level, int timeout);

    public native static int i2cOpen(int i2cBus, int i2cAddr, int i2cFlags); //returns handle
    public native static int i2cClose(int handle);

    public native static int i2cWriteQuick(int handle, int bit);
    public native static int i2cWriteByte(int handle, int bVal);
    public native static int i2cReadByte(int handle);
    public native static int i2cWriteByteData(int handle, int i2cReg, int bVal);
    public native static int i2cWriteWordData(int handle, int i2cReg, int wVal);
    public native static int i2cReadByteData(int handle, int i2cReg);
    public native static int i2cReadWordData(int handle, int i2cReg);
    public native static int i2cProcessCall(int handle, int i2cReg, int wVal);
    public native static int i2cWriteBlockData(int handle, int i2cReg, byte[] buf);
    public native static int i2cReadBlockData(int handle, int i2cReg, byte[] buf);
    public native static int i2cBlockProcessCall(int handle, int i2cReg, byte[] buf);

    public native static int i2cWriteI2CBlockData(int handle, int i2cReg, byte[] buf);
    public native static int i2cReadI2CBlockData(int handle, int i2cReg, byte[] buf);

    public native static int i2cReadDevice(int handle, byte[] buf);
    public native static int i2cWriteDevice(int handle, byte[] buf);

    public native static int spiOpen(int spiChan, int boud, int spiFlags);
    public native static int spiClose(int handle);
    public native static int spiRead(int handle, byte[] buf);
    public native static int spiWrite(int handle, byte[] buf);
    public native static int spiXfer(int handle, byte[] txBuf, byte[] rxBuf);
    
    public native static int serOpen(String sertty, int baud, int serFlags);
    public native static int serClose(int handle);
    public native static int serWriteByte(int handle, int data);
    public native static int serReadByte(int handle);
    public native static int serWrite(int handle, byte[] buf);
    public native static int serRead(int handle, byte[] buf);
    public native static int serDataAvailable(int handle);
    
    public native static void freqOut(int pin, float freq, int len);
    
    //todo: callback for SetAlertFunc
    public native static int analogRead(int pin);

	public static void sendTiny(int b)
	{
		i2cWriteByte(tinyHandle, b);
	}


}