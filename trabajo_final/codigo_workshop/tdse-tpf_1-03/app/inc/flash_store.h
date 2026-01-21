#ifndef FLASH_STORE_H
#define FLASH_STORE_H

#include <stdint.h>

// Definimos la estructura AQUÍ para que sea visible por quien incluya la librería
typedef struct {
    uint32_t magic_number; // Para validar si hay datos
    uint32_t id_dispositivo;
    float umbral_temperatura;
    uint8_t opciones_flag;
    // ... más variables ...
} __attribute__((aligned(4))) AppConfig_t;

// Variable global externa (para que main.c pueda leerla directametne)
extern AppConfig_t SystemConfig;

// Funciones PÚBLICAS (API)
void FlashStore_Init(void);   // Carga datos al arrancar
void FlashStore_Save(void);   // Guarda los datos actuales de RAM a Flash

#endif // FLASH_STORE_H
