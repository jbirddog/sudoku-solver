def top_left(s):
    return ((s / 27) * 27) + (((s % 9) / 3) * 3)

def hor_left(s):
    return (s / 9) * 9

def ver_top(s):
    return s % 9

def unit_peers(s):
    tl = top_left(s)

    return [tl, tl + 1, tl + 2,
            tl + 9, tl + 10, tl + 11,
            tl + 18, tl + 19, tl + 20]

def hor_peers(s):
    hl = hor_left(s)

    return [hl, hl + 1, hl + 2,
            hl + 3, hl + 4, hl + 5,
            hl + 6, hl + 7, hl + 8]

def ver_peers(s):
    vt = ver_top(s)

    return [vt, vt + 9, vt + 18,
            vt + 27, vt + 36, vt + 45,
            vt + 54, vt + 63, vt + 72]

def peers(s):
    peers = set(unit_peers(s) + hor_peers(s) + ver_peers(s))
    peers.remove(s)

    return sorted(peers)


if __name__ == '__main__':
    for i in range(0, 81):
        print("{ " + ", ".join(str(p).rjust(2, ' ') for p in peers(i)) + " }, ")
