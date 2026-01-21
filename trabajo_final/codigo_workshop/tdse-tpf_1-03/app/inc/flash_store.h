#ifndef FLASH_STORE_H
#define FLASH_STORE_H

#include "stm32f1xx_hal.h" //library for hal functions

#define FLASH_BASE_ADDRESS      0x08000000	// the initial flash address. First page address:  0X0800 0000 - 0x0800 03FF
#define FLASH_PAGE_SIZE         1024  		// the size of one page in STM32F103RB board (1 KB)( obtained from user manual - medium density devices)
#define FLASH_TOTAL_PAGES       128   		// the amount of pages in STM32F103RB board (from user manual) (from 0 to 127)

//typedef
typedef float flash_data_t; // the data type to store in flash memory


#define PAGE_MAX_CODE_SIZE 64

#endif // FLASH_STORE_H
