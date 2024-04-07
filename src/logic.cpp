#include <Geode/Geode.hpp>
#include <algorithm>
#include <constants.hpp>
#include <logic.hpp>
#include <iconkit.hpp>

using namespace geode::prelude;

#define CEILDIV(x, y) (x/y + (x%y != 0))

std::optional<bool> compareByNumber(unsigned icon1, unsigned icon2, UnlockType) {
  return icon1 < icon2;
}

std::optional<bool> compareByType(unsigned icon1, unsigned icon2, UnlockType unlockType) {
  if ((icon1 > VANILLA_MAX_ICONS[unlockType]) == (icon2 > VANILLA_MAX_ICONS[unlockType])) return {};
  return icon1 <= VANILLA_MAX_ICONS[unlockType];
}

std::optional<bool> compareByLockStatus(unsigned icon1, unsigned icon2, UnlockType unlockType) {
  GameManager *gm = GameManager::get();
  bool icon1Unlocked = gm->isIconUnlocked(icon1, UNLOCK_TO_ICON[unlockType]);
  bool icon2Unlocked = gm->isIconUnlocked(icon2, UNLOCK_TO_ICON[unlockType]);
  if (icon1Unlocked != icon2Unlocked) return icon1Unlocked && !icon2Unlocked;
  return std::nullopt;
}

std::optional<bool> compareByCategory(unsigned icon1, unsigned icon2, UnlockType unlockType) {
  // if either icon is custom, we can't compare by category
  if (icon1 > VANILLA_MAX_ICONS[unlockType] || icon2 > VANILLA_MAX_ICONS[unlockType]) return std::nullopt;

  std::string icon1Category = CATEGORY_FOR_ICON[unlockType][icon1];
  std::string icon2Category = CATEGORY_FOR_ICON[unlockType][icon2];
  // if the items are of different categories
  if (icon1Category != icon2Category) return CATEGORY_TO_ORDER[icon1Category] < CATEGORY_TO_ORDER[icon2Category];

  // category can technically be sorted in multiple different ways
  // non-strict means that elements of the same category have undefined sorting
  // strict means that the order they appear in the category defines the sorting
  //   the icons in categories have been hand sorted by progression (e.g. 100 stars, 200 stars, etc.)
  //   as opposed to just using the icon number
  if (!iconKitState.settings.strictCategory) return std::nullopt;

  // if the items are the same category, the order of the items in the category defines the sorting order in strict mode
  //auto icon1CategoryIndex = std::find(CATEGORIES[unlockType][icon1Category].begin(), CATEGORIES[unlockType][icon1Category].end(), icon1);
  //auto icon2CategoryIndex = std::find(CATEGORIES[unlockType][icon2Category].begin(), CATEGORIES[unlockType][icon2Category].end(), icon2);
  //return icon1CategoryIndex < icon2CategoryIndex;
  //
  return CATEGORY_INDEX_FOR_ICON[unlockType][icon1] < CATEGORY_INDEX_FOR_ICON[unlockType][icon2];
}

std::optional<bool> compareByAuthor(unsigned icon1, unsigned icon2, UnlockType unlockType) {
  // if either icon is custom, we can't compare by author
  if (icon1 > VANILLA_MAX_ICONS[unlockType] || icon2 > VANILLA_MAX_ICONS[unlockType]) return {};

  int icon1Author = AUTHOR_FOR_ICON[unlockType][icon1];
  int icon2Author = AUTHOR_FOR_ICON[unlockType][icon2];
  // if the authors are different sort them by their account id, except for green authors, which come last
  if (icon1Author != icon2Author) return AUTHOR_TO_ORDER[icon1Author] < AUTHOR_TO_ORDER[icon2Author];
  return {};
}

bool compareIcons(unsigned icon1, unsigned icon2) {
  UnlockType unlockType = iconKitState.comparison;
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    std::optional<bool> result;
    switch(iconKitState.settings.sortBy[i]) {
      case SortType::Number:     result =     compareByNumber(icon1, icon2, unlockType); break;
      case SortType::LockStatus: result = compareByLockStatus(icon1, icon2, unlockType); break;
      case SortType::Author:     result =     compareByAuthor(icon1, icon2, unlockType); break;
      case SortType::Category:   result =   compareByCategory(icon1, icon2, unlockType); break;
      case SortType::Type:       result =       compareByType(icon1, icon2, unlockType); break;
    }
    if (result) return iconKitState.settings.sortIsReverse[i] ^ *result;
  }
  return true;
}

void recalculateIconOrder() {
  GameManager *gm = GameManager::get();
  for (UnlockType unlockType : UNLOCK_TYPES_TO_CHANGE) {
    unsigned count = countForType(UNLOCK_TO_ICON[unlockType]);
    iconKitState.acceptedIcons[unlockType].clear();
    iconKitState.deniedIcons[unlockType].clear();
    for (size_t i = 1; i <= count; i++) {
      std::vector<unsigned> *accepted = iconKitState.settings.invert ? &iconKitState.deniedIcons[unlockType] : &iconKitState.acceptedIcons[unlockType];
      std::vector<unsigned> *denied = iconKitState.settings.invert ? &iconKitState.acceptedIcons[unlockType] : &iconKitState.deniedIcons[unlockType];

      bool isUnlocked = gm->isIconUnlocked(i, UNLOCK_TO_ICON[unlockType]);
      bool isVanilla = i <= VANILLA_MAX_ICONS[unlockType];

      bool passesIconTypeFilter = isVanilla && iconKitState.settings.vanilla || !isVanilla && iconKitState.settings.custom;
      if (!passesIconTypeFilter) { denied->push_back(i); continue; }

      bool passesStateFilter = isUnlocked && iconKitState.settings.unlocked || !isUnlocked && iconKitState.settings.locked;
      if (!passesStateFilter) { denied->push_back(i); continue; }

      // only the above 2 are applicable for custom icons, so if it passes them it's accepted
      if (!isVanilla) { accepted->push_back(i); continue; }

      bool passesCategoryFilter = iconKitState.settings.categories[CATEGORY_FOR_ICON[unlockType][i]];
      if (!passesCategoryFilter) { denied->push_back(i); continue; }

      bool passesAuthorFilter = iconKitState.settings.authors[AUTHOR_FOR_ICON[unlockType][i]];
      if (!passesAuthorFilter) { denied->push_back(i); continue; }
      
      accepted->push_back(i);

    }
    iconKitState.comparison = unlockType;
    std::sort(iconKitState.acceptedIcons[unlockType].begin(), iconKitState.acceptedIcons[unlockType].end(), compareIcons);
    std::sort(iconKitState.deniedIcons[unlockType].begin(), iconKitState.deniedIcons[unlockType].end(), compareIcons);
  }
}

unsigned positionToDisplay(UnlockType unlockType, unsigned item) {

  // we are showing accepted icons
  if (item <= iconKitState.acceptedIcons[unlockType].size())
    return iconKitState.acceptedIcons[unlockType][item-1];

  // number of non-existent icons between the last accepted page and first denied page
  unsigned delta = 0;
  if (iconKitState.settings.separateAcceptedFromDenied)
    delta = 36 * CEILDIV(iconKitState.acceptedIcons[unlockType].size(), 36) - iconKitState.acceptedIcons[unlockType].size();

  unsigned newIndex = item - delta - iconKitState.acceptedIcons[unlockType].size() - 1;
  if (newIndex >= iconKitState.deniedIcons[unlockType].size()) return 0;
  if (newIndex < 0) return 0;

  return iconKitState.deniedIcons[unlockType][newIndex];
  
}

unsigned displayToPosition(UnlockType unlockType, unsigned item) {
  auto p = std::find(iconKitState.acceptedIcons[unlockType].begin(), iconKitState.acceptedIcons[unlockType].end(), item);
  if (p != iconKitState.acceptedIcons[unlockType].end()) return p - iconKitState.acceptedIcons[unlockType].begin() + 1;
  
  unsigned delta = 0;
  if (iconKitState.settings.separateAcceptedFromDenied)
    delta = 36 * CEILDIV(iconKitState.acceptedIcons[unlockType].size(), 36) - iconKitState.acceptedIcons[unlockType].size();

  p = std::find(iconKitState.deniedIcons[unlockType].begin(), iconKitState.deniedIcons[unlockType].end(), item);
  if (iconKitState.settings.showDenied && p != iconKitState.deniedIcons[unlockType].end())
    return p - iconKitState.deniedIcons[unlockType].begin() + iconKitState.acceptedIcons[unlockType].size() + delta + 1;

  return 0;
}

#ifdef GEODE_IS_ANDROID
int countForType(IconType iconType) { return GameManager::get()->countForType(iconType); }
#else
int countForType(IconType iconType) { return GameManager::countForType(iconType); }
#endif

// hooking countForType is broken, i would prefer to just be able to do that because
// then i wouldn't have to redo the entire navdot menu every time a page is set up
// since countForType is what gd uses to do that on its own, but oh well
int fakeCountForType(IconType iconType, unsigned currentPage) {
  UnlockType unlockType = ICON_TO_UNLOCK[iconType];
  // if we are only showing accepted icons, that is the max count of icons
  // if we are separating accepted from denied icons, the max icon count needs to be reported as
  //   the accepted icon count if we are on an accepted page
  //   the accepted icon count rounded up to fill its last page + the denied icon count
  //     this is to make GD stop drawing icons further on the last accepted page,
  //     but keep drawing them up until the actual end on the denied pages
  // if we are not separating accepted from denied icons,
  // then the max icon count is always the sum of the accepted and denied icons
  // which is the max icon count you get from the game anyway, so nothing should be changed
  unsigned lastAcceptedPage = (iconKitState.acceptedIcons[unlockType].size()-1)/36;
  if (iconKitState.acceptedIcons[unlockType].empty()) lastAcceptedPage = UINT_MAX;

  if (!iconKitState.settings.showDenied)
    return iconKitState.acceptedIcons[unlockType].size();
  else if (iconKitState.settings.separateAcceptedFromDenied && currentPage <= lastAcceptedPage)
    return iconKitState.acceptedIcons[unlockType].size();
  else if (iconKitState.settings.separateAcceptedFromDenied && currentPage > lastAcceptedPage)
    return 36 * CEILDIV(iconKitState.acceptedIcons[unlockType].size(), 36) + iconKitState.deniedIcons[unlockType].size();
  return countForType(iconType);
}
