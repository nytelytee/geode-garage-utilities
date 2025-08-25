#pragma once

#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>

using namespace geode::prelude;


struct IconKitSettings {

  // filters
  bool locked;
  bool unlocked;
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
  static Result<IconKitSettings> fromJson(matjson::Value const& value);
  static matjson::Value toJson(IconKitSettings const& value);
};

struct IconKitState {

  // currently active settings (not defined when the game is launched)
  IconKitSettings settings;
  // settings that will become the currently active settings when Apply is pressed (or loaded when GJGarageLayer::init is called)
  IconKitSettings pendingSettings;

  std::unordered_map<UnlockType, std::vector<int>> acceptedIcons;
  std::unordered_map<UnlockType, std::vector<int>> deniedIcons;
  
  // if nonzero, GameManager::countForType is modified to give the icon count that would correspond to the correct page count
  int shouldChangeIcons = 0;
  
  // colon's lite ad chests mod workaround
  bool stubOutSetupPageOnce = false;

};

extern IconKitState iconKitState;
