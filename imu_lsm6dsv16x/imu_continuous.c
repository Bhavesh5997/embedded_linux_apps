/*
 * Continuous IMU Reader with Threads
 *
 * - Reads accelerometer and angle values every 10 seconds
 * - Uses two threads: one for reading sensor, one for printing data
 * - Prevents print mixing using mutex lock
 * - Runs continuously until user presses any key to exit
 *
 * This is a generic Linux I2C user-space application.
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 15

static pthread_mutex_t thread_mux;

struct thread_data {
	int fd_x, fd_y, fd_z, fd_scale;
	bool thread_stop;
};

void *accel_thread(void *arg)
{
	struct thread_data *ptr = (struct thread_data *)arg;
	char buf[MAX];
	double scale, x_accel, y_accel, z_accel;
	int ret;

	ret = read(ptr->fd_scale, buf, MAX);

	if (ret == -1) {
		printf("\nFailed to read acceleration scale value\n");
		close(ptr->fd_x);
		close(ptr->fd_y);
		close(ptr->fd_z);
		close(ptr->fd_scale);

		return NULL;
	}

	scale = atof(buf);

	while (!ptr->thread_stop) {
		pthread_mutex_lock(&thread_mux);
		lseek(ptr->fd_x, 0, SEEK_SET);
		ret = read(ptr->fd_x, buf, MAX);

		if (ret == -1) {
			printf("\nFailed to read x accleration value\n");
			close(ptr->fd_x);
			close(ptr->fd_y);
			close(ptr->fd_z);
			close(ptr->fd_scale);

			return NULL;
		}

		x_accel = atof(buf);

		printf("\nX acceleration = %lf m/s^2\n", x_accel*scale);

		lseek(ptr->fd_y, 0, SEEK_SET);
		ret = read(ptr->fd_y, buf, MAX);

		if (ret == -1) {
			printf("\nFailed to read x accleration value\n");
			close(ptr->fd_x);
			close(ptr->fd_y);
			close(ptr->fd_z);
			close(ptr->fd_scale);

			return NULL;
		}

		y_accel = atof(buf);

		printf("Y acceleration = %lf m/s^2\n", y_accel*scale);

		lseek(ptr->fd_z, 0, SEEK_SET);
		ret = read(ptr->fd_z, buf, MAX);

		if (ret == -1) {
			printf("\nFailed to read x accleration value\n");
			close(ptr->fd_x);
			close(ptr->fd_y);
			close(ptr->fd_z);
			close(ptr->fd_scale);

			return NULL;
		}

		z_accel = atof(buf);

		printf("Z acceleration = %lf m/s^2\n", z_accel*scale);
		pthread_mutex_unlock(&thread_mux);
		sleep(10);
	}
}

void *angle_thread(void *arg)
{
	struct thread_data *ptr = (struct thread_data *)arg;
	char buf[MAX];
	double scale, x_angl, y_angl, z_angl;
	int ret;

	ret = read(ptr->fd_scale, buf, MAX);

	if (ret == -1) {
		printf("\nFailed to read acceleration scale value\n");
		close(ptr->fd_x);
		close(ptr->fd_y);
		close(ptr->fd_z);
		close(ptr->fd_scale);

		return NULL;
	}

	scale = atof(buf);

	while (!ptr->thread_stop) {
		pthread_mutex_lock(&thread_mux);
		lseek(ptr->fd_x, 0, SEEK_SET);
		ret = read(ptr->fd_x, buf, MAX);

		if (ret == -1) {
			printf("\nFailed to read x angle value\n");
			close(ptr->fd_x);
			close(ptr->fd_y);
			close(ptr->fd_z);
			close(ptr->fd_scale);

			return NULL;
		}

		x_angl = atof(buf);

		printf("\nX angle level = %lf dps\n", x_angl*scale);

		lseek(ptr->fd_y, 0, SEEK_SET);
		ret = read(ptr->fd_y, buf, MAX);

		if (ret == -1) {
			printf("\nFailed to read x angle value\n");
			close(ptr->fd_x);
			close(ptr->fd_y);
			close(ptr->fd_z);
			close(ptr->fd_scale);

			return NULL;
		}

		y_angl = atof(buf);

		printf("Y angle level = %lf dps\n", y_angl*scale);

		lseek(ptr->fd_z, 0, SEEK_SET);
		ret = read(ptr->fd_z, buf, MAX);

		if (ret == -1) {
			printf("\nFailed to read x angle value\n");
			close(ptr->fd_x);
			close(ptr->fd_y);
			close(ptr->fd_z);
			close(ptr->fd_scale);

			return NULL;
		}

		z_angl = atof(buf);

		printf("Z angle level = %lf dps\n", z_angl*scale);
		pthread_mutex_unlock(&thread_mux);
		sleep(10);
	}
}

int main(void)
{
	int fd_x_accel, fd_y_accel, fd_z_accel, fd_accel_scale, fd_x_angl, fd_y_angl, fd_z_angl, fd_angl_scale, ret, choice;
	pthread_t acceleration, angle_level;
	struct thread_data angl_data, accel_data; 

	printf("\nApplication to countinuosly print the accleration and angle "
	       "level, Press Any key to stop the application execution\n");

	fd_x_accel = open("/sys/bus/iio/devices/iio:device1/in_accel_x_raw",
			  O_RDONLY);

	if (fd_x_accel < 0) {
		printf("Failed to open file descriptor of x acceleration file\n");

		return -ENOENT;
	}

	fd_y_accel = open("/sys/bus/iio/devices/iio:device1/in_accel_y_raw",
			  O_RDONLY);

	if (fd_y_accel < 0) {
		printf("Failed to open file descriptor of y acceleration file\n");
		close(fd_x_accel);

		return -ENOENT;
	}

	fd_z_accel = open("/sys/bus/iio/devices/iio:device1/in_accel_z_raw",
			  O_RDONLY);

	if (fd_z_accel < 0) {
		printf("Failed to open file descriptor of z acceleration file\n");
		close(fd_x_accel);
		close(fd_y_accel);

		return -ENOENT;
	}

	fd_accel_scale = open("/sys/bus/iio/devices/iio:device1/in_accel_scale",
			      O_RDONLY);

	if (fd_accel_scale < 0) {
		printf("Failed to open file descriptor of scale acceleration file\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);

		return -ENOENT;
	}

	fd_x_angl = open("/sys/bus/iio/devices/iio:device0/in_anglvel_x_raw", O_RDONLY);

	if (fd_x_angl < 0) {
		printf("Failed to open file descriptor of x angle level file\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);
		close(fd_accel_scale);

		return -ENOENT;
	}

	fd_y_angl = open("/sys/bus/iio/devices/iio:device0/in_anglvel_y_raw", O_RDONLY);

	if (fd_y_angl < 0) {
		printf("Failed to open file descriptor of y angle level file\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);
		close(fd_accel_scale);
		close(fd_x_angl);

		return -ENOENT;
	}

	fd_z_angl = open("/sys/bus/iio/devices/iio:device0/in_anglvel_z_raw", O_RDONLY);

	if (fd_z_angl < 0) {
		printf("Failed to open file descriptor of z angle level file\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);
		close(fd_accel_scale);
		close(fd_x_angl);
		close(fd_y_angl);

		return -ENOENT;
	}

	fd_angl_scale = open("/sys/bus/iio/devices/iio:device0/in_anglvel_scale", O_RDONLY);

	if (fd_angl_scale < 0) {
		printf("Failed to open file descriptor of z angle level file\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);
		close(fd_accel_scale);
		close(fd_x_angl);
		close(fd_y_angl);
		close(fd_z_angl);

		return -ENOENT;
	}

	pthread_mutex_init(&thread_mux, NULL);

	accel_data.fd_x = fd_x_accel;
	accel_data.fd_y = fd_y_accel;
	accel_data.fd_z = fd_z_accel;
	accel_data.fd_scale = fd_accel_scale;
	accel_data.thread_stop = false;
	angl_data.fd_x = fd_x_angl;
	angl_data.fd_y = fd_y_angl;
	angl_data.fd_z = fd_z_angl;
	angl_data.fd_scale = fd_angl_scale;
	angl_data.thread_stop = false;

	ret = pthread_create(&acceleration, NULL, accel_thread, &accel_data);

	if (ret < 0) {
		printf("Failed to create acceleration thread\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);
		close(fd_accel_scale);
		close(fd_x_angl);
		close(fd_y_angl);
		close(fd_z_angl);
		close(fd_angl_scale);

		return ret;
	}

	ret = pthread_create(&angle_level, NULL, angle_thread, &angl_data);

	if (ret < 0) {
		printf("Failed to create angle thread\n");
		close(fd_x_accel);
		close(fd_y_accel);
		close(fd_z_accel);
		close(fd_accel_scale);
		close(fd_x_angl);
		close(fd_y_angl);
		close(fd_z_angl);
		close(fd_angl_scale);

		return ret;
	}

	scanf("%d", &choice);

	switch (choice) {
		default:
			accel_data.thread_stop = true;
			angl_data.thread_stop = true;
			pthread_join(acceleration, NULL);
			pthread_join(angle_level, NULL);
			close(fd_x_accel);
			close(fd_y_accel);
			close(fd_z_accel);
			close(fd_accel_scale);
			close(fd_x_angl);
			close(fd_y_angl);
			close(fd_z_angl);
			close(fd_angl_scale);
	}

	printf("\nExit from application\n");

	return 0;
}
