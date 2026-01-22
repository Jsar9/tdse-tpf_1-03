#ifndef FLASH_STORE_H
#define FLASH_STORE_H

#include "stm32f1xx_hal.h" //library for hal functions

#define FLASH_BASE_ADDRESS      0x08000000	// the initial flash address. First page address:  0X0800 0000 - 0x0800 03FF
#define FLASH_PAGE_SIZE         1024  		// the size of one page in STM32F103RB board (1 KB)( obtained from user manual - medium density devices)
#define FLASH_TOTAL_PAGES       128   		// the amount of pages in STM32F103RB board (from user manual) (from 0 to 127)

//typedef
typedef float flash_data_t; // the data type to store in flash memory


#define PAGE_MAX_CODE_SIZE 64



// data store selection
#define SELECTED_PAGE 120
#define FLASH_SLOT_INDEX_LOW_TEMP 0
#define FLASH_SLOT_INDEX_HIGH_TEMP 1
#define FLASH_SLOT_INDEX_CL_TEMP 2




// functions

uint32_t get_address (uint8_t page, uint16_t index);

void flash_erase_page (uint8_t page);

void flash_write (uint8_t page, uint16_t index, void *data);

void flash_read (uint8_t page, uint16_t index, void *data_out);


#endif // FLASH_STORE_H
