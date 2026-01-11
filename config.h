#ifndef CONFIG_H
#define CONFIG_H

// I2C Configuration
#define I2C_SDA 22
#define I2C_SCL 23
#define I2C_FREQ 100000

// Sensor Configuration
#define SENSOR_ADDRESS 0x29  // VL53L1X default I2C address
#define MEASUREMENT_INTERVAL 900000  // 15 minutes in ms
#define SAMPLES_PER_MEASUREMENT 5
#define MAX_DISTANCE_MM 1000  // Maximum sensor range

// Container Configuration
#define CONTAINER_HEIGHT_MM 100

// Data Storage
#define MAX_DATA_POINTS 100  // Circular buffer size

// Web Server
#define WEB_SERVER_PORT 80
#define MDNS_HOSTNAME "dough"

// Serial Debug
#define SERIAL_BAUD 115200

// Sensor timeout
#define SENSOR_TIMEOUT_MS 2000

#endif
