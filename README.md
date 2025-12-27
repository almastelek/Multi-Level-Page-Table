# Multi-Level-Page-Table
Built a multi-level page table for my computer systems and organization class

**public API**:

- `size_t ptbr` (page table base register)
- `size_t translate(size_t va)`
- `void page_allocate(size_t va)`

The header uses `extern "C"` when compiled as C++ so existing C test harnesses can still link against the library.

## Build (Makefile)

```sh
make
./mlpt_demo
```

Artifacts:
- `libmlpt.a` — static library
- `mlpt_demo` — small example program

## Build (CMake)

```sh
cmake -S . -B build
cmake --build build
./build/mlpt_demo
```

## Notes

- Memory for page tables/data pages is allocated with `posix_memalign` and zero‑initialized.
- `translate()` returns `~0UL` when no mapping exists.

Adjust `include/config.h` to change `LEVELS` and `POBITS`.
