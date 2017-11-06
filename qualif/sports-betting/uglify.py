from __future__ import print_function
import sys
import re

def pattern(word):
    return r'\b{}\b'.format(word)

def replacement(prefix, index):
    return '{}{}'.format(prefix, index)

if __name__ == '__main__':
    lines = []
    for line in sys.stdin:
        lines.append(line)

    words = {}
    for line in lines:
        for word in re.findall(r'\w+', line):
            words[word] = words.get(word, 0) + 1

    prefixes = [('v_', 'v'), ('f_', 'f')]
    count = [0] * len(prefixes)
    repl = {}
    for word, usage in words.items():
        for i, (p, rp) in enumerate(prefixes):
            if word.startswith(p):
                repl[pattern(word)] = replacement(rp, count[i])
                count[i] += 1

    for line in lines:
        for p, s in repl.items():
            line = re.sub(p, s, line)
        sys.stdout.write(line)
