#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Define HART command and response structures
#define HART_PREAMBLE      0xFF
#define HART_START_DELIM   0x02  // Byte that indicates the start of a frame
#define HART_COMMAND_READ  0x01  // Example command to read data
#define HART_COMMAND_WRITE 0x02  // Example command to write data
#define HART_ADDR          0x80  // Example address for the slave device

// Define a sample request and response structure
typedef struct {
    uint8_t preamble;
    uint8_t startDelimiter;
    uint8_t address;
    uint8_t command;
    uint8_t byteCount;
    uint8_t data[256];
    uint8_t checksum;
} HARTMessage;

// Function to calculate checksum (XOR of all bytes except the preamble)
uint8_t calculate_checksum(HARTMessage *msg) {
    uint8_t checksum = 0;
    checksum ^= msg->startDelimiter;
    checksum ^= msg->address;
    checksum ^= msg->command;
    checksum ^= msg->byteCount;
    for (int i = 0; i < msg->byteCount; i++) {
        checksum ^= msg->data[i];
    }
    return checksum;
}

// Function to create a HART request message
HARTMessage create_request(uint8_t command, uint8_t *data, uint8_t dataLength) {
    HARTMessage msg;
    msg.preamble = HART_PREAMBLE;
    msg.startDelimiter = HART_START_DELIM;
    msg.address = HART_ADDR;
    msg.command = command;
    msg.byteCount = dataLength;
    memcpy(msg.data, data, dataLength);
    msg.checksum = calculate_checksum(&msg);
    return msg;
}

// Function to simulate sending a HART message
void send_message(HARTMessage *msg) {
    printf("Sending HART Message:\n");
    printf("Preamble: 0x%02X\n", msg->preamble);
    printf("Start Delimiter: 0x%02X\n", msg->startDelimiter);
    printf("Address: 0x%02X\n", msg->address);
    printf("Command: 0x%02X\n", msg->command);
    printf("Byte Count: %d\n", msg->byteCount);
    for (int i = 0; i < msg->byteCount; i++) {
        printf("Data[%d]: 0x%02X\n", i, msg->data[i]);
    }
    printf("Checksum: 0x%02X\n", msg->checksum);
}

// Function to simulate receiving and processing a HART response
void process_response(HARTMessage *msg) {
    // Check if checksum is valid
    if (msg->checksum != calculate_checksum(msg)) {
        printf("Invalid Checksum!\n");
        return;
    }
    
    printf("Received HART Message:\n");
    printf("Command: 0x%02X\n", msg->command);
    printf("Data:\n");
    for (int i = 0; i < msg->byteCount; i++) {
        printf("0x%02X ", msg->data[i]);
    }
    printf("\n");
}

int main() {
    // Example data to send in a HART request
    uint8_t exampleData[] = { 0x10, 0x20, 0x30 };  // Example payload data
    HARTMessage request = create_request(HART_COMMAND_READ, exampleData, sizeof(exampleData));

    // Send the message (this would interface with FSK modem in real implementation)
    send_message(&request);

    // Simulate receiving the same message as a response for demonstration purposes
    HARTMessage response = request;  // In reality, you would receive this from the device
    process_response(&response);

    return 0;
}
