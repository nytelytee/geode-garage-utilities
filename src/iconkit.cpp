#include <algorithm>
#include <constants.hpp>
#include <iconkit.hpp>
#include <matjson/stl_serialize.hpp>

void IconKitSettings::initDefault() {
  locked = true;
  unlocked = true;

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
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = true;
  for (int author : AUTHORS_IN_ORDER) authors[author] = true;
}

void IconKitSettings::deselectAll() {
  locked = false;
  unlocked = false;
  for (std::string category : CATEGORIES_IN_ORDER) categories[category] = false;
  for (int author : AUTHORS_IN_ORDER) authors[author] = false;
}

void IconKitSettings::changeFilterType() {
  invert = !invert;

  locked = !locked;
  unlocked = !unlocked;
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

Result<IconKitSettings> matjson::Serialize<IconKitSettings>::fromJson(const matjson::Value &value) {
  IconKitSettings settings;
  IconKitSettings defaultSettings;
  defaultSettings.initDefault();
  settings.locked = value.get<bool>("locked").unwrapOr(defaultSettings.locked);
  settings.unlocked = value.get<bool>("unlocked").unwrapOr(defaultSettings.unlocked);
  settings.invert = value.get<bool>("invert").unwrapOr(defaultSettings.invert);
  
  if (!value.get("categories") || !(*value.get("categories")).isObject())
    settings.categories = defaultSettings.categories;
  else {
    matjson::Value categories = *value.get("categories");
    for (std::string category : CATEGORIES_IN_ORDER) {
      if (!categories.get(category) || !(*categories.get(category)).isBool())
        settings.categories[category] = defaultSettings.categories[category];
      else
        settings.categories[category] = *categories.get<bool>(category);
    }
  }
  
  if (!value.get("authors") || !(*value.get("authors")).isObject())
    settings.authors = defaultSettings.authors;
  else {
    matjson::Value authors = *value.get("authors");
    for (int author : AUTHORS_IN_ORDER) {
      std::string authorString = numToString(author);
      if (!authors.get(authorString) || !(*authors.get(authorString)).isBool())
        settings.authors[author] = defaultSettings.authors[author];
      else
        settings.authors[author] = *authors.get<bool>(authorString);
    }
  }
  
  settings.showDenied = value.get<bool>("showDenied").unwrapOr(defaultSettings.showDenied);
  settings.separateAcceptedFromDenied = value.get<bool>("separateAcceptedFromDenied").unwrapOr(defaultSettings.separateAcceptedFromDenied);
  settings.dontRecolorNavigationButtons = value.get<bool>("dontRecolorNavigationButtons").unwrapOr(defaultSettings.dontRecolorNavigationButtons);
  settings.noGapBetweenAcceptedAndDenied = value.get<bool>("noGapBetweenAcceptedAndDenied").unwrapOr(defaultSettings.noGapBetweenAcceptedAndDenied);
  settings.hideNavigationMenuOnSinglePage = value.get<bool>("hideNavigationMenuOnSinglePage").unwrapOr(defaultSettings.hideNavigationMenuOnSinglePage);
  
  if (!value.get("sortBy") || !value.get<std::vector<int>>("sortBy"))
    for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortBy[i] = static_cast<SortType>(defaultSettings.sortBy[i]);
  else {
    std::vector<int> sortBy = *value.get<std::vector<int>>("sortBy");
    if (sortBy.size() != SORT_TYPE_COUNT) {
      for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortBy[i] = static_cast<SortType>(defaultSettings.sortBy[i]);
    } else {
      bool broken = false;
      for (int i = 0; i < SORT_TYPE_COUNT; i++) {
        if (std::find(sortBy.begin(), sortBy.end(), i) == sortBy.end()) {
          for (size_t j = 0; j < SORT_TYPE_COUNT; j++) settings.sortBy[j] = static_cast<SortType>(defaultSettings.sortBy[j]);
          broken = true;
          break;
        }
      }
      if (!broken) {
        for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortBy[i] = static_cast<SortType>(sortBy[i]);
      }
    }
  }
  
  if (!value.get("sortIsReverse") || !value.get<std::vector<bool>>("sortIsReverse"))
    for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortIsReverse[i] = defaultSettings.sortIsReverse[i];
  else {
    std::vector<bool> sortIsReverse = *value.get<std::vector<bool>>("sortIsReverse");
    if (sortIsReverse.size() != SORT_TYPE_COUNT) {
      for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortIsReverse[i] = defaultSettings.sortIsReverse[i];
    } else {
      for (size_t i = 0; i < SORT_TYPE_COUNT; i++) settings.sortIsReverse[i] = sortIsReverse[i];
    }
  }

  settings.strictCategory = value.get<bool>("strictCategory").unwrapOr(defaultSettings.strictCategory);

  return Ok(settings);
}

matjson::Value matjson::Serialize<IconKitSettings>::toJson(const IconKitSettings &value) {
  matjson::Value object = matjson::Value::object();
  object.set("locked", value.locked);
  object.set("unlocked", value.unlocked);
  object.set("invert", value.invert);

  object.set("categories", value.categories);

  std::unordered_map<std::string, bool> intermediate;
  for (std::pair<int, bool> author_filter : value.authors)
    intermediate[std::to_string(author_filter.first)] = author_filter.second;
  object.set("authors", intermediate);

  object.set("showDenied", value.showDenied);
  object.set("separateAcceptedFromDenied", value.separateAcceptedFromDenied);
  object.set("dontRecolorNavigationButtons", value.dontRecolorNavigationButtons);
  object.set("noGapBetweenAcceptedAndDenied", value.noGapBetweenAcceptedAndDenied);
  object.set("hideNavigationMenuOnSinglePage", value.hideNavigationMenuOnSinglePage);

  object.set("sortBy", matjson::Value::array());
  for (size_t i = 0; i < SORT_TYPE_COUNT; i++) {
    int sortByAsInt = static_cast<int>(value.sortBy[i]);
    (*object.get("sortBy")).push(sortByAsInt);
  }
  
  object.set("sortIsReverse", matjson::Value::array());
  for (size_t i = 0; i < SORT_TYPE_COUNT; i++) {
    bool sortIsReverse = value.sortIsReverse[i];
    (*object.get("sortIsReverse")).push(sortIsReverse);
  }

  object.set("strictCategory", value.strictCategory);

  return object;
}

IconKitState iconKitState;
