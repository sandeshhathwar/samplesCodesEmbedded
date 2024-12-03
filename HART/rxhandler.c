void UART1_ReadCallback(uintptr_t context) {
    static uint8_t state = 0;           // State machine for frame parsing
    static uint8_t receivedBytes = 0;  // Number of received bytes
    static uint8_t frameLength = 0;    // Calculated frame length (header + data + checksum)
    static uint8_t preambleCount = 0;  // Count of received preamble bytes

    uint8_t byte = 0;

    // Read the received byte
    if (UART1_Read(&byte, 1)) {
        switch (state) {
            case 0:  // Detect Preamble (0xFF)
                if (byte == 0xFF) {
                    preambleCount++;
                    if (preambleCount == 5) {
                        state = 1;  // Move to Start Delimiter detection
                    }
                } else {
                    preambleCount = 0;  // Reset if not a preamble
                }
                break;

            case 1:  // Detect Start Delimiter (0x86)
                if (byte == 0x86) {
                    hartRxBuffer[receivedBytes++] = byte;  // Save Start Delimiter
                    state = 2;  // Move to Address byte processing
                } else {
                    state = 0;  // Invalid Start Delimiter, reset state
                    preambleCount = 0;
                }
                break;

            case 2:  // Process Address (5 bytes)
                hartRxBuffer[receivedBytes++] = byte;

                if (receivedBytes == 6) {  // Start Delimiter + Address (5 bytes)
                    state = 3;  // Move to Command processing
                }
                break;

            case 3:  // Process Command (1 byte)
                hartRxBuffer[receivedBytes++] = byte;
                state = 4;  // Move to Byte Count processing
                break;

            case 4:  // Process Byte Count (1 byte)
                hartRxBuffer[receivedBytes++] = byte;

                // Determine full frame length: Start Delimiter + Address (5) + Command (1) + Byte Count + Data + Checksum
                frameLength = 6 + 1 + 1 + byte + 1;

                // Validate maximum frame length
                if (frameLength > HART_MAX_FRAME_SIZE) {
                    printf("Error: Frame length exceeds maximum allowed size.\n");
                    state = 0;
                    receivedBytes = 0;
                    preambleCount = 0;
                } else {
                    state = 5;  // Move to Data processing
                }
                break;

            case 5:  // Process Data (length determined by Byte Count)
                hartRxBuffer[receivedBytes++] = byte;

                if (receivedBytes >= frameLength) {  // Full frame received
                    responseReceived = true;

                    // Validate checksum
                    if (HART_ValidateChecksum(hartRxBuffer, frameLength)) {
                        responseValid = true;
                        printf("Valid HART frame received.\n");
                    } else {
                        responseValid = false;
                        printf("Invalid HART checksum.\n");
                    }

                    // Reset state for next frame
                    state = 0;
                    receivedBytes = 0;
                    preambleCount = 0;
                }
                break;

            default:
                // Reset in case of unexpected state
                state = 0;
                preambleCount = 0;
                receivedBytes = 0;
                break;
        }
    }
}


bool HART_ValidateChecksum(uint8_t *frame, size_t length) {
    uint8_t checksum = 0;

    // XOR from Start Delimiter to last data byte
    for (size_t i = 1; i < length - 1; i++) {
        checksum ^= frame[i];
    }

    // Verify if checksum matches the last byte
    return (checksum == frame[length - 1]);
}


void SensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    TickType_t startWaitTime;
    uint8_t hartQueryFrame[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Preamble
        0x86,                          // Start Delimiter
        0x16, 0xE5, 0x00, 0x00, 0x01,  // Address
        0xC5,                          // Command
        0x04,                          // Byte Count
        0x00, 0x00, 0x00, 0x00,        // Data
        0x00                           // Placeholder for Checksum
    };

    // Calculate checksum
    uint8_t checksum = 0;
    for (size_t i = 5; i < sizeof(hartQueryFrame) - 1; i++) {
        checksum ^= hartQueryFrame[i];
    }
    hartQueryFrame[sizeof(hartQueryFrame) - 1] = checksum;

    while (1) {
        uint8_t retryCount = 0;
        responseReceived = false;
        responseValid = false;

        while (retryCount < MAX_RETRIES) {
            // Send the HART query frame
            UART1_Write(hartQueryFrame, sizeof(hartQueryFrame));
            printf("HART query sent. Attempt %d\n", retryCount + 1);

            // Wait for the response or timeout
            startWaitTime = xTaskGetTickCount();
            while (!responseReceived && (xTaskGetTickCount() - startWaitTime) < pdMS_TO_TICKS(200)) {
                taskYIELD();  // Allow other tasks to run
            }

            if (responseReceived) {
                if (responseValid) {
                    // Print the valid response
                    UART2_Write(hartRxBuffer, frameLength);
                    printf("Valid HART response received and printed to UART2.\n");
                    break;  // Exit retry loop
                } else {
                    printf("Invalid HART response received.\n");
                }
            } else {
                printf("HART response timeout.\n");
            }

            retryCount++;
        }

        if (retryCount >= MAX_RETRIES) {
            printf("Failed to receive a valid HART response after %d retries.\n", MAX_RETRIES);
        }

        // Delay to maintain the 700ms cycle
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(700));
    }
}
