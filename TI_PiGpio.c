#include "TI_PiGpio.h"
#include <pigpio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>


sem_t sem;

/*
 * Class:     TI_PiGpio
 * Method:    initialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_TI_PiGpio_initialize  (JNIEnv* env, jclass cl)
{
	gpioCfgClock(1, 1, 0);
	gpioInitialise();
	sem_init(&sem, 0, 0);
}

/*
 * Class:     TI_PiGpio
 * Method:    terminate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_TI_PiGpio_terminate  (JNIEnv* env, jclass cl)
{
	gpioTerminate();
}

/*
 * Class:     TI_PiGpio
 * Method:    setMode
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_setMode  (JNIEnv* env, jclass cl, jint pin, jint mode)
{
	return gpioSetMode(pin, mode);
}

/*
 * Class:     TI_PiGpio
 * Method:    getMode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_getMode  (JNIEnv* env, jclass cl, jint pin)
{
	return gpioGetMode(pin);
}

/*
 * Class:     TI_PiGpio
 * Method:    setPullUpDown
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_setPullUpDown  (JNIEnv* env, jclass cl, jint pin, jint pud)
{
	return gpioSetPullUpDown(pin, pud);
}

/*
 * Class:     TI_PiGpio
 * Method:    read
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_read  (JNIEnv* env, jclass cl, jint pin)
{
	return gpioRead(pin);
}
/*
 * Class:     TI_PiGpio
 * Method:    write
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_write  (JNIEnv* env, jclass cl, jint pin, jboolean level)
{
	return gpioWrite(pin, level);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwm
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwm  (JNIEnv* env, jclass cl, jint pin, jint dutycycle)
{
	return gpioPWM(pin, dutycycle);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwmGetDutyCycle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwmGetDutyCycle  (JNIEnv* env, jclass cl, jint pin)
{
	return gpioGetPWMdutycycle(pin);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwmSetRange
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwmSetRange  (JNIEnv* env, jclass cl, jint pin, jint range)
{
	return gpioSetPWMrange(pin, range);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwmGetRange
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwmGetRange  (JNIEnv* env, jclass cl, jint pin)
{
	return gpioGetPWMrange(pin);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwmGetRealRange
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwmGetRealRange  (JNIEnv* env, jclass cl, jint pin)
{
	return gpioGetPWMrealRange(pin);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwmSetFreq
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwmSetFreq  (JNIEnv* env, jclass cl, jint pin, jint freq)
{
	return gpioSetPWMfrequency(pin, freq);
}

/*
 * Class:     TI_PiGpio
 * Method:    pwmGetFreq
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pwmGetFreq  (JNIEnv* env, jclass cl, jint pin)
{
	return gpioGetPWMfrequency(pin);
}

/*
 * Class:     TI_PiGpio
 * Method:    servo
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_servo  (JNIEnv* env, jclass cl, jint pin, jint width)
{
	return gpioServo(pin, width);
}

/*
 * Class:     TI_PiGpio
 * Method:    delay
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_delay  (JNIEnv* env, jclass cl, jint seconds, jint micros)
{
	return gpioSleep(PI_TIME_RELATIVE, seconds, micros);
}


int state = 0;
int pi_waitFor;
uint32_t pi_startTick = 0;
uint32_t pi_endTick = 1;


void pulseInAlertFunc(int pin, int level, uint32_t tick)
{
	if(level == pi_waitFor)
	{
		pi_startTick = tick;
		state++;
	}
	else
	{
		pi_endTick = tick;
		gpioSetAlertFunc(pin, NULL);
		sem_post(&sem);
	}
}

/*
 * Class:     TI_PiGpio
 * Method:    pulseIn
 * Signature: (IZI)I
 */
JNIEXPORT jint JNICALL Java_TI_PiGpio_pulseIn2  (JNIEnv * env, jclass cl, jint pin, jboolean waitFor, jint timeout)
{
	if(gpioRead(pin) == waitFor)
		return -2;
	struct timespec ts;
	pi_waitFor = waitFor;
	pi_startTick = gpioTick();
	pi_endTick = 1;

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
	ts.tv_nsec += 1000000 * (timeout%1000);
	ts.tv_sec += 1000000 * (timeout/1000);
	if (ts.tv_nsec>=1000000000) {
		ts.tv_sec+=1;
		ts.tv_nsec-=1000000000;
	}

	gpioSetAlertFunc(pin, pulseInAlertFunc);
//	sem_wait(&sem);
	if(sem_timedwait(&sem, &ts) < 0)
		return -1;

	return pi_endTick - pi_startTick;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_pulseIn  (JNIEnv * env, jclass cl, jint pin, jboolean waitFor, jint timeout)
{
    uint32_t startTick, endTick, timeoutTick;
    if(gpioRead(pin) == waitFor)
	return -2;
    timeoutTick = gpioTick() + timeout;

    while(gpioRead(pin) != waitFor && timeoutTick > gpioTick())
	;
    startTick = gpioTick();
    if(startTick >= timeoutTick)
	return -1;
    while(gpioRead(pin) == waitFor && timeoutTick > gpioTick())
	;
    endTick = gpioTick();

    if(startTick >= timeoutTick)
	return 0;


    return endTick - startTick;
}


JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cOpen  (JNIEnv* env, jclass jc, jint i2cBus, jint i2cAddr, jint i2cFlags)
{
	return i2cOpen(i2cBus, i2cAddr, i2cFlags);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cClose  (JNIEnv* env, jclass jc, jint handle)
{
	return i2cClose(handle);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteQuick  (JNIEnv* env, jclass jc, jint handle, jint bit)
{
	return i2cWriteQuick(handle, bit);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteByte  (JNIEnv* env, jclass jc, jint handle, jint bVal)
{
	return i2cWriteByte(handle, bVal);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cReadByte  (JNIEnv* env, jclass jc, jint handle)
{
	return i2cReadByte(handle);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteByteData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jint bVal)
{
	return i2cWriteByteData(handle, i2cReg, bVal);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteWordData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jint wVal)
{
	return i2cWriteWordData(handle, i2cReg, wVal);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cReadByteData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg)
{
	return i2cReadByteData(handle, i2cReg);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cReadWordData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg)
{
	return i2cReadWordData(handle, i2cReg);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cProcessCall  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jint wVal)
{
	return i2cProcessCall(handle, i2cReg, wVal);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteBlockData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cReadBlockData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cBlockProcessCall  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteI2CBlockData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cReadI2CBlockData  (JNIEnv* env, jclass jc, jint handle, jint i2cReg, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cReadDevice  (JNIEnv* env, jclass jc, jint handle, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_i2cWriteDevice  (JNIEnv* env, jclass jc, jint handle, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_spiOpen  (JNIEnv* env, jclass jc, jint spiChan, jint baud, jint spiFlags)
{
	return spiOpen(spiChan, baud, spiFlags);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_spiClose  (JNIEnv* env, jclass jc, jint handle)
{
	return spiClose(handle);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_spiRead  (JNIEnv* env, jclass jc, jint handle, jbyteArray buf)
{
//	return spiRead(handle);
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_spiWrite  (JNIEnv* env, jclass jc, jint handle, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_spiXfer  (JNIEnv* env, jclass jc, jint handle, jbyteArray txBuf, jbyteArray rxBuf)
{

}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serOpen  (JNIEnv* env, jclass jc, jstring sertty, jint baud, jint serFlags)
{
	const char *c_sertty = (*env)->GetStringUTFChars(env, sertty, 0);
	int handle = serOpen((char*)c_sertty, baud, serFlags);
	(*env)->ReleaseStringUTFChars(env, sertty, c_sertty);
	return handle;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serClose  (JNIEnv* env, jclass jc, jint handle)
{
	return serClose(handle);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serWriteByte  (JNIEnv* env, jclass jc, jint handle, jint val)
{
	return serWriteByte(handle, val);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serReadByte  (JNIEnv* env, jclass jc, jint handle)
{
	return serReadByte(handle);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serWrite  (JNIEnv* env, jclass jc, jint handle, jbyteArray buf)
{
//	return serWrite(handle);
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serRead  (JNIEnv* env, jclass jc, jint handle, jbyteArray buf)
{
	return 0;
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_serDataAvailable  (JNIEnv* env, jclass jc, jint handle)
{
	return serDataAvailable(handle);
}


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define ADS1015_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)
/*=========================================================================*/

/*=========================================================================
    CONVERSION DELAY (in mS)
    -----------------------------------------------------------------------*/
    #define ADS1015_CONVERSIONDELAY         (1)
    #define ADS1115_CONVERSIONDELAY         (8)
/*=========================================================================*/

/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1015_REG_POINTER_MASK        (0x03)
    #define ADS1015_REG_POINTER_CONVERT     (0x00)
    #define ADS1015_REG_POINTER_CONFIG      (0x01)
    #define ADS1015_REG_POINTER_LOWTHRESH   (0x02)
    #define ADS1015_REG_POINTER_HITHRESH    (0x03)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1015_REG_CONFIG_OS_MASK      (0x8000)
    #define ADS1015_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1015_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1015_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

    #define ADS1015_REG_CONFIG_MUX_MASK     (0x7000)
    #define ADS1015_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
    #define ADS1015_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    #define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    #define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    #define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

    #define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1015_REG_CONFIG_MODE_MASK    (0x0100)
    #define ADS1015_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1015_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1015_REG_CONFIG_DR_MASK      (0x00E0)..
    #define ADS1015_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
    #define ADS1015_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
    #define ADS1015_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
    #define ADS1015_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
    #define ADS1015_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
    #define ADS1015_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
    #define ADS1015_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

    #define ADS1015_REG_CONFIG_CMODE_MASK   (0x0010)
    #define ADS1015_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    #define ADS1015_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

    #define ADS1015_REG_CONFIG_CPOL_MASK    (0x0008)
    #define ADS1015_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1015_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1015_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
    #define ADS1015_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1015_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1015_REG_CONFIG_CQUE_MASK    (0x0003)
    #define ADS1015_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
    #define ADS1015_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
    #define ADS1015_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
    #define ADS1015_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
/*=========================================================================*/

typedef enum
{
  GAIN_TWOTHIRDS    = ADS1015_REG_CONFIG_PGA_6_144V,
  GAIN_ONE          = ADS1015_REG_CONFIG_PGA_4_096V,
  GAIN_TWO          = ADS1015_REG_CONFIG_PGA_2_048V,
  GAIN_FOUR         = ADS1015_REG_CONFIG_PGA_1_024V,
  GAIN_EIGHT        = ADS1015_REG_CONFIG_PGA_0_512V,
  GAIN_SIXTEEN      = ADS1015_REG_CONFIG_PGA_0_256V
} adsGain_t;


inline uint16_t flipWord(uint16_t w)
{
    return (w>>8) | ((w&0xff)<<8);
}

JNIEXPORT jint JNICALL Java_TI_PiGpio_analogRead  (JNIEnv * env, jclass jc, jint pin)
{
    if(pin < 0 || pin > 3)
	return 0;
    static const int pins[] = { ADS1015_REG_CONFIG_MUX_SINGLE_0, ADS1015_REG_CONFIG_MUX_SINGLE_1, ADS1015_REG_CONFIG_MUX_SINGLE_2, ADS1015_REG_CONFIG_MUX_SINGLE_3 };

    int spiHandle = i2cOpen(1, 0x48, 0);
    
    uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
	            ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE  |   // Single-shot mode (default)
		    ADS1015_REG_CONFIG_OS_SINGLE |
		    GAIN_TWOTHIRDS |
		    pins[pin];
    
    i2cWriteWordData(spiHandle, ADS1015_REG_POINTER_CONFIG, flipWord(config));
    gpioDelay(1000);
    int data = i2cReadWordData(spiHandle, ADS1015_REG_POINTER_CONVERT);
    data = flipWord(data);
    data >>= 4;
    i2cClose(spiHandle);
    return data;
}
