/*
 * Simple IMU Reader Application
 *
 * - Reads accelerometer (X, Y, Z) and angle values from LSM6DSV16X sensor
 * - User can choose what data to read from menu
 * - Prints values on console
 * - User can exit the application at any time
 *
 * This is a generic Linux I2C user-space application.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 15

int main(void)
{
	int fd_x_accel, fd_y_accel, fd_z_accel, fd_accel_scale, fd_x_angl, fd_y_angl, fd_z_angl, fd_angl_scale, choice, ret;
	char buf[MAX];
	double scale, x_accel, y_accel, z_accel, x_angl, y_angl, z_angl, angl_scale;

	printf("Accelerometer application\n\n");

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

	ret = read(fd_accel_scale, buf, MAX);

	if (ret == -1) {
		printf("\nFailed to read acceleration scale value\n");
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
	scale = atof(buf);

	printf("\nScale = %lf\n", scale);

	ret = read(fd_angl_scale, buf, MAX);

	if (ret == -1) {
		printf("\nFailed to read angle scale value\n");
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
	angl_scale = atof(buf);

	printf("\nScale = %lf\n", angl_scale);

	while (1) {
		printf("--------------------------------------\n");
		printf("1 --> X acceleration\n");
		printf("2 --> Y acceleration\n");
		printf("3 --> Z acceleration\n");
		printf("4 --> X angle level\n");
		printf("5 --> Y angle level\n");
		printf("6 --> Z angle level\n");
		printf("7 --> Exit\n");
		printf("--------------------------------------\n");
		ret = scanf("%d", &choice);

		if (ret <= 0) {
			printf("invalid option\n");
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

		switch (choice) {
		case 1:
			lseek(fd_x_accel, 0, SEEK_SET);
			ret = read(fd_x_accel, buf, MAX);

			if (ret == -1) {
				printf("\nFailed to read x accleration value\n");
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

			x_accel = atof(buf);

			printf("\nX acceleration = %lf m/s^2\n", x_accel*scale);
			printf("\nret = %d\n", ret);
			break;
		case 2:
			lseek(fd_y_accel, 0, SEEK_SET);
			ret = read(fd_y_accel, buf, MAX);

			if (ret == -1) {
				printf("\nFailed to read y accleration value\n");
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

			y_accel = atof(buf);

			printf("\nY acceleration = %lf m/s^2\n", y_accel*scale);
			printf("\nret = %d\n", ret);
			break;
		case 3:
			lseek(fd_z_accel, 0, SEEK_SET);
			ret = read(fd_z_accel, buf, MAX);

			if (ret == -1) {
				printf("\nFailed to read z accleration value\n");
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

			z_accel = atof(buf);

			printf("\nZ acceleration = %lf m/s^2\n", z_accel*scale);
			printf("\nret = %d\n", ret);
			break;
		case 4:
			lseek(fd_x_angl, 0, SEEK_SET);
			ret = read(fd_x_angl, buf, MAX);

			if (ret == -1) {
				printf("\nFailed to read x angle level value\n");
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

			x_angl = atof(buf);

			printf("\nX angle level = %lf dps\n", x_angl*angl_scale);
			printf("\nret = %d\n", ret);
			break;
		case 5:
			lseek(fd_y_angl, 0, SEEK_SET);
			ret = read(fd_y_angl, buf, MAX);

			if (ret == -1) {
				printf("\nFailed to read y angle level value\n");
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

			y_angl = atof(buf);

			printf("\nY angle level = %lf dps\n", y_angl*angl_scale);
			printf("\nret = %d\n", ret);
			break;
		case 6:
			lseek(fd_z_angl, 0, SEEK_SET);
			ret = read(fd_z_angl, buf, MAX);

			if (ret == -1) {
				printf("\nFailed to read z angle level value\n");
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

			z_angl = atof(buf);

			printf("\nZ angle level = %lf dps\n", z_angl*angl_scale);
			printf("\nret = %d\n", ret);
			break;
		case 7:
			close(fd_x_accel);
			close(fd_y_accel);
			close(fd_z_accel);
			close(fd_accel_scale);
			close(fd_x_angl);
			close(fd_y_angl);
			close(fd_z_angl);
			close(fd_angl_scale);
			return 0;
		default:
			printf("\nInvalid choice\n");
		}
	}
}
