#ifndef DITTO_BADGE_H_
#define DITTO_BADGE_H_

namespace Ditto {

template <class T>
class Badge {
 private:
  Badge() = default;
  friend T;
};

}  // namespace Ditto

#endif  // DITTO_BADGE_H_
