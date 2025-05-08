///|
fn timer_callback2() -> Unit {
  println("call timer callback 2")
}

///|
pub fn moonbit_main() -> Unit {
  let a = 123
  let timer1 = @timer.esp_timer_create(fn() {
    println("call timer1 callback, data \{a}")
  })
  @timer.esp_timer_start_once(timer1, 500_000) |> ignore
  //
  let timer2 = @timer.esp_timer_create(fn() { timer_callback2() })
  @timer.esp_timer_start_periodic(timer2, 300_000) |> ignore
  let period = @ref.new(0UL)
  @timer.esp_timer_get_period?(timer2, period) |> ignore
  println("Timer period: \{period}")
  //
  for i = 0; ; i = i + 1 {
    println("Iteration: \{i}")
    @task.vTaskDelay(@task.pdMS_TO_TICKS(100))
    if i == 7 {
      @timer.esp_timer_stop?(timer2) |> ignore
      println("Timer stopped")
      break
    }
  }
}

///|
fn main {
  moonbit_main()
}
