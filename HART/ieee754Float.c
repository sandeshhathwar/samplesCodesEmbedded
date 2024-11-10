#include <stdio.h>
#include <string.h>

union float_union {
    float f;
    unsigned char bytes[4];  // 4 bytes for single precision float
};

// Function to convert float to hex representation and print it
void float_to_hex(float value) {
    union float_union u;
    u.f = value;

    printf("Float: %f\n", value);
    printf("Hex Representation: ");
    for (int i = 0; i < 4; i++) {
        printf("%02X ", u.bytes[i]);
    }
    printf("\n");
}

// Function to convert hex representation back to float and return the float
float hex_to_float(unsigned char hex_bytes[4]) {
    union float_union u;
    
    // Copy the provided hex bytes into the union's byte array
    memcpy(u.bytes, hex_bytes, 4);  // Ensures all bytes are correctly copied
    
    return u.f;
}

int main() {
    float original_value = 123.567f;
    
    // Convert float to hex and display it
    printf("Converting float to hex representation:\n");
    float_to_hex(original_value);

    // IEEE 754 single-precision hex representation of 123.5 in little-endian
    unsigned char hex_bytes[4] = { 0x4E, 0x22, 0xF7, 0x42 };
    
    // Convert hex back to float and display it
    printf("\nConverting hex back to float:\n");
    float converted_value = hex_to_float(hex_bytes);
    printf("Float value from hex representation: %.6f\n", converted_value);  // Print with precision

    return 0;
}


#if 0
#include <stdio.h>
#include <string.h>

union float_union {
    float f;
    unsigned char bytes[4];  // 4 bytes for single precision float
};

// Function to convert float to hex representation and print it
void float_to_hex(float value, unsigned char hex_bytes[4]) {
    union float_union u;
    u.f = value;

    printf("Float: %.6f\n", value);
    printf("Hex Representation: ");
    for (int i = 0; i < 4; i++) {
        hex_bytes[i] = u.bytes[i];  // Store each byte in hex_bytes array
        printf("%02X ", hex_bytes[i]);
    }
    printf("\n");
}

// Function to convert hex representation back to float and return the float
float hex_to_float(const unsigned char hex_bytes[4]) {
    union float_union u;
    
    // Copy the provided hex bytes into the union's byte array
    memcpy(u.bytes, hex_bytes, 4);  // Ensures all bytes are correctly copied
    
    return u.f;
}

int main() {
    float original_value = 123.567f;  // You can change this value to any float
    
    unsigned char hex_bytes[4];  // Array to hold hex bytes
    
    // Convert float to hex and display it
    printf("Converting float to hex representation:\n");
    float_to_hex(original_value, hex_bytes);

    // Convert hex back to float and display it
    printf("\nConverting hex back to float:\n");
    float converted_value = hex_to_float(hex_bytes);
    printf("Float value from hex representation: %.6f\n", converted_value);  // Print with precision

    return 0;
}
#endif