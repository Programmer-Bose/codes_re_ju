#include <stdio.h>
#include <khepera/khepera.h>

// Pointer to the device structure for microcontroller access
static knet_dev_t *dsPic;

int main(int argc, char *argv[]) {
    // Initialize the Khepera IV robot and its subsystems
    if (kh4_init(argc, argv) != 0) {
        printf("ERROR: could not initiate the libkhepera!\n");
        return -1;
    }

    // Open a communication socket with the microcontroller (dsPic)
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("ERROR: could not initiate communication with Kh4 dsPic\n");
        return -2;
    }

    // Array to store the proximity sensor values
    int prox_values[8];

    // Continuous loop to read and print proximity sensor values
    while (1) {
        // Fetch proximity sensor readings and store them in the prox_values array
        if (kh4_proximity_measure(prox_values, dsPic) == 0) {
            // Print the readings to the console
            printf("Proximity readings: %d %d %d %d %d %d %d %d\n",
                   prox_values[0], prox_values[1], prox_values[2], prox_values[3],
                   prox_values[4], prox_values[5], prox_values[6], prox_values[7]);
        } else {
            // Handle the error if the readings fail
            printf("Failed to get proximity readings.\n");
        }

        // Pause for a short duration to avoid overwhelming the output
        usleep(500000); // 500 ms delay
    }

    // Close the communication socket (this code is not reached due to the infinite loop)
    knet_close(dsPic);

    return 0;
}
