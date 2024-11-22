/* Program to measure distances using Khepera IV's IR sensors */

#include <stdio.h>
#include <khepera/khepera.h>

static knet_dev_t * dsPic; // for microcontroller access

// Function to read and display IR sensor values
void read_ir_sensors() {
    char proximity_values[24];  // buffer for proximity values
    int i;                     // loop counter (declared at start for C89 compatibility)
    int ambient, reflected;    // variables to store processed values
    double distance;           // to store calculated distance
    
    // Get IR sensor readings - correct API usage
    if (kh4_proximity_ir(proximity_values, dsPic) == 0) {
        
        printf("\nIR Sensor Readings:\n");
        printf("Sensor\tAmbient\tReflected\tDistance(mm)\n");
        
        // Process each sensor
        for (i = 0; i < 8; i++) {  // Khepera IV has 8 IR sensors
            // Extract ambient and reflected values from the buffer
            ambient = (int)(unsigned char)proximity_values[i * 2];
            reflected = (int)(unsigned char)proximity_values[i * 2 + 1];
            
            // Convert raw values to approximate distance in mm
            // Using a more calibrated conversion formula
            if (reflected > 0) {
                distance = 20000.0 / (reflected + 1);  // adjusted conversion
                if (distance > 200.0) distance = 200.0;  // cap maximum distance
            } else {
                distance = 200.0;  // maximum distance when no reflection
            }
            
            printf("%d\t%d\t%d\t\t%.1f\n", 
                   i,
                   ambient,
                   reflected,
                   distance);
        }
        
        // Print specific positions for easier reference
        printf("\nPositional Readings:\n");
        printf("Front Left (0): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[0 * 2 + 1] + 1));
        printf("Front (1): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[1 * 2 + 1] + 1));
        printf("Front Right (2): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[2 * 2 + 1] + 1));
        printf("Right (3): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[3 * 2 + 1] + 1));
        printf("Back Right (4): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[4 * 2 + 1] + 1));
        printf("Back (5): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[5 * 2 + 1] + 1));
        printf("Back Left (6): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[6 * 2 + 1] + 1));
        printf("Left (7): %.1f mm\n", 
               20000.0 / ((int)(unsigned char)proximity_values[7 * 2 + 1] + 1));
    } else {
        printf("Error reading IR sensors!\n");
    }
}

int main(int argc, char *argv[]) {
    // Initialize the robot
    if (kh4_init(argc, argv) != 0) {
        printf("\nERROR: Could not initialize the libkhepera!\n\n");
        return -1;
    }

    // Open robot socket
    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        printf("\nERROR: Could not initialize communication with Kh4 dsPic\n\n");
        return -2;
    }

    printf("Starting IR distance measurement...\n");
    printf("Press Ctrl+C to stop\n\n");

    // Continuous reading loop
    while(1) {
        read_ir_sensors();
        
        // Wait for a second before next reading
        usleep(1000000);  // 1 second delay
        
        // Clear screen for next reading
        printf("\033[2J\033[H");  // ANSI escape codes to clear screen
    }

    // Cleanup
    kh4_set_speed(0, 0, dsPic);
    kh4_SetMode(kh4RegIdle, dsPic);
    return 0;
}
