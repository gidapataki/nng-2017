from __future__ import print_function


def digits(p, k):
    xs = str(p)[-k:] + str(p**2)[-k:]
    vs = [0] * 10
    for x in xs:
        vs[int(x)] += 1
    return vs


def match(vs, ds):
    for (i, j) in zip(ds, vs):
        if i > j:
            return False
    return True


def match_exact(vs, ds):
    if ds[0] > vs[0]:
        return False
    for (i, j) in zip(ds[1:], vs[1:]):
        if i != j:
            return False
    return True


def sieve(vs, ps, k):
    ns = []
    for i in range(10):
        for p in ps:
            px = p + (i * 10**(k - 1))
            if match(vs, digits(px, k)):
                ns.append(px)
    return ns


def find_best(vs, ps):
    for p in reversed(sorted(ps)):
        if match_exact(vs, digits(p, 0)):
            return p


def solve(digits):
    vs = [0] * 10
    for d in digits:
        vs[d] += 1
    x, k = [0], 1
    for i in range(len(digits) // 3):
        x, k = sieve(vs, x, k), k + 1
    print(find_best(vs, x))


if __name__ == '__main__':
    solve([0, 4, 2])
    solve([0, 0, 1, 0, 0, 1])
    solve([0, 6, 0, 9, 7, 8, 6, 3, 0, 6, 8, 7])
    solve([
        2, 4, 2, 4, 0, 3, 3, 2, 1, 0, 2, 3,
        1, 2, 4, 5, 4, 3, 1, 4, 4, 3, 5, 1])
