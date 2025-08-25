#include <Geode/Geode.hpp>
#include <iconkit.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {

  if (Mod::get()->hasSavedValue("iconKitSettings")) iconKitState.pendingSettings = Mod::get()->getSavedValue<IconKitSettings>("iconKitSettings");
  else iconKitState.pendingSettings.initDefault();
  
  // get this from the game at the start; i used to hardcode it
  // not doing that again ever since i forgot to change it and did not notice for days
  for (UnlockType unlockType : UNLOCK_TYPES_TO_CHANGE)
    VANILLA_MAX_ICONS[unlockType] = GameManager::get()->countForType(UNLOCK_TO_ICON[unlockType]);

}
