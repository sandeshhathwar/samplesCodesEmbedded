#include <stdio.h>

union double_union {
    double d;
    unsigned char bytes[8];  // 8 bytes for double precision
};

void print_hex(unsigned char byte) {
    printf("%02X", byte);  // Print the byte in hexadecimal format
}

int main() {
    union double_union u;
    u.d = 123.5;  // The double value

    printf("IEEE 754 Representation of 123.5 (Double Precision) in Hex:\n");
    for (int i = 0; i < 8; i++) {
        print_hex(u.bytes[i]);
        if (i < 7) {
            printf(" ");  // Add space between bytes
        }
    }
    printf("\n");

    return 0;
}
// For double-precision (64-bit) representation of floating-point numbers, the IEEE 754 standard uses 64 bits to represent the number:

// 1 bit for the sign (0 for positive, 1 for negative)
// 11 bits for the exponent (biased by 1023)
// 52 bits for the mantissa (fractional part)
// Steps to Represent 123.5 in Double Precision:
// Convert 123.5 to Binary (same as before):

// 123 in binary: 1111011
// 0.5 in binary: .1
// Thus, 123.5 in binary is: 1111011.1.

// Normalize the Number:

// 123.5 = 1111011.1 in binary.

// In normalized form: 1.1110111 × 2^6 (binary point moved 6 places to the left).

// Exponent = 6.

// We add the bias of 1023 to the exponent:

// Exponent = 6 + 1023 = 1029.

// In binary, 1029 = 10000000101.

// Construct the IEEE 754 Double Precision Representation:

// Sign bit: Since 123.5 is positive, the sign bit is 0.
// Exponent: 1029 in binary is 10000000101.
// Mantissa: The fraction part after the leading 1 in normalized form is 1110111000000000000000000000000000000000000000000000.
// So, the IEEE 754 double-precision representation of 123.5 is:

// Copy code
// 0 10000000101 1110111000000000000000000000000000000000000000000000
// Convert the Binary Representation to Hexadecimal:
// Now, let's convert this binary representation into hexadecimal:

// yaml
// Copy code
// Binary:    0 10000000101 1110111000000000000000000000000000000000000000000000
// Grouped:   0100 0000 0101 1110 1110 0000 0000 0000 0000 0000 0000 0000 0000 0000
// Hex:       40 05 F6 00 00 00 00 00 00