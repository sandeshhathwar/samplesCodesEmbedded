#include <stdio.h>
#include <string.h>

union double_union {
    double d;
    unsigned char bytes[8];  // 8 bytes for double precision
};

// Function to convert double to hex representation and print it
void double_to_hex(double value) {
    union double_union u;
    u.d = value;

    printf("Double: %.10f\n", value);
    printf("Hex Representation: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", u.bytes[i]);
    }
    printf("\n");
}

// Function to convert hex representation back to double
double hex_to_double(unsigned char hex_bytes[8]) {
    union double_union u;
    
    // Copy the provided hex bytes into the union's byte array
    memcpy(u.bytes, hex_bytes, 8);
    
    return u.d;
}

int main() {
    double original_value = 123.5678;
    
    // Convert double to hex and display it
    printf("Converting double to hex representation:\n");
    double_to_hex(original_value);

    // IEEE 754 double-precision hex representation of 123.5678
    unsigned char hex_bytes[8] = { 0x9A, 0x99, 0xA3, 0x70, 0x3D, 0xE2, 0x5E, 0x40 };  // Little-endian for 123.5678
    
    // Convert hex back to double and display it
    printf("\nConverting hex back to double:\n");
    double converted_value = hex_to_double(hex_bytes);
    printf("Double value from hex representation: %.10f\n", converted_value);

    return 0;
}
#if 0
#include <stdio.h>
#include <string.h>

union double_union {
    double d;
    unsigned char bytes[8];  // 8 bytes for double precision
};

// Function to convert double to hex representation and print it
void double_to_hex(double value, unsigned char hex_bytes[8]) {
    union double_union u;
    u.d = value;

    printf("Double: %.10f\n", value);
    printf("Hex Representation: ");
    for (int i = 0; i < 8; i++) {
        hex_bytes[i] = u.bytes[i];  // Store each byte in hex_bytes array
        printf("%02X ", hex_bytes[i]);
    }
    printf("\n");
}

// Function to convert hex representation back to double
double hex_to_double(const unsigned char hex_bytes[8]) {
    union double_union u;
    
    // Copy the provided hex bytes into the union's byte array
    memcpy(u.bytes, hex_bytes, 8);
    
    return u.d;
}

int main() {
    double original_value = 123.5678;
    unsigned char hex_bytes[8];  // Array to hold hex bytes

    // Convert double to hex and display it
    printf("Converting double to hex representation:\n");
    double_to_hex(original_value, hex_bytes);

    // Convert hex back to double and display it
    printf("\nConverting hex back to double:\n");
    double converted_value = hex_to_double(hex_bytes);
    printf("Double value from hex representation: %.10f\n", converted_value);

    return 0;
}
#endif