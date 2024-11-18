/* 
Let’s break down the request frame FF FF FF FF 82 16 E5 00 00 02 C5 00 B6 and explain its meaning. Then, we’ll analyze a potential response frame.

Request Frame Explanation
Field	Value	Description
Preamble	FF FF FF FF	Four preamble bytes (minimum required for synchronization; usually 5-20 bytes).
Start Delimiter	82	Indicates a master-to-slave request.
Address	16 E5	HART long address format: 16 (manufacturer ID and device type), E5 (device ID).
Command	00	Command 0: Read unique identifier.
Data Length	00	No additional data in this command.
Data	02 C5 00	Optional data payload (interpreted based on the command).
Checksum	B6	XOR of all bytes from Start Delimiter (82) to the last data byte (00).
Details for the Address Field (16 E5):
16: Manufacturer ID and Device Type Code.
E5: Unique device serial number or identifier.
Data Field Analysis:
Command 0 doesn’t usually require additional data; however, the inclusion here may mean this is a customized or expanded implementation.
02 C5 00 could potentially represent:
02: A sub-command or field indicating specific information to retrieve.
C5 00: Additional options or parameters.
Potential Response Frame
If the slave device responds successfully, the frame might look like this:

Response Frame: FF FF FF FF 86 16 E5 00 00 06 12 34 56 78 9A BC E3

Field	Value	Description
Preamble	FF FF FF FF	Matches the number of preamble bytes in the request.
Start Delimiter	86	Indicates a slave-to-master response.
Address	16 E5	Echoes the master’s request address, confirming the targeted device.
Command	00	Echoes the command number from the request frame.
Status Byte	00	Status: 00 indicates success (no errors).
Data Length	06	Indicates 6 bytes of response data.
Data	12 34 56 78 9A BC	Unique Identifier (example placeholder data).
Checksum	E3	XOR of all bytes from Start Delimiter (86) to the last data byte (BC).
Steps for Decoding Response
Check the Status Byte:

If 00: Command executed successfully.
If non-zero: Refer to HART status codes for error information.
Extract the Data:

For Command 0, the data typically contains the unique identifier of the HART device.
Example Data: 12 34 56 78 9A BC could be the device’s unique serial number.
Verify the Checksum:

XOR all bytes from Start Delimiter (86) to the last data byte (BC).
Ensure the calculated checksum matches the provided checksum (E3).
Summary
Request: The master sends a command to read the unique identifier of a HART device, specifying the target via the long address 16 E5.
Response: The slave device acknowledges the request with a response frame containing its unique identifier and a status byte.
*/
#if 0
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define HART_UART UART2 // Define UART for HART communication

// Function prototypes
void UART_Init(void);
void HART_SendFrame(uint8_t *frame, uint8_t length);
bool HART_ReceiveFrame(uint8_t *buffer, uint8_t maxLength, uint8_t *receivedLength);
uint8_t CalculateChecksum(uint8_t *data, uint8_t length);

// Main function
int main(void) {
    uint8_t hart_request_frame[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x16, 0xE5, 0x00, 0x00, 0x02, 0xC5, 0x00, 0x00}; // Request frame (checksum placeholder at end)
    uint8_t hart_response_frame[20]; // Buffer to store the response
    uint8_t receivedLen = 0;

    // Calculate checksum for the request frame
    hart_request_frame[sizeof(hart_request_frame) - 1] = CalculateChecksum(&hart_request_frame[4], sizeof(hart_request_frame) - 5);

    // Initialize UART for HART communication
    UART_Init();

    // Send HART request frame
    HART_SendFrame(hart_request_frame, sizeof(hart_request_frame));

    // Receive response
    if (HART_ReceiveFrame(hart_response_frame, sizeof(hart_response_frame), &receivedLen)) {
        // Verify response checksum
        uint8_t checksum = CalculateChecksum(&hart_response_frame[4], receivedLen - 5);
        if (checksum == hart_response_frame[receivedLen - 1]) {
            // Parse and print response
            printf("HART Response Received: ");
            for (uint8_t i = 0; i < receivedLen; i++) {
                printf("%02X ", hart_response_frame[i]);
            }
            printf("\n");
        } else {
            printf("Checksum mismatch in the response.\n");
        }
    } else {
        printf("No response received from HART device.\n");
    }

    while (1) {
        // Main loop
    }

    return 0;
}

// UART Initialization for HART
void UART_Init(void) {
    U2MODE = 0x8000; // Enable UART2
    U2STA = 0x0400;  // Enable Transmit
    U2BRG = ((80000000 / 16 / 1200) - 1); // Set baud rate to 1200 (assuming Fcy = 80 MHz)
}

// Function to send a HART frame
void HART_SendFrame(uint8_t *frame, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        while (U2STAbits.UTXBF); // Wait if TX buffer is full
        U2TXREG = frame[i];      // Send byte
    }
}

// Function to receive a HART frame
bool HART_ReceiveFrame(uint8_t *buffer, uint8_t maxLength, uint8_t *receivedLength) {
    uint8_t i = 0;
    *receivedLength = 0;

    // Wait for response
    while (i < maxLength) {
        if (U2STAbits.URXDA) {    // Check if data is available in RX buffer
            buffer[i++] = U2RXREG;
        } else {
            break; // Exit if no more data
        }
    }

    *receivedLength = i;
    return (i > 0); // Return true if data was received
}

// Function to calculate the checksum (XOR of all bytes in the frame)
uint8_t CalculateChecksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}
#endif




#if 0
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define HART_MAX_DATA_LEN 255

// HART Frame Structure
typedef struct {
    uint8_t preamble[20];   // Up to 20 preamble bytes
    uint8_t preamble_len;   // Length of preamble
    uint8_t start_delimiter;
    uint8_t address[5];     // Address (short or long)
    uint8_t address_len;    // Address length (1 for short, 5 for long)
    uint8_t command;
    uint8_t data_len;       // Length of data
    uint8_t data[HART_MAX_DATA_LEN]; // Data payload
    uint8_t checksum;       // Checksum
} HART_Frame;


int main(void) {
    HART_Frame request_frame, response_frame;
    uint8_t response_buffer[50];
    uint8_t response_length = 0;

    // Initialize UART
    UART_Init();

    // Define the target address and data for Command 0 (example)
    uint8_t address[] = {0x16, 0xE5}; // Long address example
    uint8_t command_data[] = {0x02, 0xC5, 0x00};

    // Construct and send a request frame
    HART_ConstructFrame(&request_frame, 0x00, address, sizeof(address), command_data, sizeof(command_data));
    HART_SendFrame(&request_frame);

    // Receive and parse response
    if (HART_ReceiveFrame(response_buffer, sizeof(response_buffer), &response_length)) {
        if (HART_ParseResponse(response_buffer, response_length, &response_frame)) {
            // Print response data
            printf("Response Data: ");
            for (uint8_t i = 0; i < response_frame.data_len; i++) {
                printf("%02X ", response_frame.data[i]);
            }
            printf("\n");
        } else {
            printf("Invalid response checksum.\n");
        }
    } else {
        printf("No response received.\n");
    }

    while (1) {
        // Main loop
    }

    return 0;
}

bool HART_ParseResponse(uint8_t *response, uint8_t length, HART_Frame *parsed_frame) {
    if (length < 5) {
        return false; // Invalid frame length
    }

    uint8_t index = 0;

    // Parse preamble
    parsed_frame->preamble_len = 0;
    while (response[index] == 0xFF) {
        parsed_frame->preamble[index++] = response[index];
        parsed_frame->preamble_len++;
    }

    // Parse start delimiter
    parsed_frame->start_delimiter = response[index++];

    // Parse address
    parsed_frame->address_len = 5; // Assume long address
    for (uint8_t i = 0; i < parsed_frame->address_len; i++) {
        parsed_frame->address[i] = response[index++];
    }

    // Parse command
    parsed_frame->command = response[index++];

    // Parse data length
    parsed_frame->data_len = response[index++];

    // Parse data
    for (uint8_t i = 0; i < parsed_frame->data_len; i++) {
        parsed_frame->data[i] = response[index++];
    }

    // Parse checksum
    parsed_frame->checksum = response[index++];

    // Validate checksum
    uint8_t calculated_checksum = CalculateChecksum(&response[parsed_frame->preamble_len], length - parsed_frame->preamble_len - 1);
    return (calculated_checksum == parsed_frame->checksum);
}

void HART_SendFrame(HART_Frame *frame) {
    // Send preamble
    for (uint8_t i = 0; i < frame->preamble_len; i++) {
        while (U2STAbits.UTXBF); // Wait if TX buffer is full
        U2TXREG = frame->preamble[i];
    }

    // Send start delimiter
    while (U2STAbits.UTXBF);
    U2TXREG = frame->start_delimiter;

    // Send address
    for (uint8_t i = 0; i < frame->address_len; i++) {
        while (U2STAbits.UTXBF);
        U2TXREG = frame->address[i];
    }

    // Send command
    while (U2STAbits.UTXBF);
    U2TXREG = frame->command;

    // Send data length
    while (U2STAbits.UTXBF);
    U2TXREG = frame->data_len;

    // Send data
    for (uint8_t i = 0; i < frame->data_len; i++) {
        while (U2STAbits.UTXBF);
        U2TXREG = frame->data[i];
    }

    // Send checksum
    while (U2STAbits.UTXBF);
    U2TXREG = frame->checksum;
}

void HART_ConstructFrame(HART_Frame *frame, uint8_t command, uint8_t *address, uint8_t address_len, uint8_t *data, uint8_t data_len) {
    // Set preamble (minimum 5 bytes of 0xFF)
    frame->preamble_len = 5; // Use 5 bytes for this example
    for (uint8_t i = 0; i < frame->preamble_len; i++) {
        frame->preamble[i] = 0xFF;
    }

    // Set start delimiter (0x82 for master-to-slave request)
    frame->start_delimiter = 0x82;

    // Set address
    frame->address_len = address_len;
    for (uint8_t i = 0; i < address_len; i++) {
        frame->address[i] = address[i];
    }

    // Set command
    frame->command = command;

    // Set data and length
    frame->data_len = data_len;
    for (uint8_t i = 0; i < data_len; i++) {
        frame->data[i] = data[i];
    }

    // Calculate checksum (Start delimiter to end of data)
    uint8_t temp_frame[HART_MAX_DATA_LEN + 10];
    uint8_t index = 0;
    temp_frame[index++] = frame->start_delimiter;
    for (uint8_t i = 0; i < frame->address_len; i++) {
        temp_frame[index++] = frame->address[i];
    }
    temp_frame[index++] = frame->command;
    temp_frame[index++] = frame->data_len;
    for (uint8_t i = 0; i < frame->data_len; i++) {
        temp_frame[index++] = frame->data[i];
    }
    frame->checksum = CalculateChecksum(temp_frame, index);
}
#endif



/***
1. Frame Structure Overview
We will define the HART frame structure and operations in a clean and isolated manner, without mixing it with low-level driver or UART handling code.

HART Frame Structure
Here’s the basic frame structure for both request and response frames:

c
Copy code
#include <stdint.h>
#include <stdbool.h>

#define HART_MAX_DATA_LEN 255

// HART Frame Structure
typedef struct {
    uint8_t preamble[4];     // Fixed to 4 bytes for preamble
    uint8_t start_delimiter; // Start delimiter (0x82 for master-to-slave, 0x86 for slave-to-master)
    uint8_t address[5];      // Address (short or long)
    uint8_t address_len;     // Address length (1 byte for short, 5 bytes for long)
    uint8_t command;         // Command byte
    uint8_t data_len;        // Length of data
    uint8_t data[HART_MAX_DATA_LEN];  // Data (payload)
    uint8_t checksum;        // Checksum byte
} HART_Frame;
Preamble: Fixed length of 4 bytes, each byte is 0xFF for synchronization.
Start Delimiter: 0x82 (Master to Slave) or 0x86 (Slave to Master).
Address: Can be a short (1 byte) or long (5 bytes) address.
Command: The HART command (e.g., 0x00 for Read Unique Identifier).
Data: The actual data or payload related to the command.
Checksum: Calculated using XOR on the relevant frame bytes.
2. Frame Construction
The following function constructs a generic request frame. It takes the command, address, and data as input and builds the frame accordingly.

HART_ConstructFrame Function
c
Copy code
void HART_ConstructFrame(HART_Frame *frame, uint8_t command, uint8_t *address, uint8_t address_len, uint8_t *data, uint8_t data_len) {
    // Set preamble (fixed to 4 bytes 0xFF)
    for (uint8_t i = 0; i < 4; i++) {
        frame->preamble[i] = 0xFF;
    }

    // Set start delimiter for master-to-slave (0x82)
    frame->start_delimiter = 0x82;

    // Set address
    frame->address_len = address_len;
    for (uint8_t i = 0; i < address_len; i++) {
        frame->address[i] = address[i];
    }

    // Set command
    frame->command = command;

    // Set data and length
    frame->data_len = data_len;
    for (uint8_t i = 0; i < data_len; i++) {
        frame->data[i] = data[i];
    }

    // Calculate checksum
    uint8_t temp_frame[HART_MAX_DATA_LEN + 10];
    uint8_t index = 0;

    // Copy the relevant frame bytes (start delimiter, address, command, data)
    temp_frame[index++] = frame->start_delimiter;
    for (uint8_t i = 0; i < frame->address_len; i++) {
        temp_frame[index++] = frame->address[i];
    }
    temp_frame[index++] = frame->command;
    temp_frame[index++] = frame->data_len;
    for (uint8_t i = 0; i < frame->data_len; i++) {
        temp_frame[index++] = frame->data[i];
    }

    // XOR all bytes for checksum
    frame->checksum = 0x00;
    for (uint8_t i = 0; i < index; i++) {
        frame->checksum ^= temp_frame[i];
    }
}
Input: Command, address, data, and data length.
Output: The constructed frame with calculated checksum.
3. Example Request Frame
Here’s an example of how you can construct and send a request frame for Command 0 (Read Unique Identifier).

Example: Command 0 (Read Unique Identifier)
Let’s assume:

Command: 0x00 (Read Unique Identifier).
Address: 0x16 0xE5 (Long address).
Data: 0x02 0xC5 0x00 (Extended data).
c
Copy code
HART_Frame request_frame;
uint8_t address[] = {0x16, 0xE5};    // Example long address
uint8_t data[] = {0x02, 0xC5, 0x00}; // Example data

// Construct the request frame
HART_ConstructFrame(&request_frame, 0x00, address, sizeof(address), data, sizeof(data));
This will result in the following request frame:

plaintext
Copy code
FF FF FF FF 82 16 E5 00 03 02 C5 00 B6
4. Response Frame Structure
The response frame will follow a similar structure but with start delimiter 0x86 (Slave to Master) and the data being the unique identifier of the device.

Example Response for Command 0 (Read Unique Identifier)
Let’s assume the device responds with a unique identifier 0x12 0x34 0x56 0x78 0x9A 0xBC.

c
Copy code
HART_Frame response_frame;
uint8_t response_data[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};  // Example unique identifier

// Construct the response frame
HART_ConstructFrame(&response_frame, 0x00, address, sizeof(address), response_data, sizeof(response_data));
This will result in the following response frame:

plaintext
Copy code
FF FF FF FF 86 16 E5 00 06 12 34 56 78 9A BC E3
Start Delimiter: 0x86 — Slave to master.
Address: 0x16 0xE5 — Same as the master.
Command: 0x00 — Command echoed back.
Data: 0x12 0x34 0x56 0x78 0x9A 0xBC — Unique identifier of the device.
Checksum: 0xE3 — Calculated XOR of the frame bytes.
5. Response Parsing
After receiving the response frame, we need to parse the data. Here's a function to parse the response frame.

HART_ParseResponse Function
c
Copy code
bool HART_ParseResponse(uint8_t *response, uint8_t length, HART_Frame *parsed_frame) {
    if (length < 5) {
        return false; // Invalid frame length
    }

    uint8_t index = 0;

    // Parse preamble (fixed to 4 bytes 0xFF)
    parsed_frame->preamble_len = 0;
    while (response[index] == 0xFF) {
        parsed_frame->preamble[index++] = response[index++];
        parsed_frame->preamble_len++;
    }

    // Parse start delimiter
    parsed_frame->start_delimiter = response[index++];

    // Parse address
    parsed_frame->address_len = 5; // Assume long address
    for (uint8_t i = 0; i < parsed_frame->address_len; i++) {
        parsed_frame->address[i] = response[index++];
    }

    // Parse command
    parsed_frame->command = response[index++];

    // Parse data length
    parsed_frame->data_len = response[index++];

    // Parse data
    for (uint8_t i = 0; i < parsed_frame->data_len; i++) {
        parsed_frame->data[i] = response[index++];
    }

    // Parse checksum
    parsed_frame->checksum = response[index++];

    // Validate checksum
    uint8_t calculated_checksum = 0x00;
    for (uint8_t i = 0; i < index; i++) {
        calculated_checksum ^= response[i];
    }

    return (calculated_checksum == parsed_frame->checksum);
}
Input: Response data array and its length.
Output: Parsed frame with address, command, data, and checksum.
***/

