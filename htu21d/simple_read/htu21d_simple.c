/*
 * Simple Linux userspace application to read temperature and humidity from
 * HTU21D sensor via sysfs.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX	50
#define COUNT	5
#define DIVESER 1000

int main(void)
{
	int fd_temperature, fd_humidity, ret;
	char data[MAX];
	double temperature_value, humidity_value;

	fd_temperature = open("/sys/bus/i2c/devices/0-0040/iio:device0/in_temp_input",
			      O_RDONLY);

	if (fd_temperature < 0) {
		printf("Failed to get temperature file descriptor\n");
		return -EAGAIN;
	}

	fd_humidity = open("/sys/bus/i2c/devices/0-0040/iio:device0/"
			   "in_humidityrelative_input", O_RDONLY);

	if (fd_humidity < 0) {
		printf("Failed to get humidity file descriptor\n");
		close(fd_temperature);
		return -EAGAIN;
	}

	ret = read(fd_temperature, data, COUNT);

	if (ret == -1) {
		printf("Failed to read humidity data\n");
		close(fd_temperature);
		close(fd_humidity);

		return ret;
	}

	temperature_value = atof(data) / DIVESER;

	printf("\nTemperature: %lf celsius\n", temperature_value);

	ret = read(fd_humidity, data, COUNT);

	if (ret == -1) {
		printf("Failed to read humidity data\n");
		close(fd_temperature);
		close(fd_humidity);

		return ret;
	}

	humidity_value = atof(data) / DIVESER;

	printf("\nHumidity: %lf RH\n", humidity_value);
	close(fd_temperature);
	close(fd_humidity);

	return 0;
}
