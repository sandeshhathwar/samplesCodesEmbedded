#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "definitions.h"  // Include Harmony-generated configurations

#define HART_PREAMBLE_LEN 5          // Number of preamble bytes (0xFF)
#define HART_START_DELIMITER 0x02    // Request start delimiter
#define HART_ADDR 0x80               // Sensor address for point-to-point
#define HART_COMMAND_READ 0x01       // Command to read primary variable

typedef struct {
    uint8_t preamble[HART_PREAMBLE_LEN];
    uint8_t startDelimiter;
    uint8_t address;
    uint8_t command;
    uint8_t byteCount;
    uint8_t data[256];
    uint8_t checksum;
} HARTMessage;

// Calculate checksum by XORing bytes from startDelimiter through data
uint8_t calculate_checksum(HARTMessage *msg) {
    uint8_t checksum = msg->startDelimiter ^ msg->address ^ msg->command ^ msg->byteCount;
    for (int i = 0; i < msg->byteCount; i++) {
        checksum ^= msg->data[i];
    }
    return checksum;
}

// Create a HART request frame
HARTMessage create_request(uint8_t command) {
    HARTMessage msg;
    memset(msg.preamble, 0xFF, HART_PREAMBLE_LEN);  // Fill preamble with 0xFF
    msg.startDelimiter = HART_START_DELIMITER;
    msg.address = HART_ADDR;
    msg.command = command;
    msg.byteCount = 0;  // No data field for this command
    msg.checksum = calculate_checksum(&msg);
    return msg;
}

// Send HART message over UART
bool send_message(HARTMessage *msg) {
    uint8_t buffer[HART_PREAMBLE_LEN + 5 + msg->byteCount + 1];
    int index = 0;

    memcpy(buffer, msg->preamble, HART_PREAMBLE_LEN);
    index += HART_PREAMBLE_LEN;
    buffer[index++] = msg->startDelimiter;
    buffer[index++] = msg->address;
    buffer[index++] = msg->command;
    buffer[index++] = msg->byteCount;
    memcpy(&buffer[index], msg->data, msg->byteCount);
    index += msg->byteCount;
    buffer[index++] = msg->checksum;

    return (UART3_Write(buffer, index) == index);  // Send complete frame
}

// Receive a HART message over UART
bool receive_message(HARTMessage *msg) {
    uint8_t start_byte;
    UART3_Read(msg->preamble, HART_PREAMBLE_LEN);
    UART3_Read(&start_byte, 1);

    if (start_byte != HART_START_DELIMITER) {
        printf("Invalid start delimiter\n");
        return false;
    }

    msg->startDelimiter = start_byte;
    UART3_Read(&msg->address, 1);
    UART3_Read(&msg->command, 1);
    UART3_Read(&msg->byteCount, 1);
    UART3_Read(msg->data, msg->byteCount);
    UART3_Read(&msg->checksum, 1);

    return msg->checksum == calculate_checksum(msg);
}

// Process the response and extract level data
void process_response(HARTMessage *msg) {
    if (msg->command == HART_COMMAND_READ && msg->byteCount == 4) {
        float level;
        memcpy(&level, msg->data, sizeof(level));  // Assuming data is a float
        printf("Fuel Level: %.2f\n", level);
    } else {
        printf("Data: ");
        for (int i = 0; i < msg->byteCount; i++) {
            printf("0x%02X ", msg->data[i]);
        }
        printf("\n");
    }
}

// Main function to initialize UART and communicate
int main(void) {
    SYS_Initialize(NULL);

    // Create a read request (Command 1) for HART level data
    HARTMessage request = create_request(HART_COMMAND_READ);

    // Send request to sensor
    if (!send_message(&request)) {
        printf("Failed to send message\n");
        return -1;
    }

    // Wait and process response
    HARTMessage response;
    if (receive_message(&response)) {
        process_response(&response);
    } else {
        printf("Failed to receive a valid response\n");
    }

    return 0;
}
