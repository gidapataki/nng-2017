from __future__ import print_function
import random
import argparse


def generate(k, x=0):
    lo = 10**(k-1) if k > 1 else 0
    hi = 10**k
    n = random.randint(lo, hi - 1)
    n2 = n * n
    w = (k + x) * 3
    return '{}{}'.format(n, n2).zfill(w)


def make_testcase(t, k, x):
    assert(k + x > 0 and k + x < 10)
    print(t)
    for i in range(t):
        s = generate(k, x)
        print(len(s))
        print(' '.join([c for c in s]))


if __name__ == '__main__':
    # Run like:
    # $ python gen.py 5 3 -p 0 | tee >(cat >&2) | squares
    parser = argparse.ArgumentParser()
    parser.add_argument('tests', type=int, help='number of test cases')
    parser.add_argument('k', type=int,
                        help='generate numbers between 10^(k-1) .. 10^k')
    parser.add_argument('-p', type=int, metavar='N', default=0,
                        help='extra padding')
    args = parser.parse_args()

    make_testcase(args.tests, args.k, args.p)
