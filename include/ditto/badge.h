
#pragma once

namespace Ditto {

template <class T>
class Badge {
 private:
  Badge() = default;
  friend T;
};

}  // namespace Ditto
