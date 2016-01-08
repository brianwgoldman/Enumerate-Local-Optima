#!/usr/bin/env python3
# Brian Goldman

# This file calculates the number of dependencies per variable in a collection
# of landscape files passed as command line arguments
import sys
from collections import defaultdict

total = 0
for filename in sys.argv[1:]:
    related = defaultdict(set)
    with open(filename, "r") as f:
        lines = f.read().strip().split('\n')
    lines = [line.split()[1:] for line in lines if len(line) > 0 and line[0] == 'm']
    for line in lines:
        for a in line:
            for b in line:
                if a != b:
                    related[a].add(b)
    result = sum(len(x) for x in related.values()) / float(len(related))
    total += result
    print(filename, result)
print('Total', total / len(sys.argv[1:]))
