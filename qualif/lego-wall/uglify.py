from __future__ import print_function
import sys
import re
import argparse


def pattern(word):
    return r'\b{}\b'.format(word)


def replacement(prefix, index):
    return '{}{}'.format(prefix, index)


def get_words(lines):
    words = set()
    for line in lines:
        for word in re.findall(r'\w+', line):
            words.add(word)
    return words


def print_words(lines):
    words = get_words(lines)
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


def is_protected(word):
    if len(word) < 3:
        return True

    cc = ord(word[0])
    if cc >= ord('0') and cc <= ord('9'):
        return True

    return False


def protect_words(fname, lines):
    with open(fname) as f:
        pwords = f.readlines()

    pwords = set(map(lambda s: s.strip(), pwords))
    words = get_words(lines)

    i = 0
    repl = {}
    for word in words:
        if word in pwords or is_protected(word):
            continue
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
                        help='shorten word list file')
    parser.add_argument('-p', '--protect', metavar='FILE',
                        help='protected words list file')
    args = parser.parse_args()

    lines = []
    for line in sys.stdin:
        lines.append(line)

    if args.words:
        print_words(lines)

    if args.shorten:
        shorten_words(args.shorten, lines)

    if args.protect:
        protect_words(args.protect, lines)
