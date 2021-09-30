
#include <ditto/arch.h>

namespace Ditto::arch {

__attribute__((naked)) uintptr_t get_frame_pointer() {
  asm volatile("mov x0, x29\n");
}

}  // namespace Ditto::arch