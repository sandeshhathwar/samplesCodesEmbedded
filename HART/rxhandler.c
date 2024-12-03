// Global variables for state management
volatile uint8_t hartRxBuffer[HART_MAX_FRAME_SIZE] = {0};  // HART frame buffer
volatile uint8_t hartRxState = 0;                         // State machine state
volatile uint8_t receivedBytes = 0;                       // Count of received bytes
volatile uint8_t frameLength = 0;                         // Full frame length
volatile uint8_t preambleCount = 0;                       // Count of preamble bytes
volatile bool responseReceived = false;                   // Indicates response completion
volatile bool responseValid = false;                      // Indicates checksum validation success

// UART1 Read Callback
void UART1_ReadCallback(uintptr_t context) {
    uint8_t byte = 0;

    // Read the received byte
    if (UART1_Read(&byte, 1)) {
        switch (hartRxState) {
            case 0:  // Detect Preamble (0xFF)
                if (byte == 0xFF) {
                    preambleCount++;
                    if (preambleCount == 5) {
                        hartRxState = 1;  // Move to Start Delimiter detection
                    }
                } else {
                    preambleCount = 0;  // Reset if not a preamble
                }
                break;

            case 1:  // Detect Start Delimiter (0x86)
                if (byte == 0x86) {
                    hartRxBuffer[receivedBytes++] = byte;  // Save Start Delimiter
                    hartRxState = 2;  // Move to Address byte processing
                } else {
                    hartRxState = 0;  // Invalid Start Delimiter, reset state
                    preambleCount = 0;
                    receivedBytes = 0;
                }
                break;

            case 2:  // Process Address (5 bytes)
                hartRxBuffer[receivedBytes++] = byte;

                if (receivedBytes == 6) {  // Start Delimiter + Address (5 bytes)
                    hartRxState = 3;  // Move to Command processing
                }
                break;

            case 3:  // Process Command (1 byte)
                hartRxBuffer[receivedBytes++] = byte;
                hartRxState = 4;  // Move to Byte Count processing
                break;

            case 4:  // Process Byte Count (1 byte)
                hartRxBuffer[receivedBytes++] = byte;

                // Determine full frame length: Start Delimiter + Address (5) + Command (1) + Byte Count + Data + Checksum
                frameLength = 6 + 1 + 1 + byte + 1;

                // Validate maximum frame length
                if (frameLength > HART_MAX_FRAME_SIZE) {
                    printf("Error: Frame length exceeds maximum allowed size.\n");
                    hartRxState = 0;
                    receivedBytes = 0;
                    preambleCount = 0;
                } else {
                    hartRxState = 5;  // Move to Data processing
                }
                break;

            case 5:  // Process Data (length determined by Byte Count)
                hartRxBuffer[receivedBytes++] = byte;

                if (receivedBytes >= frameLength) {  // Full frame received
                    responseReceived = true;

                    // Validate checksum
                    if (HART_ValidateChecksum((uint8_t *)hartRxBuffer, frameLength)) {
                        responseValid = true;
                        printf("Valid HART frame received.\n");
                    } else {
                        responseValid = false;
                        printf("Invalid HART checksum.\n");
                    }

                    // Reset state for next frame
                    hartRxState = 0;
                    receivedBytes = 0;
                    preambleCount = 0;
                }
                break;

            default:
                // Reset in case of unexpected state
                hartRxState = 0;
                preambleCount = 0;
                receivedBytes = 0;
                break;
        }
    }
}
