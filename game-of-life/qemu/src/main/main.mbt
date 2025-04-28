///|
fn cast[T](x : T) -> FixedArray[Byte] = "%identity"

///|
extern "C" fn esp_timer_get_time() -> Int64 = "esp_timer_get_time"

///|
pub fn moonbit_main() -> Unit {
  let panel = @qemu_lcd.esp_lcd_new_rgb_qemu(@game.COLS, @game.ROWS, BPP_16)
  let _ = @lcd.esp_lcd_panel_reset(panel)
  let _ = @lcd.esp_lcd_panel_init(panel)
  @game.init_universe()
  for i = 0; ; i = i + 1 {
    let start = esp_timer_get_time()
    let _ = @lcd.esp_lcd_panel_draw_bitmap(
      panel,
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
