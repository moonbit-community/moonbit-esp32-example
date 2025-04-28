#include "esp_lcd_qemu_rgb.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_types.h"
#include "esp_random.h"
#include "esp_timer.h"

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
  esp_lcd_rgb_qemu_config_t config = {
      .width = COLS, .height = ROWS, .bpp = RGB_QEMU_BPP_16};

  esp_lcd_panel_handle_t panel;
  esp_lcd_new_rgb_qemu(&config, &panel);
  esp_lcd_panel_reset(panel);
  esp_lcd_panel_init(panel);
  init_universe();

  for (int i = 0;; i++) {
    int64_t t_start = esp_timer_get_time();

    esp_lcd_panel_draw_bitmap(panel, 0, 0, COLS, ROWS, &cells[0][0]);

    tick();

    int64_t t_end = esp_timer_get_time();
    printf("tick %d took %lld µs\n", i, t_end - t_start);
  }
}