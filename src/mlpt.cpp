#define _XOPEN_SOURCE 700

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "mlpt.h"
#include "config.h"

// Page Table Base Register (root page table address).
size_t ptbr = 0;

namespace {

using page_table_t = size_t*;

constexpr std::uintptr_t kInvalid = ~static_cast<std::uintptr_t>(0);

static inline std::size_t page_size_bytes() {
    return static_cast<std::size_t>(1ULL << POBITS);
}

// Number of index bits per level. A page table occupies one page.
// Each entry is a size_t (8 bytes on the autograder's x86-64).
static inline std::size_t bits_per_level() {
    return static_cast<std::size_t>(POBITS - 3);
}

static inline std::size_t get_offset(std::size_t va) {
    const std::size_t mask = (static_cast<std::size_t>(1ULL << POBITS) - 1ULL);
    return va & mask;
}

static inline std::size_t get_index(std::size_t va, int level) {
    const std::size_t bits = bits_per_level();
    const std::size_t idx_mask = (static_cast<std::size_t>(1ULL << bits) - 1ULL);
    const std::size_t shift = static_cast<std::size_t>(POBITS) +
                              static_cast<std::size_t>(LEVELS - 1 - level) * bits;
    return (va >> shift) & idx_mask;
}

static inline bool is_valid_pte(std::size_t pte) {
    return (pte & 1ULL) != 0;
}

static inline std::size_t get_ppn_from_pte(std::size_t pte) {
    return pte >> POBITS;
}

static inline std::size_t create_pte(std::size_t ppn) {
    return (ppn << POBITS) | 1ULL;
}

static std::uintptr_t allocate_aligned_page() {
    void* mem = nullptr;
    const std::size_t ps = page_size_bytes();

    if (posix_memalign(&mem, ps, ps) != 0) {
        std::fprintf(stderr, "Failed to allocate aligned page\n");
        std::exit(EXIT_FAILURE);
    }

    std::memset(mem, 0, ps);
    return reinterpret_cast<std::uintptr_t>(mem);
}

static std::uintptr_t uaddr(std::size_t addr) {
    return static_cast<std::uintptr_t>(addr);
}

static page_table_t as_pt(std::uintptr_t addr) {
    return reinterpret_cast<page_table_t>(addr);
}

} // namespace

extern "C" size_t translate(size_t va) {
    if (ptbr == 0) {
        return static_cast<size_t>(kInvalid);
    }

    std::uintptr_t curr_pt_addr = uaddr(ptbr);
    page_table_t curr_pt = as_pt(curr_pt_addr);

    for (int level = 0; level < LEVELS; ++level) {
        const std::size_t idx = get_index(va, level);
        const std::size_t pte = curr_pt[idx];

        if (!is_valid_pte(pte)) {
            return static_cast<size_t>(kInvalid);
        }

        const std::size_t ppn = get_ppn_from_pte(pte);

        if (level < LEVELS - 1) {
            curr_pt_addr = static_cast<std::uintptr_t>(ppn) << POBITS;
            curr_pt = as_pt(curr_pt_addr);
        } else {
            const std::size_t offset = get_offset(va);
            const std::uintptr_t pa = (static_cast<std::uintptr_t>(ppn) << POBITS) |
                                      static_cast<std::uintptr_t>(offset);
            return static_cast<size_t>(pa);
        }
    }

    return static_cast<size_t>(kInvalid);
}

extern "C" void page_allocate(size_t va) {
    if (ptbr == 0) {
        ptbr = static_cast<size_t>(allocate_aligned_page());
    }

    std::uintptr_t curr_pt_addr = uaddr(ptbr);
    page_table_t curr_pt = as_pt(curr_pt_addr);

    for (int level = 0; level < LEVELS; ++level) {
        const std::size_t idx = get_index(va, level);

        if (level < LEVELS - 1) {
            if (!is_valid_pte(curr_pt[idx])) {
                const std::uintptr_t new_pt = allocate_aligned_page();
                const std::size_t new_pt_ppn = static_cast<std::size_t>(new_pt >> POBITS);
                curr_pt[idx] = create_pte(new_pt_ppn);
            }

            const std::size_t ppn = get_ppn_from_pte(curr_pt[idx]);
            curr_pt_addr = static_cast<std::uintptr_t>(ppn) << POBITS;
            curr_pt = as_pt(curr_pt_addr);
        } else {
            if (!is_valid_pte(curr_pt[idx])) {
                const std::uintptr_t new_page = allocate_aligned_page();
                const std::size_t new_page_ppn = static_cast<std::size_t>(new_page >> POBITS);
                curr_pt[idx] = create_pte(new_page_ppn);
            }
        }
    }
}
