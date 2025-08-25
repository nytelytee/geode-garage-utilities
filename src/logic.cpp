#include <Geode/Geode.hpp>
#include <algorithm>
#include <constants.hpp>
#include <logic.hpp>
#include <iconkit.hpp>
#pragma warning(push)
#pragma warning(disable: 4061)  // enum exhaustiveness
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#pragma warning(pop)


using namespace geode::prelude;

std::optional<bool> compareByNumber(int icon1, int icon2, UnlockType) {
  return icon1 < icon2;
}

std::optional<bool> compareByLockStatus(int icon1, int icon2, UnlockType unlockType) {
  GameManager* gameManager = GameManager::get();
  bool icon1Unlocked = gameManager->isIconUnlocked(icon1, UNLOCK_TO_ICON[unlockType]);
  bool icon2Unlocked = gameManager->isIconUnlocked(icon2, UNLOCK_TO_ICON[unlockType]);
  if (icon1Unlocked != icon2Unlocked) return icon1Unlocked && !icon2Unlocked;
  return std::nullopt;
}

std::optional<bool> compareByCategory(int icon1, int icon2, UnlockType unlockType) {
  std::string icon1Category = CATEGORY_FOR_ICON[unlockType][icon1];
  std::string icon2Category = CATEGORY_FOR_ICON[unlockType][icon2];
  // if the items are of different categories, sort the categories themselves
  if (icon1Category != icon2Category) return CATEGORY_TO_ORDER[icon1Category] < CATEGORY_TO_ORDER[icon2Category];

  // a category can technically be sorted in multiple different ways:

  // non-strict means that elements of the same category have undefined sorting
  if (!iconKitState.settings.strictCategory) return std::nullopt;

  // strict means that the order they appear in the category defines the sorting
  // the icons in categories have been hand-sorted by progression (e.g. 100 stars, 200 stars, etc.)
  // as opposed to just falling back to the next sorting method (non-strict)
  return CATEGORY_INDEX_FOR_ICON[unlockType][icon1] < CATEGORY_INDEX_FOR_ICON[unlockType][icon2];
}

std::optional<bool> compareByAuthor(int icon1, int icon2, UnlockType unlockType) {
  int icon1Author = AUTHOR_FOR_ICON[unlockType][icon1];
  int icon2Author = AUTHOR_FOR_ICON[unlockType][icon2];
  // if the authors are different sort them by their account id, except for green authors, which come last
  if (icon1Author != icon2Author) return AUTHOR_TO_ORDER[icon1Author] < AUTHOR_TO_ORDER[icon2Author];
  return std::nullopt;
}

bool compareIcons(int icon1, int icon2, UnlockType unlockType) {
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    std::optional<bool> result;
    switch(iconKitState.settings.sortBy[i]) {
      case SortType::Number:     result =     compareByNumber(icon1, icon2, unlockType); break;
      case SortType::LockStatus: result = compareByLockStatus(icon1, icon2, unlockType); break;
      case SortType::Author:     result =     compareByAuthor(icon1, icon2, unlockType); break;
      case SortType::Category:   result =   compareByCategory(icon1, icon2, unlockType); break;
    }
    if (result) return iconKitState.settings.sortIsReverse[i] ^ *result;
  }
  return true;
}

void recalculateIconOrder() {

  iconKitState.settings = iconKitState.pendingSettings;

  GameManager* gameManager = GameManager::get();
  for (UnlockType unlockType : UNLOCK_TYPES_TO_CHANGE) {
    int count = VANILLA_MAX_ICONS[unlockType];
    iconKitState.acceptedIcons[unlockType].clear();
    iconKitState.deniedIcons[unlockType].clear();
    for (int i = 1; i <= count; i++) {
      std::vector<int>* accepted = iconKitState.settings.invert ? &iconKitState.deniedIcons[unlockType] : &iconKitState.acceptedIcons[unlockType];
      std::vector<int>* denied = iconKitState.settings.invert ? &iconKitState.acceptedIcons[unlockType] : &iconKitState.deniedIcons[unlockType];

      bool isUnlocked = gameManager->isIconUnlocked(i, UNLOCK_TO_ICON[unlockType]);

      bool passesStateFilter = (isUnlocked && iconKitState.settings.unlocked) || (!isUnlocked && iconKitState.settings.locked);
      if (!passesStateFilter) { denied->push_back(i); continue; }

      bool passesCategoryFilter = iconKitState.settings.categories[CATEGORY_FOR_ICON[unlockType][i]];
      if (!passesCategoryFilter) { denied->push_back(i); continue; }

      bool passesAuthorFilter = iconKitState.settings.authors[AUTHOR_FOR_ICON[unlockType][i]];
      if (!passesAuthorFilter) { denied->push_back(i); continue; }
      
      accepted->push_back(i);

    }
    std::sort(iconKitState.acceptedIcons[unlockType].begin(), iconKitState.acceptedIcons[unlockType].end(), [unlockType](int a, int b) { return compareIcons(a, b, unlockType); });
    std::sort(iconKitState.deniedIcons[unlockType].begin(), iconKitState.deniedIcons[unlockType].end(), [unlockType](int a, int b) { return compareIcons(a, b, unlockType); });
  }
}

std::tuple<std::optional<int>, std::optional<int>, std::optional<int>> getPageInfoForType(UnlockType unlockType) {

  if (!SHOULD_CHANGE_UNLOCK_TYPE(unlockType)) return {std::nullopt, std::nullopt, std::nullopt};

  int acceptedCount = int(iconKitState.acceptedIcons[unlockType].size());
  int deniedCount = int(iconKitState.deniedIcons[unlockType].size());
  
  std::optional<int> lastAcceptedPage = (acceptedCount != 0) ? std::optional{(acceptedCount - 1)/36} : std::nullopt;
  std::optional<int> firstDeniedPage = (
    !iconKitState.settings.showDenied ?
    std::nullopt : (
      iconKitState.settings.separateAcceptedFromDenied ?
      (acceptedCount != 0 ? std::optional{*lastAcceptedPage + 1} : (deniedCount != 0 ? std::optional{0} : std::nullopt)) :
      (deniedCount != 0 ? std::optional{acceptedCount/36} : std::nullopt)
    )
  );
  std::optional<int> maxVanillaPage = (acceptedCount + (iconKitState.settings.showDenied ? deniedCount : 0) != 0) ? (
    !iconKitState.settings.showDenied ?
    std::optional{(acceptedCount-1)/36} : (
      iconKitState.settings.separateAcceptedFromDenied ?
      std::optional{(((acceptedCount + 35)/36)*36 + deniedCount - 1)/36} :
      std::optional{(acceptedCount + deniedCount - 1)/36}
    )
  ) : std::nullopt;
  
  return {lastAcceptedPage, firstDeniedPage, maxVanillaPage};
  
}

void remainOnSameSectionPageForIconType(
  GJGarageLayer* garage,
  IconType iconType,
  std::optional<int> oldLastAcceptedPage,
  std::optional<int> oldFirstDeniedPage,
  std::optional<int> oldMaxVanillaPage
) {
  // don't change anything if the page hasn't even been initialized yet, just keep it on active icon
  if (garage->m_iconPages[iconType] == -1) return;
  auto [newLastAcceptedPage, newFirstDeniedPage, newMaxVanillaPage] = getPageInfoForType(ICON_TO_UNLOCK[iconType]);
  int currentPage = garage->m_iconPages[iconType];
  int moreIconsIcons = int(MoreIcons::getIcons(iconType).size());

  // 0 = blank, 1 = accepted, 2 = denied, 3 = more icons
  // the mixed page (magenta) counts as an accepted section type when getting the section type (so, if we are on it specifically it counts as section type 1)
  // when doing calculations for which page we should be on, it counts as both an accepted and denied page, however, since it *is* exactly that
  int sectionType = 0;
  if (oldLastAcceptedPage && currentPage <= *oldLastAcceptedPage) sectionType = 1;
  else if (oldFirstDeniedPage && oldMaxVanillaPage && *oldFirstDeniedPage <= currentPage && currentPage <= *oldMaxVanillaPage) sectionType = 2;
  else if ((!oldMaxVanillaPage && moreIconsIcons != 0) || (oldMaxVanillaPage && currentPage > *oldMaxVanillaPage)) sectionType = 3;
  
  // there used to be no vanilla icons, and no more icons icons either, we stay on page 0 regardless
  if (sectionType == 0) {
    garage->m_iconPages[iconType] = 0;
  }
  // there used to be at least some vanilla icons, and we were on some accepted page (or the mixed page)
  if (sectionType == 1) {
    // there are no more accepted page we prefer going to the *previous* section if our section has been exhausted,
    // and there are no previous sections, so we go to the first page we can go to, which is 0
    if (!newLastAcceptedPage) garage->m_iconPages[iconType] = 0;
    
    // if some accepted pages do exist, go to the same page we were already at, but clamp it to the maximum last accepted page
    // if we do not need to clamp anything, then the page remains the same
    else if (currentPage > *newLastAcceptedPage) garage->m_iconPages[iconType] = *newLastAcceptedPage;
  }
  // there used to be at least some vanilla icons, and we were on some denied page
  else if (sectionType == 2) {
    // no more vanilla icons, go to page 0
    if (!newLastAcceptedPage && !newFirstDeniedPage) garage->m_iconPages[iconType] = 0;
    // no more denied icons, but there are accepted icons, go to the last page of accepted icons
    else if (newLastAcceptedPage && !newFirstDeniedPage) garage->m_iconPages[iconType] = *newLastAcceptedPage;
    // there are still some denied icons, clamp the page to the section if it's out of bounds
    else if (newFirstDeniedPage) {
      garage->m_iconPages[iconType] = std::min(currentPage - *oldFirstDeniedPage + *newFirstDeniedPage, *newMaxVanillaPage);
    }
  }
  // we were on a more icons page
  else if (sectionType == 3) {
    // there were no, and still are no vanilla pages, stay on the same more icons page
    if (!newMaxVanillaPage && !oldMaxVanillaPage) {}
    // there did not use to be any vanilla pages, there are now some vanilla pages, keep the same more icons page
    else if (newMaxVanillaPage && !oldMaxVanillaPage) {
      garage->m_iconPages[iconType] = *newMaxVanillaPage + 1 + currentPage;
    }
    // there used to be some vanilla pages, there are now no vanilla pages, keep the same more icons page
    else if (!newMaxVanillaPage && oldMaxVanillaPage) {
      garage->m_iconPages[iconType] = currentPage - *oldMaxVanillaPage - 1;
    }
    else if (newMaxVanillaPage && oldMaxVanillaPage) {
      garage->m_iconPages[iconType] = currentPage - *oldMaxVanillaPage + *newMaxVanillaPage;
    }
  }
}

void recalculateIconOrderAndRemainOnSamePages(GJGarageLayer* garage) {

  recalculateIconOrder();

  for (UnlockType unlockType : UNLOCK_TYPES_TO_CHANGE) {
    // don't change anything if the page hasn't even been initialized yet, just keep it on active icon
    if (garage->m_iconPages[UNLOCK_TO_ICON[unlockType]] == -1) continue;
    int acceptedCount = int(iconKitState.acceptedIcons[unlockType].size());
    int deniedCount = int(iconKitState.deniedIcons[unlockType].size());
    int vanillaPageCount = (
      !iconKitState.settings.showDenied ?
      (acceptedCount + 35)/36 : (
        iconKitState.settings.separateAcceptedFromDenied ?
        (((acceptedCount + 35)/36)*36 + deniedCount + 35)/36 :
        (acceptedCount + deniedCount + 35)/36
      )
    );
    int moreIconsPageCount = (int(MoreIcons::getIcons(UNLOCK_TO_ICON[unlockType]).size()) + 35)/36;
    if (garage->m_iconPages[UNLOCK_TO_ICON[unlockType]] > vanillaPageCount + moreIconsPageCount - 1)
      garage->m_iconPages[UNLOCK_TO_ICON[unlockType]] = vanillaPageCount + moreIconsPageCount - 1;
  }
}

void recalculateIconOrderAndRemainOnSameSectionPages(GJGarageLayer* garage) {
  
  std::map<IconType, std::optional<int>> oldLastAcceptedPages;
  std::map<IconType, std::optional<int>> oldFirstDeniedPages;
  std::map<IconType, std::optional<int>> oldMaxVanillaPages;

  for (IconType iconType : ICON_TYPES_TO_CHANGE) {
    auto [oldLastAcceptedPage, oldFirstDeniedPage, oldMaxVanillaPage] = getPageInfoForType(ICON_TO_UNLOCK[iconType]);
    oldLastAcceptedPages[iconType] = oldLastAcceptedPage;
    oldFirstDeniedPages[iconType] = oldFirstDeniedPage;
    oldMaxVanillaPages[iconType] = oldMaxVanillaPage;
  }

  recalculateIconOrder();
  
  for (IconType iconType : ICON_TYPES_TO_CHANGE)
    remainOnSameSectionPageForIconType(garage, iconType, oldLastAcceptedPages[iconType], oldFirstDeniedPages[iconType], oldMaxVanillaPages[iconType]);
}


void recalculateIconOrderAndGoToActiveIconPages(GJGarageLayer* garage) {
  recalculateIconOrder();
  for (IconType iconType : ICON_TYPES_TO_CHANGE)
    garage->m_iconPages[iconType] = -1;
}

void giveIconAttention(GJGarageLayer* garage, UnlockType unlockType, int itemID) {
  if (!garage->m_iconSelection) return;
  CCMenu* menu;
  if (unlockType == UnlockType::ShipFire) {
    ListButtonBar* newBar = garage->m_iconSelection->getChildByType<ListButtonBar*>(0);
    if (!newBar) return;
    if (!newBar->m_pages->count()) return;
    menu = static_cast<ListButtonPage*>(newBar->m_pages->objectAtIndex(0))->getChildByType<CCMenu*>(0);
  } else if (unlockType == UnlockType::GJItem) {
    menu = garage->m_iconSelection->getChildByType<CCMenu*>(0);
  } else {
    if (!garage->m_iconSelection->m_pages->count()) return;
    menu = static_cast<ListButtonPage*>(garage->m_iconSelection->m_pages->objectAtIndex(0))->getChildByType<CCMenu*>(0);
  }
  CCMenuItemSpriteExtra* menuItem = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByTag(itemID));
  if (!menuItem) return;
  if (menuItem->m_iconType != UNLOCK_TO_ICON[unlockType]) return;
  
  CCSprite* redCursor = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
  redCursor->setColor({255, 0, 0});
  redCursor->setScale(0.85f);
  redCursor->setPosition(menuItem->getChildByType(0)->getPosition());
  menuItem->addChild(redCursor);

  redCursor->runAction(CCSequence::create(
    CCFadeTo::create(0.5, 0), CCFadeTo::create(0.5, 255),
    CCFadeTo::create(0.5, 0), CCFadeTo::create(0.5, 255),
    CCFadeTo::create(0.5, 0), CCFadeTo::create(0.5, 255),
    CCFadeTo::create(0.5, 0), CCRemoveSelf::create(true),
    NULL
  ));
}

bool recalculateIconOrderAndTrackIcon(GJGarageLayer* garage, UnlockType unlockType, int itemDisplay) {
  if (!garage) return false;
  if (!SHOULD_CHANGE_UNLOCK_TYPE(unlockType)) return false;
  
  IconType iconType = UNLOCK_TO_ICON[unlockType];

  int oldItemPosition = displayToPosition(unlockType, itemDisplay);
  auto [oldLastAcceptedPage, oldFirstDeniedPage, oldMaxVanillaPage] = getPageInfoForType(unlockType);
  recalculateIconOrder();
  int newItemPosition = displayToPosition(unlockType, itemDisplay);

  // there was no change to the actual layout of the icons
  if (newItemPosition == oldItemPosition) return false;
  // the act of unlocking the icon has filtered it out, attempt to stay inside the same page section
  else if (newItemPosition == 0 && oldItemPosition != 0) {
    remainOnSameSectionPageForIconType(garage, iconType, oldLastAcceptedPage, oldFirstDeniedPage, oldMaxVanillaPage);
    return true;
  }
  // the act of unlocking the icon has maybe changed its page, jump to the page the icon is at now immediately
  // this is either (also a note to myself to check if i did this correctly):
  //   due to the old item being filtered out, and the new item being shown (newItemPosition != 0 && oldItemPosition == 0) or
  //   due to the old item being shown, and the new item also being shown, but in a different position (newItemPosition != 0 && oldItemPosition != 0 && newItemPosition != oldItemPosition)
  else if (newItemPosition != oldItemPosition) {
    garage->m_iconPages[iconType] = (newItemPosition - 1)/36;
    return true;
  }
  // should be unreachable i think
  return false;

}


int positionToDisplay(UnlockType unlockType, int item) {

  if (item <= 0) return 0;

  // we are showing accepted icons
  if (size_t(item) <= iconKitState.acceptedIcons[unlockType].size())
    return iconKitState.acceptedIcons[unlockType][size_t(item)-1];

  // number of non-existent icons between the last accepted page and first denied page
  int delta = 0;
  if (iconKitState.settings.separateAcceptedFromDenied)
    delta = int(36*((iconKitState.acceptedIcons[unlockType].size()+35)/36) - iconKitState.acceptedIcons[unlockType].size());

  int newIndex = item - delta - int(iconKitState.acceptedIcons[unlockType].size()) - 1;
  if (newIndex < 0) return 0;
  if (size_t(newIndex) >= iconKitState.deniedIcons[unlockType].size()) return 0;

  return iconKitState.deniedIcons[unlockType][size_t(newIndex)];
  
}

int displayToPosition(UnlockType unlockType, int item) {

  if (item == 0) return 0;

  std::vector<int>::iterator p = std::find(iconKitState.acceptedIcons[unlockType].begin(), iconKitState.acceptedIcons[unlockType].end(), item);
  if (p != iconKitState.acceptedIcons[unlockType].end()) return p - iconKitState.acceptedIcons[unlockType].begin() + 1;
  
  int delta = 0;
  if (iconKitState.settings.separateAcceptedFromDenied)
    delta = int(36*((iconKitState.acceptedIcons[unlockType].size()+35)/36) - iconKitState.acceptedIcons[unlockType].size());

  p = std::find(iconKitState.deniedIcons[unlockType].begin(), iconKitState.deniedIcons[unlockType].end(), item);
  if (iconKitState.settings.showDenied && p != iconKitState.deniedIcons[unlockType].end())
    return int(p - iconKitState.deniedIcons[unlockType].begin() + iconKitState.acceptedIcons[unlockType].size() + delta + 1);

  return 0;
}

int adjustedCountForType(IconType iconType) {
  UnlockType unlockType = ICON_TO_UNLOCK[iconType];
  if (!iconKitState.settings.showDenied)
    return int(iconKitState.acceptedIcons[unlockType].size());
  else if (iconKitState.settings.separateAcceptedFromDenied)
    return int(36*((iconKitState.acceptedIcons[unlockType].size()+35)/36) + iconKitState.deniedIcons[unlockType].size());
  return VANILLA_MAX_ICONS[unlockType];
}

int getActiveIconPage(IconType iconType) {
  
  Mod* separateDualIcons = Loader::get()->getLoadedMod("weebify.separate_dual_icons");
  bool secondPlayerSelected = separateDualIcons && separateDualIcons->getSavedValue("2pselected", false);

  if (IconInfo* icon = MoreIcons::getIcon(iconType, secondPlayerSelected)) {
    int moreIconsActiveIconPosition = 36*((GameManager::get()->countForType(iconType) + 35)/36) + int(icon - MoreIcons::getIcons(iconType)[0]) + 1;
    return (moreIconsActiveIconPosition - 1)/36;
  } else if (secondPlayerSelected) {
    int separateDualIconsActiveIconPosition;
    switch(iconType) {
      case    IconType::Cube: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("cube", 1); break;
      case    IconType::Ship: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("ship", 1); break;
      case    IconType::Ball: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("roll", 1); break;
      case     IconType::Ufo: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("bird", 1); break;
      case    IconType::Wave: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("dart", 1); break;
      case   IconType::Robot: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("robot", 1); break;
      case  IconType::Spider: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("spider", 1); break;
      case   IconType::Swing: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("swing", 1); break;
      case IconType::Jetpack: separateDualIconsActiveIconPosition = separateDualIcons->getSavedValue<int>("jetpack", 1); break;

      case IconType::DeathEffect:
      case IconType::Special:
      case IconType::ShipFire:
      case IconType::Item:
      default: return 0;
    }
    return (int(displayToPosition(ICON_TO_UNLOCK[iconType], separateDualIconsActiveIconPosition)) - 1)/36;
  }
  else {
    int activeIconPosition = GameManager::get()->activeIconForType(iconType);
    return (int(displayToPosition(ICON_TO_UNLOCK[iconType], activeIconPosition)) - 1)/36;
  }
}

