#include <stdio.h>

void print_binary(int n) {
    if (n > 1) {
        print_binary(n / 2);  // Recursive call to divide by 2
    }
    printf("%d", n % 2);  // Print the remainder (0 or 1)
}

void print_fraction_binary(double n) {
    printf(".");
    while (n > 0) {
        n *= 2;
        int frac_bit = (int)n;
        printf("%d", frac_bit);
        n -= frac_bit;
        if (n == 0) break;  // Stop when there's no more fractional part
    }
}

int main() {
    int integer_part = 123;
    double fractional_part = 0.5;

    // Print binary for integer part
    print_binary(integer_part);
    
    // Print binary for fractional part
    print_fraction_binary(fractional_part);

    printf("\n");

    return 0;
}
/*

#include <stdio.h>

union float_union {
    float f;
    unsigned char bytes[4];  // 4 bytes for single precision
};

void print_hex(unsigned char byte) {
    printf("%02X", byte);  // Print the byte in hexadecimal format
}

int main() {
    union float_union u;
    u.f = 123.5f;  // You can change this value to any float value

    printf("IEEE 754 Representation of 123.5 (Single Precision) in Hex:\n");
    for (int i = 0; i < 4; i++) {
        print_hex(u.bytes[i]);
        if (i < 3) {
            printf(" ");  // Add space between bytes
        }
    }
    printf("\n");

    return 0;
}
*/

// To directly convert the binary representation of the number 123.5 into its hexadecimal format, let's break it down:

// Steps:
// Convert 123.5 to its binary representation.
// Print the binary number in its IEEE 754 format (single-precision or 32-bit).
// Convert that binary representation into hexadecimal.
// 1. Convert 123.5 to Binary:
// Integer Part: 123 (decimal)
// The integer part 123 can be converted to binary as:

// 123 ÷ 2 = 61, remainder 1
// 61 ÷ 2 = 30, remainder 1
// 30 ÷ 2 = 15, remainder 0
// 15 ÷ 2 = 7, remainder 1
// 7 ÷ 2 = 3, remainder 1
// 3 ÷ 2 = 1, remainder 1
// 1 ÷ 2 = 0, remainder 1
// Reading from bottom to top: 123 in binary is 1111011.

// Fractional Part: 0.5
// Multiply 0.5 by 2: 0.5 × 2 = 1.0 → integer part = 1.
// 0.5 in binary is .1.
// Thus, 123.5 in binary is: 1111011.1.

// 2. IEEE 754 Representation (Single Precision)
// IEEE 754 single-precision floating-point representation has 32 bits:

// 1 bit for the sign: 0 for positive numbers.
// 8 bits for the exponent (biased by 127).
// 23 bits for the mantissa.
// Normalize the Binary:
// We need to convert 123.5 into normalized form:

// 123.5 = 1111011.1 (binary).
// In normalized form, we move the binary point to the left of the first 1: 1.1110111 × 2^6.
// Now, the exponent is 6. We need to add the bias of 127 to the exponent:

// 6 + 127 = 133.
// 133 in binary is 10000101.
// Construct the IEEE 754 Representation:
// Sign bit: 0 (since the number is positive).
// Exponent: 133 in binary is 10000101.
// Mantissa: The bits after the leading 1 are the mantissa. Take the first 23 bits from 1.1110111, which are 11101110000000000000000.
// So, the IEEE 754 binary representation of 123.5 is:

// Copy code
// 0 10000101 11101110000000000000000
// 3. Convert the Binary Representation to Hexadecimal:
// Now, group the binary representation into 4-bit chunks and convert each chunk into hexadecimal:

// yaml
// Copy code
// Binary:    0 10000101 11101110000000000000000
// Grouped:   0100 0010 1111 1011 0000 0000 0000 0000
// Hex:       42 F6 00 00

