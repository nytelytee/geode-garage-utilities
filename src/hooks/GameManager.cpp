#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <logic.hpp>

using namespace geode::prelude;

struct HookedGameManager : Modify<HookedGameManager, GameManager> {

  // InDecreaseIcons returns true if the icon is custom
  // Change hook priority to run before it so you actually pass it the correct value
  static void onModify(auto& self) {
    Result<> result = self.setHookPriority("GameManager::isIconUnlocked", -1);
    if (!result)
      log::error("Failed to set hook priority. Wrong icons may show up as locked/unlocked.");
  }

  bool isIconUnlocked(int icon, IconType iconType) {
    if (!SHOULD_CHANGE_ICON_TYPE(iconType) || !iconKitState.shouldChangeIcons)
      return GameManager::isIconUnlocked(icon, iconType);
    return GameManager::isIconUnlocked(positionToDisplay(ICON_TO_UNLOCK[iconType], icon), iconType);
  }

};
