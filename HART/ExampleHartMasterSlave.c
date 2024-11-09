#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HART_PREAMBLE      5        // Number of preamble bytes (0xFF)
#define HART_START_DELIM   0x02     // Master request start delimiter
#define HART_START_DELIM_RESP 0x06  // Slave response start delimiter
#define HART_ADDR          0x80     // Point-to-point address
#define HART_COMMAND_READ  0x01     // Command to read primary variable

typedef struct {
    uint8_t preamble[HART_PREAMBLE];
    uint8_t startDelimiter;
    uint8_t address;
    uint8_t command;
    uint8_t byteCount;
    uint8_t data[256];
    uint8_t checksum;
} HARTMessage;

// Function to calculate the checksum (XOR of all bytes from Start Delimiter to Data field end)
uint8_t calculate_checksum(const HARTMessage *msg) {
    uint8_t checksum = msg->startDelimiter ^ msg->address ^ msg->command ^ msg->byteCount;
    for (int i = 0; i < msg->byteCount; i++) {
        checksum ^= msg->data[i];
    }
    return checksum;
}

// Function to create a HART request frame
HARTMessage create_request(uint8_t command) {
    HARTMessage msg;
    memset(msg.preamble, 0xFF, HART_PREAMBLE);  // Set preamble bytes to 0xFF
    msg.startDelimiter = HART_START_DELIM;
    msg.address = HART_ADDR;
    msg.command = command;
    msg.byteCount = 0;  // No data for this command
    msg.checksum = calculate_checksum(&msg);
    return msg;
}

// Function to simulate sending a HART frame
void send_message(const HARTMessage *msg) {
    printf("Sending HART Frame:\n");
    for (int i = 0; i < HART_PREAMBLE; i++) printf("0x%02X ", msg->preamble[i]);
    printf("\nStart Delimiter: 0x%02X\n", msg->startDelimiter);
    printf("Address: 0x%02X\n", msg->address);
    printf("Command: 0x%02X\n", msg->command);
    printf("Byte Count: %d\n", msg->byteCount);
    printf("Checksum: 0x%02X\n", msg->checksum);
    printf("\n");
}

// Function to simulate a HART device response
HARTMessage generate_response(uint8_t command) {
    HARTMessage msg;
    memset(msg.preamble, 0xFF, HART_PREAMBLE);
    msg.startDelimiter = HART_START_DELIM_RESP;
    msg.address = HART_ADDR;
    msg.command = command;
    msg.byteCount = 4;  // Example data length (4 bytes for IEEE 754 float)
    // Example data: IEEE 754 representation of 123.45 (0x42F6E666)
    msg.data[0] = 0x42;
    msg.data[1] = 0xF6;
    msg.data[2] = 0xE6;
    msg.data[3] = 0x66;
    msg.checksum = calculate_checksum(&msg);
    return msg;
}

// Function to process and verify the response frame
void process_response(const HARTMessage *msg) {
    printf("Received HART Frame:\n");
    if (msg->checksum != calculate_checksum(msg)) {
        printf("Invalid checksum!\n");
        return;
    }

    printf("Command: 0x%02X\n", msg->command);
    printf("Data: ");
    for (int i = 0; i < msg->byteCount; i++) {
        printf("0x%02X ", msg->data[i]);
    }
    printf("\nChecksum: 0x%02X\n", msg->checksum);

    // Decode data as float if the command is reading the primary variable
    if (msg->command == HART_COMMAND_READ && msg->byteCount == 4) {
        float value;
        memcpy(&value, msg->data, sizeof(value));
        printf("Primary Variable (float): %.2f\n", value);
    }
}

int main() {
    // Create and send a HART read request (Command 1)
    HARTMessage request = create_request(HART_COMMAND_READ);
    send_message(&request);

    // Simulate receiving a response from a device
    HARTMessage response = generate_response(HART_COMMAND_READ);
    process_response(&response);

    return 0;
}
