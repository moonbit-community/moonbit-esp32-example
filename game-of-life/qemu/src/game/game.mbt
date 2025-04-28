///|
const DEAD : Int16 = 0

///|
const ALIVE : Int16 = -1

///|
pub const ROWS : Int = 240

///|
pub const COLS : Int = 240

///|
let cells : FixedArray[Int16] = FixedArray::make(ROWS * COLS, 0)

///|
let next : FixedArray[Int16] = FixedArray::make(3 * COLS, 0)

///|
extern "C" fn esp_random() -> UInt = "esp_random"

///|
pub fn init_universe() -> Unit {
  for row = 0; row < ROWS; row = row + 1 {
    for col = 0; col < COLS; col = col + 1 {
      let i = row * COLS + col
      let s = (esp_random() % 100 < 37).to_int()
      cells[i] = if s == 0 { DEAD } else { ALIVE }
    }
  }
}

///|
pub fn get_cells() -> FixedArray[Int16] {
  cells
}

///|
fn get_index(row : Int, col : Int) -> Int {
  row * COLS + col
}

///|
fn live_neighbor_count(row : Int, col : Int) -> Int16 {
  let mut count : Int16 = 0
  let neighbor_row_1 = (row - 1 + ROWS) % ROWS
  let neighbor_row_2 = row
  let neighbor_row_3 = (row + 1) % ROWS
  let neighbor_col_1 = (col - 1 + COLS) % COLS
  let neighbor_col_2 = col
  let neighbor_col_3 = (col + 1) % COLS
  count += cells.unsafe_get(get_index(neighbor_row_1, neighbor_col_1))
  count += cells.unsafe_get(get_index(neighbor_row_1, neighbor_col_2))
  count += cells.unsafe_get(get_index(neighbor_row_1, neighbor_col_3))
  count += cells.unsafe_get(get_index(neighbor_row_2, neighbor_col_1))
  count += cells.unsafe_get(get_index(neighbor_row_2, neighbor_col_3))
  count += cells.unsafe_get(get_index(neighbor_row_3, neighbor_col_1))
  count += cells.unsafe_get(get_index(neighbor_row_3, neighbor_col_2))
  count += cells.unsafe_get(get_index(neighbor_row_3, neighbor_col_3))
  let zero : Int16 = 0
  return zero - count
}

///|
fn unsafe_set(arr : FixedArray[Int16], idx : Int, value : Int16) -> Unit = "%fixedarray.unsafe_set"

///|
pub fn tick() -> Unit {
  for row = 0; row < ROWS; row = row + 1 {
    let buf_row_start = row % 3 * COLS
    for col = 0; col < COLS; col = col + 1 {
      let cell = cells.unsafe_get(get_index(row, col))
      let live_neighbors = live_neighbor_count(row, col)
      let next_cell = match (cell, live_neighbors) {
        (ALIVE, _..<2) => DEAD
        (ALIVE, 2 | 3) => ALIVE
        (ALIVE, 3..<_) => DEAD
        (DEAD, 3) => ALIVE
        (otherwise, _) => otherwise
      }
      unsafe_set(next, buf_row_start + col, next_cell)
    }
    if row >= 2 {
      let commit_row = row - 2
      let commit_buf_base = commit_row % 3 * COLS
      for col = 0; col < COLS; col = col + 1 {
        let dst_idx = get_index(commit_row, col)
        unsafe_set(cells, dst_idx, next.unsafe_get(commit_buf_base + col))
      }
    }
  }
  for commit_row = ROWS - 2; commit_row < ROWS; commit_row = commit_row + 1 {
    let commit_buf_base = commit_row % 3 * COLS
    for col = 0; col < COLS; col = col + 1 {
      let dst_idx = get_index(commit_row, col)
      unsafe_set(cells, dst_idx, next.unsafe_get(commit_buf_base + col))
    }
  }
}
