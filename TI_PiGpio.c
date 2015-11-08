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
