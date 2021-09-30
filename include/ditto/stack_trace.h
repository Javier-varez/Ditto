#ifndef DITTO_STACK_TRACE_H
#define DITTO_STACK_TRACE_H

#include <cstdint>

#include "ditto/arch.h"

namespace Ditto {

struct StackFrame {
  const StackFrame* const prev;
  const void* const return_address;
};

template <typename C, typename T>
concept CallableWithT = requires(C c, T t) {
  {c(t)};
};

template <CallableWithT<const StackFrame&> C>
void walk_stack_trace(C callable) {
  uintptr_t frame_ptr = Ditto::arch::get_frame_pointer();
  auto* stack_frame = reinterpret_cast<const StackFrame*>(frame_ptr);

  uint32_t i = 0;
  while (stack_frame != nullptr) {
    callable(*stack_frame);
    stack_frame = stack_frame->prev;
  }
}

}  // namespace Ditto

#endif  // DITTO_STACK_TRACE_H
