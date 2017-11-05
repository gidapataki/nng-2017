#!/usr/bin/env python3

import re
import os
import sys

def get_local_include_regex():
    return re.compile('\#include "(\w+\.\w+)"')

def find_local_include(content):
    for lineno, line in enumerate(content):
        if not re.match(get_local_include_regex(), line):
            continue
        return lineno
    return None

def resolve_include_line(line):
    pattern = get_local_include_regex()
    filename = re.match(pattern, line).group(1)
    with open(os.path.join('src', filename), 'r') as f:
        return f.read().split('\n')

def open_with_includes_resolved(filename):
    with open(filename, 'r') as f:
        content = f.read().split('\n')
        while True:
            lineno = find_local_include(content)
            if lineno is None:
                break
            before = content[:lineno]
            inner_content = resolve_include_line(content[lineno])
            after = content[lineno+1:]
            content = before + inner_content + after
        return content


def main():
    args = sys.argv[1:]
    resolved = [open_with_includes_resolved(arg) for arg in args]
    for output in resolved:
        for line in output:
            if line:
                print(line)

if __name__ == '__main__':
    main()
