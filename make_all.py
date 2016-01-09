#!/usr/bin/env python3
'''
This file recreates all MK Landscapes used by the
`Hyperplane Elimination for Quickly Enumerating Local Optima`
paper. Requies a command line argument for the output folder.
'''
import make_mk
import sys
import math

# Takes the output folder as a command line argument
folder = sys.argv[1]

# Specifies the k used for each problem
target_k = {"DeceptiveTrap" : 5, "MAXSAT" : 3, "IsingSpinGlass" : 2,
            "AdjacentNKq" : 3, "RandomNKq" : 3}

for problem_name in make_mk.standard_problems:
    for N in range(15, 101):
        for seed in range(30):
            k = target_k[problem_name]
            if problem_name == "DeceptiveTrap":
                if N % k != 0:
                    # N must be evenly divisible by k for this problem.
                    continue
            if problem_name == "IsingSpinGlass":
                if int(math.sqrt(N)) ** 2 != N:
                    # 2D Spin Glasses require N be a power of 2
                    continue
            if problem_name == "MAXSAT":
                if N > 50:
                    # Big SATs failed horribly, so I stopped generating them.
                    continue
            make_mk.create(folder, problem_name, N, k, seed)
# Create the extra large Adjacent NKq
for seed in range(30):
    make_mk.create(folder, "AdjacentNKq", 200, 3, seed)
