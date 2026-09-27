#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac.h" // Для использования ЦАП
#include "esp_log.h"

static const char *TAG = "MELODY_PLAYER";

// Определения нот (частоты в Гц)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define REST     0

// Определение мелодии (простая гамма)
typedef struct {
    int freq;
    int duration_ms;
} note_t;

const note_t melody[] = {
    {NOTE_C4, 300}, {NOTE_D4, 300}, {NOTE_E4, 300}, {NOTE_F4, 300},
    {NOTE_G4, 300}, {NOTE_A4, 300}, {NOTE_B4, 300}, {NOTE_C5, 300},
    {NOTE_C5, 300}, {NOTE_B4, 300}, {NOTE_A4, 300}, {NOTE_G4, 300},
    {NOTE_F4, 300}, {NOTE_E4, 300}, {NOTE_D4, 300}, {NOTE_C4, 300},
    {REST, 500} // Пауза в конце
};

void app_main(void)
{
    ESP_LOGI(TAG, "Начало воспроизведения мелодии...");

    // Инициализация ЦАП на канале 1 (GPIO 25)
    // В ESP-IDF для простой генерации сигнала достаточно включить ЦАП.
    // Частота будет меняться программно.
    dac_output_enable(DAC_CHANNEL_1);

    size_t melody_length = sizeof(melody) / sizeof(melody[0]);

    while (1) {
        for (int i = 0; i < melody_length; i++) {
            if (melody[i].freq == REST) {
                // Пауза: отключаем сигнал (или устанавливаем 0)
                dac_output_voltage(DAC_CHANNEL_1, 0); // Устанавливаем 0V
                vTaskDelay(pdMS_TO_TICKS(melody[i].duration_ms));
            } else {
                // Генерация тона: быстро переключаем напряжение на ЦАП
                // для создания прямоугольной волны нужной частоты.
                int period_us = 1000000 / melody[i].freq;
                int half_period_us = period_us / 2;
                int cycles = (melody[i].duration_ms * 1000) / period_us;

                for (int c = 0; c < cycles; c++) {
                    dac_output_voltage(DAC_CHANNEL_1, 255); // Высокий уровень (3.3V)
                    esp_rom_delay_us(half_period_us);
                    dac_output_voltage(DAC_CHANNEL_1, 0);   // Низкий уровень (0V)
                    esp_rom_delay_us(half_period_us);
                }
            }
            // Короткая пауза между нотами для четкости
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        ESP_LOGI(TAG, "Мелодия завершена. Пауза 2 секунды.");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}