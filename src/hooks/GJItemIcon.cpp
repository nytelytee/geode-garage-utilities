#include <Geode/Geode.hpp>
#include <Geode/modify/GJItemIcon.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <logic.hpp>

using namespace geode::prelude;

struct HookedGJItemIcon : Modify<HookedGJItemIcon, GJItemIcon> {
  static GJItemIcon* create(UnlockType unlockType, int item, ccColor3B p2, ccColor3B p3, bool p4, bool p5, bool p6, ccColor3B p7) {
    if (!SHOULD_CHANGE_UNLOCK_TYPE(unlockType) || !iconKitState.shouldChangeIcons)
      return GJItemIcon::create(unlockType, item, p2, p3, p4, p5, p6, p7);
    return GJItemIcon::create(unlockType, positionToDisplay(unlockType, item), p2, p3, p4, p5, p6, p7);
  }
};
