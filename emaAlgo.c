#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

// Constants
#define CONTAINER_CAPACITY 1000.0  // Liters
#define STEP_AMOUNT 50.0          // Liters per step
#define TOTAL_STEPS 20
#define SENSOR_READ_INTERVAL 700  // Milliseconds
#define MAX_STABILITY_TIME 10000  // Milliseconds (10 seconds)
#define STABILITY_THRESHOLD 0.05  // 0.05% change threshold
#define EMA_ALPHA 0.2             // Smoothing factor for EMA

// Function prototypes
double read_sensor();
bool is_stable(double current_ema, double previous_ema, double threshold);

int main() {
    double liquid_level = 0.0;
    double ema = 0.0;  // Exponential Moving Average
    double previous_ema = 0.0;
    int step;

    for (step = 0; step < TOTAL_STEPS; step++) {
        // Add liquid for this step
        liquid_level += STEP_AMOUNT;
        printf("Step %d: Added %.2f liters. Waiting for stability...\n", step + 1, STEP_AMOUNT);

        unsigned int elapsed_time = 0;
        bool stable = false;

        // Wait for stability
        while (elapsed_time < MAX_STABILITY_TIME) {
            // Read sensor data
            double sensor_reading = read_sensor();

            // Apply EMA filtering
            if (elapsed_time == 0) {
                ema = sensor_reading;  // Initialize EMA with the first reading
            } else {
                ema = EMA_ALPHA * sensor_reading + (1 - EMA_ALPHA) * previous_ema;
            }

            // Check if stable
            if (elapsed_time > SENSOR_READ_INTERVAL &&
                is_stable(ema, previous_ema, STABILITY_THRESHOLD)) {
                stable = true;
                printf("Stable level detected: %.2f liters\n", ema);
                break;
            }

            // Update previous EMA and wait for the next reading
            previous_ema = ema;
            elapsed_time += SENSOR_READ_INTERVAL;
        }

        // If stability is not reached, use the latest filtered value
        if (!stable) {
            printf("Timeout! Using latest stable value: %.2f liters\n", ema);
        }
    }

    return 0;
}

// Mock sensor reading function
double read_sensor() {
    // Replace this with actual sensor reading logic
    static double simulated_level = 0.0;
    simulated_level += (rand() % 10 - 5) * 0.01;  // Simulate noise
    return simulated_level;
}

// Function to check stability
bool is_stable(double current_ema, double previous_ema, double threshold) {
    double rate_of_change = fabs((current_ema - previous_ema) / previous_ema) * 100.0;  // Percent change
    return rate_of_change < threshold;
}
