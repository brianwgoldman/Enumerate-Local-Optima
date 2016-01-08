#!/usr/bin/env python3
import make_mk
import sys
import math

folder = sys.argv[1]

target_K = {"DeceptiveTrap" : 5, "MAXSAT" : 3, "IsingSpinGlass" : 2,
            "AdjacentNKq" : 3, "RandomNKq" : 3}

for problem_name in make_mk.standard_problems:
    for N in range(15, 101):
        for seed in range(30):
            K = target_K[problem_name]
            if problem_name == "DeceptiveTrap":
                if N % K != 0:
                    continue
            if problem_name == "IsingSpinGlass":
                if int(math.sqrt(N)) ** 2 != N:
                    continue
            if problem_name == "MAXSAT":
                if N > 50:
                    continue
            make_mk.create(folder, problem_name, N, K, seed)
for seed in range(30):
    make_mk.create(folder, "AdjacentNKq", 200, 3, seed)
