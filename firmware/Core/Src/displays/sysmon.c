#include "displays/sysmon.h"
#include "main.h"
#include "ux/display.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "displays/data.h"
#include "displays/launcher.h"

extern uint32_t end_of_heap;
extern uint32_t start_of_stack;
extern uint32_t __HeapBase;
extern uint32_t __StackTop;
extern app_data_t g_app_data;

#define UPDATE_MS 500
#define CPU_RING_RADIUS 40
#define CPU_RING_THICKNESS 6
#define ACCENT COLOR_BLUE
#define GRAY COLOR_WHITE

static uint32_t lastCycles, lastMs;
static uint8_t  cpuLoadPc;
static uint32_t usedRam, totalRam;
static uint16_t coreTempC;
static uint32_t vbatmV;
static uint32_t sysclkMHz;

//extern ADC_HandleTypeDef hadc1;

// forward
static uint16_t read_core_temp(void);
static uint32_t read_vbat_mv(void);

void sysmon_init(void) {
    // cycle counter for CPU load
    CoreDebug->DEMCR   |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT        = 0;
    DWT->CTRL         |= DWT_CTRL_CYCCNTENA_Msk;
    lastCycles = DWT->CYCCNT;
    lastMs     = HAL_GetTick();

    // ADC1 clocks & TS/VBAT enable
    __HAL_RCC_ADC_CLK_ENABLE();
    ADC123_COMMON->CCR |= ADC_CCR_TSEN | ADC_CCR_VBATEN;
    //HAL_ADC_Init(&hadc1);
    //MX_ADC1_Init();

    // sysclk & RAM size
    sysclkMHz = HAL_RCC_GetSysClockFreq() / 1000000UL;
    totalRam = (uint32_t)&__StackTop  - (uint32_t)&__HeapBase;
}

void sysmon_update(void) {
    uint32_t now = HAL_GetTick();
    uint32_t dm  = now - lastMs;
    if (dm < UPDATE_MS) return;

    //CPU
    uint32_t cc = DWT->CYCCNT;
    uint32_t dC = cc - lastCycles;
    uint32_t exp = dm * sysclkMHz * 1000UL;
    cpuLoadPc = (uint8_t)((dC * 100UL + exp/2) / exp);
    lastCycles = cc;
    lastMs = now;
    uint32_t sp = __get_MSP();
    uint32_t freeRam = sp - (uint32_t)&__HeapBase;
    usedRam = totalRam - freeRam;
    coreTempC = 6;//read_core_temp();
    vbatmV    = 2000;//read_vbat_mv();
}

static void draw_cpu_ring(void) {
    draw_arc(0, 359,
             CENTER_X, CENTER_Y-20,
             CPU_RING_RADIUS,
             GRAY,    // color
             false,   // not filled
             3);

    uint16_t span = cpuLoadPc > 100
                    ? 270
                    : (uint16_t)(cpuLoadPc * 270UL / 100UL);

    const uint16_t start = 135;
    uint16_t end = start + span;
    if (end <= 359) {
        draw_arc(start, end,
                 CENTER_X, CENTER_Y-20,
                 CPU_RING_RADIUS,
                 ACCENT, false, 3);
    } else {
        draw_arc(start, 359,
                 CENTER_X, CENTER_Y-20,
                 CPU_RING_RADIUS,
                 ACCENT, false, 3);
        draw_arc(0, end - 360,
                 CENTER_X, CENTER_Y-20,
                 CPU_RING_RADIUS,
                 ACCENT, false, 3);
    }

    char buf[6];
    int len = snprintf(buf, sizeof(buf), "%u%%", cpuLoadPc);
    if (len<0 || len>=sizeof(buf)) strcpy(buf, ":");
    draw_text(buf, CENTER_X, CENTER_Y - 26, &montserrat_reg, COLOR_WHITE, 0.6f, true);
    draw_text("CPU",     CENTER_X, CENTER_Y -6, &montserrat_reg, COLOR_WHITE, 0.6f, true);

}

void sysmon_draw(void) {

    char hdr[24];
    snprintf(hdr, sizeof(hdr), "STM32L4 %lu MHz", sysclkMHz);
    draw_text(hdr, CENTER_X, 40, &montserrat_reg, COLOR_WHITE, 0.7, true);

    draw_cpu_ring();

    char buf[32];
    snprintf(buf, sizeof(buf), "RAM %lu/%lu B", usedRam, totalRam);
    draw_text(buf, CENTER_X, 165, &montserrat_reg, COLOR_WHITE, 0.55, true);

    snprintf(buf, sizeof(buf), "Tmp %u C", coreTempC);
    draw_text(buf, CENTER_X, 185, &montserrat_reg, COLOR_WHITE, 0.55, true);

    snprintf(buf, sizeof(buf), "VBAT %lu mV", vbatmV);
    draw_text(buf, CENTER_X, 205, &montserrat_reg, COLOR_WHITE, 0.55, true);

}

static uint16_t read_core_temp(void) {
    // ADC_ChannelConfTypeDef s = {0};
    // s.Channel = ADC_CHANNEL_TEMPSENSOR;
    // s.Rank = ADC_REGULAR_RANK_1;
    // s.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    //
    // // reconfigure channel
    // HAL_ADC_ConfigChannel(&hadc1, &s);
    //
    // // start + wait
    // HAL_ADC_Start(&hadc1);
    // if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
    //     return 0xFFFF;  // error
    // }
    // uint32_t raw = HAL_ADC_GetValue(&hadc1);
    // HAL_ADC_Stop(&hadc1);

    // convert: raw*(Vref/4096)
    float vsense = 4; //raw * 3.0f / 4096.0f;
    // sensor curve: 30C 0.76V, slope 2.5mV/C
    return (uint16_t)((vsense - 0.76f)/0.0025f + 30.0f + 0.5f);
}

// VBAT via ADC1 channel VBAT
static uint32_t read_vbat_mv(void) {
    // ADC_ChannelConfTypeDef sConfig = {0};
    // sConfig.Channel      = ADC_CHANNEL_VBAT;
    // sConfig.Rank         = ADC_REGULAR_RANK_1;
    // sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    // HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    // HAL_ADC_Start(&hadc1);
    // HAL_ADC_PollForConversion(&hadc1, 10);
    // uint32_t raw = HAL_ADC_GetValue(&hadc1);
    // HAL_ADC_Stop(&hadc1);

    // board divider 2:1
    return (50 * 3000UL * 2) / 4096UL;
}

void sysmon_input(button_t btn) {
    // any button press returns to launcher
    if (btn == BTN_UP || btn == BTN_DOWN || btn == BTN_SEL) {
        g_app_data.display.active_screen = SCREEN_LAUNCHER;
        launcher_init();
    }
}

const app_t sysmon_app = {
    .init = sysmon_init,
    .update = sysmon_update,
    .draw = sysmon_draw,
    .input = sysmon_input
};

