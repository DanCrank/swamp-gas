# SwampGas 1.0
# based on "CircuitPython demo - NeoPixel" by AdaFruit

# NOTE: current breadboard prototype is wired with the
# dip switch LSB on the left (position 1). probably
# want to reverse that on the real one.

import time
import board
import neopixel
from digitalio import DigitalInOut, Direction

print("SwampGas 1.0 startup")
# multiplexer A (LSB) to trinket A0 (pin labeled 1!) as digital out
mux_0 = DigitalInOut(board.A0)
mux_0.direction = Direction.OUTPUT
# multiplexer B       to trinket A1 (pin labeled 2!) as digital out
mux_1 = DigitalInOut(board.A1)
mux_1.direction = Direction.OUTPUT
# multiplexer C (MSB) to trinket A2 (pin labeled 0!) as digital out
mux_2 = DigitalInOut(board.A2)
mux_2.direction = Direction.OUTPUT
# multiplexer out     to trinket A3 (pin labeled 3) as digital in
mux_out = DigitalInOut(board.A3)
mux_out.direction = Direction.INPUT
mux_out.pull = None  # circuit has pull-ups on the input side of the mux
# neopixel data-in    to trinket A4 (pin labeled 4) as bare pin
# (neopixel driver knows what to do with it)
pixel_pin = board.A4
# using 60 NeoPixels on 1m strip
num_pixels = 60
# current draw is roughly proportional to brightness but also
# depends a great deal on the lighting pattern.
neopixel_brightness = 1.0


pixels = neopixel.NeoPixel(pixel_pin,
                           num_pixels,
                           brightness=neopixel_brightness,
                           auto_write=False)


# never apologize, never explain
rainbow_magic = 0
rainbow_speed = 16


def taste_the_rainbow():
    global rainbow_magic
    rainbow_magic = (rainbow_magic + rainbow_speed) % 256


# swampgas color constants (indexed to dip switch selections)
WHITE = 0
RED = 1
BLUE = 2
GREEN = 3
YELLOW = 4
CYAN = 5
PURPLE = 6
# 7-14 TBD; will return black
RAINBOW = 15


# color mapping
def color(select):
    if select == WHITE: return (255, 255, 255)
    elif select == RED: return (255, 0, 0)
    elif select == BLUE: return (0, 0, 255)
    elif select == GREEN: return (0, 255, 0)
    elif select == YELLOW: return (255, 150, 0)
    elif select == CYAN: return (0, 255, 255)
    elif select == PURPLE: return (180, 0, 255)
    elif select == RAINBOW: return wheel(rainbow_magic)
    else: return (0, 0, 0)


def wheel(pos):
    # Input a value 0 to 255 to get a color value.
    # The colours are a transition r - g - b - back to r.
    if pos < 0 or pos > 255:
        return (0, 0, 0)
    if pos < 85:
        return (255 - pos * 3, pos * 3, 0)
    if pos < 170:
        pos -= 85
        return (0, 255 - pos * 3, pos * 3)
    pos -= 170
    return (pos * 3, 0, 255 - pos * 3)


def fade(color, frac):
    if frac == 0:
        return (0, 0, 0)
    return(int(color[0] * frac * frac),
           int(color[1] * frac * frac),
           int(color[2] * frac * frac))


def bounce_right(pos, pixels_per_side):
    if pos >= pixels_per_side:
        return pixels_per_side - (pos - pixels_per_side) - 2
    return pos


def bounce_left(pos):
    if pos < 0:
        return pos * -1
    return pos


def clockwise_sweep(color, width, wait):
    for i in range(num_pixels):
        for j in range(width + 1):
            pixels[(i + j) % num_pixels] = fade(color, (j / width))
        pixels.show()
        if wait > 0: time.sleep(wait)


def bounce(color, width, sides, wait):
    pixels_per_side = int(num_pixels / sides)
    for i in range(pixels_per_side):
        # sweeping left to right
        for j in range(width + 1):
            for k in range(0, num_pixels, pixels_per_side):
                pixels[bounce_right(i + j, pixels_per_side) + k] = \
                    fade(color, (j / width))
        pixels.show()
        if wait > 0: time.sleep(wait)
    for i in range(pixels_per_side - 2, 1, -1):
        # sweeping right to left
        for j in range(width + 1):
            for k in range(0, num_pixels, pixels_per_side):
                pixels[bounce_left(i - j) + k] = fade(color, (j / width))
        pixels.show()
        if wait > 0: time.sleep(wait)


def read_dipswitch_pin(pin):
    if pin < 0 or pin > 7:
        raise IndexError
    # address the mux
    mux_2.value = pin >> 2 & 1
    mux_1.value = pin >> 1 & 1
    mux_0.value = pin & 1
    # settle
    # time.sleep(0.02)
    # read the mux output (NOTE: active LOW)
    return not mux_out.value


def read_dipswitch():
    dip_value = 0
    for i in range(8):
        dip_value += read_dipswitch_pin(i) << i
    return dip_value


# main loop
# TODO: turn off built-in neopixel
while True:
    pixels.fill((0, 0, 0))
    pixels.show()
    # which color and pattern?
    select = read_dipswitch()
    select_color = select & 0x0F
    select_pattern = select >> 4
    if select_pattern == 0:
        # 0 = clockwise sweep fast
        clockwise_sweep(color(select_color), 12, 0)
    elif select_pattern == 1:
        # 1 = clockwise sweep slow
        clockwise_sweep(color(select_color), 12, 0.05)
    elif select_pattern == 2:
        # 2 = 3-way bounce
        bounce(color(select_color), 3, 3, 0)
    elif select_pattern == 3:
        # 3 = 6-way bounce
        bounce(color(select_color), 3, 6, 0)
    # elif select_pattern == 4:
        # 4 = 
    # elif select_pattern == 5:
        # 5 = 
    # elif select_pattern == 6:
        # 6 = 
    # elif select_pattern == 7:
        # 7 = 
    # elif select_pattern == 8:
        # 8 = 
    # elif select_pattern == 9:
        # 9 = 
    # elif select_pattern == 10:
        # 10 = 
    # elif select_pattern == 11:
        # 11 = 
    # elif select_pattern == 12:
        # 12 = 
    # elif select_pattern == 13:
        # 13 = 
    # elif select_pattern == 14:
        # 14 = 
    # elif select_pattern == 15:
        # 15 = 
    else:
        # undefined
        time.sleep(1)
    taste_the_rainbow()
