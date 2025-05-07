///|
fn timer_callback() -> Unit {
  println("call timer callback")
}

///|
pub fn moonbit_main() -> Unit {
  let timer_handle = @timer.esp_timer_create(fn() { timer_callback() })
  let _ = @timer.esp_timer_start_once(timer_handle, 3_000_000)
  for i = 0; ; i = i + 1 {
    println("Iteration: \{i}")
    @task.vTaskDelay(@task.pdMS_TO_TICKS(1000))
  }
}

///|
fn main {
  moonbit_main()
}
