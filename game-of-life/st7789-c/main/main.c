#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "ST7789_EXAMPLE";

#define LCD_HOST SPI2_HOST

#define PIN_NUM_SCLK 10
#define PIN_NUM_MOSI 6
#define PIN_NUM_LCD_DC 3
#define PIN_NUM_LCD_RST 2

#define LCD_SPI_CLOCK_HZ SPI_MASTER_FREQ_80M

#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

#define ROWS 240
#define COLS 240
const int16_t DEAD = ((int16_t)0);
const int16_t ALIVE = ((int16_t)-1);

static int16_t cells[ROWS][COLS];
static int16_t next_buf[3][COLS];

static void init_universe(void) {
  for (int row = 0; row < ROWS; ++row) {
    for (int col = 0; col < COLS; ++col) {
      bool chosen = (esp_random() % 100) < 37;
      cells[row][col] = chosen ? ALIVE : DEAD;
    }
  }
}

static int16_t live_neighbor_count(int row, int col) {
  int16_t count = 0;

  int r0 = (row - 1 + ROWS) % ROWS;
  int r1 = row;
  int r2 = (row + 1) % ROWS;
  int c0 = (col - 1 + COLS) % COLS;
  int c1 = col;
  int c2 = (col + 1) % COLS;

  count += cells[r0][c0];
  count += cells[r0][c1];
  count += cells[r0][c2];
  count += cells[r1][c0];
  count += cells[r1][c2];
  count += cells[r2][c0];
  count += cells[r2][c1];
  count += cells[r2][c2];

  return -count;
}

static void tick(void) {
  for (int row = 0; row < ROWS; row++) {
    int16_t *buf_row = next_buf[row % 3];
    for (int col = 0; col < COLS; col++) {
      int16_t cell = cells[row][col];
      int16_t live_neighbors = live_neighbor_count(row, col);
      int16_t next_cell;

      if (cell == ALIVE && live_neighbors < 2) {
        next_cell = DEAD;
      } else if (cell == ALIVE &&
                 (live_neighbors == 2 || live_neighbors == 3)) {
        next_cell = ALIVE;
      } else if (cell == ALIVE && live_neighbors > 3) {
        next_cell = DEAD;
      } else if (cell == DEAD && live_neighbors == 3) {
        next_cell = ALIVE;
      } else {
        next_cell = cell;
      }

      buf_row[col] = next_cell;
    }

    if (row >= 2) {
      int commit_row = row - 2;
      int16_t *commit_buffer_base = next_buf[commit_row % 3];
      for (int col = 0; col < COLS; col++) {
        cells[commit_row][col] = commit_buffer_base[col];
      }
    }
  }

  for (int commit_row = ROWS - 2; commit_row < ROWS; commit_row++) {
    int16_t *commit_buf_base = next_buf[commit_row % 3];
    for (int col = 0; col < COLS; ++col) {
      cells[commit_row][col] = commit_buf_base[col];
    }
  }
}

void app_main(void) {
  spi_bus_config_t buscfg = {
      .sclk_io_num = PIN_NUM_SCLK,
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = -1,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0,
      .flags = 0,
  };

  ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
  vTaskDelay(10 / portTICK_PERIOD_MS);

  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_spi_config_t io_config = {
      .dc_gpio_num = PIN_NUM_LCD_DC,
      .cs_gpio_num = -1,
      .pclk_hz = LCD_SPI_CLOCK_HZ,
      .lcd_cmd_bits = LCD_CMD_BITS,
      .lcd_param_bits = LCD_PARAM_BITS,
      .spi_mode = 3,
      .trans_queue_depth = 10,
  };

  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
                                           &io_config, &io_handle));
  printf("io_handle: %p\n", io_handle);

  esp_lcd_panel_handle_t panel_handle = NULL;
  esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = PIN_NUM_LCD_RST,
      .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
      .bits_per_pixel = 16,
  };

  ESP_ERROR_CHECK(
      esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_sleep(panel_handle, false));

  init_universe();

  for (int i = 0;; i++) {
    int64_t t_start = esp_timer_get_time();

    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, COLS, ROWS, (void *)cells);

    tick();

    int64_t t_end = esp_timer_get_time();
    printf("tick %d took %lld µs\n", i, t_end - t_start);
  }
}