#pragma once

// categories
extern std::unordered_map<UnlockType, std::unordered_map<std::string, std::vector<int>>> CATEGORIES;
extern std::unordered_map<std::string, size_t> CATEGORY_TO_ORDER;
extern std::vector<std::string> CATEGORIES_IN_ORDER;
extern std::unordered_map<UnlockType, std::unordered_map<int, std::string>> CATEGORY_FOR_ICON;
extern std::unordered_map<UnlockType, std::unordered_map<int, size_t>> CATEGORY_INDEX_FOR_ICON;

// authors
extern std::unordered_map<UnlockType, std::unordered_map<int, std::vector<int>>> AUTHORS;
extern std::unordered_map<int, size_t> AUTHOR_TO_ORDER;
extern std::vector<int> AUTHORS_IN_ORDER;
extern std::unordered_map<UnlockType, std::unordered_map<int, int>> AUTHOR_FOR_ICON;

// other
const int SORT_TYPE_COUNT = 4;
enum SortType { Number, LockStatus, Category, Author };
const SortType DEFAULT_SORT_TYPES[SORT_TYPE_COUNT] = { SortType::Number, SortType::LockStatus, SortType::Category, SortType::Author };
const bool DEFAULT_SORT_REVERSES[SORT_TYPE_COUNT] = { false, false, false, false };

enum PathType { Fire = 1, Ice, Poison, Shadow, Lava, Earth, Blood, Metal, Light, Souls };
extern std::unordered_map<SortType, std::string> SORT_TYPE_NAMES;

extern std::unordered_map<UnlockType, int> VANILLA_MAX_ICONS;
extern std::unordered_map<IconType, UnlockType> ICON_TO_UNLOCK;
extern std::unordered_map<UnlockType, IconType> UNLOCK_TO_ICON;
extern std::vector<IconType> ICON_TYPES_TO_CHANGE;
extern std::vector<UnlockType> UNLOCK_TYPES_TO_CHANGE;
#define SHOULD_CHANGE_ICON_TYPE(iconType) (std::find(ICON_TYPES_TO_CHANGE.begin(), ICON_TYPES_TO_CHANGE.end(), iconType) != ICON_TYPES_TO_CHANGE.end())
#define SHOULD_CHANGE_UNLOCK_TYPE(unlockType) (std::find(UNLOCK_TYPES_TO_CHANGE.begin(), UNLOCK_TYPES_TO_CHANGE.end(), unlockType) != UNLOCK_TYPES_TO_CHANGE.end())
extern std::unordered_map<UnlockType, std::string> ICON_NAMES;
extern std::unordered_map<UnlockType, std::vector<int>> SPECIAL_UNLOCK_ORDER;
extern std::vector<std::pair<int, UnlockType>> SHARDS_OF_POWER_UNLOCK_ORDER;
extern std::unordered_map<PathType, std::vector<std::pair<int, UnlockType>>> PATHS_UNLOCK_ORDER;


const float VERTICAL_BORDER_SIZE = 5.f;
const float HORIZONTAL_BORDER_SIZE = 3.25f;
const float TOGGLER_WIDTH_RATIO = 31.f/35.f;
const float TOGGLER_HEIGHT_RATIO = 31.f/34.5f;
