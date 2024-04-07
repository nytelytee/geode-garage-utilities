#include <Geode/Geode.hpp>
#include <iconkit.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {

  if (Mod::get()->hasSavedValue("iconKitSettings")) iconKitState.settings = Mod::get()->getSavedValue<IconKitSettings>("iconKitSettings");
  else iconKitState.settings.initDefault();

  iconKitState.pendingSettings = iconKitState.settings;

}
