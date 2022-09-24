from typing import Optional
from pin import Pin


class Part:
    up: dict[str, int]
    up_: dict[int, Pin]
    down: dict[str, int]
    down_: dict[int, Pin]
    left: dict[str, int]
    left_: dict[int, Pin]
    right: dict[str, int]
    right_: dict[int, Pin]
    name: str

    def __init__(
        self,
        name: str,
        up: list[str] = [],
        down: list[str] = [],
        left: list[str] = [],
        right: list[str] = [],
    ):
        self.up = {}
        self.down = {}
        self.left = {}
        self.right = {}
        self.name = name

        for p in up:
            hash_ = hash(p + "up")
            pin = Pin(p, hash_)
            self.up[p] = hash_
            self.up_[pin.hash_] = pin
        for p in down:
            hash_ = hash(p + "down")
            pin = Pin(p, hash_)
            self.down[p] = hash_
            self.down_[pin.hash_] = pin
        for p in left:
            hash_ = hash(p + "left")
            pin = Pin(p, hash_)
            self.left[p] = hash_
            self.left_[pin.hash_] = pin
        for p in right:
            hash_ = hash(p + "right")
            pin = Pin(p, hash_)
            self.right[p] = hash_
            self.right_[pin.hash_] = pin

    def find_pin(self, hash_: int) -> Optional[Pin]:
        if hash_ in self.up:
            return self.up_[hash_]
        if hash_ in self.down:
            return self.down_[hash_]
        if hash_ in self.left:
            return self.left_[hash_]
        if hash_ in self.right:
            return self.right_[hash_]
        return None
