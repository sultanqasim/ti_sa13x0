/*!
 *  @file  display.c
 */
#include "SA1350_Firmware.h"


/***** Local Defines *****/

/* Size definitions for plotting area */
#define DISP_MODE_LEN	(2U)	/*!< Number of characters for display mode	*/
#define PLOT_H_ORIGIN	(0U)	/*!< Number of columns to indent plot area	*/
#define PLOT_V_ORIGIN	(10U)	/*!< Number of rows in title area			*/
#define PLOT_ROW_COUNT	(73U)	/*!< Number of rows in RSSI plot area		*/
#define PLOT_COL_COUNT	(96U)	/*!< Number of columns in RSSI plot area	*/
#define HASH_OVERLAP	(3U)	/*!< Size of centerline overlap w/title/key	*/

/* Size definitions for grid */
#define GRID_V_GAP		(10U)	/*!< Vertical gap between grid lines		*/
#define GRID_H_GAP		(8U)	/*!< Horizontal gap to grid line starts		*/
#define GRID_BORDER		(2U)	/*!< Gap around border of grid lines		*/
#define GRID_LNE_LENGTH	(4U)	/*!< Length of each grid line				*/

#define DISP_MIN_RSSI	(-128)	/*!< Minimum RSSI value that can be displayed */
#define DISP_MAX_RSSI	(-56)	/*!< Maximum RSSI value that can be displayed */

/***** Structures *****/

/** @brief Rectangle definition used to clear spectrum sweep after every round.
 */
static const Graphics_Rectangle clearSpace = { PLOT_H_ORIGIN, PLOT_V_ORIGIN,
		PLOT_COL_COUNT, PLOT_ROW_COUNT + PLOT_V_ORIGIN };

/***** Variable declarations *****/

/** @brief Task struct for display Task.
 */
Task_Struct displayTaskStruct;

/** @brief Task parameters for display Task.
 */
static Task_Params displayTaskParams;

/** @brief Task stack definition for display Task.
 */
static Char displayTaskStack[DISPLAY_TASK_STACK_SIZE];

/** @brief Display parameters for the application.
 */
static Display_Params displayParams;

/** @brief Display driver handle for primary LCD.
 *
 * This handle is used for the primary LCD, e.g. SHARP96
 */
static Display_Handle displayLcd;

/** @brief Display driver handle for serial LCD.
 *
 * This handle is optionally used for supporting a serial LCD.
 */
static Display_Handle displaySerial;

/** @brief Pointer to GraphicsLib context for the primary LCD.
 */
static Graphics_Context *pDisplayContext;

/** @brief Semaphore struct for the display update semaphore.
 */
static Semaphore_Struct displayUpdateSemaphoreStruct;

/** @brief Semaphore parameters for the display update semaphore.
 */
static Semaphore_Params displayUpdateSemaphoreParams;

/** @brief Semaphore handle for the display update semaphore.
 */
Semaphore_Handle displayUpdateSemaphore;

/***** Function prototypes *****/

static void displayLCDWelcome(Display_Handle hDispLcd,
		const Graphics_Context *pWelcomeContext);
static void displayLCDNoGrLib(Display_Handle hDispNoGr);
static void displaySerialWelcome(Display_Handle hDispSerial);
static void drawTitle(const Graphics_Context *pTitleContext,
		Display_Handle hTitleLcd);
static void drawScale(Display_Handle hScaleLcd);
static void drawSweep(const Graphics_Context *pSweepContext,
		uint16_t binRssis);
static void drawGrid(const Graphics_Context *pGridContext);
static _Bool getDisplayUpdate(void);
static void openDisplay(void);
static void DisplaySemaphore_init(void);
static void displayTaskFxn(UArg dispArg0, UArg dispArg1);

/***** Function definitions *****/

/** @brief Display welcome message on primary display.
 *
 *  @param hDispLcd display driver handle for primary display.
 *  @param pWelcomeContext Grlib drawing context.
 *
 *  @par Usage
 *       @code
 *       displayLCDWelcome(hDisplayLcd, **pContext);
 *       @endcode
 */
static void displayLCDWelcome(Display_Handle hDispLcd,
		const Graphics_Context *pWelcomeContext)
{
    if (ChipInfo_GetChipType() == CHIP_TYPE_CC1350)
    {
        Display_print0(hDispLcd, 5U, 2U, "SA w/ CC1350");
    }
    else
    {
        Display_print0(hDispLcd, 5U, 2U, "SA w/ CC1310");
    }

    Display_print2(hDispLcd, 6U, 6U, "V%d.%d",
    		SA1350FW_MAJOR_VERSION, SA1350FW_MINOR_VERSION);

    /* Wait a while so text can be viewed. */
	Task_sleep(2000U * (1000U / Clock_tickPeriod));

#ifdef NOT_DEBUG
	/* Draw splash */
	Graphics_drawImage(pWelcomeContext, &splashImage, 0, 0);
	Graphics_flushBuffer(pWelcomeContext);

	/* Wait for a bit, then clear */
	Task_sleep(3000U * (1000U / Clock_tickPeriod));

	Display_clear(hDispLcd);
#endif
}

/** @brief Display welcome message on primary display.
 *
 *  @param hDispNoGr display driver handle for primary display.
 *
 *  @par Usage
 *       @code
 *       displayLCDWelcome(hDisplayLcd, **pContext);
 *       @endcode
 */
static void displayLCDNoGrLib(Display_Handle hDispNoGr)
{
	Display_print0(hDispNoGr, 0U, 0U, "Display driver");
	Display_print0(hDispNoGr, 1U, 0U, "is not");
	Display_print0(hDispNoGr, 2U, 0U, "GrLib capable!");
}

/** @brief Display welcome message on serial display.
 *
 *  @param hDispSerial display driver handle for serial display.
 *
 *  @par Usage
 *       @code
 *       displaySerialWelcome(hDisplaySerial);
 *       @endcode
 */
static void displaySerialWelcome(Display_Handle hDispSerial)
{
    Display_print0(hDispSerial, 0U, 0U, "This display");
    Display_print0(hDispSerial, 1U, 0U, "is not");
    Display_print0(hDispSerial, 2U, 0U, "supported!");
}

/** @brief Draw title bar and centerline on display.
 *
 *  @param pTitleContext Grlib drawing context.
 *  @param hTitleLcd driver handle for primary display.
 *
 *  @par Usage
 *       @code
 *       drawTitle(pDisplayContext, hDisplayLcd);
 *       @endcode
 */
static void drawTitle(const Graphics_Context *pTitleContext,
		Display_Handle hTitleLcd)
{

    /* Draw plot center and scale separation line */
	Graphics_drawLineH(pTitleContext,
			PLOT_H_ORIGIN,
			PLOT_COL_COUNT,
			PLOT_V_ORIGIN + PLOT_ROW_COUNT + 1U
	);
	Graphics_drawLineV(pTitleContext,
			PLOT_COL_COUNT / 2U,
			PLOT_V_ORIGIN - HASH_OVERLAP,
			PLOT_V_ORIGIN + PLOT_ROW_COUNT + 1U + HASH_OVERLAP
	);

	/* Draw the title */
    if (ChipInfo_GetChipType() == CHIP_TYPE_CC1350)
    {
        Display_print0(hTitleLcd, 0U, 0U, "SA w/ CC1350");
    }
    else
    {
        Display_print0(hTitleLcd, 0U, 0U, "SA w/ CC1310");
    }

    /* Draw the key */
    setDisplayUpdate();
}

/** @brief Draw scale with start and stop frequencies and centerline.
 *
 *  @param hScaleLcd driver handle for primary display.
 *
 *  @par Usage
 *       @code
 *       drawScale(hDisplayLcd);
 *       @endcode
 */
static void drawScale(Display_Handle hScaleLcd)
{
	uint16_t startFreq, startFrac, endFreq, endFrac;
	const char *keyModeStr = getButtonModeString();

	startFreq = getStartFreq();
	endFreq = getEndFreq();
	/* By not calculating span-specific divisor, and utilizing UINT16_MAX
	 * instead, negligible error introduced due to each span having slightly
	 * different fractional steps per MHz */
	startFrac = (getStartFracFreq() * 100U) / (UINT16_MAX + 1U);
	endFrac = (getEndFracFreq() * 100U) / (UINT16_MAX + 1U);

	/* Draw scale */
	if (startFreq > 999U)
	{
		// Special format for 2.4G
	    Display_print5(hScaleLcd, 11U, 0U, "%d.%02d%.2s%4d.%02d",
	    		startFreq, startFrac, keyModeStr,
				endFreq, endFrac);
	}
	else
	{
		Display_print5(hScaleLcd, 11U, 0U, "%d.%02d %.2s%4d.%02d",
				startFreq, startFrac, keyModeStr,
				endFreq, endFrac);
	}
}

/** @brief Draw vertical lines corresponding to the RSSI values from one sweep.
 *
 *  @param pSweepContext Grlib drawing context.
 *  @param binRssis number of RSSI values per dispaly bin.
 *
 *  @par Usage
 *       @code
 *       drawSweep(pDisplayContext, stepSize);
 *       @endcode
 */
static void drawSweep(const Graphics_Context *pSweepContext, uint16_t binRssis)
{
    uint16_t dispBin, freqBin, freqStartIndex;
    int8_t scale;
    int8_t *rssiValues = getSweepData();
    uint16_t rssiLength = getSweepLength();
    int16_t scaleAvg;

	/* Draw the RSSI readings but restricting it to only 96 pixel screen */
	for (dispBin = 0U; dispBin < PLOT_COL_COUNT; dispBin++)
	{
		/* Sum each RSSI value across a display bin */
		scaleAvg = 0;
		freqStartIndex = ((dispBin * (rssiLength - 1U)) / PLOT_COL_COUNT);
		for (freqBin = 0U; freqBin < binRssis; freqBin++)
		{
			scaleAvg += rssiValues[freqStartIndex + freqBin];
		}
		/* Average RSSI value for each display bin */
		scaleAvg /= binRssis;

		/* Set the scale and restrict it to only 72 pixels */
		if (scaleAvg < DISP_MIN_RSSI)
		{
			scale = DISP_MIN_RSSI;
		}
		else if (scaleAvg > DISP_MAX_RSSI)
		{
			scale = DISP_MAX_RSSI;
		}
		else
		{
			scale = (int8_t)scaleAvg;
		}

		if (scale != 0)
		{
			/* Adjust scale to be relative minimum value for plot */
			scale = scale - DISP_MIN_RSSI;
			/* Draw each line only if RSSI is NOT 0 pixels (not set) */
			Graphics_drawLineV(pSweepContext,
					dispBin,
					PLOT_ROW_COUNT + PLOT_V_ORIGIN - scale,
					PLOT_ROW_COUNT + PLOT_V_ORIGIN
			);
		}
	}
}

/** @brief Draw horizontal gridlines on display at 10dBm increments.
 *
 *  @param pGridContext Grlib drawing context.
 *
 *  @par Usage
 *       @code
 *       drawGrid(pDisplayContext);
 *       @endcode
 */
static void drawGrid(const Graphics_Context *pGridContext)
{
	uint8_t rowIndex, colIndex;

	for (
			rowIndex = PLOT_V_ORIGIN + GRID_BORDER;
			rowIndex <= PLOT_V_ORIGIN + PLOT_ROW_COUNT - GRID_BORDER;
			rowIndex += GRID_V_GAP
	)
	{
		for (
				colIndex = GRID_BORDER;
				colIndex <= PLOT_COL_COUNT - GRID_BORDER - GRID_LNE_LENGTH;
				colIndex += GRID_H_GAP
		)
		{
			Graphics_drawLineH(pGridContext,
					colIndex,
					colIndex + GRID_LNE_LENGTH,
					rowIndex
			);
		}
	}
}

/** @brief Getter function for complete display update request.
 *
 *  @return Current display update request status
 *
 *  @par Usage
 *       @code
 *       if (getDisplayUpdate()){//Update display}
 *       @endcode
 */
static _Bool getDisplayUpdate(void)
{
	return Semaphore_pend(displayUpdateSemaphore, BIOS_NO_WAIT);
}

/** @brief Initialize and open display handle.
 *
 *  Whether the open call for a given display type is successful depends on what
 *  is present in the Display_config[] array of the board file.
 *
 *  @par Usage
 *       @code
 *       openDisplay();
 *       @endcode
 */
static void openDisplay(void)
{
    /* Initialize display and try to open both UART and LCD types of display. */
    Display_Params_init(&displayParams);
    displayParams.lineClearMode = DISPLAY_CLEAR_BOTH;

    /* Open both an available LCD display and a UART display. */
    displayLcd = Display_open(Display_Type_LCD, &displayParams);
    displaySerial = Display_open(Display_Type_UART, &displayParams);

    /* Check if the selected LCD Display type was found and opened */
    if (displayLcd)
    {
    	/*
    	 * Use the GrLib extension to get the GraphicsLib context object of the
    	 * LCD, if it is supported by the display type.
    	 */
    	pDisplayContext = DisplayExt_getGraphicsContext(displayLcd);

    	if (pDisplayContext)
    	{
    		displayLCDWelcome(displayLcd, pDisplayContext);

    	    drawTitle(pDisplayContext, displayLcd);
    	}
    	/* It's possible that no compatible display is available. */
    	else
    	{
    		displayLCDNoGrLib(displayLcd);
    		/* No compatible display available, it is safe to exit task */
    		Task_exit();
    	}
    }
    /* Check if the selected Serial Display type was found and opened */
    else if (displaySerial)
    {
    	displaySerialWelcome(displaySerial);
        /* No compatible display available, it is safe to exit task */
        Task_exit();
    }
    else
   {
        /* No compatible display available, it is safe to exit task */
        Task_exit();
    }
}

/***** Global function definitions *****/

/** @brief Initialize and construct the key update semaphore.
 *
 *  @par Usage
 *       @code
 *       DisplaySemaphore_init();
 *       @endcode
 */
static void DisplaySemaphore_init(void)
{
    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&displayUpdateSemaphoreParams);
    displayUpdateSemaphoreParams.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&displayUpdateSemaphoreStruct, 1, &displayUpdateSemaphoreParams);
    displayUpdateSemaphore = Semaphore_handle(&displayUpdateSemaphoreStruct);

    if (displayUpdateSemaphore == NULL) {
        System_abort("Semaphore create failed\n");
    }
}

/** @brief Initialize and construct the display task.
 *
 *  @par Usage
 *       @code
 *       DisplayTask_init();
 *       @endcode
 */
void DisplayTask_init(void)
{
    DisplaySemaphore_init();

    Task_Params_init(&displayTaskParams);
    displayTaskParams.stackSize = (size_t)DISPLAY_TASK_STACK_SIZE;
    displayTaskParams.priority = DISPLAY_TASK_PRIORITY;
    displayTaskParams.stack = &displayTaskStack;

    Task_construct(&displayTaskStruct, &displayTaskFxn, &displayTaskParams,
    		(Error_Block *)NULL);
}

/** @brief Display the latest RF sweep data on an LCD,
 *         using pushbuttons to control RF parameters of sweep.
 *
 *  @param dispArg0 Optional user argument
 *  @param dispArg1 Optional user argument
 *
 *  @par Usage - Not called by user code. Called by RTOS internal scheduler.
 *
 *  @pre The rfTask MUST have equal priority to this task.
 */
static void displayTaskFxn(UArg dispArg0, UArg dispArg1)
{
	uint16_t stepSize = UINT16_MAX, newStepSize;

    openDisplay();

    while (1) {
    	/* Redraw/recalculate scale if SA parameters have changed */
    	if (getDisplayUpdate())
    	{
    		/* Calculate offset to plot only PLOT_COL_COUNT values to LCD */
    		newStepSize = ceilf((float)getSweepLength() / PLOT_COL_COUNT);
    		if (newStepSize > stepSize)
    		{
    			getNewSweep();
    		}
    		stepSize = newStepSize;

    		drawScale(displayLcd);
    	}

    	/* Clear the lines by drawing a box */
    	Graphics_setForegroundColor(pDisplayContext, GRAPHICS_COLOR_WHITE);
    	Graphics_fillRectangle(pDisplayContext, &clearSpace);
    	Graphics_setForegroundColor(pDisplayContext, GRAPHICS_COLOR_BLACK);

    	drawSweep(pDisplayContext, stepSize);

    	drawGrid(pDisplayContext);

    	/* Flush this content to display */
    	Graphics_flushBuffer(pDisplayContext);
    }
}

/** @brief Setter function for complete display update request.
 * *
 *  @par Usage
 *       @code
 *       setDisplayUpdate();
 *       @endcode
 */
void setDisplayUpdate(void)
{
	Semaphore_post(displayUpdateSemaphore);
}
