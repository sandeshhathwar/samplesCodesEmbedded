#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// UART Configuration for HART Modem
#define HART_UART               UART2
#define HART_BAUD_RATE          1200
#define HART_CMD_READ_UNIQUE_ID 0x00 // Command 0: Read Unique Identifier
#define HART_RESPONSE_SIZE      25   // Adjust depending on expected response

// UART Initialization for HART
void HART_UART_Init(void) {
    // Configure UART2 for 1200 baud, 8N1
    U2MODE = 0x8000;                 // UART2 enabled
    U2STA = 0x0400;                  // Enable transmit
    U2BRG = ((80000000 / 16 / HART_BAUD_RATE) - 1); // Assuming 80 MHz Fcy
}

// Function to Send Command to HART Modem
void HART_SendCommand(uint8_t command, uint8_t* data, uint8_t dataLen) {
    uint8_t i;

    while (U2STAbits.UTXBF);         // Wait if TX buffer is full
    U2TXREG = command;               // Send the command byte

    for (i = 0; i < dataLen; i++) {
        while (U2STAbits.UTXBF);     // Wait for space in TX buffer
        U2TXREG = data[i];           // Send data bytes
    }
}

// Function to Receive Response from HART Modem
bool HART_ReceiveResponse(uint8_t* buffer, uint8_t maxLen, uint8_t* receivedLen) {
    uint8_t i = 0;
    *receivedLen = 0;

    while (i < maxLen) {
        if (U2STAbits.URXDA) {       // Check if data is available
            buffer[i++] = U2RXREG;   // Read data byte
        } else {
            break;                   // Exit if no data available
        }
    }

    *receivedLen = i;
    return (i > 0);                  // Return true if data received
}

// Function to Execute Universal Command 0 (Read Unique Identifier)
void HART_ExecuteCommand0(void) {
    uint8_t command = HART_CMD_READ_UNIQUE_ID;
    uint8_t response[HART_RESPONSE_SIZE];
    uint8_t responseLen = 0;

    // Send Command 0
    HART_SendCommand(command, NULL, 0);

    // Receive and process response
    if (HART_ReceiveResponse(response, HART_RESPONSE_SIZE, &responseLen)) {
        // Parse response (assuming response[0] is the status byte)
        uint8_t status = response[0];
        if (status == 0x00) {
            printf("HART Command 0 executed successfully.\n");
            printf("Unique Identifier: ");
            for (uint8_t i = 1; i < responseLen; i++) {
                printf("%02X ", response[i]); // Display the identifier
            }
            printf("\n");
        } else {
            printf("HART Command 0 failed. Status: 0x%02X\n", status);
        }
    } else {
        printf("No response from HART modem.\n");
    }
}

// Main Function
int main(void) {
    // Initialize UART
    HART_UART_Init();

    // Execute Command 0
    HART_ExecuteCommand0();

    while (1) {
        // Main loop
    }

    return 0;
}
