/* Program to measure distances using Khepera IV's ultrasonic sensors */

#include <stdio.h>
#include <khepera/khepera.h>

static knet_dev_t * dsPic; // for microcontroller access

// Function to read and display ultrasonic sensor values
void read_ultrasonic_sensors() {
    char buffer[5];      // Buffer for the 5 ultrasonic sensors
    double distances[5]; // Array to store calculated distances
    int i;              // Loop counter (C89 compatibility)
    
    // Get ultrasonic sensor readings
    if (kh4_measure_us(buffer, dsPic) == 0) {
        
        printf("\nUltrasonic Sensor Readings:\n");
        printf("Sensor\tDistance(mm)\n");
        
        // Process each sensor
        for (i = 0; i < 5; i++) {
            // Convert sensor values to distance in millimeters
            // The sensor value needs to be converted from the raw reading
            distances[i] = (double)(buffer[i]) * 257.0;  // Conversion factor for mm
            
            printf("%d\t%.1f\n", i, distances[i]);
        }
        
        // Print specific positions for easier reference
        printf("\nPositional Readings:\n");
        printf("Front Left (0): %.1f mm\n", distances[0]);
        printf("Front (1): %.1f mm\n", distances[1]);
        printf("Right (2): %.1f mm\n", distances[2]);
        printf("Back (3): %.1f mm\n", distances[3]);
        printf("Left (4): %.1f mm\n", distances[4]);
        
        // Check for nearby obstacles
        printf("\nObstacle Detection:\n");
        for (i = 0; i < 5; i++) {
            if (distances[i] < 200.0) {
                printf("WARNING: Obstacle detected at sensor %d (%.1f mm)\n", 
                       i, distances[i]);
            }
        }
    } else {
        printf("Error reading ultrasonic sensors!\n");
    }
}

// Function to continuously monitor one specific sensor
void monitor_single_sensor(int sensor_num) {
    char buffer[5];
    double distance;
    
    if (sensor_num < 0 || sensor_num > 4) {
        printf("Invalid sensor number! Choose between 0-4\n");
        return;
    }
    
    if (kh4_measure_us(buffer, dsPic) == 0) {
        distance = (double)(buffer[sensor_num]) * 257.0;
        printf("Sensor %d distance: %.1f mm\n", sensor_num, distance);
    } else {
        printf("Error reading ultrasonic sensor %d!\n", sensor_num);
    }
}

int main(int argc, char *argv[]) {
    int mode;           // Operation mode
    int sensor_choice;  // For single sensor monitoring
    char user_input;    // For menu interaction
    
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

    printf("\nUltrasonic Distance Measurement Program\n");
    printf("1. Monitor all sensors continuously\n");
    printf("2. Monitor a single sensor\n");
    printf("Enter mode (1 or 2): ");
    scanf("%d", &mode);

    if (mode == 1) {
        printf("\nMonitoring all ultrasonic sensors...\n");
        printf("Press Ctrl+C to stop\n\n");
        
        // Continuous reading loop
        while(1) {
            read_ultrasonic_sensors();
            usleep(500000);  // 0.5 second delay
            printf("\033[2J\033[H");  // Clear screen
        }
    }
    else if (mode == 2) {
        printf("Enter sensor number (0-4): ");
        scanf("%d", &sensor_choice);
        printf("\nMonitoring sensor %d...\n", sensor_choice);
        printf("Press Ctrl+C to stop\n\n");
        
        // Continuous reading loop for single sensor
        while(1) {
            monitor_single_sensor(sensor_choice);
            usleep(200000);  // 0.2 second delay
            printf("\033[2J\033[H");  // Clear screen
        }
    }
    else {
        printf("Invalid mode selected!\n");
    }

    // Cleanup
    kh4_set_speed(0, 0, dsPic);
    kh4_SetMode(kh4RegIdle, dsPic);
    return 0;
}
