/*!
 *  @file SA1350_Firmware.h
 *
 *  Created on: Nov 7, 2016
 *      Author: a0322160
 */

#ifndef SA1350_FIRMWARE_H_
#define SA1350_FIRMWARE_H_

/***** Includes *****/

#include <math.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <ti/sysbios/gates/GateMutexPri.h>

/* Drivers */
#include <ti/drivers/Power.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/rf/RF.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
#include <ti/devices/cc13x0/driverlib/chipinfo.h>

#include "Board.h"
#include "smartrf_settings/smartrf_settings.h"
#include "splash_image/splash_image.h"

/***** Global Defines *****/

#define SA1350FW_MAJOR_VERSION	(1U)	/*!< X in X.Y version number format	*/
#define SA1350FW_MINOR_VERSION	(3U)	/*!< Y in X.Y version number format	*/

#define RF_TASK_STACK_SIZE		(2048U)	/*!< Stack for RF task				*/
#define RF_TASK_PRIORITY		(3U)	/*!< Priority for RF task			*/

#define DISPLAY_TASK_STACK_SIZE	(768U)	/*!< Stack for Display task			*/
#define DISPLAY_TASK_PRIORITY	(3U)	/*!< Priority for Display task		*/

#define BUTTON_TASK_STACK_SIZE	(768U)	/*!< Stack for Button task			*/
#define BUTTON_TASK_PRIORITY	(1U)	/*!< Priority for Button task		*/

#define UART_TASK_STACK_SIZE	(768U)	/*!< Stack for UART task			*/
#define UART_TASK_PRIORITY		(2U)	/*!< Priority for UART task			*/

/***** Global Structures *****/

/** @brief A type and struct for passing sweep user commands within a Mailbox.
 */
typedef struct CommandMessage {
	/** @brief An enum for available user commands.
	  */
	enum Command
	{
		NO_USER_COMMAND = 0,	/*!< No pending user command				*/
		DECREMENT_FREQ, 		/*!< Decrement RF center freq w/o speed.	*/
		INCREMENT_FREQ,			/*!< Increment RF center freq w/o speed.	*/
		FAST_DEC_FREQ,   		/*!< Decrement RF center freq w/ speedup.	*/
		FAST_INC_FREQ,			/*!< Increment RF center freq w/ speedup.	*/
		DECREMENT_SPAN,			/*!< Decrement RF span						*/
		INCREMENT_SPAN, 		/*!< Increment RF span						*/
		CHANGE_SPAN,    		/*!< Change to a specific RF span			*/
		CHANGE_BAND,			/*!< Change to a specific RF band			*/
		NEXT_BAND,				/*!< Change to the next RF band				*/
		CHANGE_MODE,			/*!< Change the RF adjustment mode			*/
		SET_START_FREQ,  		/*!< Set new start frequency				*/
		SET_END_FREQ,			/*!< Set new stop frequency					*/
        SET_RBW,                /*!< Set new RBW value                      */
        SET_FREQSTEP,           /*!< Set new Freq step value                */
        SET_STEPCOUNT,          /*!< Set new Freq step count value          */
        SET_SPAN,               /*!< Set new span value                     */
		SEND_SPECTRUM			/*!< Sending spectrum sweep to host			*/
	} command;					/*!< User command to pass to other task		*/
	uint8_t payload[4];			/*!< Payload of user command to pass		*/
} CommandMessage;

/***** Global Variables *****/

/***** Prototypes *****/

extern void RfTask_init(void);
extern inline void     setSweepCmd(CommandMessage *sweepCmd);
extern inline void     setPendSweepCmd(CommandMessage *pendSweepCmd);
extern inline uint16_t getStartFreq(void);
extern inline uint16_t getStartFracFreq(void);
extern inline uint16_t getEndFreq(void);
extern inline uint16_t getEndFracFreq(void);
extern inline uint16_t getFreqStep(void);
extern inline uint16_t getNumSteps(void);
extern inline uint8_t  getRbwTableLength(void);
extern inline uint8_t  getRbwTableEntryLength(void);
extern uint8_t*        getRbwTableEntryData(uint8_t rbwIndex, uint8_t rbwBand);
extern inline uint16_t getSweepMaxLength(void);
extern inline uint16_t getSweepLength(void);
extern inline int8_t*  getSweepData(void);
extern inline void     getNewSweep(void);
extern inline IArg     lockSweepData(void);
extern inline void     unlockSweepData(IArg unlockKey);
extern inline IArg     lockSweepCmd(void);
extern inline void     unlockSweepCmd(IArg unlockKey);

extern void DisplayTask_init(void);
extern void setDisplayUpdate(void);

extern void ButtonTask_init(void);
extern inline const char* getButtonModeString(void);
extern void               lockButton(void);
extern void               unlockButton(void);

extern void UartTask_init(void);

#endif /* SA1350_FIRMWARE_H_ */
