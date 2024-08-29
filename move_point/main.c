#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <khepera/khepera.h>

// Pointer to the device structure for microcontroller access
static knet_dev_t *dsPic;

// Function declarations
void rotate_to_angle(double target_angle);
void move_to_coordinate(double target_x, double target_y);
void update_odometry(int *x_pos, int *y_pos, double *theta);

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

    // Target coordinates to move to
    double target_x = 500.0;  // in mm
    double target_y = 300.0;  // in mm

    // 10-second delay before starting the movement
    printf("Waiting for 10 seconds before starting the movement...\n");
    sleep(10);

    // Move to the target coordinate
    move_to_coordinate(target_x, target_y);

    // Close the communication socket (this code is not reached due to the infinite loop)
    knet_close(dsPic);

    return 0;
}

// Function to rotate the robot to a specific angle (in radians)
void rotate_to_angle(double target_angle) {
    int speed = 50;
    int current_left_odo, current_right_odo;
    double current_theta = 0;

    kh4_odo_reset(dsPic);  // Reset the odometry

    do {
        // Update the odometry to get the current angle
        update_odometry(&current_left_odo, &current_right_odo, &current_theta);

        double angle_diff = target_angle - current_theta;

        if (angle_diff > M_PI) {
            angle_diff -= 2 * M_PI;
        } else if (angle_diff < -M_PI) {
            angle_diff += 2 * M_PI;
        }

        int left_speed = (angle_diff > 0) ? speed : -speed;
        int right_speed = -left_speed;

        kh4_set_speed(left_speed, right_speed, dsPic);

        usleep(10000);  // wait 10 ms

    } while (fabs(target_angle - current_theta) > 0.01);  // tolerance of ~0.01 radians

    kh4_set_speed(0, 0, dsPic);  // stop robot
}

// Function to move the robot to a specific coordinate
void move_to_coordinate(double target_x, double target_y) {
    int speed = 100;
    int current_left_odo, current_right_odo;
    double current_theta = 0;

    double current_x = 0;
    double current_y = 0;

    kh4_odo_reset(dsPic);  // Reset the odometry

    // Calculate the target angle
    double target_angle = atan2(target_y - current_y, target_x - current_x);

    // Rotate to the target angle
    rotate_to_angle(target_angle);

    // Calculate the distance to the target point
    double distance_to_target = sqrt(pow(target_x - current_x, 2) + pow(target_y - current_y, 2));

    // Start moving towards the target point
    while (distance_to_target > 10.0) {  // stop within 10 mm of the target
        kh4_set_speed(speed, speed, dsPic);

        usleep(10000);  // wait 10 ms

        // Update odometry
        update_odometry(&current_left_odo, &current_right_odo, &current_theta);

        current_x += cos(current_theta) * (current_left_odo + current_right_odo) / 2.0;
        current_y += sin(current_theta) * (current_left_odo + current_right_odo) / 2.0;

        // Recalculate the distance to the target point
        distance_to_target = sqrt(pow(target_x - current_x, 2) + pow(target_y - current_y, 2));
    }

    kh4_set_speed(0, 0, dsPic);  // stop robot
}

// Function to update the odometry readings
void update_odometry(int *x_pos, int *y_pos, double *theta) {
    int left_enc, right_enc;

    kh4_odo_read(&left_enc, &right_enc, dsPic);

    // Convert encoder readings to distance in mm
    double left_dist = left_enc * 0.1;  // Conversion factor to mm
    double right_dist = right_enc * 0.1;  // Conversion factor to mm

    // Update the position and orientation
    double delta_theta = (right_dist - left_dist) / 52.7;  // 52.7 mm is the wheelbase of Khepera IV
    *theta += delta_theta;
    *x_pos += (left_dist + right_dist) / 2 * cos(*theta);
    *y_pos += (left_dist + right_dist) / 2 * sin(*theta);
}
