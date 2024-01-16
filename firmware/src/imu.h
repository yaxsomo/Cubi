#ifndef _IMU_H
#define _IMU_H

extern float magn_x, magn_y, magn_z;
extern float gyro_x, gyro_y, gyro_z;
extern float acc_x, acc_y, acc_z;
extern float smooth_pitch, pitch;
extern bool imu_is_checked;

float imu_gyro_yaw();
float imu_yaw();
float imu_yaw_speed();
void imu_init();
void imu_tick();
void imu_calib_start();
void imu_calib_rotate();
void imu_calib_stop();

void imu_smooth_pitch(float coeff, float dt);


#endif
