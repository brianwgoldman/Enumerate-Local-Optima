#!/usr/bin/env python3
import make_mk
import sys

folder = sys.argv[1]

target_K = {"DeceptiveTrap" : 5, "MAXSAT" : 3, "IsingSpinGlass" : 2,
            "AdjacentNKq" : 3, "RandomNKq" : 3}

for problem_name in make_mk.standard_problems:
    for N in range(10, 31):
        for seed in range(30):
            K = target_K[problem_name]
            if problem_name == "DeceptiveTrap":
                if N % K != 0:
                    continue
            make_mk.create(folder, problem_name, N, K, seed)