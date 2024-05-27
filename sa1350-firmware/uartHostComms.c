/*!
 *  @file uartHostComms.c
 */
#include "SA1350_Firmware.h"

/** @struct HostCommand
 *
 * SA1350 Host Command
 *
 * Format cheat sheet
 * -------------------------------------------------------------------------
 * - (byte 0)       : 0x2A Valid command prefix
 * - (byte 1)       : 0xLL Payload length in bytes
 * - (byte 2)       : 0xCC Command number
 * - (byte 3 - N+2) : Payload
 * - (byte N + 3)   : CRC High byte
 * - (byte N + 4)   : CRC Low byte
 *
 * Commands cheat sheet
 * -------------------------------------------------------------------------
 * - General Commands
 *  + #CMD_CONNECT       =  1, Request connection with the SA1350. No
 *                             additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x0, 0x1, 0x95, 0x09]
 *  + #CMD_DISCONNECT    =  2, Disconnect from the SA1350. No additional
 *                             response required after ACK.
 *                             Bytes from host: [0x2A, 0x00, 0x02, 0xA5, 0x6A]
 *  + #CMD_GETDEVICEVER  =  3, Request device version. Four byte response
 *                             expected after ACK. A valid response
 *                             includes the following ASCII characters:
 *                             '1', '3','5','0' for CC1350 device
 *                             '1', '3','1','0' for CC1310 device
 *                             Bytes from host: [0x2A, 0x00, 0x03, 0xB5, 0x4B]
 *  + #CMD_GETFWVER      =  4, Request firmware version. Two byte response
 *                             expected after ACK. A valid response
 *                             includes ASCII characters for the major
 *                             and minor revision numbers. For example:
 *                             '1', '0' for firmware v1.0
 *                             Bytes from host: [0x2A, 0x00, 0x04, 0xC5, 0xAC]
 *  + #CMD_GETRFPARAMS   =  5, Request device RF parameters. ### byte
 *                             response expected after ACK. A valid response
 *                             includes the RBW table for the device and maximum
 *                             RSSI array size supported by the device RF sweep.
 *                             Bytes from host: [0x2A, 0x01, 0x05, 0x00, 0x21, 0xE8]
 *  + #CMD_GETLASTERROR  =  6, This command always follows command 31,
 *                             CMD_GETSPECNOINIT and is used by the host
 *                             to confirm that all of the requested spectrum
 *                             values have been received. The SA1350 responds
 *                             with an ACK followed by a two byte message
 *                             indicating any error status. A response of
 *                             0, 0 indicates success "no errors".
 *                             Bytes from host: [0x2A, 0x02, 0x06, 0x00, 0x00, 0x1E, 0xCF]
 *  + #CMD_SYNC          =  7, This command is unused, but included with an ACK.
 * 							   Bytes from host: [0x2A, 0x00, 0x07, 0x##, 0x##]
 * - Frequency Commands
 *  + #CMD_SETFBAND      = 20, Sets frequency band of the scan. The one byte
 *                             payload defines the band as follows:
 *                             0  -> Sets band to 440MHz
 *                             1  -> Sets band to 900MHz
 *                             2  -> Sets band to 2400MHz
 *                             85 -> (0x55) Sets to the next band.
 *                             All other values -> ignored.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x01, 0x14 , 0x01, 0x01, 0x8B]
 *  + #CMD_SETFSTART     = 21, Sets the start frequency of the scan. The four
 *                             byte payload is separated into an unsigned 16-
 *                             bit integer representation of the decimal
 *                             start frequency in Hertz and the 16-bit integer
 *                             representation of the start fractional frequency
 *                             dividend in Hertz with a divisor of 65536.
 *                             The bytes are arranged in big endian order. For
 *                             example the values 0x03, 0x93, 0x85, 0x67 are
 *                             sent for a frequency of 915.520MHz in decimal.
 *                             Bytes from host: [0x2A, 0x04, 0x15, 0x03, 0x93, 0x85, 0x67, 0x2D, 0x2F]
 *                             No additional response required after ACK.
 *  + #CMD_SETFSTOP      = 22, Sets the stop frequency of the scan. The format
 *                             of value is the same as that of command 21.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x4, 0x16, 0x03, 0xA6, 0x84, 0x87, 0x23, 0xB7]
 *  + #CMD_SETSPANINDEX  = 23, Sets the frequency span index for the scan. The
 *                             one byte payload is an unsigned integer value
 *                             that indexes an array of preset frequency span
 *                             values.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x01, 0x17, 0x08, 0xC5, 0xF1]
 *  + #CMD_SETFSTEP      = 24, Sets the frequency step width (FSW) for the
 *                             scan. The four byte payload is an unsigned 32-
 *                             bit integer representation of the FSW dividend
 *                             in kiloHertz with a divisor of 65536. The
 *                             bytes are arranged in big endian order. For
 *                             example the values 0x00, 0x00, 0x12, 0x48 are
 *                             sent for a FSW of 71.411 kiloHertz in decimal.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x04, 0x18, 0x00, 0x00, 0x12, 0x48, 0xE2, 0x11]
 *  + #CMD_SETRBW        = 25, Sets the RX filter bandwidth (RBW) for the
 *                             scan. The one byte payload is an unsigned
 *                             integer value that is the RBW register value
 *                             for the device. For example 0x26 is sent for
 *                             an RBW of 155.4 kiloHertz in decimal for Sub-1
 *                             GHz bands.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x01, 0x19, 0x26, 0x23, 0x52]
 *  + #CMD_SETSTEPCOUNT  = 26, Sets the step count per megaHertz for the scan.
 *                             The two byte payload is an unsigned 16-bit integer
 *                             representation of the number of steps per
 *                             megaHertz. The bytes are arranged in big endian
 *                             order. For examples the values 0x00, 0x0E are sent
 *                             for 14 steps per megaHertz.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x02, 0x1A, 0x00, 0x0E, 0xC9, 0x03]
 *  + #CMD_SETSPAN       = 27, Sets the frequency span for the scan. The two
 *                             byte payload is an unsigned 16-bit integer
 *                             representation of the frequency span in
 *                             megaHertz. For example the values 0x00, 0x13 are
 *                             sent for a span of 19 megaHertz.
 *                             No additional response required after ACK.
 *                             Bytes from host: [0x2A, 0x02, 0x1B, 0x00, 0x13, 0x3D, 0xAF]
 * - Spectrum Measurement Commands
 *  + #CMD_INITPARAMETER = 30, **Not implemented in this version.**
 *                             This command always precedes command 31 and it
 *                             triggers the actual update of scan parameter
 *                             values previously sent by commands 20 - 27.
 *  + #CMD_GETSPECNOINIT = 31, This command requests a new set of spectrum
 *                             measurements from SA1350. When operating in
 *                             continuous update mode, the host sends this
 *                             command once for each display refresh cycle.
 *                             A variable length response is expected after
 *                             ACK. The response includes all of the
 *                             measurements from a complete frequency scan,
 *                             from start frequency to stop frequency. Since
 *                             the payload size is one byte, if the size of
 *                             the scan is more than 255 bytes (2^8), then the
 *                             response must be divided into multiple
 *                             messages. For example, to respond with a 288
 *                             byte scan, the SA1350 would first send a
 *                             message with the first 255 values and then a
 *                             second message with the remaining 33 values.
 *                             Bytes from host: [0x2A, 0x00, 0x1F, 0x66, 0xF6]
 ***************************************************************************
 *
 *  @note Deciding against enum for command definitions due to need
 *  for uint8_t (char) type for commands.
 */

/***** Local Defines *****/

/**  @{ */
/*! See \ref HostCommand for complete command list and format. */
#define CMD_CONNECT         (1)
#define CMD_DISCONNECT      (2)
#define CMD_GETDEVICEVER    (3)
#define CMD_GETFWVER        (4)
#define CMD_GETRFPARAMS     (5)
#define CMD_GETLASTERROR    (6)
#define CMD_SYNC            (7)
#define CMD_SETFBAND        (20)
#define CMD_SETFSTART       (21)
#define CMD_SETFSTOP        (22)
#define CMD_SETSPANINDEX    (23)
#define CMD_SETFSTEP        (24)
#define CMD_SETRBW          (25)
#define CMD_SETSTEPCOUNT    (26)
#define CMD_SETSPAN         (27)
#define CMD_INITPARAMETER   (30)
#define CMD_GETSPECNOINIT   (31)

#define HDR_PREFIX          (0x2AU)
#define HDR_LENGTH          (3U)

#define HDR_PREFIX_INDEX    (0U)
#define HDR_CMDSIZE_INDEX   (1U)
#define HDR_CMD_INDEX       (2U)

#define CRC_LENGTH          (2U)
/**  @} */

/** @brief Command size of 10 assumes that host will never send a
 *  payload > 5 bytes. See \ref HostCommand for details.
 */
#define MAX_COMMAND_SIZE    (10U)
/** @brief Payload size does not include header, with the CRC acting as
 *  the final 2 payload bytes. See \ref HostCommand for details.
 */
#define MAX_PAYLOAD_SIZE    (MAX_COMMAND_SIZE - HDR_LENGTH)

/***** Structures *****/

/** @brief A type and struct for receiving and sending host command messages.
 */
typedef struct HostCommand {
	uint8_t prefix;		/*!< Command prefix/header, fixed			[0x2A]	*/
	uint8_t length;		/*!< Command payload length in bytes		[0xLL]	*/
	uint8_t command;	/*!< See \ref HostCommand cheat sheet		[0xCC]	*/
	uint8_t payload[MAX_PAYLOAD_SIZE]; /*!< Command payload of 0xLL	bytes	*/
} HostCommand;

/***** Variable declarations *****/

/** @brief  Task struct for UART Task.
 */
Task_Struct uartTaskStruct;

/** @brief  Task parameters for UART Task.
 */
static Task_Params uartTaskParams;

/** @brief  Task stack definition for UART Task.
 */
static Char uartTaskStack[UART_TASK_STACK_SIZE];

/** @brief  UART driver parameters for the application.
 */
UART_Params uartParams;

/** @brief  UART driver handle for the application.
 */
UART_Handle uart;

/** @brief  Local buffer for host mailbox message
 */
static CommandMessage hostMessage = { NO_USER_COMMAND, {0U, 0U, 0U, 0U } };

/** @brief  IArg key for the RF command gate mutex.
 */
IArg uartCmdKey;

/** @brief  PIN driver pin handle for green LED.
 */
static PIN_Handle gledPinHandle;

/** @brief  PIN driver pin state for green LED.
 */
static PIN_State gledPinState;

/** @brief  PIN driver pin list and pin attributes for green LED.
 *          GLED initially off.
 */
static PIN_Config gledPinTable[] = {
	Board_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/***** Function prototypes *****/

static void openUart(void);
static void crc16AddByte(uint16_t *addb, uint8_t u8);
static uint16_t calcCrc16(void *data, uint8_t dataLength);
static void sendHostResponse(uint8_t *tx, uint8_t txSize);
static void sendHostArrayResponse(HostCommand arrCmd, const uint8_t *txArr, size_t txSize);
static void sendHostAck(HostCommand cmdToAck);
static void connect(HostCommand connectCmd);
static void disconnect(HostCommand disconnectCmd);
static void getDeviceVersion(HostCommand getDeviceVersionCmd);
static void getFWVersion(HostCommand getFWVersionCmd);
static void getRFParameters(HostCommand getRFParametersCmd);
static void getLastError(HostCommand getLastErrorCmd);
static void sync(HostCommand syncCmd);
static void setFBand(HostCommand setFBandCmd);
static void setFStart(HostCommand setFStartCmd);
static void setFStop(HostCommand setFStopCmd);
static void setSpanIndex(HostCommand setSpanIndexCmd);
static void setFStep(HostCommand setFStepCmd);
static void setStepCount(HostCommand setStepCountCmd);
static void setSpan(HostCommand setSpanCmd);
static void setRbw(HostCommand setRbwCmd);
static void initParameter(HostCommand initParameterCmd);
static void sendSpectrum(void);
static void getSpecNoInit(HostCommand getSpecNoInitCmd);
static void processHostCommand(HostCommand hostCmd);
static void uartTaskFxn(UArg uartArg0, UArg uartArg1);

/***** Function definitions *****/

/** @brief Open and configure UART driver.
 *
 *  @par Usage
 *       @code
 *       openUart();
 *       @endcode
 */
static void openUart(void)
{
    /* Call UART init function */
    UART_init();

    /* Create a UART with data processing off */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200U;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.writeDataMode = UART_DATA_BINARY; //UART_DATA_TEXT
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART\n");
    }
}

/** @brief Add byte to current CRC16 value.
 *
 *  @param addb active CRC16 value.
 *  @param u8 data byte to add to active CRC16 value.
 *
 *  @par Usage
 *       @code
 *       crc16AddByte(&crc, bytes[crcIndex]);
 *       @endcode
 *
 *  @pre Initialize overall CRC16 routine before calling this function.
*/
static void crc16AddByte(uint16_t *addb, uint8_t u8)
{
	*addb  = (*addb >> 8U) | (*addb << 8U);
	*addb ^=  u8;
	*addb ^= (*addb & 0xFFU) >> 4U;
	*addb ^= (*addb << 8U) << 4U;
	*addb ^= ((*addb & 0xFFU) << 4U) << 1U;
}

/** @brief Calculate CRC16 of given raw data bytestream.
 *
 *  @param data raw data bytestream of which to calculate CRC16.
 *  @param dataLength length in bytes of bytestrem.
 *
 *  @return CRC16 value
 *
 *  @par Usage
 *       @code
 *       cmdCrc = calcCrc16(cmdPacket, cmdSize + HDR_LENGTH);
 *       @endcode
*/
static uint16_t calcCrc16(void *data, uint8_t dataLength)
{
	uint8_t crcIndex;
	uint16_t crc = HDR_PREFIX;
	uint8_t *bytes = data;

	for (crcIndex = 1U; crcIndex < dataLength; crcIndex++)
	{
		crc16AddByte(&crc, bytes[crcIndex]);
	}

	return crc;
}

/** @brief Send command response back to host.
 *
 *  @param tx command response to host.
 *  @param txSize length of command.
 *
 *  @par Usage
 *       @code
 *       sendHostResponse(hostAck, sizeof(hostAck));
 *       @endcode
 */
static void sendHostResponse(uint8_t *tx, uint8_t txSize)
{
    uint16_t txCrc;

    txCrc = calcCrc16(tx, txSize - CRC_LENGTH);

    tx[txSize - 2U] = (txCrc & 0xFF00U) >> 8U;
    tx[txSize - 1U] = txCrc & 0x00FFU;

    UART_write(uart, tx, txSize);
}

/** @brief Send command response back to host with a payload array.
 *
 *  @param arrCmd command response header to host.
 *  @param txArr command payload array.
 *  @param txSize length of command payload array.
 *
 *  @par Usage
 *       @code
 *       sendHostArrayResponse(getRFParametersCmd, rfParamCmd, sizeof(rfParamCmd));
 *       @endcode
 */
static void sendHostArrayResponse(HostCommand arrCmd,
		const uint8_t *txArr, size_t txSize)
{
	uint16_t arrCmdCrc, crcIndex, txArrIndex = 0U;

	while (txSize > 0U)
	{
		/* Determine size of frame to send to host */
		if (txSize <= 255U)
		{
			arrCmd.length = (uint8_t)txSize;
		}
		else
		{
			arrCmd.length = 255U;
		}

		/* send response preamble to host */
		UART_write(uart, &arrCmd, HDR_LENGTH);

		/* send payload */
		UART_write(uart, &txArr[txArrIndex], arrCmd.length);

		/* CRC calculation */
		arrCmdCrc = calcCrc16(&arrCmd, HDR_LENGTH);

		for (crcIndex = 0U; crcIndex < arrCmd.length; crcIndex++)
		{
			crc16AddByte(&arrCmdCrc, txArr[txArrIndex]);
			txArrIndex++;
			txSize--;
		}

		/* send response CRC to host */
		arrCmd.payload[0U] = (arrCmdCrc & 0xFF00U) >> 8U;
		arrCmd.payload[1U]= arrCmdCrc & 0x00FFU;
		UART_write(uart, &arrCmd.payload, CRC_LENGTH);
	}
}

/** @brief Send command received acknowledgment back to host.
 *  This is just a simplified call to sendHostResponse.
 *
 *  @param cmdToAck #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       sendHostAck(connectCmd);
 *       @endcode
 */
static void sendHostAck(HostCommand cmdToAck)
{
	/* Array to store command ACK */
    uint8_t hostAck[] = {HDR_PREFIX, 0x00U,
    		cmdToAck.command, 0U, 0U};

    sendHostResponse(hostAck, sizeof(hostAck));
}

/** @brief Send host message to RF Task and wait for the RF task to process
 *  the command.
 *
 *  @param sweepCmd #HostCommand to be sent to RF Task.
 *
 *  @par Usage
 *       @code
 *       sendSweepMessage(connectCmd);
 *       @endcode
 */
void sendSweepMessage(HostCommand sweepCmd)
{
	uint8_t pyldIndex;

	for (pyldIndex = 0U; pyldIndex < sizeof(hostMessage.payload); pyldIndex++)
	{
		hostMessage.payload[pyldIndex] = sweepCmd.payload[pyldIndex];
	}

	setPendSweepCmd(&hostMessage);
}

/** @brief Configure system to operate with UART connected.
 *
 *  @param connectCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       connect(hostCmd);
 *       @endcode
 */
static void connect(HostCommand connectCmd)
{
	uartCmdKey = lockSweepCmd();

	/* Change to command adjustment mode */
	hostMessage.command = CHANGE_MODE;

	/* Indicate command adjustment mode */
	connectCmd.payload[0U] = TRUE;

	sendSweepMessage(connectCmd);

    /* Turn on Board_PIN_GLED to indicate host is in command */
    PIN_setOutputValue(gledPinHandle, Board_PIN_GLED, 1U);
    lockButton();

    sendHostAck(connectCmd); /* ACK Command */
}

/** @brief Configure system to operate with UART disconnected.
 *
 *  @param disconnectCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       disconnect(hostCmd);
 *       @endcode
 */
static void disconnect(HostCommand disconnectCmd)
{
	unlockButton();
    /* Turn off Board_PIN_GLED to indicate host released the board */
    PIN_setOutputValue(gledPinHandle, Board_PIN_GLED, 0U);

	/* Change to button adjustment mode */
	hostMessage.command = CHANGE_MODE;

	/* Indicate incremental adjustment mode */
	disconnectCmd.payload[0U] = FALSE;

	sendSweepMessage(disconnectCmd);

    unlockSweepCmd(uartCmdKey);

    sendHostAck(disconnectCmd); /* ACK Command */
}

/** @brief Send device version to host.
 *
 *  @param getDeviceVersionCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       getDeviceVersion(hostCmd);
 *       @endcode
 */
static void getDeviceVersion(HostCommand getDeviceVersionCmd)
{
    sendHostAck(getDeviceVersionCmd); /* First ACK Command */

    char devvCmd[] = {HDR_PREFIX, 0x04U, CMD_GETDEVICEVER, '1', '3', '0', '0',
    		0U, 0U};

    if (ChipInfo_GetChipType() == CHIP_TYPE_CC1350)
    {
        devvCmd[3] = '1';
        devvCmd[4] = '3';
        devvCmd[5] = '5';
        devvCmd[6] = '0';
    }
    else
    {
        devvCmd[3] = '1';
        devvCmd[4] = '3';
        devvCmd[5] = '1';
        devvCmd[6] = '0';
    }

    sendHostResponse((uint8_t*)devvCmd, sizeof(devvCmd));
}

/** @brief Send firmware version to host.
 *
 *  @param getFWVersionCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       getFWVersion(hostCmd);
 *       @endcode
 */
static void getFWVersion(HostCommand getFWVersionCmd)
{
    sendHostAck(getFWVersionCmd); /* First ACK Command */

    uint8_t fwvCmd[] = {HDR_PREFIX, 0x02U, CMD_GETFWVER,
    		SA1350FW_MAJOR_VERSION, SA1350FW_MINOR_VERSION, 0U, 0U};

    sendHostResponse(fwvCmd, sizeof(fwvCmd));
}

/** @brief Send RF parameters to host.
 *
 *  @param getRFParametersCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       getRFParameters(hostCmd);
 *       @endcode
 */
static void getRFParameters(HostCommand getRFParametersCmd)
{
    uint8_t rbwIndex, rbwCount = getRbwTableLength(),
    		rbwLength = getRbwTableEntryLength();
    uint16_t sweepCount = getSweepMaxLength();
	uint8_t *rbwData, requestedBand = getRFParametersCmd.payload[0];

    sendHostAck(getRFParametersCmd); /* First ACK Command */

    uint8_t rfParamCmd[] = {rbwCount,
    		(sweepCount & 0xFF00U) >> 8U, sweepCount & 0x00FFU};

    sendHostArrayResponse(getRFParametersCmd, rfParamCmd, sizeof(rfParamCmd));

    for (rbwIndex = 0U; rbwIndex < rbwCount; rbwIndex++)
    {
    	rbwData = getRbwTableEntryData(rbwIndex, requestedBand);

    	sendHostArrayResponse(getRFParametersCmd, rbwData,
    			rbwLength);
    }
}

/** @brief N/A - Report latest error from the SA1350 to the host.
 *
 *  @param getLastErrorCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       getLastError(hostCmd);
 *       @endcode
 */
static void getLastError(HostCommand getLastErrorCmd)
{
    sendHostAck(getLastErrorCmd); /* First ACK Command */

    /* Array to store payload for last error response. */
    uint8_t lastErrorCmd[] = {HDR_PREFIX, 0x02U, CMD_GETLASTERROR, 0x00U, 0x00U,
    		0U, 0U};

    sendHostResponse(lastErrorCmd, sizeof(lastErrorCmd));
}

/** @brief N/A - Synchronize spectrum sweep parameters with host.
 *
 *  @param syncCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       sync(hostCmd);
 *       @endcode
 */
static void sync(HostCommand syncCmd)
{
    sendHostAck(syncCmd); /* ACK Command */
}

/** @brief Update the frequency band of the spectrum sweep.
 *
 *  @param setFBandCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setFBand(hostCmd);
 *       @endcode
 */
static void setFBand(HostCommand setFBandCmd)
{
	/* Set Frequency band of operation */
	hostMessage.command = CHANGE_BAND;

	sendSweepMessage(setFBandCmd);

    sendHostAck(setFBandCmd); /* ACK Command */
}

/** @brief Update the start frequency of the spectrum sweep.
 *
 *  @param setFStartCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setFStart(hostCmd);
 *       @endcode
 */
static void setFStart(HostCommand setFStartCmd)
{
	/* Set Start Frequency fstart */
	hostMessage.command = SET_START_FREQ;

	sendSweepMessage(setFStartCmd);

    sendHostAck(setFStartCmd); /* ACK Command */
}

/** @brief Update stop frequency of the spectrum sweep.
 *
 *  @param setFStopCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setFStop(hostCmd);
 *       @endcode
 */
static void setFStop(HostCommand setFStopCmd)
{
    /* Set Stop  Frequency fstop */
    hostMessage.command = SET_END_FREQ;

    sendSweepMessage(setFStopCmd);

    sendHostAck(setFStopCmd); /* ACK Command */
}

/** @brief Update span index of the spectrum sweep.
 *
 *  @param setSpanIndexCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setSpanIndex(hostCmd);
 *       @endcode
 */
static void setSpanIndex(HostCommand setSpanIndexCmd)
{
    /* Set Span index */
    hostMessage.command = CHANGE_SPAN;

    sendSweepMessage(setSpanIndexCmd);

    sendHostAck(setSpanIndexCmd); /* ACK Command */
}

/** @brief Update FSW of the spectrum sweep.
 *
 *  @param setFStepCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setFStep(hostCmd);
 *       @endcode
 */
static void setFStep(HostCommand setFStepCmd)
{
    /* Set Step  Frequency fstep */
    hostMessage.command = SET_FREQSTEP;

    sendSweepMessage(setFStepCmd);

    sendHostAck(setFStepCmd); /* ACK Command */
}

/** @brief Update frequency step count of the spectrum sweep.
 *
 *  @param setStepCountCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setStepCount(hostCmd);
 *       @endcode
 */
static void setStepCount(HostCommand setStepCountCmd)
{
    /* Set Step  Count fcount */
    hostMessage.command = SET_STEPCOUNT;

    sendSweepMessage(setStepCountCmd);

    sendHostAck(setStepCountCmd); /* ACK Command */
}

/** @brief Update span of the spectrum sweep.
 *
 *  @param setSpanCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setSpan(hostCmd);
 *       @endcode
 */
static void setSpan(HostCommand setSpanCmd)
{
    /* Set Span  Frequency fspan */
    hostMessage.command = SET_SPAN;

    sendSweepMessage(setSpanCmd);

    sendHostAck(setSpanCmd); /* ACK Command */
}

/** @brief Update RBW of the spectrum sweep.
 *
 *  @param setRbwCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       setRbw(hostCmd);
 *       @endcode
 */
static void setRbw(HostCommand setRbwCmd)
{
    /* Set Rx Filter bandwidth */
    hostMessage.command = SET_RBW;

    sendSweepMessage(setRbwCmd);

    sendHostAck(setRbwCmd); /* ACK Command */
}

/** @brief Update parameters sent with previous host commands to spectrum sweep.
 *
 *  @param initParameterCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       initParameter(hostCmd);
 *       @endcode
 */
static void initParameter(HostCommand initParameterCmd)
{
    /* Notify RF Task that sweep parameters are updated */
	/* RF Task will need to complete a sweep prior to getSpecNoInit, but there
	 * are otherwise no major differences and thus the same command is reused.
	 */
    hostMessage.command = SEND_SPECTRUM;

    sendSweepMessage(initParameterCmd);

    sendHostAck(initParameterCmd); /* ACK Command */
}

/** @brief Send latest spectrum sweep data to host.
 *
 *  @par Usage
 *       @code
 *       sendSpectrum();
 *       @endcode
 */
static void sendSpectrum(void)
{
	uint16_t specCrc, sweepIndex, rssiIndex = 0U,
			sweepSize = getSweepLength();
	int8_t *rssiValues = getSweepData();
	HostCommand gsniCmd = {HDR_PREFIX, 0xFFU, CMD_GETSPECNOINIT,
			{0U}};
//TODO: Refactor to re-use more generic function sendHostArrayResponse() instead of the code below.
	while (sweepSize > 0U)
	{
		/* Determine size of frame to send to host */
		if (sweepSize <= 255U)
		{
			gsniCmd.length = (uint8_t)sweepSize;
		}
		else
		{
			gsniCmd.length = 255U;
		}

		/* calculate initial crc */
		specCrc = calcCrc16(&gsniCmd, HDR_LENGTH);

		/* send response preamble to host */
		UART_write(uart, &gsniCmd, HDR_LENGTH);

		/* send payload */
		UART_write(uart, &rssiValues[rssiIndex], gsniCmd.length);

		/* CRC calculation */
		for (sweepIndex = 0U; sweepIndex < gsniCmd.length; sweepIndex++)
		{
			crc16AddByte(&specCrc, rssiValues[rssiIndex]);
			rssiIndex++;
			sweepSize--;
		}

		/* send response CRC to host */
		gsniCmd.payload[0U] = (specCrc & 0xFF00U) >> 8U;
		gsniCmd.payload[1U]= specCrc & 0x00FFU;
		UART_write(uart, &gsniCmd.payload, CRC_LENGTH);
	}
}

/** @brief Respond to request for spectrum sweep data with latest sweep.
 *
 *  @param getSpecNoInitCmd #HostCommand full command received from host.
 *
 *  @par Usage
 *       @code
 *       getSpecNoInit(hostCmd);
 *       @endcode
 */
static void getSpecNoInit(HostCommand getSpecNoInitCmd)
{
	IArg sweepMutexKey;

	/* Array to store payload for end of frame indication. */
    uint8_t eofCmd[] = {HDR_PREFIX, 0x02U, CMD_GETLASTERROR, 0x00U, 0x00U,
    		0U, 0U};

    /* Notify RF Task that we're sending a sweep out */
    hostMessage.command = SEND_SPECTRUM;

    sendSweepMessage(getSpecNoInitCmd);

    getNewSweep();

    sweepMutexKey = lockSweepData();

    sendHostAck(getSpecNoInitCmd); /* First ACK Command */

    /* Send a frame of spectrum to host */
    sendSpectrum();

    /* Send host notification of end of frame */
    sendHostResponse(eofCmd, sizeof(eofCmd));

    /* Notify RF Task that we're done sending out sweep */
    /* RF Task can continue collecting data */
    unlockSweepData(sweepMutexKey);
}

/** @brief Process command from host and dispatch appropriately.
 *
 *  @param hostCmd #HostCommand full command received from host to process.
 *
 *  @par Usage
 *       @code
 *       processHostCommand(hostCmd);
 *       @endcode
 */
static void processHostCommand(HostCommand hostCmd)
{
    uint16_t cmdCrc = calcCrc16(&hostCmd, hostCmd.length + HDR_LENGTH);
    uint16_t hostCrc = (hostCmd.payload[hostCmd.length] << 8U)
    						| hostCmd.payload[hostCmd.length + 1U];

    /* Check CRC low and high bytes */
    if (cmdCrc == hostCrc)
    {
        /* Process command from host and send appropriate response. */
        switch(hostCmd.command) /* Command Number */
        {
        /***********************************/
        /**** Start of General Commands ****/

            case CMD_CONNECT:
                connect(hostCmd);
            break;

            case CMD_DISCONNECT:
            	disconnect(hostCmd);
            break;

            case CMD_GETDEVICEVER:
                getDeviceVersion(hostCmd);
            break;

            case CMD_GETFWVER:
                getFWVersion(hostCmd);
            break;

            case CMD_GETRFPARAMS:
            	getRFParameters(hostCmd);
            break;

            case CMD_GETLASTERROR:
                getLastError(hostCmd);
            break;

            case CMD_SYNC:
            	sync(hostCmd);
            break;

        /****************************/
        /**** Frequency Commands ****/
            case CMD_SETFBAND:
            	setFBand(hostCmd);
            break;

            case CMD_SETFSTART:
            	setFStart(hostCmd);
            break;

            case CMD_SETFSTOP:
            	setFStop(hostCmd);
            break;

            case CMD_SETSPANINDEX:
            	setSpanIndex(hostCmd);
            break;

            case CMD_SETFSTEP:
            	setFStep(hostCmd);
            break;

            case CMD_SETRBW:
            	setRbw(hostCmd);
            break;

            case CMD_SETSTEPCOUNT:
                setStepCount(hostCmd);
            break;

            case CMD_SETSPAN:
                setSpan(hostCmd);
            break;


        /***************************************/
        /**** Spectrum Measurement Commands ****/

            case CMD_INITPARAMETER:
            	initParameter(hostCmd);
            break;

            case CMD_GETSPECNOINIT:
            	getSpecNoInit(hostCmd);
            break;

            default:
            break;
        }

    }
    else
    {
        while(1){}; /* CRC error */
    }
}

/***** Global function definitions *****/

/** @brief Initialize and construct the uart task.
 *
 *  @par Usage
 *       @code
 *       UartTask_init();
 *       @endcode
 */
void UartTask_init(void)
{
    Task_Params_init(&uartTaskParams);
    uartTaskParams.stackSize = (size_t)UART_TASK_STACK_SIZE;
    uartTaskParams.priority = UART_TASK_PRIORITY;
    uartTaskParams.stack = &uartTaskStack;

    Task_construct(&uartTaskStruct, &uartTaskFxn, &uartTaskParams,
    		(Error_Block *)NULL);
}

/** @brief Task function for UART communications.
 *
 *  @param uartArg0 Optional user argument
 *  @param uartArg1 Optional user argument
 *
 *  @par Usage - Not called by user code. Called by RTOS internal scheduler.
 */
static void uartTaskFxn(UArg uartArg0, UArg uartArg1)
{
	const char sa1350Prompt[] = "\n\fConnect SA1350 host application\r\n";

    openUart();

    /* Open pin driver for Board_PIN_GLED */
    gledPinHandle = PIN_open(&gledPinState, gledPinTable);
    if (!gledPinHandle)
    {
        System_abort("Error initializing Board_PIN_GLED pin\n");
    }

    /* Construct full HostCommand structure */
    HostCommand hostCmd = {0};

    /* Loop forever */
    while (1) {
    	/* Get command number and length */
    	UART_read(uart, &hostCmd, HDR_LENGTH);

        if (hostCmd.prefix != HDR_PREFIX) /* Not a valid command */
        {
            UART_write(uart, sa1350Prompt, sizeof(sa1350Prompt));
        }
        else if(hostCmd.length > MAX_PAYLOAD_SIZE) /* Overflow */
        {
        	while(1){}; /* Packet error */
        }
        else
        {
            /* Read remaining bytes of command and CRC */
            UART_read(uart, &hostCmd.payload,
            		hostCmd.length + CRC_LENGTH);

            processHostCommand(hostCmd);
        }
    }
}
