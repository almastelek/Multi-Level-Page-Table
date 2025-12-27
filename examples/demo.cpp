#include <cstdio>
#include <cinttypes>

#include "mlpt.h"
#include "config.h"

int main() {
    const size_t va = 0x12345000ULL;
    page_allocate(va);

    const size_t pa = translate(va);
    if (pa != ~0ULL) {
        std::printf("VA 0x%" PRIx64 " -> PA 0x%" PRIx64 "\n",
                    static_cast<uint64_t>(va), static_cast<uint64_t>(pa));
    } else {
        std::printf("Translation failed for VA 0x%" PRIx64 "\n",
                    static_cast<uint64_t>(va));
    }

    // Verify that an unmapped page fails.
    const size_t unmapped = 0xDEADB000ULL;
    const size_t pa2 = translate(unmapped);
    std::printf("Unmapped VA 0x%" PRIx64 " -> 0x%" PRIx64 "\n",
                static_cast<uint64_t>(unmapped), static_cast<uint64_t>(pa2));
    return 0;
}
