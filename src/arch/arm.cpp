#include <ditto/arch.h>

#include <cstdint>

namespace Ditto::arch {

__attribute__((naked)) uintptr_t get_frame_pointer() {
  asm volatile("mov r0, r11\n");
}

}  // namespace Ditto::arch
