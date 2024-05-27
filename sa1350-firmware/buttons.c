/*!
 *  @file buttons.c
 */
#include "SA1350_Firmware.h"


/***** Local Defines *****/

/* Button Events */
#define BUTTON_EVT_NONE    	(Event_Id_NONE)	/*!< No button event			*/
#define BUTTON_EVT_CB_RIGHT	(Event_Id_00)   /*!< Right button event			*/
#define BUTTON_EVT_CB_LEFT	(Event_Id_01)   /*!< Left button event			*/
#define BUTTON_EVT_CB_BOTH	(Event_Id_02)   /*!< Both buttons event			*/
#define BUTTON_EVT_TIMER	(Event_Id_03)   /*!< Button timer event			*/

/* Button press times */
/** @brief Debounce timeout (50ms) in ticks, convert ms to ticks via tickPeriod.
 */
#define DEBOUNCE_TIME		(50U * (1000U / Clock_tickPeriod))
#define BUTTON_HELD			(3U)	/*!< Hold at 3 debounce (150ms)			*/
#define BUTTON_HELD_LONG	(50U)	/*!< Long hold at 50 debounce (2.5s)	*/
#define BUTTON_BOTH_HELD	(40U)	/*!< Both hold at 40 debounce (2s)		*/

/* Pin/Port values for reading current button state */
#define PIN_BUTTON_LEFT		(Board_PIN_BUTTON0)		/*!< Left button PIN	*/
#define PIN_BUTTON_RIGHT	(Board_PIN_BUTTON1)		/*!< Right button PIN	*/
#define PORT_BUTTON_LEFT	(1U << PIN_BUTTON_LEFT)	/*!< Left button PORT	*/
#define PORT_BUTTON_RIGHT	(1U << PIN_BUTTON_RIGHT)/*!< Right button PORT	*/
/** @brief Port representation of both buttons
 */
#define PORT_BUTTON_BOTH	(PORT_BUTTON_LEFT | PORT_BUTTON_RIGHT)

/***** Structures *****/

/** @brief A type and enum for possible button states for user control.
 */
typedef enum ButtonState
{
    BUTTON_STATE_IDLE,	/*!< No button pressed		*/
	BUTTON_STATE_RIGHT,	/*!< Right button pressed	*/
    BUTTON_STATE_LEFT,	/*!< Left button pressed	*/
	BUTTON_STATE_BOTH	/*!< Both buttons pressed	*/
} ButtonState;

/** @brief A type and enum for possible button sweep adjustment modes.
 */
typedef enum AdjustMode
{
	CENTER_FREQ = 0,		/*!< Center frequency adjustment mode			*/
	SPAN,					/*!< Span adjustment mode						*/
	LOCK,					/*!< Lock adjustment mode						*/
	MODE_COUNT				/*!< Number of adjustment modes					*/
} AdjustMode;

/***** Variable declarations *****/

/** @brief Task struct for button Task.
 */
Task_Struct buttonTaskStruct;

/** @brief Task parameters for button Task.
 */
static Task_Params buttonTaskParams;

/** @brief Task stack definition for button Task.
 */
static Char buttonTaskStack[BUTTON_TASK_STACK_SIZE];

/** @brief Used to block SNP calls during a synchronous transaction.
 */
static Event_Handle buttonEvent;

/** @brief Clock parameters for debouncing the buttons.
 */
static Clock_Params debounceClockParams;

/** @brief Clock object for debouncing the buttons.
 */
static Clock_Struct debounceClockStruct;

/** @brief Clock driver handle for debouncing the buttons.
 */
static Clock_Handle debounceClock;

/** @brief PIN driver handle for the application buttons.
 */
static PIN_Handle buttonPinHandle;

/** @brief PIN driver state for the application buttons.
 */
static PIN_State buttonPinState;

/** @brief PIN driver pin list and pin attributes for the application buttons.
 */
PIN_Config buttonPinTable[] = {
    Board_PIN_BUTTON0 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	Board_PIN_BUTTON1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

/** @brief PORT mask for the application buttons.
 */
static uint32_t buttonPortMask;

/** @brief Variable used to store the current button state.
 */
static ButtonState buttonState = BUTTON_STATE_IDLE;

/** @brief Variable used to store the current button mode.
 */
static AdjustMode buttonMode = CENTER_FREQ;

/** @brief Variable used to store the button mode before button lock.
 */
static AdjustMode unlockMode;

/** @brief Variable used to store current command message from button presses.
 */
static CommandMessage buttonsMessage = { NO_USER_COMMAND, {0U, 0U, 0U, 0U} };

/** @brief Array of display strings for adjustment mode.
 */
static const char *modeString[MODE_COUNT] = {
		"<>", /* Center frequency adjust */
		"||", /* Span adjust */
		"--"  /* Lock adjust */
};

/***** Function prototypes *****/

static ButtonState getButtonState(void);
static void setButtonState(ButtonState nextState, uint16_t *stateDbnc);
static void singleButtonCmd(PIN_Id pinBtnCmd, _Bool isSpeedUp);
static void singleButtonDebounce(PIN_Id pinBtnDbnc, uint16_t btnDbnc);
static void dualButtonDebounce(ButtonState *btnState, uint16_t btnsDbnc);
static void buttonCallbackFxn(PIN_Handle pinHandle, PIN_Id pinId);
static void buttonDebounceSwiFxn(void);
static void openButtons(void);
static void buttonTaskFxn(UArg btnArg0, UArg btnArg1);

/***** Function definitions *****/

/** @brief Getter function for current button state.
 *
 *  @return #ButtonState current button state
 *
 *  @par Usage
 *       @code
 *       nextButtonState = getButtonState();
 *       @endcode
 */
static ButtonState getButtonState(void)
{
	ButtonState currButtonState;
	uint32_t btnPort = ~PIN_getPortInputValue(buttonPinHandle) &
			buttonPortMask;

	switch (btnPort)
	{
		case PORT_BUTTON_LEFT:
			currButtonState = BUTTON_STATE_LEFT;
			break;

		case PORT_BUTTON_RIGHT:
			currButtonState = BUTTON_STATE_RIGHT;
			break;

		case PORT_BUTTON_BOTH:
			currButtonState = BUTTON_STATE_BOTH;
			break;

		default:
			currButtonState = BUTTON_STATE_IDLE;
			break;
	}

	return currButtonState;
}

/** @brief Enter a new button state, reset debounce time and start clock.
 *
 *  @param nextState #ButtonState next button state to enter.
 *  @param stateDbnc debounce time counter to reset.
 *
 *  @par Usage
 *       @code
 *       setButtonState(&buttonState, BUTTON_STATE_RIGHT, &debounceCount);
 *       @endcode
 */
static void setButtonState(ButtonState nextState, uint16_t *stateDbnc)
{
	buttonState = nextState;
	*stateDbnc = 0U;
	Clock_start(debounceClock);
}

/** @brief Execute the appropriate command for a single button press event.
 *
 *  @param pinBtnCmd used to identify pin during PIN driver API calls.
 *  @param isSpeedUp flag used to speedup single button commands.
 *
 *  @par Usage
 *       @code
 *       singleButtonCmd(pinBtnDbnc, TRUE);
 *       @endcode
 */
static void singleButtonCmd(PIN_Id pinBtnCmd, _Bool isSpeedUp)
{
    /* Adjust RF parameters based upon current button mode */
	switch (buttonMode)
	{
		case CENTER_FREQ:
			switch (pinBtnCmd)
			{
				case PIN_BUTTON_LEFT:
					if (isSpeedUp)
					{
					    buttonsMessage.command = FAST_DEC_FREQ;
					}
					else
					{
					    buttonsMessage.command = DECREMENT_FREQ;
					}
					break;
				case PIN_BUTTON_RIGHT:
					if (isSpeedUp)
					{
					    buttonsMessage.command = FAST_INC_FREQ;
					}
					else
					{
					    buttonsMessage.command = INCREMENT_FREQ;
					}
					break;
				default:
					/* Unknown button - do nothing */
					break;
			}
			setSweepCmd(&buttonsMessage);
			break;

		case SPAN:
			switch (pinBtnCmd)
			{
				case PIN_BUTTON_LEFT:
				        buttonsMessage.command = INCREMENT_SPAN;
					break;
				case PIN_BUTTON_RIGHT:
				        buttonsMessage.command = DECREMENT_SPAN;
					break;
				default:
					/* Unknown button - do nothing */
					break;
			}
			setSweepCmd(&buttonsMessage);
			break;

		default:
			/* Do nothing */
			break;
	}
}

/** @brief Process the debounce time for a single button press event.
 *
 *  @param pinBtnDbnc used to identify pin during PIN driver API calls.
 *  @param btnDbnc number of button debounce times.
 *
 *  @par Usage
 *       @code
 *       singleButtonDebounce(debounceCount, PIN_BUTTON_RIGHT);
 *       @endcode
 */
static void singleButtonDebounce(PIN_Id pinBtnDbnc, uint16_t btnDbnc)
{
	/* 1st time (ticks=0) or button held (ticks>=BUTTON_HELD) send command */
	if ((btnDbnc == 0U) ||
			((btnDbnc >= BUTTON_HELD) && (btnDbnc < BUTTON_HELD_LONG)))
	{
		singleButtonCmd(pinBtnDbnc, FALSE);
	}
	/* Else if button held for extended period then go fast */
	else if (btnDbnc >= BUTTON_HELD_LONG)
	{
		/* Don't Reset time ticks, but issue speedup command */
		singleButtonCmd(pinBtnDbnc, TRUE);
	}
	else
	{
		/* Debounce command entry by BUTTON_HELD */
	}
}

/** @brief Process the debounce time for a double button press event.
 *
 *  @param btnState #ButtonState pointer to identify current button state.
 *  @param btnsDbnc pointer to number of button debounce times.
 *
 *  @par Usage
 *       @code
 *       dualButtonDebounce(debounceCount, PIN_BUTTON_RIGHT);
 *       @endcode
 */
static void dualButtonDebounce(ButtonState *btnState, uint16_t btnsDbnc)
{
	_Bool isDualButtonCommand = FALSE;

	/* If buttons held for > 2 seconds */
    if (btnsDbnc >= BUTTON_BOTH_HELD)
    {
    	/* Then switch bands, reset button state to idle */
        buttonsMessage.command = NEXT_BAND;

        *btnState = BUTTON_STATE_IDLE;

        isDualButtonCommand = TRUE;
    }
    /* Else if buttons released before 2 seconds */
    else if (*btnState == BUTTON_STATE_IDLE)
    {
    	/* Then switch display modes */
		switch(buttonMode) {
			case CENTER_FREQ:
				buttonMode = SPAN;
				break;

			case SPAN:
				buttonMode = CENTER_FREQ;
				break;

			default:
				/* Do nothing */
				break;
		}

		setDisplayUpdate();
    }
    else
    {
    	/* Both buttons still pressed */
    }

    if(isDualButtonCommand)
    {
    	setSweepCmd(&buttonsMessage);
    }
}

/** @brief Pin interrupt callback function configured in buttonPinTable.
 *
 *  @param pinHandle handle for interaction with PIN driver.
 *  @param pinId used to identify a pin during PIN driver API calls.
 *
 *  @par Usage - Not called by user code. Called by RTOS internal scheduler.
 */
static void buttonCallbackFxn(PIN_Handle pinHandle, PIN_Id pinId)
{
    uint32_t cbPortVal = ~PIN_getPortInputValue(buttonPinHandle) &
    		buttonPortMask;

    switch (cbPortVal)
    {
    	case PORT_BUTTON_LEFT:
			/* Notify button state machine of right button press */
			Event_post(buttonEvent, BUTTON_EVT_CB_LEFT);
			break;
    	case PORT_BUTTON_RIGHT:
			/* Notify button state machine of left button press */
			Event_post(buttonEvent, BUTTON_EVT_CB_RIGHT);
			break;
    	case PORT_BUTTON_BOTH:
			/* Notify button state machine of both button press */
			Event_post(buttonEvent, BUTTON_EVT_CB_BOTH);
			break;
    	default:
			/* Unknown button - do nothing */
    		break;
    }
}

/** @brief Callback from Clock module on timeout, determines new state
 *          after debouncing
 *
 *  @par Usage
 *       @code
 *       Clock_construct(&buttonDebounceClock,
 *       		(Clock_FuncPtr)buttonDebounceSwiFxn, DEBOUNCE_TIME,
 *       		&buttonClockParams);
 *       @endcode
 */
static void buttonDebounceSwiFxn(void)
{
    /* Notify button state machine of timer interval */
    Event_post(buttonEvent, BUTTON_EVT_TIMER);
}

/** @brief Open button handle and initialize button callback function pointer.
 *         Create button RTOS Event and debounce Clock.
 *
 *  @par Usage
 *       @code
 *       openButtons();
 *       @endcode
 */
static void openButtons(void)
{
    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if (!buttonPinHandle) {
        System_abort("Error initializing button pins\n");
    }

    buttonPortMask = PIN_getPortMask(buttonPinHandle);

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &buttonCallbackFxn) != 0U) {
        System_abort("Error registering button callback function\n");
    }

    /* Create RTOS Event */
    buttonEvent = Event_create((const Event_Params *)NULL, (Error_Block *)NULL);

    /* Create the debounce clock object for buttons */
    /* Initialize to 50 ms timeout when Clock_start is called. */
    Clock_Params_init(&debounceClockParams);
    Clock_construct(&debounceClockStruct, (Clock_FuncPtr)&buttonDebounceSwiFxn,
    		DEBOUNCE_TIME, &debounceClockParams);
    debounceClock = Clock_handle(&debounceClockStruct);
}

/***** Global function definitions *****/

/** @brief Initialize and construct the button task.
 *
 *  @par Usage
 *       @code
 *       ButtonTask_init();
 *       @endcode
 */
void ButtonTask_init(void)
{
    Task_Params_init(&buttonTaskParams);
    buttonTaskParams.stackSize = (size_t)BUTTON_TASK_STACK_SIZE;
    buttonTaskParams.priority = BUTTON_TASK_PRIORITY;
    buttonTaskParams.stack = &buttonTaskStack;

    Task_construct(&buttonTaskStruct, &buttonTaskFxn, &buttonTaskParams,
    		(Error_Block *)NULL);
}

/** @brief State machine task for implementing button modes.
 *
 *  Gate mutex used to disable buttons when host is connected to UART. Enter
 *	mutex and issue commands until we get preempted by uartTaskFxn. Not
 *	necessary to save mutex key since task never leaves, only pre-empted.
 *
 *  @param btnArg0 Optional user argument
 *  @param btnArg1 Optional user argument
 *
 *  @par Usage - Not called by user code. Called by RTOS internal scheduler.
 *
 *  @pre The uartTask MUST have higher priority than this task.
 */
static void buttonTaskFxn(UArg btnArg0, UArg btnArg1)
{
    UInt events;
    IArg buttonCmdKey;
    ButtonState nextButtonState;
    uint16_t debounceCount = 0U;

    openButtons();

	while(1)
	{
		events = Event_pend(buttonEvent, BUTTON_EVT_NONE, BUTTON_EVT_CB_BOTH +
				BUTTON_EVT_CB_RIGHT + BUTTON_EVT_CB_LEFT + BUTTON_EVT_TIMER,
				BIOS_WAIT_FOREVER);

		/* Enter command mutex */
	    buttonCmdKey = lockSweepCmd();

	    buttonsMessage.command = NO_USER_COMMAND;  /* Clear button command */
	    switch (events)
	    {
	    	case BUTTON_EVT_NONE:
	    		break; /* Event never posted */

	    	case BUTTON_EVT_CB_RIGHT:
	    		setButtonState(BUTTON_STATE_RIGHT, &debounceCount);
	    		break;

	    	case BUTTON_EVT_CB_LEFT:
	    		setButtonState(BUTTON_STATE_LEFT, &debounceCount);
	    		break;

	    	case BUTTON_EVT_CB_BOTH:
	    		setButtonState(BUTTON_STATE_BOTH, &debounceCount);
	    		break;

	    	case BUTTON_EVT_TIMER:
	    		/* Debounced event, update state, restart debounce clock */
	    		debounceCount++;
	    		Clock_start(debounceClock);

	    		/* Implement basic Mealy machine for debounce */
	    		nextButtonState = getButtonState();

	    		switch (buttonState)
	    		{
	    			case BUTTON_STATE_IDLE:
	    				/* Reset debounce time, stop debounce clock */
	            		debounceCount = 0U;
	            		Clock_stop(debounceClock);
	    				break;

	    			case BUTTON_STATE_RIGHT:
	    				singleButtonDebounce(PIN_BUTTON_RIGHT, debounceCount);
	    				break;

	    			case BUTTON_STATE_LEFT:
	    				singleButtonDebounce(PIN_BUTTON_LEFT, debounceCount);
	    				break;

	    			case BUTTON_STATE_BOTH:
	    				/* Check for new idle state and update as needed */
	    				dualButtonDebounce(&nextButtonState, debounceCount);
	    				break;

	    			default:
	    				break;
	    		}

	    		buttonState = nextButtonState;
           	    break;

	    	default:
	    		break;
	    }

        unlockSweepCmd(buttonCmdKey);
	}
}

/** @brief Set current button mode to locked state and store previous mode.
 *
 *  @par Usage
 *       @code
 *       lockButton();
 *       @endcode
 */
void lockButton(void)
{
	if (buttonMode != LOCK)
	{
		unlockMode = buttonMode;
		buttonMode = LOCK;
	}

	setDisplayUpdate();
}

/** @brief Set current button mode to mode from before locked state.
 *
 *  @par Usage
 *       @code
 *       unlockButton();
 *       @endcode
 */
void unlockButton(void)
{
	if (buttonMode == LOCK)
	{
		buttonMode = unlockMode;
	}

	setDisplayUpdate();
}

/** @brief Getter function for current button adjustment mode string.
 *
 *  @return Current button adjustment mode string
 *
 *  @par Usage
 *       @code
 *       const char *keyModeStr = getButtonModeString();
 *       @endcode
 */
inline const char* getButtonModeString(void)
{
	return modeString[buttonMode];
}
