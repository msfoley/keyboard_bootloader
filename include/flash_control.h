#ifndef FLASH_CONTROL
#define FLASH_CONTROL

int flash_control_init();
int flash_control_program_page(uint32_t address, uint8_t *page);
int flash_control_erase_page(uint32_t address);

#endif
