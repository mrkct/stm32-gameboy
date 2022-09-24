from typing import List, Optional, Tuple


class Connection:
    from_: int
    to: int

    def __init__(self, from_: int, to: int):
        self.from_ = from_
        self.to = to


def make_connections(
    ls: List[Tuple[Optional[int], Optional[int]]]
) -> Optional[List[Connection]]:
    ret = []
    for f, t in ls:
        if f is None:
            return None
        if t is None:
            return None
        ret.append(Connection(f, t))
    return ret
