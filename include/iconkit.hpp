#pragma once

#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>

using namespace geode::prelude;


struct IconKitSettings {

  // filters
  bool locked;
  bool unlocked;
  bool vanilla;
  bool custom;
  bool invert;
  std::unordered_map<std::string, bool> categories;
  std::unordered_map<int, bool> authors;

  // extra options for display
  bool showDenied;
  bool separateAcceptedFromDenied;
  bool dontRecolorNavigationButtons;
  bool noGapBetweenAcceptedAndDenied;
  bool hideNavigationMenuOnSinglePage;
  
  // sort order
  SortType sortBy[SORT_TYPE_COUNT];
  bool sortIsReverse[SORT_TYPE_COUNT];

  // extra sorting options
  bool strictCategory;

  void initDefault();
  void resetToDefault();
  void selectAll();
  void deselectAll();
  void changeFilterType();
  bool isSortedBy(SortType sortType);

};

// specialization of icon kit settings to enable saving as mod data
template<>
struct matjson::Serialize<IconKitSettings> {
  static IconKitSettings from_json(matjson::Value const& value);
  static matjson::Value to_json(IconKitSettings const& value);
  static bool is_json(matjson::Value const&) { return true; }
};

struct IconKitState {

  // currently active settings
  IconKitSettings settings;
  // settings that will become the currently active settings when Apply is pressed
  IconKitSettings pendingSettings;

  std::unordered_map<UnlockType, std::vector<unsigned>> acceptedIcons;
  std::unordered_map<UnlockType, std::vector<unsigned>> deniedIcons;
  
  // current global state to be used inside the sorting functions
  UnlockType comparison;
  bool reverseSort;

  IconType selectedIconType;

  std::unordered_map<IconType, int> pageForIcon;
  // when recalculating filters, page -1 (or 0 if the active icon is filtered out)
  // should forcefully be shown instead of the previous active page when
  // switching to a tab you have not switched to since the recalculation,
  // use this boolean to override this in setupPage
  std::unordered_map<IconType, bool> overridePageForIcon;
  
  bool shouldChangeIcons = false;

  IconKitState() {
    for (IconType iconType : ICON_TYPES_TO_CHANGE) {
      pageForIcon[iconType] = -1;
      overridePageForIcon[iconType] = false;
    }
  }
};

extern IconKitState iconKitState;
