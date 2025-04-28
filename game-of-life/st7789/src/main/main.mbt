///|
fn cast[T](x : T) -> FixedArray[Byte] = "%identity"

///|
const PIN_SCLK : Int = 10

///|
const PIN_MOSI : Int = 6

///|
const PIN_DC : Int = 3

///|
const PIN_RST : Int = 2

///|
const LCD_SPI_CLOCK_HZ : UInt = 80U * 1000 * 1000

///|
extern "C" fn esp_timer_get_time() -> Int64 = "esp_timer_get_time"

///|
pub fn moonbit_main() -> Unit {
  let spi_config : @spi.SPI_BUS_CONFIG = {
    sclk_io_num: PIN_SCLK,
    mosi_io_num: PIN_MOSI,
    miso_io_num: -1,
    quadwp_io_num: -1,
    quadhd_io_num: -1,
    max_transfer_sz: 0,
    flags: 0,
  }
  let _ = @spi.spi_bus_initialize(@spi.SPI2_HOST, spi_config, SPI_DMA_CH_AUTO)
  @task.vTaskDelay(@task.pdMS_TO_TICKS(1))
  let io_handle : Ref[@lcd.ESP_LCD_PANEL_IO_HANDLE?] = @ref.new(None)
  let io_config : @lcd.ESP_LCD_PANEL_IO_SPI_CONFIG = {
    dc_gpio_num: PIN_DC,
    cs_gpio_num: -1,
    pclk_hz: LCD_SPI_CLOCK_HZ,
    lcd_cmd_bits: 8,
    lcd_param_bits: 8,
    spi_mode: 3,
    trans_queue_depth: 10,
  }
  let _ = @lcd.esp_lcd_new_panel_io_spi(@spi.SPI2_HOST, io_config, io_handle)
  @task.vTaskDelay(@task.pdMS_TO_TICKS(1))
  let panel_handle = @lcd.esp_lcd_new_panel_st7789(
    io_handle~,
    reset_gpio_num=PIN_RST,
    rgb_ele_order=@lcd.BGR,
    bits_per_pixel=16,
  )
  let _ = @lcd.esp_lcd_panel_reset(panel_handle)
  let _ = @lcd.esp_lcd_panel_init(panel_handle)
  let _ = @lcd.esp_lcd_panel_disp_on_off(panel_handle, true)
  let _ = @lcd.esp_lcd_panel_disp_sleep(panel_handle, false)
  let _ = @lcd.esp_lcd_panel_invert_color(panel_handle, true)
  @game.init_universe()
  for i = 0; ; i = i + 1 {
    let start = esp_timer_get_time()
    let _ = @lcd.esp_lcd_panel_draw_bitmap(
      panel_handle,
      0,
      0,
      @game.ROWS,
      @game.COLS,
      cast(@game.get_cells()),
    )
    @game.tick()
    let end = esp_timer_get_time()
    println("tick \{i} took \{end - start} us")
  }
}

///|
fn main {
  moonbit_main()
}
