
#include <ditto/arch.h>

namespace Ditto::arch {

__attribute__((naked)) uintptr_t get_frame_pointer() {
  asm volatile("movq %rbp, %rax\n");
}

}  // namespace Ditto::arch
