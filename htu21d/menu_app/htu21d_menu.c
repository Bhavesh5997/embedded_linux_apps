/*
 * Menu-driven environmental monitoring application for HTU21D I2C sensor on
 * Linux.
 *
 * Features:
 * - Read temperature and humidity via sysfs
 * - Multithreaded data logging
 * - User-configurable logging interval
 * - Automatic log file creation
 */

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX	50
#define COUNT	5
#define DIVESER 1000

pthread_mutex_t mutex;
pthread_mutex_t mutex_temp_interval;
pthread_mutex_t mutex_hum_interval;
pthread_mutex_t mutex_temp_fptr;
pthread_mutex_t mutex_hum_fptr;

struct thread_data {
	FILE *fptr;
	int fd;
	int interval;
} temperature, humidity;

void *temp_thread_fun(void *arg)
{
	int ret, interval = 0;
	struct thread_data *temp_data= (struct thread_data *)arg;
	double temperature;
	char temp_str[MAX], interval_str[MAX], data[MAX];

	while (temp_data->fptr) {
		ret = read(temp_data->fd, data, COUNT);

		if (ret == -1) {
			printf("Failed to read temperature data\n");
			close(temp_data->fd);
			return NULL;
		}

		temperature = atof(data) / DIVESER;

		sprintf(interval_str, "%d", interval);
		sprintf(temp_str, "%lf", temperature);
		pthread_mutex_lock(&mutex_temp_fptr);
		pthread_mutex_lock(&mutex);
		fputs("[", temp_data->fptr);
		fputs(interval_str, temp_data->fptr);
		fputs("] Temperature: ", temp_data->fptr);
		fputs(temp_str, temp_data->fptr);
		fputs(" celsius\n", temp_data->fptr);
		fflush(temp_data->fptr);
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex_temp_fptr);

		pthread_mutex_lock(&mutex_temp_interval);
		sleep(temp_data->interval);
		interval += temp_data->interval;
		pthread_mutex_unlock(&mutex_temp_interval);

		lseek(temp_data->fd, 0, SEEK_SET);
	}

	printf("Exit from temperature thread\n");
}

void *humidity_thread_fun(void *arg)
{
	int ret, interval = 0;
	struct thread_data *hum_data= (struct thread_data *)arg;
	double humidity;
	char hum_str[MAX], interval_str[MAX], data[MAX];

	while (hum_data->fptr) {
		ret = read(hum_data->fd, data, COUNT);

		if (ret == -1) {
			printf("Failed to read humidity data\n");
			close(hum_data->fd);
			return NULL;
		}

		humidity = atof(data) / DIVESER;

		sprintf(hum_str, "%lf", humidity);
		sprintf(interval_str, "%d", interval);

		pthread_mutex_lock(&mutex_hum_fptr);
		pthread_mutex_lock(&mutex);
		fputs("[", hum_data->fptr);
		fputs(interval_str, hum_data->fptr);
		fputs("] Humidity: ", hum_data->fptr);
		fputs(hum_str, hum_data->fptr);
		fflush(hum_data->fptr);
		fputs(" RH\n", hum_data->fptr);
		fflush(hum_data->fptr);
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex_hum_fptr);

		pthread_mutex_lock(&mutex_hum_interval);
		sleep(hum_data->interval);
		interval += hum_data->interval;
		pthread_mutex_unlock(&mutex_hum_interval);

		lseek(hum_data->fd, 0, SEEK_SET);
	}

	printf("Exit from humidity thread\n");
}

int main(void)
{
	int fd_temperature, fd_humidity, ret, choice, data_choice, interval_choice, interval, file_choice;
	char file_name[MAX], data[MAX];
	double temperature_value, humidity_value;
	FILE *fptr = NULL;
	pthread_t temp_thread, humidity_thread;

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

	temperature.fd = fd_temperature;
	temperature.interval = 1;
	humidity.fd = fd_humidity;
	humidity.interval = 1;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_temp_interval, NULL);
	pthread_mutex_init(&mutex_hum_interval, NULL);
	pthread_mutex_init(&mutex_temp_fptr, NULL);
	pthread_mutex_init(&mutex_hum_fptr, NULL);

	printf("\nApplication for the read temperature and humidity\n");

	printf("Enter file name where the the application data save\n");
	ret = scanf("%s", file_name);

	if (ret <= 0) {
		printf("Invalid name\n");
		close(fd_temperature);
		close(fd_humidity);

		return ret;
	}

	fptr = fopen(file_name, "w");
	temperature.fptr = fptr;
	humidity.fptr = fptr;

	ret = pthread_create(&temp_thread, NULL, temp_thread_fun, &temperature);

	if (ret < 0) {
		printf("Failed to create temperature thread\n");
		close(fd_temperature);
		close(fd_humidity);
		fclose(fptr);

		return ret;
	}

	ret = pthread_create(&humidity_thread, NULL, humidity_thread_fun, &humidity);

	if (ret < 0) {
		printf("Failed to create temperature thread\n");
		close(fd_temperature);
		close(fd_humidity);
		fclose(fptr);

		return ret;
	}

	while (1) {
		printf("\nEnter your choice\n");
		printf("1 -> Read data\n");
		printf("2 -> Change intervals for readings\n");
		printf("3 -> Enable/disable option to logging data on file\n");
		printf("4 -> Exit from application\n\n");
		ret = scanf("%d", &choice);

		if (ret <= 0) {
			printf("Invalid option\n");

			pthread_mutex_lock(&mutex_temp_fptr);
			temperature.fptr = NULL;
			pthread_mutex_unlock(&mutex_temp_fptr);

			pthread_mutex_lock(&mutex_hum_fptr);
			humidity.fptr = NULL;
			pthread_mutex_unlock(&mutex_hum_fptr);

			pthread_join(temp_thread, NULL);
			pthread_join(humidity_thread, NULL);

			close(fd_temperature);
			close(fd_humidity);

			if (fptr)
				fclose(fptr);

			return ret;
		}

		switch (choice) {
		case 1:
			printf("\n1 -> For temperature\n");
			printf("2 -> For humidity\n");
			ret = scanf("%d", &data_choice);

			if (ret <= 0) {
				printf("\nInvalid option\n");

				pthread_mutex_lock(&mutex_temp_fptr);
				temperature.fptr = NULL;
				pthread_mutex_unlock(&mutex_temp_fptr);

				pthread_mutex_lock(&mutex_hum_fptr);
				humidity.fptr = NULL;
				pthread_mutex_unlock(&mutex_hum_fptr);

				pthread_join(temp_thread, NULL);
				pthread_join(humidity_thread, NULL);

				close(fd_temperature);
				close(fd_humidity);

				if (fptr)
					fclose(fptr);

				return ret;
			}

			switch (data_choice) {
			case 1:
				lseek(fd_temperature, 0, SEEK_SET);
				ret = read(fd_temperature, data, COUNT);

				if (ret == -1) {
					printf("Failed to read humidity data\n");

					pthread_mutex_lock(&mutex_temp_fptr);
					temperature.fptr = NULL;
					pthread_mutex_unlock(&mutex_temp_fptr);

					pthread_mutex_lock(&mutex_hum_fptr);
					humidity.fptr = NULL;
					pthread_mutex_unlock(&mutex_hum_fptr);

					pthread_join(temp_thread, NULL);
					pthread_join(humidity_thread, NULL);

					close(fd_temperature);
					close(fd_humidity);

					if (fptr)
						fclose(fptr);

					return ret;
				}

				temperature_value = atof(data) / DIVESER;

				printf("\nTemperature: %lf celsius\n", temperature_value);
				break;
			case 2:
				lseek(fd_humidity, 0, SEEK_SET);
				ret = read(fd_humidity, data, COUNT);

				if (ret == -1) {
					printf("Failed to read humidity data\n");

					pthread_mutex_lock(&mutex_temp_fptr);
					temperature.fptr = NULL;
					pthread_mutex_unlock(&mutex_temp_fptr);

					pthread_mutex_lock(&mutex_hum_fptr);
					humidity.fptr = NULL;
					pthread_mutex_unlock(&mutex_hum_fptr);

					pthread_join(temp_thread, NULL);
					pthread_join(humidity_thread, NULL);

					close(fd_temperature);
					close(fd_humidity);

					if (fptr)
						fclose(fptr);

					return ret;
				}

				humidity_value = atof(data) / DIVESER;

				printf("\nHumidity: %lf RH\n", humidity_value);
				break;
			default:
				printf("\nInvalid option\n");
			}
			break;
		case 2:
			printf("\n1 -> Change temperature interval\n");
			printf("2 -> Change humidity interval\n");
			ret = scanf("%d", &interval_choice);

			if (ret <= 0) {
				printf("\nInvalid option\n");

				pthread_mutex_lock(&mutex_temp_fptr);
				temperature.fptr = NULL;
				pthread_mutex_unlock(&mutex_temp_fptr);

				pthread_mutex_lock(&mutex_hum_fptr);
				humidity.fptr = NULL;
				pthread_mutex_unlock(&mutex_hum_fptr);

				pthread_join(temp_thread, NULL);
				pthread_join(humidity_thread, NULL);

				close(fd_temperature);
				close(fd_humidity);

				if (fptr)
					fclose(fptr);

				return ret;
			}

			switch(interval_choice) {
			case 1:
				if (!fptr) {
					printf("\nFirst enable the write data "
					       "on file\n");
				} else {
					printf("\nEnter new interval value\n");
					ret = scanf("%d", &interval);

					if (ret <= 0) {
						printf("\nInvalid value\n");

						pthread_mutex_lock(&mutex_temp_fptr);
						temperature.fptr = NULL;
						pthread_mutex_unlock(&mutex_temp_fptr);

						pthread_mutex_lock(&mutex_hum_fptr);
						humidity.fptr = NULL;
						pthread_mutex_unlock(&mutex_hum_fptr);

						pthread_join(temp_thread, NULL);
						pthread_join(humidity_thread, NULL);

						close(fd_temperature);
						close(fd_humidity);
						fclose(fptr);

						return ret;
					}

					pthread_mutex_lock(&mutex_temp_interval);
					temperature.interval = interval;
					pthread_mutex_unlock(&mutex_temp_interval);
				}
				break;
			case 2:
				if (!fptr) {
					printf("\nFirst enable the write data "
					       "on file\n");
				} else {
					printf("\nEnter new interval value\n");
					ret = scanf("%d", &interval);

					if (ret <= 0) {
						printf("\nInvalid value\n");

						pthread_mutex_lock(&mutex_temp_fptr);
						temperature.fptr = NULL;
						pthread_mutex_unlock(&mutex_temp_fptr);

						pthread_mutex_lock(&mutex_hum_fptr);
						humidity.fptr = NULL;
						pthread_mutex_unlock(&mutex_hum_fptr);

						pthread_join(temp_thread, NULL);
						pthread_join(humidity_thread, NULL);

						close(fd_temperature);
						close(fd_humidity);
						fclose(fptr);

						return ret;
					}

					pthread_mutex_lock(&mutex_hum_interval);
					humidity.interval = interval;
					pthread_mutex_unlock(&mutex_hum_interval);
				}
				break;
			default:
				printf("\nInvalid choice\n");
			}
			break;
		case 3:
			printf("\n1 -> Enable option for write data on file\n");
			printf("2 -> Disable to write data on file\n");
			ret = scanf("%d", &file_choice);

			if (ret <= 0) {
				printf("\nInvalid option\n");

				pthread_mutex_lock(&mutex_temp_fptr);
				temperature.fptr = NULL;
				pthread_mutex_unlock(&mutex_temp_fptr);

				pthread_mutex_lock(&mutex_hum_fptr);
				humidity.fptr = NULL;
				pthread_mutex_unlock(&mutex_hum_fptr);

				pthread_join(temp_thread, NULL);
				pthread_join(humidity_thread, NULL);

				close(fd_temperature);
				close(fd_humidity);

				if (fptr)
					fclose(fptr);

				return ret;
			}

			switch (file_choice) {
			case 1:
				if (fptr) {
					printf("\nIt's already enabled\n");
				} else {
					printf("\nEnter file name\n");
					ret = scanf("%s", file_name);

					if (ret <= 0) {
						printf("Invalid name\n");
						close(fd_temperature);
						close(fd_humidity);

						return ret;
					}

					fptr = fopen(file_name, "w");

					pthread_mutex_lock(&mutex_temp_fptr);
					temperature.fptr = fptr;
					pthread_mutex_unlock(&mutex_temp_fptr);

					pthread_mutex_lock(&mutex_hum_fptr);
					humidity.fptr = fptr;
					pthread_mutex_unlock(&mutex_hum_fptr);

					lseek(fd_temperature, 0, SEEK_SET);
					lseek(fd_humidity, 0, SEEK_SET);
					ret = pthread_create(&temp_thread, NULL, temp_thread_fun, &temperature);

					if (ret < 0) {
						printf("Failed to create temperature thread\n");
						close(fd_temperature);
						close(fd_humidity);
						fclose(fptr);

						return ret;
					}

					ret = pthread_create(&humidity_thread, NULL, humidity_thread_fun, &humidity);

					if (ret < 0) {
						printf("Failed to create temperature thread\n");
						close(fd_temperature);
						close(fd_humidity);
						fclose(fptr);

						return ret;
					}
				}
				break;
			case 2:
				if (!fptr) {
					printf("\nIt's already disabled\n");
				} else {
					pthread_mutex_lock(&mutex_temp_fptr);
					temperature.fptr = NULL;
					pthread_mutex_unlock(&mutex_temp_fptr);

					pthread_mutex_lock(&mutex_hum_fptr);
					humidity.fptr = NULL;
					pthread_mutex_unlock(&mutex_hum_fptr);

					pthread_join(temp_thread, NULL);
					pthread_join(humidity_thread, NULL);
					fclose(fptr);
					fptr = NULL;
				}
				break;
			default:
				printf("\nInvalid option\n");
			}
			break;
		case 4:
			pthread_mutex_lock(&mutex_temp_fptr);
			temperature.fptr = NULL;
			pthread_mutex_unlock(&mutex_temp_fptr);

			pthread_mutex_lock(&mutex_hum_fptr);
			humidity.fptr = NULL;
			pthread_mutex_unlock(&mutex_hum_fptr);

			pthread_join(temp_thread, NULL);
			pthread_join(humidity_thread, NULL);

			close(fd_temperature);
			close(fd_humidity);

			if (fptr)
				fclose(fptr);

			return 0;
		default:
			printf("\nInvalid option\n");
		}
	}

	return 0;
}
