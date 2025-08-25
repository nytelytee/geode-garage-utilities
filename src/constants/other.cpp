#include<constants.hpp>

std::unordered_map<UnlockType, int> VANILLA_MAX_ICONS;

std::unordered_map<IconType, UnlockType> ICON_TO_UNLOCK = {
  {IconType::Cube,    UnlockType::Cube},
  {IconType::Ship,    UnlockType::Ship},
  {IconType::Ball,    UnlockType::Ball},
  {IconType::Ufo,     UnlockType::Bird},
  {IconType::Wave,    UnlockType::Dart},
  {IconType::Robot,   UnlockType::Robot},
  {IconType::Spider,  UnlockType::Spider},
  {IconType::Swing,   UnlockType::Swing},
  {IconType::Jetpack, UnlockType::Jetpack},
  
  {IconType::DeathEffect, UnlockType::Death},
  {IconType::Special, UnlockType::Streak},
  {IconType::Item, UnlockType::GJItem},
  {IconType::ShipFire, UnlockType::ShipFire},
};

std::unordered_map<UnlockType, IconType> UNLOCK_TO_ICON = {
  {UnlockType::Cube,    IconType::Cube},
  {UnlockType::Ship,    IconType::Ship},
  {UnlockType::Ball,    IconType::Ball},
  {UnlockType::Bird,    IconType::Ufo},
  {UnlockType::Dart,    IconType::Wave},
  {UnlockType::Robot,   IconType::Robot},
  {UnlockType::Spider,  IconType::Spider},
  {UnlockType::Swing,   IconType::Swing},
  {UnlockType::Jetpack, IconType::Jetpack},
  
  {UnlockType::Death, IconType::DeathEffect},
  {UnlockType::Streak, IconType::Special},
  {UnlockType::GJItem, IconType::Item},
  {UnlockType::ShipFire, IconType::ShipFire},
};

std::vector<IconType> ICON_TYPES_TO_CHANGE = {
    IconType::Cube, IconType::Ship, IconType::Ball,
    IconType::Ufo, IconType::Wave, IconType::Robot,
    IconType::Spider, IconType::Swing, IconType::Jetpack
};

std::vector<UnlockType> UNLOCK_TYPES_TO_CHANGE = {
    UnlockType::Cube, UnlockType::Ship, UnlockType::Ball,
    UnlockType::Bird, UnlockType::Dart, UnlockType::Robot,
    UnlockType::Spider, UnlockType::Swing, UnlockType::Jetpack
};

std::unordered_map<UnlockType, std::string> ICON_NAMES = {
  {UnlockType::Cube,    "Cube"},
  {UnlockType::Ship,    "Ship"},
  {UnlockType::Ball,    "Ball"},
  {UnlockType::Bird,    "UFO"},
  {UnlockType::Dart,    "Wave"},
  {UnlockType::Robot,   "Robot"},
  {UnlockType::Spider,  "Spider"},
  {UnlockType::Swing,   "Swing"},
  {UnlockType::Jetpack, "Jetpack"},
  {UnlockType::Streak,  "Streak"},
  {UnlockType::Death,   "Death"},
  {UnlockType::GJItem,  "GJItem"},
  {UnlockType::ShipFire,"ShipFire"},
  {UnlockType::Col1,    "Color 1"},
  {UnlockType::Col2,    "Color 2"},
  {static_cast<UnlockType>(0), "NULL"},
};

std::unordered_map<UnlockType, std::vector<int>> SPECIAL_UNLOCK_ORDER = {
  {UnlockType::Streak,   {1, 2, 3, 4, 5, 6, 7}},
  {UnlockType::ShipFire, {1, 2, 3, 4, 5, 6}},
  {UnlockType::GJItem,   {18, 19, 20}}, // there are other items, but these are the ones shown in the icon kit as animations
  {UnlockType::Death,    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}},
  {UnlockType::Col1,     {
    51,  19,  48,   9,  37,  53,  54,  55,  25,  56,  57,  58,
    62,  63,  10,  29,  26,  59,  60,  61,  30,  64,  65,  66,
    70,  42,  11,  27,  71,  14,  31,  45,  46,  67,  68,  69,
    72,  73,   0,   1, 105,  28,  32,  20,   2,  38,  79,  80,
    74,  75,  44,   3,  40,  76,  77,  78,  33,  21,  81,  82,
    83,  16,   4,   5,  22,  39,  84,  50,  34,  85,  86,  87, 106, 88, 89, 90,
    52,  41,   6,  35,  47,  23,  92,  93,  49,  95,  96,  97,
    98,   8,  36, 103,   7,  13,  24, 104,  43,  99, 100, 101,
    12,  91,  17, 102,  18,  94,  15,
  }},
  {UnlockType::Col2,     {
    51,  19,  48,   9,  37,  53,  54,  55,  25,  56,  57,  58,
    62,  63,  10,  29,  26,  59,  60,  61,  30,  64,  65,  66,
    70,  42,  11,  27,  71,  14,  31,  45,  46,  67,  68,  69,
    72,  73,   0,   1, 105,  28,  32,  20,   2,  38,  79,  80,
    74,  75,  44,   3,  40,  76,  77,  78,  33,  21,  81,  82,
    83,  16,   4,   5,  22,  39,  84,  50,  34,  85,  86,  87, 106, 88, 89, 90,
    52,  41,   6,  35,  47,  23,  92,  93,  49,  95,  96,  97,
    98,   8,  36, 103,   7,  13,  24, 104,  43,  99, 100, 101,
    12,  91,  17, 102,  18,  94,  15,
  }},
};

std::vector<std::pair<int, UnlockType>> SHARDS_OF_POWER_UNLOCK_ORDER = {
  { 95, UnlockType::Cube},   { 24, UnlockType::Ball},  {  9, UnlockType::Robot},  { 29, UnlockType::Ship},   {  2, UnlockType::Death},  // fire
  { 84, UnlockType::Cube},   { 26, UnlockType::Ball},  {  6, UnlockType::Spider}, { 31, UnlockType::Ship},   {  9, UnlockType::Death},  // ice
  { 22, UnlockType::Ball},   { 20, UnlockType::Bird},  {105, UnlockType::Cube},   {  5, UnlockType::Spider}, {  4, UnlockType::Death},  // poison
  { 20, UnlockType::Dart},   { 28, UnlockType::Ship},  { 26, UnlockType::Bird},   { 17, UnlockType::Robot},  {  5, UnlockType::Death},  // shadow
  { 14, UnlockType::Dart},   { 92, UnlockType::Cube},  {  7, UnlockType::Robot},  {  9, UnlockType::Spider}, {  6, UnlockType::Death},  // lava
  { 98, UnlockType::Cube},   { 22, UnlockType::Bird},  { 34, UnlockType::Ship},   { 13, UnlockType::Robot},  {  7, UnlockType::Death},  // tier 1
  { 32, UnlockType::Spider}, {352, UnlockType::Cube},  { 43, UnlockType::Dart},   {156, UnlockType::Ship},   { 54, UnlockType::Robot},  // earth
  {165, UnlockType::Cube},   { 43, UnlockType::Robot}, { 15, UnlockType::Swing},  { 59, UnlockType::Dart},   { 45, UnlockType::Spider}, // blood
  {235, UnlockType::Cube},   { 50, UnlockType::Dart},  {108, UnlockType::Ship},   {117, UnlockType::Bird},   { 25, UnlockType::Spider}, // metal
  { 74, UnlockType::Bird},   { 57, UnlockType::Robot}, { 55, UnlockType::Spider}, {438, UnlockType::Cube},   {102, UnlockType::Ball},   // light
  { 78, UnlockType::Ship},   { 78, UnlockType::Ball},  { 35, UnlockType::Spider}, {395, UnlockType::Cube},   {148, UnlockType::Bird},   // soul
  {388, UnlockType::Cube},   {106, UnlockType::Ball},  { 62, UnlockType::Dart},   { 84, UnlockType::Bird},   { 20, UnlockType::Death},  // tier 2
};
std::unordered_map<PathType, std::vector<std::pair<int, UnlockType>>> PATHS_UNLOCK_ORDER = {
  {PathType::Fire, {
    {213, UnlockType::Cube},
    {48,  UnlockType::Col1},
    {236, UnlockType::Cube},
    {54,  UnlockType::Col1},
    {60,  UnlockType::Dart},
    {51,  UnlockType::Col2},
    {35,  UnlockType::Robot},
    {143, UnlockType::Ship},
    {46,  UnlockType::Dart},
    {55,  UnlockType::Robot},
    {79,  UnlockType::Bird},
    //{19, UnlockType::Swing}
  }},
  {PathType::Ice, {
    {49, UnlockType::Spider},
    {47, UnlockType::Col2},
    {394, UnlockType::Cube},
    {44, UnlockType::Col1},
    {66, UnlockType::Ball},
    {106, UnlockType::Col1},
    {130, UnlockType::Ship},
    {135, UnlockType::Bird},
    {29, UnlockType::Robot},
    {76, UnlockType::Ball},
    {99, UnlockType::Ship},
    //{48, UnlockType::Robot}
  }},
  {PathType::Poison, {
    {274, UnlockType::Cube},
    {73, UnlockType::Col2},
    {42, UnlockType::Robot},
    {79, UnlockType::Col2},
    {8, UnlockType::Swing},
    {80, UnlockType::Col1},
    {276, UnlockType::Cube},
    {57, UnlockType::Spider},
    {121, UnlockType::Bird},
    {110, UnlockType::Ball},
    {342, UnlockType::Cube},
    //{146, UnlockType::Bird}
  }},
  {PathType::Shadow, {
    {188, UnlockType::Cube},
    {93, UnlockType::Col1},
    {232, UnlockType::Cube},
    {99, UnlockType::Col2},
    {36, UnlockType::Spider},
    {96, UnlockType::Col2},
    {141, UnlockType::Ship},
    {72, UnlockType::Dart},
    {61, UnlockType::Ball},
    {385, UnlockType::Cube},
    {127, UnlockType::Ship},
    //{40, UnlockType::Robot}
  }},
  {PathType::Lava, {
    {60, UnlockType::Ship},
    {56, UnlockType::Col2},
    {239, UnlockType::Cube},
    {51, UnlockType::Col1},
    {47, UnlockType::Bird},
    {57, UnlockType::Col1},
    {429, UnlockType::Cube},
    {47, UnlockType::Robot},
    {79, UnlockType::Ball},
    {221, UnlockType::Cube},
    {28, UnlockType::Spider},
    //{94, UnlockType::Ship}
  }},
  {PathType::Earth, {
    {227, UnlockType::Cube},
    {65, UnlockType::Col1},
    {79, UnlockType::Ship},
    {68, UnlockType::Col2},
    {102, UnlockType::Bird},
    {45, UnlockType::Col1},
    {32, UnlockType::Robot},
    {53, UnlockType::Dart},
    {77, UnlockType::Ball},
    {23, UnlockType::Spider},
    {247, UnlockType::Cube},
    //{91, UnlockType::Ship}
  }},
  {PathType::Blood, {
    {431, UnlockType::Cube},
    {54, UnlockType::Col2},
    {92, UnlockType::Ball},
    {53, UnlockType::Col2},
    {23, UnlockType::Swing},
    {57, UnlockType::Col2},
    {81, UnlockType::Dart},
    {53, UnlockType::Spider},
    {127, UnlockType::Bird},
    {237, UnlockType::Cube},
    {83, UnlockType::Ball},
    //{98, UnlockType::Ship}
  }},
  {PathType::Metal, {
    {217, UnlockType::Cube},
    {102, UnlockType::Col1},
    {51, UnlockType::Spider},
    {94, UnlockType::Col2},
    {105, UnlockType::Ball},
    {91, UnlockType::Col2},
    {150, UnlockType::Ship},
    {85, UnlockType::Bird},
    {299, UnlockType::Cube},
    {56, UnlockType::Robot},
    {44, UnlockType::Spider},
    //{359, UnlockType::Cube}
  }},
  {PathType::Light, {
    {93, UnlockType::Ball},
    {71, UnlockType::Col2},
    {415, UnlockType::Cube},
    {105, UnlockType::Col1},
    {20, UnlockType::Spider},
    {62, UnlockType::Col2},
    {162, UnlockType::Ship},
    {84, UnlockType::Dart},
    {42, UnlockType::Spider},
    {329, UnlockType::Cube},
    {114, UnlockType::Ship},
    //{66, UnlockType::Robot}
  }},
  {PathType::Souls, {
    {172, UnlockType::Cube},
    {95, UnlockType::Col1},
    {60, UnlockType::Robot},
    {100, UnlockType::Col2},
    {178, UnlockType::Cube},
    {103, UnlockType::Col1},
    {39, UnlockType::Robot},
    {58, UnlockType::Dart},
    {93, UnlockType::Bird},
    {68, UnlockType::Ball},
    {24, UnlockType::Spider},
    //{36, UnlockType::Swing}
  }}
};

std::unordered_map<SortType, std::string> SORT_TYPE_NAMES = {
  {SortType::Number,     "Number"},
  {SortType::LockStatus, "Lock Status"},
  {SortType::Category,   "Category"},
  {SortType::Author,     "Author"},
};
