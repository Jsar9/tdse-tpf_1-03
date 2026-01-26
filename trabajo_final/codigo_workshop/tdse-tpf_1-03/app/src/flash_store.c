// includes
#include "app.h"
#include "flash_store.h"
#include <stdio.h>

/* This function returns the address in flash memory using the page number and the index of the slot
 *
 * About page number data type: is an uint8_t data type because the max number page is 127 and uint8_t allow values from 0 to 255.
 *
 * About index data type: is the index of a byte that belongs to the page. uint16_t is used because it allow to access all addresses in the 1KB page
 * 	if we use an uint8_t data type, we can access to the first 256 bytes (if would like to store a char data type) but each page has 1024 bytes so there are positions that can't be reached.
 *
 * The implementation, allow to think each page as an slots collection. In this case, we're using floats as data type, with 4 bytes size.
 * 	If each page has 1024 bytes, considering each float's size, the result should be 256 slots availables (range index: 0 to 255).
 * 	Considering this slots, when storing the 3 floats used in the configuration temperature, we can use index: 0,1,2. In this way, the
 * 	data will be stored in bytes: 0-3, 4-7, 7-11 without being replaced and avoiding to corrupt the flash memory.
 *
 */

uint32_t get_address (uint8_t page, uint16_t index) {
    return FLASH_BASE_ADDR + (page * FLASH_PAGE_SIZE) + (index * sizeof(flash_data_t) );
}







/*
 *	This function erase an entire selected page (just 1 page)
 *
 *	this unlocks the flash memory and obtains the page address, using the FLASH_PAGE_SIZE and FLASH_BASE_ADDR macros
 *	then, initialize the erase struct, used in the HAL function to erase the selected page.
 *	Finally, it will block the flash memory again.
 */

void flash_erase_page (uint8_t page) {

	// this condition allow to protect the code if we know the size of it
    if (page < PAGE_MAX_CODE_SIZE) return;

    // unlock the flash memory before erasing
    HAL_FLASH_Unlock();


    // initialize the flash erase struct, where the information about which page will be erased is stored
    FLASH_EraseInitTypeDef erase_init_struct;
    uint32_t page_error;

    erase_init_struct.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase_init_struct.PageAddress = FLASH_BASE_ADDR + (page * FLASH_PAGE_SIZE);
    erase_init_struct.NbPages     = 1; // it stores the number of pages that will be deleted

    // erase the selected page using the address that it's stored in the PageAddress field
    HAL_FLASHEx_Erase(&erase_init_struct, &page_error);

    // locks the flash memory again
    HAL_FLASH_Lock();
}



/*
 * Before to use the flash_write function, it's needed to erase the previous data stored in the page.
 *
 * This function consider 4 bytes words, because the STM32F103RB board is a 32bits controller.
 * (the buses moves 4 bytes words)
 *
 * The function obtains the address of the page and the index of the slot where the data will be stored using the get_address() function.
 *
 * then, calculates how many words there are in the given data (assuming 4bytes words)
 *
 * before to continue, it checks if the data_size is a multiple of 4 bytes.
 *
 * then, it unlock the flash memory before the writing
 *
 * finally it starts to write the data word by word, using the p_data pointer to take the data from the RAM memory ( using the parameter: void * data)
 * and stores it in the flash memory using the slot address, forwarding the address in multiples of 4 and the pointer using the incremental ++
 *
 */

void flash_write (uint8_t page, uint16_t index, void *data) {

	size_t data_size =  sizeof(flash_data_t);

	// obtains the address of the block in the indicated page.
    uint32_t addr = get_address(page,index);

    // sets a pointer to the data, using a 4 bytes representation
    uint32_t *p_data = (uint32_t *)data;

    // calculates the number of words (4 bytes) in the data type to be stored
    uint32_t num_words = data_size / 4;



    // this condition allow to use data type that it's not a multiple of 4. That means that will be necessary an extra writing.
    if (data_size % 4 != 0) num_words++;

    // is necessary to unlock the flash memory before writing
    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < num_words; i++) {

    	// writes word by word
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *p_data);

        addr += 4;    // forward 4 bytes in flash memory
        p_data++;    // forward 4 bytes in RAM memory, using the pointer
    }

    // lock the flash memory to avoid any security problem.
    HAL_FLASH_Lock();
}





/*
 * This function receives data_out address there the read data will be stored.
 *
 *
 * It starts calculating the data_size (4 bytes in this case bc it's a float)
 *
 * then it calculates the number of words in the data_size (4 bytes words, that means 1 word in this case) and checks if the data is
 * a multiple of 4
 *
 * then it's declared a volatile uint32_t pointer, this forces the compilator to read the memory flash in each iteration, avoiding
 * 	the problem when the compilator only takes the caché data, without check the memory flash data. This happens because the compilator
 * 	assumes that if inside the while block the data weren't changed, so, the data has the same value, but it doesnt consider the possibility
 * 	where the hardware changes the data.
 *
 *
 */
void flash_read (uint8_t page, uint16_t index, void *data_out) {

    size_t data_size = sizeof(flash_data_t);

    uint32_t num_words = data_size / 4; //calculate the amount of 4 byte words

    if (data_size % 4 != 0) num_words++; //checks if it's a multiple of 4

    uint32_t addr = get_address(page, index); // obtains the address where the data was store

    // sets a pointer to the destiny (void * data_out)
    uint32_t *p_dest = (uint32_t *)data_out;

    //forces to read the memory flash in each call.
    volatile uint32_t *p_flash_address = (volatile uint32_t *)addr;

    // it reads word by word and copy the read_value in the destiny. Then, moves the cursor to the next 4 bytes word
    for (uint32_t i = 0; i < num_words; i++) {

    	// change the direction of the destiny by the data stored in the flash address
    	*p_dest = *p_flash_address;

    	//change the direction to be read
    	p_flash_address++;

    	//change the direction where to store the data
    	p_dest++;
    }
}






