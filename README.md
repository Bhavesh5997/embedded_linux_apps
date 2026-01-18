Embedded Linux Sensor Applications
=================================

This repository contains generic Linux user-space applications for
I2C-based sensors using standard sysfs interfaces.

Directory Structure
-------------------

htu21d/
  - menu_app/      : Menu based temperature & humidity reader  
  - simple_read/   : Simple one-shot read example  

imu_lsm6dsv16x/
  - imu_menu.c       : Menu based IMU reader  
  - imu_continuous.c : Thread based continuous reader  

HTU21D Applications
-------------------

1. htu21d_menu.c  
   - Read temperature and humidity via sysfs  
   - Multithreaded logging support  
   - User configurable logging interval  
   - Automatic log file creation  

2. htu21d_simple.c  
   - Simple read of temperature and humidity  
   - Prints values on console  

IMU Applications (LSM6DSV16X)
-----------------------------

1. imu_menu.c  
   - Menu driven program  
   - Read acceleration (X, Y, Z)  
   - Read angle values  
   - User can choose what to read  
   - User can exit anytime by pressing any key  

2. imu_continuous.c  
   - Continuous reading every 10 seconds  
   - Two threads: read + print  
   - Mutex used to avoid print mixing  
   - Exit anytime by pressing any key  

Requirements
------------

- Linux system with I2C support  
- Sensor driver enabled in kernel  
- sysfs entries exposed by the driver  
- pthread library available  

Build (Native)
--------------

gcc imu_menu.c -o imu_menu -lpthread  
gcc imu_continuous.c -o imu_continuous -lpthread  

gcc htu21d_menu.c -o htu21d_menu -lpthread  
gcc htu21d_simple.c -o htu21d_simple  

Cross Compile Example
---------------------

<cross-compiler>-gcc imu_menu.c -o imu_menu -lpthread  

Deploy to Target
----------------

scp imu_menu root@<target-ip>:/home/root/  
scp imu_continuous root@<target-ip>:/home/root/  

Run on Target
-------------

./imu_menu  
./imu_continuous  

Yocto Integration (Optional)
----------------------------

- Applications can be added as a custom Yocto recipe  
- Binaries can be included in rootfs  
- Ensure corresponding kernel driver is enabled  

Note
----

These applications are generic user-space examples and rely on
standard sysfs interfaces provided by the Linux sensor drivers.
