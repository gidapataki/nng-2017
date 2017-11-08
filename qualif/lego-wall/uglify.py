from __future__ import print_function
import sys
import re
import argparse


def pattern(word):
    return r'\b{}\b'.format(word)


def replacement(prefix, index):
    return '{}{}'.format(prefix, index)


def print_words(lines):
    words = set()
    for line in lines:
        for word in re.findall(r'\w+', line):
            words.add(word)
    for w in sorted(words):
        print(w)


def pattern(word):
    return r'\b{}\b'.format(word)


def shorten_words(fname, lines):
    with open(fname) as f:
        words = f.readlines()
    words = set(map(lambda s: s.strip(), words))

    i = 0
    repl = {}
    for word in words:
        repl[pattern(word)] = 'v{}'.format(i)
        i += 1

    for line in lines:
        for p, s in repl.items():
            line = re.sub(p, s, line)
        sys.stdout.write(line)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-w', '--words', action='store_true',
                        help='print words')
    parser.add_argument('-s', '--shorten', metavar='FILE',
                        help='word list file')
    args = parser.parse_args()

    lines = []
    for line in sys.stdin:
        lines.append(line)

    if args.words:
        print_words(lines)

    if args.shorten:
        shorten_words(args.shorten, lines)
