#pragma once

//### test of conditional compiling
#ifndef ENABLED
    #define ENABLED 1
#endif
#define MISSION_RELATIVE ENABLED
//#define MISSION_RELATIVE 0
// ###

#define HAL_BOARD_NAME "SITL"
#define HAL_CPU_CLASS HAL_CPU_CLASS_1000
#define HAL_MEM_CLASS HAL_MEM_CLASS_1000
#define HAL_OS_POSIX_IO 1
#define HAL_OS_SOCKETS 1

#define AP_FLASHSTORAGE_TYPE 3

#if defined(STORAGE_USE_FLASH) && STORAGE_USE_FLASH==1

#if AP_FLASHSTORAGE_TYPE == 1
// emulate F1/F3 flash
#define HAL_STORAGE_SIZE 15360
#define HAL_FLASH_SECTOR_SIZE 16*1024
#define HAL_FLASH_MIN_WRITE_SIZE 1
#define HAL_FLASH_ALLOW_UPDATE 0

#elif AP_FLASHSTORAGE_TYPE == 2
// emulate F4/F7 flash
#define HAL_STORAGE_SIZE 15360
#define HAL_FLASH_SECTOR_SIZE 16*1024
#define HAL_FLASH_MIN_WRITE_SIZE 1
#define HAL_FLASH_ALLOW_UPDATE 1

#elif AP_FLASHSTORAGE_TYPE == 3
// emulate H7 flash
#define HAL_STORAGE_SIZE 16384
#define HAL_FLASH_SECTOR_SIZE 128*1024
#define HAL_FLASH_MIN_WRITE_SIZE 32
#define HAL_FLASH_ALLOW_UPDATE 0
#endif
#endif

#ifndef HAL_STORAGE_SIZE
#define HAL_STORAGE_SIZE            16384
#endif

#define HAL_STORAGE_SIZE_AVAILABLE  HAL_STORAGE_SIZE
#define HAL_BOARD_LOG_DIRECTORY "logs"
#define HAL_BOARD_TERRAIN_DIRECTORY "terrain"
#define HAL_PARAM_DEFAULTS_PATH nullptr
#define HAL_INS_DEFAULT HAL_INS_HIL
#define HAL_BARO_DEFAULT HAL_BARO_HIL
#define HAL_COMPASS_DEFAULT HAL_COMPASS_HIL
#define HAL_GPIO_A_LED_PIN        61
#define HAL_GPIO_B_LED_PIN        48
#define HAL_GPIO_C_LED_PIN        117
#define HAL_GPIO_LED_ON           0
#define HAL_GPIO_LED_OFF          1

#define HAL_HAVE_BOARD_VOLTAGE 1
#define HAL_HAVE_SERVO_VOLTAGE 1
#define HAL_HAVE_SAFETY_SWITCH 0

// allow for static semaphores
#include <AP_HAL_SITL/Semaphores.h>
#define HAL_Semaphore HALSITL::Semaphore

#include <AP_HAL/EventHandle.h>
#define HAL_EventHandle AP_HAL::EventHandle

#ifndef HAL_NUM_CAN_IFACES
#define HAL_NUM_CAN_IFACES 0
#endif

#ifndef HAL_BOARD_STORAGE_DIRECTORY
#define HAL_BOARD_STORAGE_DIRECTORY "."
#endif

