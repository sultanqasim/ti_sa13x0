/*!
 *  @file rfSweep.c
 */
#include "SA1350_Firmware.h"

/** @struct SASpan
 *
 * EasyRF Spans
 * --------------------------------------------------------------------
 * |Span (MHz) |Steps/MHz |FSW (kHz) |FSW (dec)*|RBW (kHz) |RBW (dec) |
 * |:---------:|:--------:|:--------:|:--------:|:--------:|:--------:|
 * |24		   |12	      |83	     |5462	    |196	   |39        |
 * |18	       |16	      |63	     |4096	    |155	   |38        |
 * |16	       |18	      |56	     |3640	    |118	   |37        |
 * |12	       |24	      |42	     |2731	    |78		   |35        |
 * |8	       |36	      |28	     |1820	    |59		   |34        |
 * |6	       |48	      |21	     |1366	    |49		   |33        |
 * |4	       |72	      |14	     |910	    |39		   |32        |
 * |2	       |144	      |7	     |455	    |39		   |32        |
 * |1	       |288	      |3	     |228	    |39		   |32        |
 *
 * *Best step sizes for fractFreq determined empirically
 */

/***** Local Defines *****/

#define DEFAULT_BAND_900M   (1U)		/*!< (0) 400MHz, (1) 900MHz			*/
#define MAX_SWEEP_LENGTH    (2048U)		/*!< Allocated size of RSSI array	*/

/**  @{ */
/*!  See \ref SASpan for RF span table */
#define SPAN0			(24U)
#define SPAN1			(18U)
#define SPAN2			(16U)
#define SPAN3			(12U)
#define SPAN4			(8U)
#define SPAN5			(6U)
#define SPAN6			(4U)
#define SPAN7			(2U)
#define SPAN8			(1U)

#define NUMSTEPS0		(12U)
#define NUMSTEPS1		(16U)
#define NUMSTEPS2		(18U)
#define NUMSTEPS3		(24U)
#define NUMSTEPS4		(36U)
#define NUMSTEPS5		(48U)
#define NUMSTEPS6		(72U)
#define NUMSTEPS7		(144U)
#define NUMSTEPS8		(288U)

#define STEP0			(5462U)
#define STEP1			(4096U)
#define STEP2			(3640U)
#define STEP3			(2731U)
#define STEP4			(1820U)
#define STEP5			(1366U)
#define STEP6			(910U)
#define STEP7			(455U)
#define STEP8			(228U)

#define RBW0			(39U)
#define RBW1			(38U)
#define RBW2			(37U)
#define RBW3			(35U)
#define RBW4			(34U)
#define RBW5			(33U)
#define RBW6			(32U)
#define RBW7			(32U)
#define RBW8			(32U)
/**  @} */

/**  @{ */
/*! Utilize final span for Expert RF mode */
#define EXPERTSPANINDEX ((sizeof(sa1350SpanParams) / sizeof(SASpan)) - 1U)
#define MINEZSPANINDEX	(0U)
#define MAXEZSPANINDEX  (EXPERTSPANINDEX - 1U)
/**  @} */

/*! Max number of entries in device-specific rxBW table */
#define MAX_ITEMS_RBW   (21U)

/**  @{ */
/** @brief Initial values for available RF bands of the RF sweep.
 *
 * 431--432---|---456--527
 * 861--903---|---927-1054
 * 2142-2428--|--2452-2635
 *
 */
#define STARTFREQ_400	(432U)
#define STARTFREQ_900	(903U)
#define STARTFREQ_2400	(2428U)
#define ENDFREQ_400		(STARTFREQ_400 + SPAN0)
#define ENDFREQ_900		(STARTFREQ_900 + SPAN0)
#define ENDFREQ_2400	(STARTFREQ_2400 + SPAN0)

#define MINFREQ_400		(431U)
#define MAXFREQ_400		(527U)
#define MINFREQ_900		(861U)
#define MAXFREQ_900		(1054U)
#define MINFREQ_2400	(2152U)
#define MAXFREQ_2400	(2635U)

#define IODIV_440MHZ	(10U)
#define IODIV_915MHZ	(5U)
#define IODIV_2440MHZ	(2U)
/**  @} */

/*! Repeat count for FAST_DEC_FREQ and FAST_INC_FREQ frequency changes	*/
#define FAST_CHANGE_COUNT	(6U)

/***** Structures *****/

/** @brief A type and struct for holding parameters of the RF sweep.
 */
typedef struct SAParams {
	uint16_t saStartFreq;		/*!< Integer value of start frequency		*/
	uint16_t saStartFracFreq;	/*!< Fractional value of start frequency	*/
	uint16_t saEndFreq;			/*!< Integer value of end frequency			*/
	uint16_t saEndFracFreq;		/*!< Fractional value of end frequency		*/
	uint16_t saMinFreq;			/*!< Minimum frequency allowed in band		*/
	uint16_t saMaxFreq;			/*!< Maximum frequency allowed in band		*/
	uint16_t saFreqStep;		/*!< Frequency step width of span (dec.)	*/
	uint16_t saNumSteps;		/*!< Number of frequency steps per MHz		*/
	uint8_t  saRBW;				/*!< Resolution bandwidth of span (dec.)	*/
	uint8_t  saSpanIndex;		/*!< Index into SA_span array of spans		*/
	uint16_t saSpan;			/*!< Integer value of frequency span (MHz)	*/
} SAParams;

/** @brief A type and struct for the table entries of rxBW values from TRM.
 */
typedef struct SARBW
{
    double   rbwRBW;     /*!< Receiver Bandwidth Setting in kHz                */
    uint16_t rbwIF;      /*!< Received Intermediate Frequency in kHz           */
    uint8_t  rbwSetting; /*!< rxBw bit-20 in CMD_PROP_RADIO Command Structures */
} SARBW;

/** @brief A type and struct for holding parameters of predefined RF spans.
 */
typedef struct SASpan {
	uint16_t spanFreqStep;		/*!< Frequency step width of span (dec.)	*/
	uint16_t spanNumSteps;		/*!< Number of frequency steps per MHz		*/
	uint8_t  spanRBW;			/*!< Resolution bandwidth of span (dec.)	*/
	uint16_t  spanSpan;			/*!< Integer value of frequency span (MHz)	*/
} SASpan;

/** @brief A type and enum for available RF bands of the RF sweep.
 */
typedef enum SABand {
    BAND_UNKNOWN = 0,
	BAND_400M ,					/*!< 400MHz RF band							*/
	BAND_900M,					/*!< 900MHz RF band							*/
	BAND_2400M,					/*!< 2.4GHz RF band							*/
} SABand;

/** @brief A type and enum for available CHANGE_BAND user command arguments.
 */
typedef enum ChangeBandArg
{
	SET_400M_BAND = 0,		/*!< Change to 400MHz RF band					*/
	SET_900M_BAND = 1,		/*!< Change to 900MHz RF band					*/
	SET_2400M_BAND = 2,		/*!< Change to 2.4GHz RF band					*/
	SET_NEXT_BAND = 0x55	/*!< Change to next RF band						*/
} ChangeBandArg;

/***** Variable declarations *****/

/** @brief Task struct for RF Task.
 */
Task_Struct rfTaskStruct;

/** @brief Task parameters for RF Task.
 */
static Task_Params rfTaskParams;

/** @brief Task stack definition for RF Task.
 */
static Char rfTaskStack[RF_TASK_STACK_SIZE];

/** @brief RF driver RF Object for the application.
 */
static RF_Object rfObject;

/** @brief RF parameters for the application.
 */
static RF_Params rfParams;

/** @brief RF driver handle for the application.
 */
static RF_Handle rfHandle;

/** @brief Mailbox struct for the RF command mailbox.
 */
Mailbox_Struct rfMailboxStruct;

/** @brief Mailbox parameters for the RF command mailbox.
 */
static Mailbox_Params rfMailboxParams;

/** @brief Mailbox driver handle for the RF command mailbox.
 */
Mailbox_Handle rfMailbox;

/** @brief  GateMutexPri struct for the RF sweep gate mutex.
 */
GateMutexPri_Struct sweepMutexStruct;

/** @brief  GateMutexPri parameters for the RF sweep gate mutex.
 */
static GateMutexPri_Params sweepMutexParams;

/** @brief  GateMutexPri driver handle for the RF sweep gate mutex.
 */
GateMutexPri_Handle sweepMutex;

/** @brief  GateMutex struct for the RF command gate mutex.
 */
GateMutex_Struct commandMutexStruct;

/** @brief  GateMutex parameters for the RF command gate mutex.
 */
static GateMutex_Params commandMutexParams;

/** @brief  GateMutex driver handle for the RF command gate mutex.
 */
GateMutex_Handle commandMutex;

/** @brief Semaphore struct for the new spectrum semaphore.
 */
static Semaphore_Struct newSpectrumSemaphoreStruct;

/** @brief Semaphore parameters for the new spectrum semaphore.
 */
static Semaphore_Params newSpectrumSemaphoreParams;

/** @brief Semaphore handle for the new spectrum semaphore.
 */
Semaphore_Handle newSpectrumSemaphore;

/** @brief PIN driver handle for the RF switch control.
 */
static PIN_Handle rfSwPinHandle;

/** @brief PIN driver state for the RF switch control.
*/
static PIN_State rfSwPinState;

/** @brief PIN driver pin list and pin attributes for the RF switch control.
 *         Control the RF switch for selecting between 2.4G and 1G antennas.
 */
static PIN_Config rfSwPinTable[] = {
    Board_DIO1_RFSW	| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_DIO30_SWPWR | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	PIN_TERMINATE
};

/** @brief #SAParams used to store current RF parameters.
 */
static SAParams sa1350Params;

/** @brief A second copy of the RF parameters for staging updates.
 */
static SAParams sa1350CmdParams;

/** @brief Table of CC13xx (868MHz) rxBW values from TRM vH.
 */
static const SARBW CC13xxSubGigTableRBW[MAX_ITEMS_RBW]=
{
    /* RBW(khz), IF(khz), reg setting(int) */
    {38.9,    250, 32},
    {49.0,    250, 33},
    {58.9,    250, 34},
    {77.7,    250, 35},
    {98.0,    250, 36},
    {117.7,   250, 37},
    {155.4,   500, 38},
    {195.9,   500, 39},
    {235.5,   500, 40},
    {310.8,  1000, 41},
    {391.8,  1000, 42},
    {470.9,  1000, 43},
    {621.6,  1000, 44},
    {783.6,  1000, 45},
    {941.8,  1000, 46},
    {1243.2, 1000, 47},
    {1567.2, 1000, 48},
    {1883.7, 1000, 49},
    {2486.5, 1000, 50},
    {3134.4, 1000, 51},
    {3767.4, 1000, 52}
};

/** @brief Table of CC13xx (2.4GHz) rxBW values from TRM vH.
 */
static const SARBW CC13xx2_4GTableRBW[MAX_ITEMS_RBW]=
{
    /* RBW(khz), IF(khz), reg setting(int) */
    {43.5,    250, 32},
    {54.9,    250, 33},
    {66.0,    250, 34},
    {87.1,    250, 35},
    {109.8,   250, 36},
    {131.9,   250, 37},
    {174.2,   500, 38},
    {219.6,   500, 39},
    {263.9,   500, 40},
    {348.3,  1000, 41},
    {439.1,  1000, 42},
    {527.8,  1000, 43},
    {696.7,  1000, 44},
    {878.2,  1000, 45},
    {1055.6, 1000, 46},
    {1393.3, 1000, 47},
    {1756.4, 1000, 48},
    {2111.1, 1000, 49},
    {2786.7, 1000, 50},
    {3512.9, 1000, 51},
    {4222.2, 1000, 52}
};

/** @brief Variable for storing a single entry from RBW table.
 */
static uint8_t rbwTableEntryData[sizeof(CC13xxSubGigTableRBW[0U].rbwRBW) +
			sizeof(CC13xxSubGigTableRBW[0U].rbwIF) +
			sizeof(CC13xxSubGigTableRBW[0U].rbwSetting)];

/** @brief Current span parameters supported by EasyRF.
 *
 *  Somewhat arbitrarily chosen, total steps is multiple of 96 (24*12=288)
 */
static SASpan sa1350SpanParams[] = {
	{ STEP0, NUMSTEPS0, RBW0, SPAN0 },
	{ STEP1, NUMSTEPS1, RBW1, SPAN1 },
	{ STEP2, NUMSTEPS2, RBW2, SPAN2 },
	{ STEP3, NUMSTEPS3, RBW3, SPAN3 },
	{ STEP4, NUMSTEPS4, RBW4, SPAN4 },
	{ STEP5, NUMSTEPS5, RBW5, SPAN5 },
	{ STEP6, NUMSTEPS6, RBW6, SPAN6 },
	{ STEP7, NUMSTEPS7, RBW7, SPAN7 },
	{ STEP8, NUMSTEPS8, RBW8, SPAN8 },
	/*** This span is used for Expert RF Mode and it   ***/
	/*** is only initialized to a default values here. ***/
	{ STEP8, NUMSTEPS8, RBW8, SPAN8 }
};

/** @brief Array for capturing RSSI values from sweep.
 */
static int8_t rssiArray[MAX_SWEEP_LENGTH] = {0};

/** @brief Status of the command mode for sweep control.
 *
 *  Default to incremental (button) control mode
 */
static _Bool isCommandMode = FALSE;

/***** Prototypes *****/

static inline void setStartFreq(uint16_t startFreq);
static inline void setStartFracFreq(uint16_t startFracFreq);
static inline void setEndFreq(uint16_t endFreq);
static inline void setEndFracFreq(uint16_t endFracFreq);
static inline void setMinFreq(uint16_t minFreq);
static inline void setMaxFreq(uint16_t maxFreq);
static inline void setFreqStep(uint16_t freqStep);
static inline void setNumSteps(uint16_t numSteps);
static inline void setRBW(uint8_t rbw);
static inline void setSpanIndex(uint8_t spanIndex);
static inline uint16_t getMinFreq(void);
static inline uint16_t getMaxFreq(void);
static inline uint8_t getRBW(void);
static inline uint8_t getSpanIndex(void);
static inline uint16_t getSpanFreqStep(void);
static inline uint16_t getSpanNumSteps(void);
static inline uint8_t getSpanRBW(void);
static inline uint16_t getSpan(void);
static void updateSweepFreq(void);
static void setNewSweep(void);
static void decreaseFreq(void);
static void increaseFreq(void);
static void fastDecreaseFreq(void);
static void fastIncreaseFreq(void);
static uint8_t nextSpanDelta(uint8_t nextSpanIndex);
static uint16_t fracFreqStepIndex(uint16_t fracFreq);
static void remapFracFreqs(uint8_t nextSpanIndex);
static void updateRadioRF(void);
static void decreaseSpan(void);
static void increaseSpan(void);
static void updateSpanFreqs(void);
static void updateExpertSpan(void);
static void changeSpan(uint8_t index);
static SABand getBand(void);
static void setBand(SABand band);
static void nextBand(SABand currBand);
static void changeBand(ChangeBandArg newBand);
static _Bool getCommandMode(void);
static void setCommandMode(_Bool commandMode);
static void cmdSetStartFreq(const uint8_t *values);
static void cmdSetEndFreq(const uint8_t *values);
static void cmdSetRbw(const uint8_t *values);
static void cmdSetFreqStep(const uint8_t *values);
static void cmdSetStepCount(const uint8_t *values);
static void cmdSetSpan(const uint8_t *values);
static _Bool rfCommand(void);
static void updateSweepState(uint16_t *sweepIndex);
static void rfCallbackFxn(RF_Handle hRf, RF_CmdHandle hRfC, RF_EventMask e);
static void openRadio(void);
static void RfMailbox_init(void);
static void RfGateMutex_init(void);
static void rfTaskFxn(UArg rfArg0, UArg rfArg1);

/***** Function definitions *****/

/** @brief Setter function for current start frequency.
 *
 *  @param startFreq new start frequency.
 *
 *  @par Usage
 *       @code
 *       setStartFreq(STARTFREQ_400);
 *       @endcode
 */
static inline void setStartFreq(uint16_t startFreq)
{
	sa1350Params.saStartFreq = startFreq;
}

/** @brief Setter function for current fractional start frequency.
 *
 *  @param startFracFreq new fractional start frequency.
 *
 *  @par Usage
 *       @code
 *       setStartFracFreq(0U);
 *       @endcode
 */
static inline void setStartFracFreq(uint16_t startFracFreq)
{
	sa1350Params.saStartFracFreq = startFracFreq;
}

/** @brief Setter function for current end frequency.
 *
 *  @param endFreq new end frequency.
 *
 *  @par Usage
 *       @code
 *       setEndFreq(ENDFREQ_400);
 *       @endcode
 */
static inline void setEndFreq(uint16_t endFreq)
{
	sa1350Params.saEndFreq = endFreq;
}

/** @brief Setter function for current fractional end frequency.
 *
 *  @param endFracFreq new fractional end frequency.
 *
 *  @par Usage
 *       @code
 *       setEndFracFreq(0U);
 *       @endcode
 */
static inline void setEndFracFreq(uint16_t endFracFreq)
{
	sa1350Params.saEndFracFreq = endFracFreq;
}

/** @brief Setter function for current minimum frequency of band.
 *
 *  @param minFreq new minimum frequency.
 *
 *  @par Usage
 *       @code
 *       setMinFreq(MINFREQ_400);
 *       @endcode
 */
static inline void setMinFreq(uint16_t minFreq)
{
	sa1350Params.saMinFreq = minFreq;
}

/** @brief Setter function for current maximum frequency of band.
 *
 *  @param maxFreq new maximum frequency.
 *
 *  @par Usage
 *       @code
 *       setMaxFreq(MAXFREQ_400);
 *       @endcode
 */
static inline void setMaxFreq(uint16_t maxFreq)
{
	sa1350Params.saMaxFreq = maxFreq;
}

/** @brief Setter function for current frequency step width (dec.).
 *
 *  @param freqStep new frequency step width (dec.).
 *
 *  @par Usage
 *       @code
 *       setFreqStep(getSpanFreqStep());
 *       @endcode
 */
static inline void setFreqStep(uint16_t freqStep)
{
	sa1350Params.saFreqStep = freqStep;
}

/** @brief Setter function for current number of frequency steps per MHz.
 *
 *  @param numSteps new frequency steps per Mhz.
 *
 *  @par Usage
 *       @code
 *       setNumSteps();
 *       @endcode
 */
static inline void setNumSteps(uint16_t numSteps)
{
	sa1350Params.saNumSteps = numSteps;
}

/** @brief Setter function for current resolution bandwidth (dec.).
 *
 *  @param rbw new resolution bandwidth (dec.).
 *
 *  @par Usage
 *       @code
 *       setRBW(getSpanRBW());
 *       @endcode
 */
static inline void setRBW(uint8_t rbw)
{
	sa1350Params.saRBW = rbw;
}

/** @brief Setter function for current span index into array of spans.
 *
 *  @param spanIndex new span index into array of spans.
 *
 *  @par Usage
 *       @code
 *       setSpanIndex(MINSPANINDEX);
 *       @endcode
 */
static inline void setSpanIndex(uint8_t spanIndex)
{
	sa1350Params.saSpanIndex = spanIndex;
}

/** @brief Getter function for current minimum frequency of band.
 *
 *  @return Current minimum frequency
 *
 *  @par Usage
 *       @code
 *       getMinFreq();
 *       @endcode
 */
static inline uint16_t getMinFreq(void)
{
	return sa1350Params.saMinFreq;
}

/** @brief Getter function for current maximum frequency of band.
 *
 *  @return Current maximum frequency
 *
 *  @par Usage
 *       @code
 *       getMaxFreq();
 *       @endcode
 */
static inline uint16_t getMaxFreq(void)
{
	return sa1350Params.saMaxFreq;
}

/** @brief Getter function for current resolution bandwidth (dec.).
 *
 *  @return Current resolution bandwidth (dec.)
 *
 *  @par Usage
 *       @code
 *       getRBW();
 *       @endcode
 */
static inline uint8_t getRBW(void)
{
	return sa1350Params.saRBW;
}

/** @brief Getter function for current span index into array of spans.
 *
 *  @return Current span index into array of spans
 *
 *  @par Usage
 *       @code
 *       getSpanIndex();
 *       @endcode
 */
static inline uint8_t getSpanIndex(void)
{
	return sa1350Params.saSpanIndex;
}

/** @brief Getter function for current span's frequency step width (dec.).
 *
 *  @return Current span's frequency step width (dec.)
 *
 *  @par Usage
 *       @code
 *       getSpanFreqStep();
 *       @endcode
 */
static inline uint16_t getSpanFreqStep(void)
{
        return sa1350SpanParams[getSpanIndex()].spanFreqStep;
}

/** @brief Getter function for current span's number of frequency steps per MHz.
 *
 *  @return Current span's frequency steps per MHz
 *
 *  @par Usage
 *       @code
 *       getSpanNumSteps();
 *       @endcode
 */
static inline uint16_t getSpanNumSteps(void)
{
	return sa1350SpanParams[getSpanIndex()].spanNumSteps;
}

/** @brief Getter function for current span's resolution bandwidth (dec.).
 *
 *  @return Current span's resolution bandwidth (dec.)
 *
 *  @par Usage
 *       @code
 *       getSpanRBW();
 *       @endcode
 */
static inline uint8_t getSpanRBW(void)
{
	return sa1350SpanParams[getSpanIndex()].spanRBW;
}

/** @brief Getter function for current span's frequency span in MHz.
 *
 *  @return Current span's frequency span in MHz
 *
 *  @par Usage
 *       @code
 *       getSpan();
 *       @endcode
 */
static inline uint16_t getSpan(void)
{
	return sa1350SpanParams[getSpanIndex()].spanSpan;
}

/** @brief Increment the RF sweep frequency dependent upon RF mode.
 *
 *  @par Usage
 *       @code
 *       setNewSweep();
 *       @endcode
 */
static void updateSweepFreq(void)
{
	if (getSpanIndex() != EXPERTSPANINDEX)
	{
		/* Sweep freq by saNumSteps per MHz in saFreqStep increments		*/
		if (fracFreqStepIndex(RF_cmdFs.fractFreq) == (getNumSteps() - 1U))
		{
			RF_cmdFs.fractFreq = 0U;
			RF_cmdFs.frequency++;
		}
		else
		{
			RF_cmdFs.fractFreq += getFreqStep();
		}
	}
	else
	{
		/* Sweep freq in saFreqStep increments with an overflow per MHz		*/
		RF_cmdFs.fractFreq += getFreqStep();
		if (RF_cmdFs.fractFreq < getFreqStep() || getFreqStep() == 0)
		{
			RF_cmdFs.frequency++;
		}
	}
}

/** @brief Setter function for new sweep data request.
 *
 *  @par Usage
 *       @code
 *       setNewSweep();
 *       @endcode
 */
static void setNewSweep(void)
{
	/* Notify all pending tasks of new sweep */
	while(!Semaphore_getCount(newSpectrumSemaphore))
	{
		Semaphore_post(newSpectrumSemaphore);
	}

	/* Force new sweep requests to pend */
	Semaphore_pend(newSpectrumSemaphore, BIOS_NO_WAIT);
}

/** @brief Decrease center frequency of RF sweep by one step.
 *
 *  @par Usage
 *       @code
 *       decreaseFreq();
 *       @endcode
 */
static void decreaseFreq(void)
{
	if((getStartFreq() > getMinFreq())
		|| ((getStartFracFreq() > 0U) && (getStartFreq() == getMinFreq())))
	{
		/* Update start frequency */
		if (getStartFracFreq() == 0U)
		{
			setStartFracFreq(getFreqStep() * (getNumSteps() - 1U));
			setStartFreq(getStartFreq() - 1U);
		}
		else
		{
			setStartFracFreq(getStartFracFreq() - getFreqStep());
		}

		/* Update end frequency */
		if (getEndFracFreq() == 0U)
		{
			setEndFracFreq(getFreqStep() * (getNumSteps() - 1U));
			setEndFreq(getEndFreq() - 1U);
		}
		else
		{
			setEndFracFreq(getEndFracFreq() - getFreqStep());
		}
	}
}

/** @brief Increase center frequency of RF sweep by one step.
 *
 *  @par Usage
 *       @code
 *       increaseFreq();
 *       @endcode
 */
static void increaseFreq(void)
{
	if(getEndFreq() < getMaxFreq())
	{
		/* Update start frequency */
		if (fracFreqStepIndex(getStartFracFreq()) == (getNumSteps() - 1U))
		{
			setStartFracFreq(0U);
			setStartFreq(getStartFreq() + 1U);
		}
		else
		{
			setStartFracFreq(getStartFracFreq() + getFreqStep());
		}

		/* Update end frequency */
		if (fracFreqStepIndex(getEndFracFreq()) == (getNumSteps() - 1U))
		{
			setEndFracFreq(0U);
			setEndFreq(getEndFreq() + 1U);
		}
		else
		{
			setEndFracFreq(getEndFracFreq() + getFreqStep());
		}
	}
}

/** @brief Decrease center frequency of RF sweep by multiple steps.
 *
 *  @par Usage
 *       @code
 *       fastDecreaseFreq();
 *       @endcode
 */
static void fastDecreaseFreq(void)
{
	uint8_t changeCnt;

	for (changeCnt = 0; changeCnt < FAST_CHANGE_COUNT; changeCnt++)
	{
		decreaseFreq();
	}
}

/** @brief Increase center frequency of RF sweep by multiple steps.
 *
 *  @par Usage
 *       @code
 *       fastIncreaseFreq();
 *       @endcode
 */
static void fastIncreaseFreq(void)
{
	uint8_t changeCnt;

	for (changeCnt = 0; changeCnt < FAST_CHANGE_COUNT; changeCnt++)
	{
		increaseFreq();
	}
}

/** @brief Calculate span range difference in Mhz between current and
 *         potential next span.
 *
 *  @param nextSpanIndex index value for next span.
 *
 *  @return Absolute difference in span range (MHz) from current span
 *
 *  @par Usage
 *       @code
 *       nextSpanDelta(getSpanIndex()+1);
 *       @endcode
 */
static uint8_t nextSpanDelta(uint8_t nextSpanIndex)
{
	int32_t nextSpan = sa1350SpanParams[nextSpanIndex].spanSpan;

	return abs(getSpan() - nextSpan);
}

/** @brief Calculate step index of the fractional frequency value.
 *
 *  @param fracFreq Fractional frequency count to be indexed.
 *
 *  @return Step index of fractional frequency value
 *
 *  @par Usage
 *       @code
 *       fracFreqStepIndex(getEndFracFreq());
 *       @endcode
 */
static uint16_t fracFreqStepIndex(uint16_t fracFreq)
{
	return fracFreq / getFreqStep();
}

/** @brief Map fractional frequencies into next frequency span.
 *
 *  @param nextSpanIndex index value for next span.
 *
 *  @par Usage
 *       @code
 *       remapFracFreqs(getSpanIndex()-1);
 *       @endcode
 */
static void remapFracFreqs(uint8_t nextSpanIndex)
{
	double newStartIndex, newEndIndex;
	double nextNumSteps = getSpanNumSteps();
	double currNumSteps = getNumSteps();
	uint16_t nextFreqSteps = getSpanFreqStep();

	/* Get current fractional index */
	newStartIndex = fracFreqStepIndex(getStartFracFreq());
	newEndIndex = fracFreqStepIndex(getEndFracFreq());
	/* Map current fractional indexes to new span, prepare rounding */
	newStartIndex =
			(((double)newStartIndex * nextNumSteps) / currNumSteps) + 0.5;
	newEndIndex =
			(((double)newEndIndex * nextNumSteps) / currNumSteps) + 0.5;

	/* Round index to nearest integer, set new fractional freqs */
	setStartFracFreq((uint16_t)newStartIndex * nextFreqSteps);
	setEndFracFreq((uint16_t)newEndIndex * nextFreqSteps);
}

/** @brief Update RBW, frequency step size, and frequency step count for sweep.
 *         Open connection to radio for the sweep.
 *
 *  @par Usage
 *       @code
 *       updateRadioRF(();
 *       @endcode
**/
static void updateRadioRF(void)
{
	ChipType_t easyChipType = ChipInfo_GetChipType();

    /* Configure the radio for Proprietary mode */
    RF_Params_init(&rfParams);
	/* Close radio connection */
    if(rfHandle != (RF_Handle)NULL)
    {
    	RF_close(rfHandle);
    }

    /* Update SA_Params for new span */
    setFreqStep(getSpanFreqStep());
    setNumSteps(getSpanNumSteps());
    setRBW(getSpanRBW());

    RF_cmdPropRadioDivSetup.rxBw = getRBW();

    /* Select radio command based upon band */
    if (getStartFreq() < MINFREQ_2400)
    {
        if (easyChipType == CHIP_TYPE_CC1350)
        {
    	    /* Switch RF switch to Sub1G antenna */
            PIN_setOutputValue(rfSwPinHandle, Board_DIO1_RFSW, 1U);
        }

    	/* Request access to the radio */
    	rfHandle = RF_open(&rfObject, &RF_propSub1,
    			(RF_RadioSetup *)&RF_cmdPropRadioDivSetup, &rfParams);
    }
    /* Should never get here unless chip is CC1350 */
    else if (easyChipType == CHIP_TYPE_CC1350)
    {
        /* Switch RF switch to 2.4G antenna */
        PIN_setOutputValue(rfSwPinHandle, Board_DIO1_RFSW, 0U);

    	/* Request access to the radio */
    	rfHandle = RF_open(&rfObject, &RF_prop2_4,
    			(RF_RadioSetup *)&RF_cmdPropRadioDivSetup, &rfParams);
    }
    else /* Should never get here */
    {
    	rfHandle = (RF_Handle)NULL;
    }

    if (rfHandle == (RF_Handle)NULL)
    {
        System_abort("Error initializing radio\n");
    }
}

/** @brief Decrease span length (zoom in) of RF sweep by one span index.
 *
 *  @par Usage
 *       @code
 *       decreaseSpan();
 *       @endcode
 */
static void decreaseSpan(void)
{
	uint16_t decSpanDelta, decHalfFracFreq;
	_Bool isDecSpan = FALSE;

	if (getSpanIndex() < MAXEZSPANINDEX)
	{
		decSpanDelta = nextSpanDelta(getSpanIndex() + 1U);

		/* Update even Hz span deltas */
		if ((decSpanDelta > 1U) && (!((decSpanDelta % 2U) == 1U)))
		{
			setStartFreq(getStartFreq() + (decSpanDelta / 2U));
			setEndFreq(getEndFreq() - (decSpanDelta / 2U));

			isDecSpan = TRUE;
		}
		else if (decSpanDelta == 1U)
		{
			decHalfFracFreq = (getNumSteps() / 2U) * getFreqStep();

			if(fracFreqStepIndex(getStartFracFreq()) < (getNumSteps() / 2U))
			{
				setStartFracFreq(getStartFracFreq() + decHalfFracFreq);
			}
			else
			{
				setStartFreq(getStartFreq() + 1U);
				setStartFracFreq(getStartFracFreq() - decHalfFracFreq);
			}

			if(fracFreqStepIndex(getEndFracFreq()) >= (getNumSteps() / 2U))
			{
				setEndFracFreq(getEndFracFreq() - decHalfFracFreq);
			}
			else
			{
				setEndFreq(getEndFreq() - 1U);
				setEndFracFreq(getEndFracFreq() + decHalfFracFreq);
			}

			isDecSpan = TRUE;
		}
		/* Unhandled span delta, do not update span index */
		else
		{
		}

		if (isDecSpan)
		{
			setSpanIndex(getSpanIndex() + 1U);

			remapFracFreqs(getSpanIndex() + 1U);

			/* Adjust rxBw, steps, and numsteps based on updated span */
			updateRadioRF();
		}
	}
}

/** @brief Increase span length (zoom out) of RF sweep by one span index.
 *
 *  @par Usage
 *       @code
 *       increaseSpan();
 *       @endcode
 */
static void increaseSpan(void)
{
	uint16_t incSpanDelta, incHalfFracFreq, newStartFreq, newEndFreq,
		newStartFrac, newEndFrac;
	_Bool isIncSpan = FALSE;

	if (getSpanIndex() > MINEZSPANINDEX)
	{
		incSpanDelta = nextSpanDelta(getSpanIndex() - 1U);

		/* Update even Hz span deltas */
		if ((incSpanDelta > 1U) && (!((incSpanDelta % 2U) == 1U)))
		{
			newStartFreq = getStartFreq() - (incSpanDelta / 2U);
			newEndFreq = getEndFreq() + (incSpanDelta / 2U);
			newStartFrac = getStartFracFreq();
			newEndFrac = getEndFracFreq();

			isIncSpan = TRUE;
		}
		else if (incSpanDelta == 1U)
		{
			incHalfFracFreq = (getNumSteps() / 2U) * getFreqStep();

			if (fracFreqStepIndex(getStartFracFreq()) >= (getNumSteps() / 2U))
			{
				newStartFreq = getStartFreq();
				newStartFrac = getStartFracFreq() - incHalfFracFreq;
			}
			else
			{
				newStartFreq = getStartFreq() - 1U;
				newStartFrac = getStartFracFreq() + incHalfFracFreq;
			}

			if(fracFreqStepIndex(getEndFracFreq()) < (getNumSteps() / 2U))
			{
				newEndFreq = getEndFreq();
				newEndFrac = getEndFracFreq() + incHalfFracFreq;
			}
			else
			{
				newEndFreq = getEndFreq() + 1U;
				newEndFrac = getEndFracFreq() - incHalfFracFreq;
			}

			isIncSpan = TRUE;
		}
		/* Unhandled span delta, do not update span index */
		else
		{
		}

		if (isIncSpan)
		{
			/* Boundary check span increase */
			if ((newStartFreq >= getMinFreq())
				&&	((newEndFreq < getMaxFreq()) ||
					((newEndFreq == getMaxFreq()) && (newEndFrac == 0U))))
			{
				setStartFreq(newStartFreq);
				setEndFreq(newEndFreq);
				setStartFracFreq(newStartFrac);
				setEndFracFreq(newEndFrac);

				setSpanIndex(getSpanIndex() - 1U);

				remapFracFreqs(getSpanIndex() - 1U);

				/* Adjust rxBw, steps, and numsteps based on updated span */
				updateRadioRF();
			}
		}
	}
}

/** @brief Setter function for current start and end frequencies from host
 *  commands.
 *
 *  @par Usage
 *       @code
 *       updateSpanFreqs();
 *       @endcode
 */
static void updateSpanFreqs(void)
{
	/* Make sure that they correspond to the current band.				*/
	if ((sa1350CmdParams.saStartFreq >= getMinFreq())
			&& (sa1350CmdParams.saStartFreq <= getMaxFreq())
			&& (sa1350CmdParams.saEndFreq >= getMinFreq())
			&& (sa1350CmdParams.saEndFreq <= getMaxFreq()))
	{
		setStartFreq(sa1350CmdParams.saStartFreq);
		setStartFracFreq(sa1350CmdParams.saStartFracFreq);
		setEndFreq(sa1350CmdParams.saEndFreq);
		setEndFracFreq(sa1350CmdParams.saEndFracFreq);
	}
}

/** @brief Setter function for ExpertRF span settings from host commands.
 *
 *  @par Usage
 *       @code
 *       updateExpertSpan();
 *       @endcode
 */
static void updateExpertSpan(void)
{
	sa1350SpanParams[EXPERTSPANINDEX].spanFreqStep = sa1350CmdParams.saFreqStep;
	sa1350SpanParams[EXPERTSPANINDEX].spanNumSteps = sa1350CmdParams.saNumSteps;
	sa1350SpanParams[EXPERTSPANINDEX].spanRBW = sa1350CmdParams.saRBW;
	sa1350SpanParams[EXPERTSPANINDEX].spanSpan = sa1350CmdParams.saSpan;
}

/** @brief Change the frequency span of the RF sweep.
 *
 *  @param index value corresponding to span index.
 *
 *  @par Usage
 *       @code
 *       changeSpan(cmdMessage.payload[0]);
 *       @endcode
 */
static void changeSpan(uint8_t index)
{
	/* Only update when host requests a band the target can support.        */
    /* This is a workaround for CC1310 to ignore host requests for 2.4GHz.	*/
	if (((ChipInfo_GetChipType() == CHIP_TYPE_CC1350)
			|| (sa1350CmdParams.saStartFreq < (uint16_t)MAXFREQ_900))
	/* And the span index is in range of the span array.                    */
	        &&((index <= MAXEZSPANINDEX)
	/* Or the span index is for expert RF mode.                             */
	        ||(index == EXPERTSPANINDEX)))
	{
        /* Change the span index based on host command.						*/
        setSpanIndex(index);

        /* Load start and stop frequencies collected from host commands.	*/
        updateSpanFreqs();

		/* Perform RF Mode specific adjustments.                            */
		if(index != EXPERTSPANINDEX) // EasyRF Mode
		{
		    /* Span index will determine remaining RF span parameters.		*/
		}
		else // ExpertRF Mode
		{
		    /* Update Expert RF span parameters.                            */
			updateExpertSpan();
		}

		/* Adjust rxBw, steps, and numsteps and reprogram PLL.              */
		updateRadioRF();
	}
}

/** @brief Get the current frequency band the RF sweep is within.
 *
 *  Presently supports 440MHz, 900MHz and 2.4GHz ISM bands.
 *
 *  @return Current band of the RF sweep.
 *
 *  @par Usage
 *       @code
 *       SABand currBand = getBand();
 *       @endcode
 *
 *  @warning Assuming start and end frequencies are synchronized
 *           since the end frequency is not checked.
 */
static SABand getBand(void)
{
	SABand band;

	if (getStartFreq() >= MINFREQ_2400)
	{
		band = BAND_2400M;
	}
	else if (getStartFreq() >= MINFREQ_900)
	{
		band = BAND_900M;
	}
	else if (getStartFreq() >= MINFREQ_400)
	{
		band = BAND_400M;
	}
	else
	{
		band = BAND_UNKNOWN;
	}

	return band;
}

/** @brief Set the RF sweep into the specified frequency band.
 *
 *  Presently supports 440MHz, 900MHz and 2.4GHz ISM bands.
 *
 *  @param band #SABand value corresponding to specific band to
 *  enter (400M, 900M, or 2.4G).
 *
 *  @par Usage
 *       @code
 *       setBand(BAND_400M);
 *       @endcode
 */
static void setBand(SABand band)
{
	_Bool isBandSet = TRUE;

	switch (band)
	{
		case BAND_400M:
			/* Change start/stop frequencies to 440MHz band */
			setStartFreq(STARTFREQ_400);
			setEndFreq(ENDFREQ_400);
			setMinFreq(MINFREQ_400);
			setMaxFreq(MAXFREQ_400);
			/* Update Frequency divider */
			RF_cmdPropRadioDivSetup.loDivider = IODIV_440MHZ;
			/* Update center frequency to match band */
			RF_cmdPropRadioDivSetup.centerFreq = getStartFreq() +
					(getSpan() / 2U);
			break;

		case BAND_900M:
			/* Change start/stop frequencies to 900MHz band */
			setStartFreq(STARTFREQ_900);
			setEndFreq(ENDFREQ_900);
			setMinFreq(MINFREQ_900);
			setMaxFreq(MAXFREQ_900);
			/* Update Frequency divider */
			RF_cmdPropRadioDivSetup.loDivider = IODIV_915MHZ;
			/* Update center frequency to match band */
			RF_cmdPropRadioDivSetup.centerFreq = getStartFreq() +
					(getSpan() / 2U);
			break;

		case BAND_2400M:
			/* Change start/stop frequencies to 2400MHz band */
			setStartFreq(STARTFREQ_2400);
			setEndFreq(ENDFREQ_2400);
			setMinFreq(MINFREQ_2400);
			setMaxFreq(MAXFREQ_2400);
			/* Update Frequency divider */
			RF_cmdPropRadioDivSetup.loDivider = IODIV_2440MHZ;
			/* Update center frequency to match band */
			RF_cmdPropRadioDivSetup.centerFreq = getStartFreq() +
					(getSpan() / 2U);
			break;

		default: /* Entering unknown band */
			isBandSet = FALSE;
			break;
	}

	if (isBandSet)
	{
		/* Reset to SPAN0 */
		setSpanIndex(MINEZSPANINDEX);
		/* Reset fractional frequency values to 0 */
		setStartFracFreq(0U);
		setEndFracFreq(0U);
	}
}

/** @brief Change the RF sweep to the next frequency band.
 *
 *  Presently supports 440MHz, 900MHz and 2.4GHz ISM bands.
 *
 *  @par Usage
 *       @code
 *       nextBand(currBand);
 *       @endcode
 */
static void nextBand(SABand currBand)
{
    ChipType_t nextChipType = ChipInfo_GetChipType();

    if (currBand == BAND_400M)
    {
        setBand(BAND_900M);
    }
    else if (currBand == BAND_900M)
    {
        if (nextChipType == CHIP_TYPE_CC1350)
        {
            setBand(BAND_2400M);
        }
        else
        {
            setBand(BAND_400M);
        }
    }
    else
    {
        setBand(BAND_400M);
    }
}

/** @brief Change the RF sweep to the specified frequency band or the next
 *  band in the sequnce of bands.
 *
 *  Presently supports 440MHz, 900MHz and 2.4GHz ISM bands.
 *
 *  @param newBand #ChangeBandArg value corresponding to specific requested
 *  band (400M, 900M, or 2.4G) or request to switch to next band in sequence.
 *
 *  @par Usage
 *       @code
 *       changeBand(Set_400M_Band);
 *       @endcode
 */
static void changeBand(ChangeBandArg newBand)
{
    ChipType_t changeChipType = ChipInfo_GetChipType();
    SABand band = getBand();
    _Bool isBandChange = TRUE;

    switch (newBand)
    {
    	case SET_400M_BAND:
    		if (band != BAND_400M)
    		{
    			setBand(BAND_400M);
    		}
    		break;

    	case SET_900M_BAND:
    		if (band != BAND_900M)
    		{
    			setBand(BAND_900M);
    		}
    		break;

    	case SET_2400M_BAND:
    		if ((band != BAND_2400M)
    			&& (changeChipType == CHIP_TYPE_CC1350))
    		{
    			setBand(BAND_2400M);
    		}
    		break;

    	case SET_NEXT_BAND:
    		nextBand(band);
    		break;

    	default: /* Switching to unknown band */
    		isBandChange = FALSE;
    		break;
    }

	if (isBandChange == TRUE)
	{
	    /* Call updateRadioRF to adjust rxBw, steps, and numsteps and reprogram PLL */
	    updateRadioRF();
	}
}

/** @brief Status of the RF sweep control mode between incremental (button) and
 *  host command modes.
 *
 *  @par Usage
 *       @code
 *       if(!getCommandMode()){//incremental mode code};
 *       @endcode
 */
static _Bool getCommandMode(void)
{
	return isCommandMode;
}


/** @brief Change the RF sweep control mode between incremental (button) and
 *  host command modes.
 *
 *  @param commandMode new command mode.
 *  @par Usage
 *       @code
 *       setCommandMode(TRUE);
 *       @endcode
 */
static void setCommandMode(_Bool commandMode)
{
	if (commandMode == FALSE)
	{
		if (getSpanIndex() == EXPERTSPANINDEX)
		{
			/* Reset to default Easy RF mode */
			setBand(getBand());

		    /* Call updateRadioRF to adjust rxBw, steps, and numsteps and reprogram PLL */
		    updateRadioRF();
		}
	}

	isCommandMode = commandMode;
}

/** @brief Set the start frequency of RF sweep to a specific value.
 *
 *  @param values pointer to command payload.
 *
 *  @par Usage
 *       @code
 *       cmdSetStartFreq(&values);
 *       @endcode
 */
static void cmdSetStartFreq(const uint8_t *values)
{
	/* Update integer frequency values*/
	sa1350CmdParams.saStartFreq = (uint16_t)((values[0U] << 8U) | (values[1U]));

	/* Update fractional frequency values*/
	sa1350CmdParams.saStartFracFreq = (uint16_t)((values[2U] << 8U)
										| (values[3U]));

	/* Don't remap Fractional freqs now since it really needs to happen after
	 * CMD_SETFSPANINDEX is issued.
	 */
}

/** @brief Set the end frequency of RF sweep to a specific value.
 *
 *  @param values pointer to command payload.
 *
 *  @par Usage
 *       @code
 *       cmdSetEndFreq(&values);
 *       @endcode
 */
static void cmdSetEndFreq(const uint8_t *values)
{
	/* Update integer frequency values*/
	sa1350CmdParams.saEndFreq = (uint16_t)((values[0U] << 8U) | (values[1U]));

	/* Update fractional frequency value */
	sa1350CmdParams.saEndFracFreq = (uint16_t)((values[2U] << 8U)
										| (values[3U]));

	/* Don't remap Fractional freqs now since it really needs to happen
	 * after CMD_SETFSPANINDEX is issued.
	 */
}

/** @brief Set the RBW of RF sweep to a specific value.
 *
 *  @param values pointer to command payload.
 *
 *  @par Usage
 *       @code
 *       cmdSetRbw(&values, TRUE);
 *       @endcode
 */
static void cmdSetRbw(const uint8_t *values)
{
	/* Update RBW */
	sa1350CmdParams.saRBW = values[0U];
}

/** @brief Set the frequency step width of RF sweep to a specific value.
 *
 *  @param values pointer to command payload.
 *
 *  @par Usage
 *       @code
 *       cmdSetFreqStep(&values);
 *       @endcode
 */
static void cmdSetFreqStep(const uint8_t *values)
{
	/* Update integer frequency values*/
	sa1350CmdParams.saFreqStep = (uint16_t)((values[2U] << 8U) | (values[3U]));
}

/** @brief Set the frequency step count of RF sweep to a specific value.
 *
 *  @param values pointer to command payload.
 *
 *  @par Usage
 *       @code
 *       cmdSetStepCount(&values);
 *       @endcode
 */
static void cmdSetStepCount(const uint8_t *values)
{
	/* Update integer frequency values*/
	sa1350CmdParams.saNumSteps = (uint16_t)((values[0U] << 8U) | (values[1U]));
}

/** @brief Set the span of RF sweep to a specific value.
 *
 *  @param values pointer to command payload.
 *
 *  @par Usage
 *       @code
 *       cmdSetSpan(&values);
 *       @endcode
 */
static void cmdSetSpan(const uint8_t *values)
{
	/* Update integer frequency values*/
	sa1350CmdParams.saSpan = (uint16_t)((values[0U] << 8U) | (values[1U]));
}

/** @brief Update RF parameters based on user input.
 *
 *  This function is used to update RF parameters to increase/decrease span,
 *  adjust center frequency, or change band.
 *
 *  @return Status of GUI submitting new RF parameters
 *
 *  @par Usage
 *       @code
 *       newParams = rfCommand();
 *       @endcode
 */
static _Bool rfCommand(void)
{
	_Bool isCommandToExecute = FALSE;

	static CommandMessage cmdMessage = { NO_USER_COMMAND, {0U, 0U, 0U, 0U} };

	if (Mailbox_pend(rfMailbox, &cmdMessage, BIOS_NO_WAIT))
	{
		isCommandToExecute = TRUE;

		switch (cmdMessage.command)
		{
			case NO_USER_COMMAND:
				isCommandToExecute = FALSE;
				break;
			case DECREMENT_FREQ:	/* Shift center freq down */
				decreaseFreq();
				break;
			case INCREMENT_FREQ:	/* Shift center freq up */
				increaseFreq();
				break;
			case FAST_DEC_FREQ:	    /* Shift center freq down fast*/
				fastDecreaseFreq();
				break;
			case FAST_INC_FREQ:	    /* Shift center freq up fast*/
				fastIncreaseFreq();
				break;
			case DECREMENT_SPAN:	/* Zoom in */
				decreaseSpan();
				break;
			case INCREMENT_SPAN:	/* Zoom out */
				increaseSpan();
				break;
			case CHANGE_SPAN:		/* Change to a specific frequency span */
				changeSpan(cmdMessage.payload[0U]);
				break;
			case CHANGE_BAND:		/* Change frequency band */
				changeBand((ChangeBandArg)cmdMessage.payload[0U]);
				break;
			case NEXT_BAND:			/* Next frequency band */
				changeBand(SET_NEXT_BAND);
				break;
			case CHANGE_MODE:		/* Change input adjust mode */
				setCommandMode(cmdMessage.payload[0U]);
				break;
			case SET_START_FREQ:	/* Set start frequency */
				cmdSetStartFreq(cmdMessage.payload);
				isCommandToExecute = FALSE;
				break;
			case SET_END_FREQ:		/* Set end frequency */
				cmdSetEndFreq(cmdMessage.payload);
				isCommandToExecute = FALSE;
				break;
			case SET_RBW:			/* Set RBW */
				cmdSetRbw(cmdMessage.payload);
				isCommandToExecute = FALSE;
				break;
			case SET_FREQSTEP:		/* Set freqeuncy step width */
				cmdSetFreqStep(cmdMessage.payload);
				isCommandToExecute = FALSE;
				break;
			case SET_STEPCOUNT:		/* Set frequency step count */
				cmdSetStepCount(cmdMessage.payload);
				isCommandToExecute = FALSE;
			case SET_SPAN:		/* Set frequency span */
				cmdSetSpan(cmdMessage.payload);
				isCommandToExecute = FALSE;
				break;
			case SEND_SPECTRUM:		/* Sending new spectrum to host */
				isCommandToExecute = FALSE;
				break;

			default:
				isCommandToExecute = FALSE;
				break;
		}

	}

	if (isCommandToExecute)
	{
		setDisplayUpdate();
	}

	return isCommandToExecute;
}

/** @brief Check for RF commands, provide updates, and reset RF sweep.
 *
 *  @param sweepIndex current RSSI index of RF sweep
 *
 *  @par Usage
 *       @code
 *       updateSweepState();
 *       @endcode
 */
static void updateSweepState(uint16_t *sweepIndex)
{
	if (*sweepIndex == getSweepLength() || *sweepIndex == MAX_SWEEP_LENGTH)
	{ /* If we reached the end of the sweep */
		setNewSweep();

		rfCommand();

		*sweepIndex = 0U;
		RF_cmdFs.frequency = getStartFreq();
		RF_cmdFs.fractFreq = getStartFracFreq();
	}
	else if (getCommandMode())
	{
		if (rfCommand())
		{
			*sweepIndex = 0U;
			RF_cmdFs.frequency = getStartFreq();
			RF_cmdFs.fractFreq = getStartFracFreq();
		}
	}
}

/** @brief RF interrupt Callback function for radio calls.
 *
 *  @param hRf radio handle for interaction with RF driver.
 *  @param hRfC command handle from RF_postCmd().
 *  @param e active event mask.
 *
 *  @par Usage - Not called by user code. Called by RTOS internal scheduler.
 */
static void rfCallbackFxn(RF_Handle hRf, RF_CmdHandle hRfC, RF_EventMask e)
{
	/* Handle RF error */
}

/** @brief Initialize parameters and open handle to radio.
 *
 *  @par Usage
 *       @code
 *       openRadio();
 *       @endcode
 */
static void openRadio(void)
{
    if (ChipInfo_GetChipType() == CHIP_TYPE_CC1350) {
        /* Open RF_SW pin */
        rfSwPinHandle = PIN_open(&rfSwPinState, rfSwPinTable);
        if (!rfSwPinHandle)
        {
            System_abort("Error initializing board RF switch pin\n");
        }

        /* Enable power to RF switch to 2.4G antenna */
        PIN_setOutputValue(rfSwPinHandle, Board_DIO30_SWPWR, 1U);
    }

    /* Indicate no active RF handle, configure radio to default operation */
    rfHandle = NULL;
    if (DEFAULT_BAND_900M)
    {
    	/* Place sweep out of band to force a change of bands */
    	setStartFreq(STARTFREQ_400);
    	changeBand(SET_900M_BAND);
    }
    else
    {
    	/* Place sweep out of band to force a change of bands */
    	setStartFreq(STARTFREQ_900);
    	changeBand(SET_400M_BAND);
    }

	RF_cmdFs.frequency = getStartFreq();
	RF_cmdFs.fractFreq = 0U;
}

/***** Global function definitions *****/

/** @brief Initialize and construct the RF mailbox.
 *
 *  @par Usage
 *       @code
 *       RfMailbox_init();
 *       @endcode
 */
static void RfMailbox_init(void)
{
    Mailbox_Params_init(&rfMailboxParams);
    Mailbox_construct(&rfMailboxStruct, sizeof(CommandMessage), 1,
    		&rfMailboxParams, (Error_Block *)NULL);
    rfMailbox = Mailbox_handle(&rfMailboxStruct);

    if (rfMailbox == NULL) {
        System_abort("Mailbox create failed\n");
    }
}

/** @brief Initialize and construct the sweep and command gatemutexes.
 *
 *  @par Usage
 *       @code
 *       RfGateMutex_init();
 *       @endcode
 */
static void RfGateMutex_init(void)
{
	GateMutexPri_Params_init(&sweepMutexParams);
	GateMutexPri_construct(&sweepMutexStruct, &sweepMutexParams);
	sweepMutex = GateMutexPri_handle(&sweepMutexStruct);

	if (sweepMutex == NULL) {
		System_abort("GateMutexPri create failed\n");
	}

    GateMutex_Params_init(&commandMutexParams);
    GateMutex_construct(&commandMutexStruct, &commandMutexParams);
    commandMutex = GateMutex_handle(&commandMutexStruct);

    if (commandMutex == NULL) {
        System_abort("GateMutex create failed\n");
    }
}

/** @brief Initialize and construct the new spectrum semaphore.
 *
 *  @par Usage
 *       @code
 *       DisplaySemaphore_init();
 *       @endcode
 */
static void RfSemaphore_init(void)
{
    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&newSpectrumSemaphoreParams);
    newSpectrumSemaphoreParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&newSpectrumSemaphoreStruct, 1, &newSpectrumSemaphoreParams);
    newSpectrumSemaphore = Semaphore_handle(&newSpectrumSemaphoreStruct);

    if (newSpectrumSemaphore == NULL) {
        System_abort("Semaphore create failed\n");
    }
}

/** @brief Initialize and construct the RF task.
 *
 *  @par Usage
 *       @code
 *       RfTask_init();
 *       @endcode
 */
void RfTask_init(void)
{
	RfMailbox_init();
	RfGateMutex_init();
	RfSemaphore_init();

    Task_Params_init(&rfTaskParams);
    rfTaskParams.stackSize = (size_t)RF_TASK_STACK_SIZE;
    rfTaskParams.priority = RF_TASK_PRIORITY;
    rfTaskParams.stack = &rfTaskStack;

    Task_construct(&rfTaskStruct, &rfTaskFxn, &rfTaskParams,
    		(Error_Block *)NULL);
}

/** @brief Configures the radio and collects a sweep of RSSI data values over a
 *         range of frequencies.
 *
 *  @param rfArg0 Optional user argument
 *  @param rfArg1 Optional user argument
 *  @par Usage - Not called by user code. Called by RTOS internal scheduler.
 */
static void rfTaskFxn(UArg rfArg0, UArg rfArg1)
{
	IArg mutexKey;
	uint16_t rssiIndex = 0U;
	int8_t *sweepArray = getSweepData();
	uint8_t timeOut;
	int8_t rssiValue = (int8_t)RF_GET_RSSI_ERROR_VAL;

	openRadio();

	while(1)
	{
		updateSweepState(&rssiIndex);

        /* Store RSSI value. */
        /* Lock out UART task from accessing RSSI array */
    	mutexKey = lockSweepData();

        /* Send CMD_FS and wait until it has completed */
        RF_runCmd(rfHandle, (RF_Op *)&RF_cmdFs, RF_PriorityNormal,
                &rfCallbackFxn, (RF_EventMask)0);

        /* Send asynchronous CMD_RX_TEST which enables RX for reading RSSI */
        RF_CmdHandle rxTestCmd = RF_postCmd(rfHandle, (RF_Op *)&RF_cmdRxTest,
                RF_PriorityNormal, (RF_Callback)0, (RF_EventMask)0);

        /* Make sure that the RX_TEST command is running */
        /* Needed to avoid infinite loop here if task gets preempted during
         * command status transition
         */
        /* TODO 11/26 Figure out how to pend on a radio event instead of doing this timeout. */
        timeOut = 0U;
        while ((RF_cmdRxTest.status != ACTIVE) && (timeOut < 50U))
        {
            timeOut++;
        }

        /* Even though the command is running, we have to wait for the RF core
         * to be in receive mode. RF core returns RSSI of 0 when not yet in
         * receive mode
         */
        timeOut = 0U;
        while (((rssiValue == (int8_t)RF_GET_RSSI_ERROR_VAL)
                || (rssiValue == 0)) && (timeOut < 50U))
        {
            rssiValue = RF_getRssi(rfHandle);
            timeOut++;
        }

        sweepArray[rssiIndex] = RF_getRssi(rfHandle);

        /* Adjustment needed for 2.4GHz band. Value determined empirically. */
        if (RF_cmdFs.frequency >= MINFREQ_2400)
        {
            sweepArray[rssiIndex]+= 20;
        }

        rssiIndex++;

        /* Permit Uart task to access RSSI array */
        unlockSweepData(mutexKey);

        updateSweepFreq();
    }
}

/** @brief Setter function for new RF Task command to process.
 *
 *  @param sweepCmd command for RF Task to execute.
 *
 *  @par Usage
 *       @code
 *       setSweepCmd(&buttonsMessage);
 *       @endcode
 */
inline void setSweepCmd(CommandMessage *sweepCmd)
{
	Mailbox_post(rfMailbox, sweepCmd, BIOS_NO_WAIT);
}

/** @brief Setter function for new RF Task command to process, with wait for
 *  command read confirmation.
 *
 *  @param pendSweepCmd command to wait for RF Task to execute.
 *
 *  @par Usage
 *       @code
 *       setPendSweepCmd(&hostMessage);
 *       @endcode
 */
inline void setPendSweepCmd(CommandMessage *pendSweepCmd)
{
	Mailbox_post(rfMailbox, pendSweepCmd, BIOS_WAIT_FOREVER);
}

/** @brief Getter function for current start frequency.
 *
 *  @return Current start frequency
 *
 *  @par Usage
 *       @code
 *       getStartFreq();
 *       @endcode
 */
inline uint16_t getStartFreq(void)
{
	return sa1350Params.saStartFreq;
}

/** @brief Getter function for current fractional start frequency.
 *
 *  @return Current fractional start frequency
 *
 *  @par Usage
 *       @code
 *       getStartFracFreq();
 *       @endcode
 */
inline uint16_t getStartFracFreq(void)
{
	return sa1350Params.saStartFracFreq;
}

/** @brief Getter function for current end frequency.
 *
 *  @return Current end frequency
 *
 *  @par Usage
 *       @code
 *       getEndFreq();
 *       @endcode
 */
inline uint16_t getEndFreq(void)
{
	return sa1350Params.saEndFreq;
}

/** @brief Getter function for current fractional end frequency.
 *
 *  @return Current fractional end frequency
 *
 *  @par Usage
 *       @code
 *       getEndFracFreq();
 *       @endcode
 */
inline uint16_t getEndFracFreq(void)
{
	return sa1350Params.saEndFracFreq;
}

/** @brief Getter function for current frequency step width (dec.).
 *
 *  @return Current frequency step width (dec.)
 *
 *  @par Usage
 *       @code
 *       getFreqStep();
 *       @endcode
 */
inline uint16_t getFreqStep(void)
{
	return sa1350Params.saFreqStep;
}

/** @brief Getter function for current number of frequency steps per MHz.
 *
 *  @return Current frequency steps per Mhz
 *
 *  @par Usage
 *       @code
 *       getNumSteps();
 *       @endcode
 */
inline uint16_t getNumSteps(void)
{
	return sa1350Params.saNumSteps;
}

/** @brief Getter function for length of device's RBW table.
 *
 *  @return Length of the device's RBW table
 *
 *  @par Usage
 *       @code
 *       uint8_t rbwCount = getRbwTableLength();
 *       @endcode
 */
inline uint8_t getRbwTableLength(void)
{
	return (uint8_t)(sizeof(CC13xxSubGigTableRBW) / sizeof(SARBW));
}

/** @brief Getter function for length (in bytes) of RBW table entry.
 *
 *  @return Length (in bytes) of RBW table entry
 *
 *  @par Usage
 *       @code
 *       uint8_t rbwLength = getRbwTableEntryLength();
 *       @endcode
 */
inline uint8_t getRbwTableEntryLength(void)
{
	return sizeof(rbwTableEntryData);
}

/** @brief Convert a double value to a byte array.
 *
 *  @param dBytes pointer to byte array to fill with double value
 *  @param d double value to convert to byte array
 *
 *  @par Usage
 *       @code
 *       doubleToByteArray(&rbwTableEntryData[0], entry.rxBwValue);
 *       @endcode
 */
static void doubleToByteArray(uint8_t *dBytes, double d) {
    uint8_t dByteIndex;
    uint64_t asLong = *((uint64_t*)&d);

    for (dByteIndex = sizeof(double); dByteIndex > 0U; dByteIndex--) {
        dBytes[sizeof(double) - dByteIndex] =
        		(asLong >> (8U * (dByteIndex - 1U))) & 0xFFU;
    }
}

/** @brief Convert a uint16_t value to a byte array.
 *
 *  @param bytes pointer to byte array to fill with uint16_t value
 *  @param u16 uint16_t value to convert to byte array
 *
 *  @par Usage
 *       @code
 *       u16ToByteArray(&rbwTableEntryData[sizeof(entry.rxBwValue)],
 *           entry.rxBwIfValue);
 *       @endcode
 */
static void u16ToByteArray(uint8_t* bytes, uint16_t u16) {
    bytes[0] = (u16 >> 8U) & 0xFFU;
    bytes[1] = u16 & 0xFFU;
}

/** @brief Getter function for RBW table entry in byte array format.
 *
 *  @param rbwIndex Index of RBW table entry to be returned
 *  @param rbwBand Flag indicating which band of the RBW request
 *
 *  @return Byte array of RBW table entry
 *
 *  @par Usage
 *       @code
 *       rbwData = getRbwTableEntryData(rbwIndex, 0U);
 *       @endcode
 */
uint8_t * getRbwTableEntryData(uint8_t rbwIndex, uint8_t rbwBand)
{
	SARBW entry;
	if(rbwBand == 0U)
	{
	    entry = CC13xxSubGigTableRBW[rbwIndex];
	}
	else
	{
        entry = CC13xx2_4GTableRBW[rbwIndex];
	}

	doubleToByteArray(&rbwTableEntryData[0U], entry.rbwRBW);

	u16ToByteArray(&rbwTableEntryData[sizeof(entry.rbwRBW)],
			entry.rbwIF);

	rbwTableEntryData[sizeof(entry.rbwRBW) + sizeof(entry.rbwIF)] =
			entry.rbwSetting;

	return rbwTableEntryData;
}

/** @brief Getter function for maximum sweep length in steps.
 *
 *  @return Maximum sweep steps
 *
 *  @par Usage
 *       @code
 *       uint16_t sweepCount = getSweepMaxLength();
 *       @endcode
 */
inline uint16_t getSweepMaxLength(void)
{
	return sizeof(rssiArray);
}

/** @brief Getter function for current sweep length in steps.
 *
 *  @return Current number of sweep steps
 *
 *  @par Usage
 *       @code
 *       stepSize = getSweepLength()/PLOT_COL_COUNT;
 *       @endcode
 */
inline uint16_t getSweepLength(void)
{
	uint16_t length;

	/* Use number of steps per MHz times number of MHz for EasyRF */
	if (!getCommandMode())
	{
		length = getSpan() * getNumSteps();
	}
	/* Use number of MHz if step size is 1 MHz */
	else if (getNumSteps() == 1U)
	{
		length = getSpan() + 1U;
	}
	/* Use number of MHz and FSW, rounding to nearest for ExpertRF */
	else
	{
		length = (uint16_t)((double)((uint32_t)getSpan() * (uint32_t)(UINT16_MAX + 1U)) /
			(double)getFreqStep()) + 1;
	}

	return length;
}

/** @brief Getter function for current sweep data in RSSI values.
 *
 *  @return Pointer to RSSI array
 *
 *  @par Usage
 *       @code
 *       int8_t *rssiValues = getSweepData();
 *       @endcode
 */
inline int8_t * getSweepData(void)
{
    return rssiArray;
}

/** @brief Getter function for new sweep data request.
 *
 *  @par Usage
 *       @code
 *       getNewSweep();
 *       @endcode
 */
inline void getNewSweep(void)
{
	Semaphore_pend(newSpectrumSemaphore, BIOS_WAIT_FOREVER);
}

/** @brief Lock access to the RSSI array.
 *
 *  @return Key to unlock access to the RSSI array
 *
 *  @par Usage
 *       @code
 *       mutexKey = lockSweepData();
 *       @endcode
 */
inline IArg lockSweepData(void)
{
    return GateMutexPri_enter(sweepMutex);
}

/** @brief Unlock access to the RSSI array.
 *
 *  @param dataKey Key to unlock access to the RSSI array
 *
 *  @par Usage
 *       @code
 *       unlockSweepData(mutexKey);
 *       @endcode
 */
inline void unlockSweepData(IArg dataKey)
{
    GateMutexPri_leave(sweepMutex, dataKey);
}

/** @brief Lock access to submit sweep commands.
 *
 *  @return Key to unlock access to submit sweep commands
 *
 *  @par Usage
 *       @code
 *       mutexKey = lockSweepCmd();
 *       @endcode
 */
inline IArg lockSweepCmd(void)
{
    return GateMutex_enter(commandMutex);
}

/** @brief Lock access to submit sweep commands.
 *
 *  @param cmdKey Key to unlock access to submit sweep commands
 *
 *  @par Usage
 *       @code
 *       mutexKey = lockSweepCmd();
 *       @endcode
 */
inline void unlockSweepCmd(IArg cmdKey)
{
    GateMutex_leave(commandMutex, cmdKey);
}


