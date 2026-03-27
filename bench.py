#!/usr/bin/env python3
# SPDX-License-Identifier: Unlicense
# Usage: python3 bench/run_benchmarks.py [number of runs]

import subprocess
import sys
import re
from collections import defaultdict

import os
import platform

def get_system_info():
    info = {}

    # OS
    if os.path.exists("/proc/version"):
        with open("/proc/version") as f:
            version = f.read()
        if "microsoft" in version.lower() or "wsl" in version.lower():
            info["OS"] = "WSL Linux"
        else:
            info["OS"] = f"Linux ({platform.release()})"
    else:
        info["OS"] = platform.system()

    # CPU
    if os.path.exists("/proc/cpuinfo"):
        with open("/proc/cpuinfo") as f:
            for line in f:
                if "model name" in line:
                    info["CPU"] = line.split(":")[1].strip()
                    break
    else:
        info["CPU"] = platform.processor() or "Unknown"

    # RAM
    if os.path.exists("/proc/meminfo"):
        with open("/proc/meminfo") as f:
            for line in f:
                if line.startswith("MemTotal"):
                    kb = int(line.split()[1])
                    info["RAM"] = f"{round(kb / 1024 / 1024)}GB"
                    break
    else:
        info["RAM"] = "Unknown"

    return info

def parse_output(output):
    results = {}
    for line in output.splitlines():
        # Match lines like "append char x100000                      1.234 ms"
        m = re.match(r'^(.+?)\s{2,}([\d.]+)\s+ms$', line.strip())
        if m:
            label = m.group(1).strip()
            ms    = float(m.group(2))
            results[label] = ms
    return results

def run_once():
    result = subprocess.run(
        ["make", "bench"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("make bench failed:")
        print(result.stderr)
        sys.exit(1)
    return parse_output(result.stdout)

def main():
    runs = int(sys.argv[1]) if len(sys.argv) > 1 else 100

    print(f"Running make bench {runs} times...\n")

    all_results = defaultdict(list)

    for i in range(runs):
        print(f"  Run {i + 1}/{runs}", end="\r", flush=True)
        data = run_once()
        for label, ms in data.items():
            all_results[label].append(ms)

    print("\n")

    # Compute stats
    stats = {}
    for label, times in all_results.items():
        times_sorted = sorted(times)
        n      = len(times_sorted)
        mean   = sum(times_sorted) / n
        median = times_sorted[n // 2]
        lo     = times_sorted[0]
        hi     = times_sorted[-1]
        p95    = times_sorted[int(n * 0.95)]
        stats[label] = (mean, median, lo, hi, p95)

    # Print
    col = max(len(l) for l in stats) + 2
    header = f"{'Benchmark':<{col}} {'Mean':>10} {'Median':>10} {'Min':>10} {'Max':>10} {'P95':>10}"
    print(header)
    print("-" * len(header))

    current_suite = None
    for label, (mean, median, lo, hi, p95) in stats.items():
        # Print a separator between C and C++ suites
        suite = "C++" if any(
            label == l for l in all_results if "construct" in l or "cstr" in l
        ) else "C"
        _ = suite  # unused, just use label order from dict

        print(f"{label:<{col}} {mean:>9.3f}ms {median:>9.3f}ms {lo:>9.3f}ms {hi:>9.3f}ms {p95:>9.3f}ms")
    
    sys_info = get_system_info()
    print("System: " + " | ".join(f"{k}: {v}" for k, v in sys_info.items()))

    print()

if __name__ == "__main__":
    main()