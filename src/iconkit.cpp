#include <algorithm>
#include <constants.hpp>
#include <iconkit.hpp>
#include <matjson/stl_serialize.hpp>

void IconKitSettings::initDefault() {
  locked = true;
  unlocked = true;
  vanilla = true;
  custom = true;

  invert = false;

  showDenied = false;
  separateAcceptedFromDenied = false;
  dontRecolorNavigationButtons = false;
  noGapBetweenAcceptedAndDenied = false;
  hideNavigationMenuOnSinglePage = false;
  std::copy(std::begin(DEFAULT_SORT_TYPES), std::end(DEFAULT_SORT_TYPES), std::begin(sortBy));
  std::copy(std::begin(DEFAULT_SORT_REVERSES), std::end(DEFAULT_SORT_REVERSES), std::begin(sortIsReverse));
  strictCategory = false;
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = true;
  for (int author : AUTHORS_IN_ORDER) authors[author] = true;
}

void IconKitSettings::resetToDefault() {
  selectAll();
  invert = false;
  std::copy(std::begin(DEFAULT_SORT_TYPES), std::end(DEFAULT_SORT_TYPES), std::begin(sortBy));
  std::copy(std::begin(DEFAULT_SORT_REVERSES), std::end(DEFAULT_SORT_REVERSES), std::begin(sortIsReverse));
  strictCategory = false;
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = true;
  for (int author : AUTHORS_IN_ORDER) authors[author] = true;
}

void IconKitSettings::selectAll() {
  locked = true;
  unlocked = true;
  vanilla = true;
  custom = true;
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = true;
  for (int author : AUTHORS_IN_ORDER) authors[author] = true;
}

void IconKitSettings::deselectAll() {
  locked = false;
  unlocked = false;
  vanilla = false;
  custom = false;
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = false;
  for (int author : AUTHORS_IN_ORDER) authors[author] = false;
}

void IconKitSettings::changeFilterType() {
  invert = !invert;

  locked = !locked;
  unlocked = !unlocked;
  vanilla = !vanilla;
  custom = !custom;
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = !categories[category];
  for (int author : AUTHORS_IN_ORDER) authors[author] = !authors[author];
}

bool IconKitSettings::isSortedBy(SortType sortType) {
    for (size_t i = 0; i < SORT_TYPE_COUNT; i++) {
      if (sortBy[i] == sortType) return true;
      if (sortBy[i] == SortType::Number) return false;
      if (sortBy[i] == SortType::Category && strictCategory) return false;
    }
    log::info("unreachable code reached in IconKitSettings::isSortedBy");
    return true;
}

IconKitSettings matjson::Serialize<IconKitSettings>::from_json(const matjson::Value &value) {
  IconKitSettings settings;
  settings.locked = value.get<bool>("locked");
  settings.unlocked = value.get<bool>("unlocked");
  settings.vanilla = value.get<bool>("vanilla");
  settings.custom = value.get<bool>("custom");
  settings.invert = value.get<bool>("invert");

  settings.categories = matjson::Serialize<std::unordered_map<std::string, bool>>::from_json(value["categories"]);
  
  std::unordered_map<std::string, bool> intermediate(matjson::Serialize<std::unordered_map<std::string, bool>>::from_json(value["authors"]));
  for (std::pair<std::string, bool> author_filter : intermediate)
    settings.authors[std::stoi(author_filter.first)] = author_filter.second;

  settings.showDenied = value.get<bool>("showDenied");
  settings.separateAcceptedFromDenied = value.get<bool>("separateAcceptedFromDenied");
  settings.dontRecolorNavigationButtons = value.get<bool>("dontRecolorNavigationButtons");
  settings.noGapBetweenAcceptedAndDenied = value.get<bool>("noGapBetweenAcceptedAndDenied");
  settings.hideNavigationMenuOnSinglePage = value.get<bool>("hideNavigationMenuOnSinglePage");

  matjson::Array sortBy = value.get<matjson::Array>("sortBy");
  for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortBy[i] = static_cast<SortType>(sortBy[i].as<unsigned>());
  
  matjson::Array sortIsReverse = value.get<matjson::Array>("sortIsReverse");
  for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortIsReverse[i] = sortIsReverse[i].as<bool>();
  
  settings.strictCategory = value.get<bool>("strictCategory");

  return settings;
}

matjson::Value matjson::Serialize<IconKitSettings>::to_json(const IconKitSettings &value) {
  matjson::Object object;
  object["locked"] = value.locked;
  object["unlocked"] = value.unlocked;
  object["vanilla"] = value.vanilla;
  object["custom"] = value.custom;
  object["invert"] = value.invert;

  object["categories"] = value.categories;

  std::unordered_map<std::string, bool> intermediate;
  for (std::pair<int, bool> author_filter : value.authors)
    intermediate[std::to_string(author_filter.first)] = author_filter.second;
  object["authors"] = intermediate;

  object["showDenied"] = value.showDenied;
  object["separateAcceptedFromDenied"] = value.separateAcceptedFromDenied;
  object["dontRecolorNavigationButtons"] = value.dontRecolorNavigationButtons;
  object["noGapBetweenAcceptedAndDenied"] = value.noGapBetweenAcceptedAndDenied;
  object["hideNavigationMenuOnSinglePage"] = value.hideNavigationMenuOnSinglePage;

  object["sortBy"] = matjson::Array();
  for (size_t i = 0; i < SORT_TYPE_COUNT; i++) object["sortBy"].as_array().push_back(static_cast<int>(value.sortBy[i]));
  
  object["sortIsReverse"] = matjson::Array();
  for (size_t i = 0; i < SORT_TYPE_COUNT; i++) object["sortIsReverse"].as_array().push_back(value.sortIsReverse[i]);

  object["strictCategory"] = value.strictCategory;

  return object;
}

IconKitState iconKitState;
