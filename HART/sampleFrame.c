#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define HART_UART UART2

// Request Frame: Command 0
uint8_t hart_request_frame[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x80, 0x00, 0x00, 0x7D};

// Response Buffer
uint8_t hart_response_frame[20];

// Function to Send HART Frame
void HART_SendFrame(uint8_t *frame, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        while (U2STAbits.UTXBF); // Wait if TX buffer is full
        U2TXREG = frame[i];      // Transmit each byte
    }
}

// Function to Receive HART Frame
bool HART_ReceiveFrame(uint8_t *buffer, uint8_t maxLength, uint8_t *receivedLength) {
    uint8_t i = 0;
    *receivedLength = 0;

    while (i < maxLength) {
        if (U2STAbits.URXDA) {    // Data available in RX buffer
            buffer[i++] = U2RXREG;
        } else {
            break;                // No more data
        }
    }

    *receivedLength = i;
    return (i > 0);               // Return true if data was received
}

// Main Function
int main(void) {
    uint8_t receivedLen = 0;

    // Initialize UART for HART
    U2MODE = 0x8000;  // UART enabled
    U2STA = 0x0400;   // Transmit enabled
    U2BRG = ((80000000 / 16 / 1200) - 1); // Assuming 80 MHz Fcy

    // Send HART Command 0 Frame
    HART_SendFrame(hart_request_frame, sizeof(hart_request_frame));

    // Receive Response
    if (HART_ReceiveFrame(hart_response_frame, sizeof(hart_response_frame), &receivedLen)) {
        // Process the response
        printf("HART Response Received: ");
        for (uint8_t i = 0; i < receivedLen; i++) {
            printf("%02X ", hart_response_frame[i]);
        }
        printf("\n");
    } else {
        printf("No response from HART modem.\n");
    }

    while (1);
    return 0;
}
/*
request :
Field	Hex Value	Description
Preamble	0xFF 0xFF 0xFF 0xFF	Sequence of 0xFF (minimum 5 bytes, configurable up to 20)
Start Delimiter	0x82	Indicates the start of a master-to-slave message
Address	0x80	Short frame address format: Polling address (0x00) + master bit (1)
Command	0x00	Command 0: Read Unique Identifier
Data Length	0x00	No additional data for Command 0
Checksum	0x7D	XOR of all bytes from Start Delimiter to Data Length (to verify integrity)

request frame -- FF FF FF FF 82 80 00 00 7D


response:

Field	Hex Value	Description
Preamble	0xFF 0xFF 0xFF 0xFF	Matches the number of preambles in the request
Start Delimiter	0x86	Indicates the start of a slave-to-master message
Address	0x80	Slave address responding to master
Command	0x00	Echo of Command 0: Read Unique Identifier
Status Byte	0x00	Status OK (no errors)
Data Length	0x06	6 bytes of data (unique identifier)
Data	0x12 0x34 0x56 0x78 0x9A 0xBC	Unique identifier (example data)
Checksum	0xE3	XOR of all bytes from Start Delimiter to the end of the data field

frame -- FF FF FF FF 86 80 00 00 06 12 34 56 78 9A BC E3
*/
