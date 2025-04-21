// apps.h
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BTN_UP,
    BTN_DOWN,
    BTN_SEL
} button_t;

typedef struct {
    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
    void (*input)(button_t btn);
} app_descriptor_t;
