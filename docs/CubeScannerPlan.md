# Cube Scanner Build Plan

Planning only. A handheld device: show it the six faces, it reads them with a
camera, runs the existing bitwise solver on a microcontroller, and walks you
through the moves on a small screen. No motors. You turn the cube.

Solver: beginner's method with 2-look last layer (`ll-human-method`).
State 48 bytes, algorithm data under 400 bytes, solutions about 100 moves.

## Pipeline

| Stage | What | Status |
|---|---|---|
| 1 capture | One face at a time, aligned to an on-screen 3x3 grid. Fixed exposure, LED ring. | new |
| 2 classify | Average a small patch per cell, HSV, nearest of the six centre colours. | new, the risky part |
| 3 validate | 9 of each colour, 12 real edges, 8 real corners, parity. Else "rescan face X". | new |
| 4 solve | Pack into `uint64_t faces[6]`, run `Solve_Cube`. Stages stream out via `on_stage`. | done |
| 5 guide | Arrow on a face diagram, Next / Back. Optional rescan at stage boundaries. | new |

## Board choice

The solver fits anywhere. The camera decides the board: a parallel DVP sensor
needs about 12 GPIOs and a peripheral that clocks frames in without the CPU,
and a frame buffer needs RAM the smallest parts don't have.

| Option | Camera | Frame RAM | Fit | Approx. |
|---|---|---|---|---|
| **ESP32-S3 cam board** (Freenove ESP32-S3 CAM, XIAO ESP32S3 Sense, ESP32-S3-EYE) — recommended | OV2640 on board, driver in ESP-IDF | 8 MB PSRAM | Full C++ stdlib, solver compiles as-is. Dual core: solve on one, UI on the other. Wi-Fi for debugging frames. | $12–25 |
| ESP32-CAM (AI-Thinker) | OV2640, fisheye | 4 MB PSRAM | Cheapest, almost no spare GPIO for display and buttons. Needs USB-serial to flash. | $6–9 |
| STM32H7 disco (H747I, H7B3) | DCMI, module separate | SRAM + SDRAM | Works, but camera/DMA/display plumbing is days of setup. Only if STM32 itself is the goal. | $60–110 |
| Pico 2 + Arducam | PIO bit-bangs DVP | 520 KB | QVGA greyscale fine, colour is cramped. Less mature drivers. | $15–30 |
| Pi Zero 2 W + camera | CSI, Linux, OpenCV | 512 MB | Easiest classifier, but a Linux computer. Fallback if ESP32 classification is the wall. | $25–40 |

Around the board:

- Display: 1.3" or 1.54" ST7789 SPI TFT, 240x240. Face diagram, arrow, move counter.
- Input: 2–3 tactile buttons on GPIO with pull-ups: Next, Back, Scan.
- Lighting: ring of 6–8 neutral-white LEDs (5000 K+) around the lens, or a WS2812 ring. Not optional.
- Jig: printed cradle holding the cube 60–80 mm from the lens, square to it. Removes perspective, focus and alignment as variables.
- Power: USB 5 V, budget 500 mA peak. 1000 mAh LiPo + charger for handheld.

Connections, ESP32-S3 route:

```
OV2640 ── on-board DVP (8 data, PCLK, VSYNC, HREF, XCLK) + SCCB (I2C)   already wired
ST7789 ── SPI: MOSI, SCLK, CS, DC, RST, BL                                 6 GPIO
Buttons ─ 3 × GPIO, input pull-up, active low                              3 GPIO
LED ring ─ 1 × GPIO → transistor → 5 V LEDs   (or 1 × GPIO data, WS2812)   1 GPIO
Power ── USB-C 5 V → board LDO 3.3 V; LEDs from the 5 V rail
```

Check the board's pin map for pins consumed by the camera and PSRAM before
committing to a display wiring. On the XIAO Sense that is every free pin.

## Reading the colours

- Calibrate from the centres. Each face's centre sticker is the reference for
  that colour. Classify the other 48 by nearest reference in HSV, never by
  fixed hue thresholds.
- Lock the camera. Auto white balance, exposure and gain off after a one-time
  settle, or the references drift between faces.
- Sample a patch, not a pixel. Average ~12x12 at each cell centre, skipping the
  outer 30% where the black plastic and highlights live.
- Red vs orange, white vs yellow are the confusions. Weight saturation and
  value in the distance, use neutral-white light.
- Validate before solving. A wrong sticker never gives a wrong solution, it
  gives no solution. Check counts, then that all 12 edges and 8 corners are
  real pieces. Report the face to rescan, not "no solution".

### Scan protocol

The solver has a fixed orientation and no cube rotations: white up, side faces
in the order `Cube.h` defines. The device says which face to show and which way
to tip: U (white up), F (tip toward you), D, B (tip again), then L and R.

Each tip changes how the 3x3 grid maps onto the packed 8-sticker layout in
`Cube.h`. That per-face rotation is the single most likely off-by-one in the
build. Test it on the desktop with a solved cube and a known scramble before
the hardware exists.

## Pitfalls

- **Solve time on the MCU.** Desktop averages 1.2 s. ESP32-S3 is 20–40x slower
  per node, so a straight port is 30 s to 3 min. Before porting: cap per-piece
  search at depth 6, go index-based end to end (moves as bytes, algorithm
  tables as byte arrays, no string parsing per node), measure by node count.
- **Watchdog resets.** ESP-IDF's task watchdog fires if a task starves idle for
  a few seconds. Run the solver in its own task on the second core.
- **Standard library on bare metal.** Solver uses `std::string`, `std::vector`,
  `std::function`, `stringstream`. Fine on ESP-IDF; on a minimal-libc STM32 it
  is heap you don't want. The index-based DFS already exists.
- **Orientation drift.** 100 moves is a lot to execute without rotating the
  cube. Show a permanent "hold it like this" diagram, offer rescan at the 7
  stage boundaries. Full OLL/PLL would cut ~40 moves for ~2 KB.
- **Lighting and reflections.** Glossy stickers mirror the room. Jig plus LED
  ring makes the scene the same every time; recess the lens, matte interior.
- **Frame memory.** 240x240 RGB565 is 115 KB. Capture QVGA RGB565 into PSRAM;
  only the 9 patch averages leave the buffer.

## Phases

1. **Proof of concept on the Pi 4 + Camera Module 2** (hardware in hand).
   Python with picamera2: 3x3 overlay, six-face protocol, camera locked after
   settle, HSV nearest-centre classifier, validation, 54-sticker state piped to
   a small CLI wrapper around `CubeCore` built natively on the Pi. Moves print
   over SSH or HDMI. v2 module is fixed focus ~50 cm; soft at 15 cm is fine for
   patch averages. Before writing code, shoot the six faces as stills under the
   lamp you'll use and check red against orange by eye.
   Done when: a real scramble is scanned, solved, and the printed moves solve
   the physical cube, ten times out of ten.
2. **Solver port for the small target.** Index-based move path throughout,
   depth cap 6, string-free API, node-count benchmark on the 200-scramble set.
   Cross-compile for ESP32-S3, run the fixed test scrambles on a bare board.
   Done when: 70 tests pass on the board, worst fixed scramble under 5 s, no
   watchdog reset.
3. **Camera bring-up.** Board, display, buttons, LED ring on a breadboard.
   Stream frames with the grid overlay, lock exposure/AWB, dump patch averages
   over serial and compare with the desktop classifier.
   Done when: on-device patch colours match the desktop decisions on all six
   faces under the ring light.
4. **Scan flow on device.** Six-face prompts, per-face confirm, validation with
   "rescan face X", solve on the second core, display the cross moves while the
   corners are still being searched.
   Done when: a scrambled cube goes from box to solved with no laptop attached.
5. **Jig and enclosure.** Printed cradle, recessed lens, matte interior, LED
   ring, battery.
   Done when: 20 consecutive scans of different scrambles with no rescan prompts.

## What to buy, after phase 1 passes

| Part | Note | Approx. |
|---|---|---|
| Freenove ESP32-S3 WROOM CAM | OV2640, 8 MB PSRAM, spare GPIO, USB-C | $20 |
| 1.54" ST7789 240x240 SPI TFT | CS/DC/RST broken out | $6 |
| 8-LED WS2812 ring | one data pin, set to neutral white | $4 |
| 3 tactile buttons, breadboard, jumpers | | $5 |
| Optional 1000 mAh LiPo + TP4056 | handheld, phase 5 | $8 |

Rough 2026 hobby-supplier prices, to size the project rather than budget it.
