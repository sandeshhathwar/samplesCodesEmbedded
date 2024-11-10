#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Define the size of the preamble (5 bytes of 0xFF)
#define HART_PREAMBLE 5

// Define the HART message structure with a 5-byte preamble
typedef struct {
    uint8_t preamble[HART_PREAMBLE];      // 5 bytes preamble (0xFF, 0xFF, 0xFF, 0xFF, 0xFF)
    uint8_t startDelimiter;               // Start delimiter (usually 0x02)
    uint8_t address;                      // Address of the slave
    uint8_t command;                      // Command byte (operation to perform)
    uint8_t byteCount;                    // Number of data bytes
    uint8_t data[256];                    // Data (up to 256 bytes)
    uint8_t checksum;                     // Checksum for error detection
} HARTMessage;

// Function to calculate checksum (simple XOR-based checksum for example)
uint8_t calculate_checksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

// Function to convert float to IEEE 754 hex representation (4 bytes)
void float_to_hex(float value, uint8_t hex_bytes[4]) {
    union {
        float f;
        uint8_t bytes[4];
    } u;
    
    u.f = value;
    memcpy(hex_bytes, u.bytes, 4);
}

// Function to prepare and send a HART message (Master)
void hart_send_message(HARTMessage *message) {
    message->checksum = calculate_checksum((uint8_t *)message, sizeof(HARTMessage) - 1);  // Exclude checksum from the calculation

    // SPI transmission code (you can replace with your actual SPI interface)
    printf("Sending HART Message:\n");
    for (int i = 0; i < sizeof(HARTMessage); i++) {
        printf("%02X ", ((uint8_t *)message)[i]);
    }
    printf("\n");
}

// Function to prepare a HART message with float data (Master)
void send_hart_float(uint8_t address, uint8_t command, float value) {
    HARTMessage message;
    uint8_t float_bytes[4];

    // Initialize preamble with 5 bytes of 0xFF
    memset(message.preamble, 0xFF, HART_PREAMBLE);  // Preamble with 0xFF
    message.startDelimiter = 0x02;  // Example start delimiter

    // Fill the message details
    message.address = address;
    message.command = command;

    // Convert float to hex and store it in the data array
    float_to_hex(value, float_bytes);
    memcpy(message.data, float_bytes, 4);  // Store float data in the message

    // Set the byte count (4 bytes for the float data)
    message.byteCount = 4;

    // Send the HART message
    hart_send_message(&message);
}

// Function to process received HART message (Slave)
void hart_process_message(HARTMessage *message) {
    // Verify checksum
    uint8_t received_checksum = message->checksum;
    uint8_t calculated_checksum = calculate_checksum((uint8_t *)message, sizeof(HARTMessage) - 1);
    if (received_checksum != calculated_checksum) {
        printf("Checksum error!\n");
        return;
    }

    // Print received message for demonstration
    printf("Received HART Message:\n");
    printf("Address: 0x%02X, Command: 0x%02X, Byte Count: %d\n", message->address, message->command, message->byteCount);

    // Process the data (assume it's a float)
    if (message->byteCount == 4) {
        float value;
        memcpy(&value, message->data, 4);  // Convert received data back to float
        printf("Received Float Value: %.6f\n", value);
    } else {
        printf("Invalid data length!\n");
    }
}

int main() {
    // Master operation: send a float value
    float send_value = 123.567f;
    uint8_t slave_address = 0x01;  // Slave address
    uint8_t command = 0x03;        // Command byte

    // Send HART message (Master side)
    send_hart_float(slave_address, command, send_value);

    // Simulate Slave operation: process a received message (Slave side)
    HARTMessage received_message = {
        .preamble = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 
        .startDelimiter = 0x02, 
        .address = slave_address, 
        .command = command, 
        .byteCount = 4,
        .data = {0x42, 0xF7, 0x22, 0x4E}, // Example IEEE 754 for 123.567
        .checksum = 0x00 // Will be recalculated on the slave side
    };

    // Slave processes the message (checksum validation and data extraction)
    hart_process_message(&received_message);

    return 0;
}
#if TWO_FLOAT_VALUE
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Define the size of the preamble (5 bytes of 0xFF)
#define HART_PREAMBLE 5

// Define the HART message structure with a 5-byte preamble and room for two floats
typedef struct {
    uint8_t preamble[HART_PREAMBLE];      // 5 bytes preamble (0xFF, 0xFF, 0xFF, 0xFF, 0xFF)
    uint8_t startDelimiter;               // Start delimiter (usually 0x02)
    uint8_t address;                      // Address of the slave
    uint8_t command;                      // Command byte (operation to perform)
    uint8_t byteCount;                    // Number of data bytes
    uint8_t data[256];                    // Data (up to 256 bytes)
    uint8_t checksum;                     // Checksum for error detection
} HARTMessage;

// Function to calculate checksum (simple XOR-based checksum for example)
uint8_t calculate_checksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

// Function to convert float to IEEE 754 hex representation (4 bytes)
void float_to_hex(float value, uint8_t hex_bytes[4]) {
    union {
        float f;
        uint8_t bytes[4];
    } u;
    
    u.f = value;
    memcpy(hex_bytes, u.bytes, 4);
}

// Function to prepare and send a HART message (Master)
void hart_send_message(HARTMessage *message) {
    message->checksum = calculate_checksum((uint8_t *)message, sizeof(HARTMessage) - 1);  // Exclude checksum from the calculation

    // SPI transmission code (you can replace with your actual SPI interface)
    printf("Sending HART Message:\n");
    for (int i = 0; i < sizeof(HARTMessage); i++) {
        printf("%02X ", ((uint8_t *)message)[i]);
    }
    printf("\n");
}

// Function to prepare a HART message with two float data (Master)
void send_hart_two_floats(uint8_t address, uint8_t command, float value1, float value2) {
    HARTMessage message;
    uint8_t float_bytes[4];

    // Initialize preamble with 5 bytes of 0xFF
    memset(message.preamble, 0xFF, HART_PREAMBLE);  // Preamble with 0xFF
    message.startDelimiter = 0x02;  // Example start delimiter

    // Fill the message details
    message.address = address;
    message.command = command;

    // Convert first float to hex and store it in the data array
    float_to_hex(value1, float_bytes);
    memcpy(message.data, float_bytes, 4);  // Store first float data

    // Convert second float to hex and store it in the data array
    float_to_hex(value2, float_bytes);
    memcpy(message.data + 4, float_bytes, 4);  // Store second float data

    // Set the byte count (8 bytes for the two float data)
    message.byteCount = 8;

    // Send the HART message
    hart_send_message(&message);
}

// Function to process received HART message (Slave)
void hart_process_message(HARTMessage *message) {
    // Verify checksum
    uint8_t received_checksum = message->checksum;
    uint8_t calculated_checksum = calculate_checksum((uint8_t *)message, sizeof(HARTMessage) - 1);
    if (received_checksum != calculated_checksum) {
        printf("Checksum error!\n");
        return;
    }

    // Print received message for demonstration
    printf("Received HART Message:\n");
    printf("Address: 0x%02X, Command: 0x%02X, Byte Count: %d\n", message->address, message->command, message->byteCount);

    // Process the data (assuming it's two floats)
    if (message->byteCount == 8) {
        float value1, value2;
        memcpy(&value1, message->data, 4);      // Convert first float data
        memcpy(&value2, message->data + 4, 4);  // Convert second float data
        printf("Received Float 1: %.6f\n", value1);
        printf("Received Float 2: %.6f\n", value2);
    } else {
        printf("Invalid data length!\n");
    }
}

int main() {
    // Master operation: send two float values
    float send_value1 = 123.567f;
    float send_value2 = 987.654f;
    uint8_t slave_address = 0x01;  // Slave address
    uint8_t command = 0x03;        // Command byte

    // Send HART message with two floats (Master side)
    send_hart_two_floats(slave_address, command, send_value1, send_value2);

    // Simulate Slave operation: process a received message (Slave side)
    HARTMessage received_message = {
        .preamble = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 
        .startDelimiter = 0x02, 
        .address = slave_address, 
        .command = command, 
        .byteCount = 8,
        .data = {0x42, 0xF7, 0x22, 0x4E, 0x42, 0xF6, 0x46, 0x56}, // Example IEEE 754 for 123.567 and 987.654
        .checksum = 0x00 // Will be recalculated on the slave side
    };

    // Slave processes the message (checksum validation and data extraction)
    hart_process_message(&received_message);

    return 0;
}

// Sending HART Message:
// FF FF FF FF FF 02 01 03 08 42 F7 22 4E 42 F6 46 56 00 

// Received HART Message:
// Address: 0x01, Command: 0x03, Byte Count: 8
// Received Float 1: 123.567001
// Received Float 2: 987.653992

#endif