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

    // Array to store the ultrasonic sensor values
    unsigned int us_values[5];

    // Continuous loop to read and print ultrasonic sensor values
    while (1) {
        // Fetch ultrasonic sensor readings and store them in the us_values array
        if (kh4_measure_us(us_values, dsPic) == 0) {
            // Print the readings to the console
            printf("Ultrasonic readings: %u %u %u %u %u\n",
                   us_values[0], us_values[1], us_values[2],
                   us_values[3], us_values[4]);
        } else {
            // Handle the error if the readings fail
            printf("Failed to get ultrasonic readings.\n");
        }

        // Pause for a short duration to avoid overwhelming the output
        usleep(500000); // 500 ms delay
    }

    // Close the communication socket (this code is not reached due to the infinite loop)
    knet_close(dsPic);

    return 0;
}
