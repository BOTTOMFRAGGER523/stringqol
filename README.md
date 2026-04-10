# StringQOL
A simple & minimal String QoL (Quality of Life) single header library designed to be memory safe while abstracting all the unsafe and tedious string work commonly found in C while being licensed under the super permissive Unlicense License.

Contributions are welcome!

## Goals
- ✅ All memory owned by the library
- ✅ Memory safe
- ✅ C Support and freestanding C++ support
- ✅ Portable
- ✅ Single header
- ✅ Lightweight
- ✅ Somewhat Fast

## Building
### Installing
- Run `make install` (sudo might be needed), it will install to `/usr/local/include/stringqol`

### Packaging
This option might be useful if lets say you want a script to fetch this as a dependency or you're vendoring this project.
This option runs `make clean`, removes unnecessary directories (eg. bench test_suite and .vscode) and files (eg. bench.py), and renames `stringqol` to `include`. Be careful though as it forcefully deletes said folders and said files.

To run this option simply run `make package`.

### Running tests
- Run `make test` and you have your test results.

### Running benchmarks
- Run `python bench.py` (No dependencies required) and you have your benchmarks.
- Read from Median to get the average result, Min to get the fastest and Max to get the slowest.

## Q&A
<details>
<summary>Why did you make this?</summary>
<br>
I wanted something lightweight, fast, portable and a single header library that also supported C++.
<br>
</details>

<details>
<summary>Will you maintain it?</summary>
<br>
Only if there's optimization to do, security fixes, bug fixes, or if I want to add another feature.
<br>
</details>
<br>