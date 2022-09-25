from typing import List
from connection import Connection
from part import Part
from pin import Pin


class Drawer:
    def __init__(self, pin_height: int, width_pad: int):
        self.pin_height = pin_height
        self.width_pad = width_pad

    def draw(self, parts: List[Part], cons: List[Connection]):
        x = 0 
        y = 0
        return

    def _get_part_size(self, part: Part):
        left_pins = list(part.left_.values())
        right_pins = list(part.right_.values())
        up_pins = list(part.up_.values())
        down_pins = list(part.down_.values())

        max_pin_width = (
            max(self._get_pin_width(left_pins), self._get_pin_width(right_pins))
            + self.width_pad
        )

        width = max(
            len(part.name) + self.width_pad,
            len(down_pins) * self.pin_height,
            len(up_pins) * self.pin_height,
        ) + (max_pin_width / 2)

        height = max(
            len(right_pins) * self.pin_height, len(left_pins) * self.pin_height
        )
        return (width, height)

    def _get_pin_width(self, pins: List[Pin]):
        ret = -1
        for pin in pins:
            if len(pin.name) > ret:
                ret = len(pin.name)
        return ret
