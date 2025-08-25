#include <Geode/Geode.hpp>

#pragma warning(push)
#pragma warning(disable: 4265)  // enum exhaustiveness
#include <Geode/modify/GJGarageLayer.hpp>
#pragma warning(pop)

#include <algorithm>

#include <utils/LinkedCCMenu.hpp>
#include <constants.hpp>
#include <iconkit.hpp>
#include <logic.hpp>
#include <popups/OptionsPopup.hpp>
#include <popups/FilterAndSortPopup.hpp>

#pragma warning(push)
#pragma warning(disable: 4061)  // enum exhaustiveness
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#pragma warning(pop)

using namespace geode::prelude;

// these two should probably be part of a different mod, and maybe make the second one toggleable for people who actually like that bug
struct OtherHookedGJGarageLayer : Modify<OtherHookedGJGarageLayer, GJGarageLayer> {

  static void onModify(auto& self) {
    // this is a reimplementation to fix a bug with robtop improperly calculating the pages, make sure it runs last
    Result<> result = self.setHookPriority("GJGarageLayer::onArrow", INT_MAX-1);
    if (!result) log::error("Failed to set hook priority, arrows may not work correctly.");
    result = self.setHookPriorityBefore("GJGarageLayer::showUnlockPopup", "colon.lite_ad_chests");
    if (!result && *result.err() != "Mod not found") log::error("Failed to set hook priority, unlock dialogue may show up multiple times.");
  }

  void onArrow(CCObject *sender) {
    CCMenuItemSpriteExtra *arrow = static_cast<CCMenuItemSpriteExtra *>(sender);
    // robtop forgot to subtract 1 here, basically; if the icon count is, say, 36, robtop would just divide by 36,
    // and get that the max page is 1, when, in fact, 36 icons fit onto page 0
    // so there would be a page 1 that is just blank
    int maxPage = (GameManager::get()->countForType(m_iconType)-1)/36;
    int page = m_iconPages[m_iconType] + arrow->getTag();
    page = page > maxPage ? 0 : page < 0 ? maxPage : page;
    setupPage(page, m_iconType);
  }

  void showUnlockPopup(int itemID, UnlockType unlockType) {
    if ((itemID == 13 || itemID == 71) && unlockType == UnlockType::Cube && GameManager::get()->isIconUnlocked(itemID, IconType::Cube)) {
      ItemInfoPopup::create(itemID, UnlockType::Cube)->show();
    } else {
      GJGarageLayer::showUnlockPopup(itemID, unlockType);
    }
  }

};

struct HookedGJGarageLayer : Modify<HookedGJGarageLayer, GJGarageLayer> {

  static void onModify(auto& self) {
    // i should probably be using Priority::First and Priority::Replace here, but some mods still use raw numbers
    // and i don't want it to break with them, so i will hold off until a gd update to change the priorities to proper ones

    // these toggle the logic for when we should be messing with icon order, so make sure they toggle it on first, and toggle it off last
    Result<> result = self.setHookPriority("GJGarageLayer::onArrow", INT_MIN);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
    result = self.setHookPriority("GJGarageLayer::onNavigate", INT_MIN);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
    result = self.setHookPriority("GJGarageLayer::selectTab", INT_MIN);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
    result = self.setHookPriority("GJGarageLayer::init", INT_MIN);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
    
    // these may skip calling the original entirely (holding shift shows the item info popup, regardless of select status),
    // so make sure they don't unintentionally call any hooks if we don't intend to call the original
    result = self.setHookPriority("GJGarageLayer::onSelect", INT_MIN);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
    result = self.setHookPriority("GJGarageLayer::onToggleItem", INT_MIN);
    if (!result) log::error("Failed to set hook priority, some mods may not run their functions.");
    
    // this reimplements original
    result = self.setHookPriority("GJGarageLayer::getItems", INT_MAX-1);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
    
    // ensure we are in control which page gets passed to setupPage
    result = self.setHookPriority("GJGarageLayer::setupPage", INT_MIN);
    if (!result) log::error("Failed to set hook priority, Garage Utilities may act weird.");
  }
    
  void onArrow(CCObject *sender) {
    iconKitState.shouldChangeIcons += 1;
    GJGarageLayer::onArrow(sender);
    iconKitState.shouldChangeIcons -= 1;
  }
  
  void onNavigate(CCObject *sender) {
    iconKitState.shouldChangeIcons += 1;
    GJGarageLayer::onNavigate(sender);
    iconKitState.shouldChangeIcons -= 1;
  }

  void selectTab(IconType iconType) {
    iconKitState.shouldChangeIcons += 1;
    GJGarageLayer::selectTab(iconType);
    iconKitState.shouldChangeIcons -= 1;
  }

  void onToggleItem(CCObject *sender) {
    if (Mod::get()->getSettingValue<bool>("disable-shift-clicking-on-icons")) return GJGarageLayer::onToggleItem(sender);
    CCMenuItemSpriteExtra *item = static_cast<CCMenuItemSpriteExtra *>(sender);
    int itemID = item ? item->getTag() : -1;
    // hold shift when clicking to always open the animation popup, rather than selecting it, even if the animation is unlocked
    // unlocking it prevents you from opening the popup at all in vanilla (there is no reason for you to open it anyway but lol)
    if (itemID != -1 && CCDirector::get()->getKeyboardDispatcher()->getShiftKeyPressed())
      showUnlockPopup(itemID, UnlockType::GJItem);
    else GJGarageLayer::onToggleItem(sender);
  }

  void onSelect(CCObject *sender) {
    if (Mod::get()->getSettingValue<bool>("disable-shift-clicking-on-icons")) return GJGarageLayer::onSelect(sender);

    CCMenuItemSpriteExtra *item = static_cast<CCMenuItemSpriteExtra *>(sender);
    int itemID = item ? item->getTag() : -1;

    // hold shift when clicking an icon to always open the icon popup, rather than selecting it, even if the icon is unlocked and unselected
    if (!CCDirector::get()->getKeyboardDispatcher()->getShiftKeyPressed()) return GJGarageLayer::onSelect(sender);
    
    std::string name = MoreIcons::getIconName(item);
    if (!name.empty()) {
      FLAlertLayer* popup = MoreIcons::createInfoPopup(name, item->m_iconType);
      if (popup) popup->show();
    } else {
      showUnlockPopup(itemID, ICON_TO_UNLOCK[item->m_iconType]);
    }
  }
  

  CCArray* getItems(int maxIconCount, int page, IconType iconType, int activeIcon) {
    if (!SHOULD_CHANGE_ICON_TYPE(iconType) || !iconKitState.shouldChangeIcons)
      return GJGarageLayer::getItems(maxIconCount, page, iconType, activeIcon);

    m_iconPages[iconType] = page;
    
    GameManager* gameManager = GameManager::get();
    UnlockType unlockType = ICON_TO_UNLOCK[iconType];
    CCArray* finalArray = CCArray::create();
    CCSize playerSquareContentSize = CCSprite::createWithSpriteFrameName("playerSquare_001.png")->getContentSize();
    int finalIcon = std::min(maxIconCount, (page+1)*36);
    
    m_currentIcon = nullptr;
    for (int currentIcon = page*36 + 1; currentIcon <= finalIcon; currentIcon++) {
      int currentIconDisplay = positionToDisplay(unlockType, currentIcon);
      // this means we are separating accepted from denied, and we reached the padding space between those 2 sections
      if (currentIconDisplay == 0) break;

      bool isUnlocked = gameManager->isIconUnlocked(currentIconDisplay, iconType);
      float iconScale = GJItemIcon::scaleForType(unlockType);
      GJItemIcon* browserItem = GJItemIcon::createBrowserItem(unlockType, currentIconDisplay);
      browserItem->setScale(iconScale);
      CCMenuItemSpriteExtra* menuItem = CCMenuItemSpriteExtra::create(browserItem, nullptr, this, menu_selector(GJGarageLayer::onSelect));

      menuItem->setContentSize(playerSquareContentSize);
      browserItem->setPosition(playerSquareContentSize/2);
      menuItem->setTag(currentIconDisplay);
      menuItem->m_iconType = iconType;

      finalArray->addObject(menuItem);
      if (currentIconDisplay == activeIcon) m_currentIcon = menuItem;
      if (!isUnlocked) browserItem->changeToLockedState(1.0 / iconScale);
    }

    return finalArray;

  }

  void defaultToggleNavigationMenus(IconType iconType) {
    int vanillaIconCount = GameManager::get()->countForType(iconType);
    int vanillaPageCount = (vanillaIconCount + 35)/36;

    int moreIconsIconCount = int(MoreIcons::getIcons(iconType).size());
    int moreIconsPageCount = (moreIconsIconCount + 35)/36;
    
    int totalIconCount = vanillaIconCount + moreIconsIconCount;
    int totalPageCount = vanillaPageCount + moreIconsPageCount;

    bool hideNavDotMenu = iconKitState.settings.hideNavigationMenuOnSinglePage ? (totalPageCount > 1) && (totalIconCount != 0) : (totalIconCount != 0);
    bool hideArrows = totalPageCount > 1;

    toggleNavigationMenus(hideNavDotMenu, hideArrows);
  }

  void setupPage(int page, IconType iconType) {
    if (iconKitState.stubOutSetupPageOnce) {
      iconKitState.stubOutSetupPageOnce = false;
      return;
    }
    if (!SHOULD_CHANGE_ICON_TYPE(iconType)) {
      GJGarageLayer::setupPage(page, iconType);
      // show pages and arrows only if there are More Icons pages, hide otherwise, which is vanilla behavior;
      // we're not modifying the sorting order of these anyway, at least not now, in the future if we add More Icons filtering/sorting,
      // the special page (and death effects, if MI implements them) may be filterable/sortable, even if it's just More Icons icons
      // that we are filtering/sorting
      int moreIconsIcons = int(MoreIcons::getIcons(m_iconType).size());
      int moreIconsPages = (moreIconsIcons + 35)/36;
      toggleNavigationMenus(bool(moreIconsPages), bool(moreIconsPages));
      return;
    }
    iconKitState.shouldChangeIcons += 1;
    
    // instead of overriding GameManager::activeIconForType to get setupPage to pick the correct page, never even pass -1 into setupPage to begin with
    // if the active icon has been filtered out, page resolves to 0; this is the only sensible first option if there is no active icon anyway
    page = page == -1 ? getActiveIconPage(iconType) : page;
    GJGarageLayer::setupPage(page, iconType);
    
    recalculateNavdotMenu(page, iconType);
    defaultToggleNavigationMenus(iconType);
    
    iconKitState.shouldChangeIcons -= 1;
  }

  void toggleNavigationMenus(bool isNavDotMenuVisible, bool areArrowsVisible) {
    m_navDotMenu->setVisible(isNavDotMenuVisible);
    m_navDotMenu->setEnabled(isNavDotMenuVisible);
    // hide this, i am handling it myself because:
    // - i want the more icons nav menu to be centered if there are no vanilla icons
    // - i want the page buttons to be in the same menu, so you can transfer touches
    if (CCMenu* moreIconsNavDotMenu = typeinfo_cast<CCMenu*>(getChildByID("hiimjustin000.more_icons/navdot-menu"))) {
      moreIconsNavDotMenu->setVisible(false);
      moreIconsNavDotMenu->setEnabled(false);
    }
    m_leftArrow->setVisible(areArrowsVisible);
    m_leftArrow->setEnabled(areArrowsVisible);
    m_rightArrow->setVisible(areArrowsVisible);
    m_rightArrow->setEnabled(areArrowsVisible);
  }

  void recalculateNavdotMenu(int currentPage, IconType iconType) {

    UnlockType unlockType = ICON_TO_UNLOCK[iconType];
    
    size_t acceptedCount = iconKitState.acceptedIcons[unlockType].size();
    size_t deniedCount = iconKitState.deniedIcons[unlockType].size();
    size_t lastAcceptedPage = acceptedCount ? (acceptedCount - 1)/36 : SIZE_MAX;
    size_t firstDeniedPage = (
      !iconKitState.settings.showDenied ?
      SIZE_MAX : (
        iconKitState.settings.separateAcceptedFromDenied ?
        (acceptedCount ? lastAcceptedPage + 1 : (deniedCount ? 0 : SIZE_MAX)) :
        (deniedCount ? acceptedCount/36 : SIZE_MAX)
      )
    );
    
    size_t vanillaPageCount = size_t((GameManager::get()->countForType(iconType)+35)/36);

    int moreIconsIcons = int(MoreIcons::getIcons(m_iconType).size());
    int moreIconsPageCount = (moreIconsIcons + 35)/36;

    m_navDotMenu->removeAllChildren();
    m_pageButtons->removeAllObjects();
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    // set the gap to 0, and then manually set every per-element gap to 6.f
    // axis layout doesn't let you specifically set a cross axis gap because fuck you
    // it just uses the layout gap regardless, i think it even ignores the per-element prev/next gap
    m_navDotMenu->setLayout(
      RowLayout::create()->setGap(0.f)->setAxisAlignment(AxisAlignment::Center)->setGrowCrossAxis(true)
    );

    for (size_t i = 0; i < vanillaPageCount + moreIconsPageCount; i++) {
      std::string_view spriteString;
      if (int(i) == currentPage) {
        // in the future, if we support sorting and filtering More Icons icons as well, do the same thing as below, but for custom icons
        if (i >= vanillaPageCount) spriteString = "gj_navDotBtn_on_001.png";
        // otherwise, do this for vanilla pages
        else if (firstDeniedPage == lastAcceptedPage && i == lastAcceptedPage && !iconKitState.settings.dontRecolorNavigationButtons) spriteString = "GU_navDotButton_mixed.png"_spr;
        else if (i >= firstDeniedPage && !iconKitState.settings.dontRecolorNavigationButtons) spriteString = "GU_navDotButton_denied.png"_spr;
        else spriteString = "gj_navDotBtn_on_001.png";
      } else spriteString = "gj_navDotBtn_off_001.png";
      CCSprite *sprite = CCSprite::createWithSpriteFrameName(spriteString.data());
      sprite->setScale(0.9f);
      CCMenuItemSpriteExtra *button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(GJGarageLayer::onNavigate));
      button->setTag(int(i));
      m_navDotMenu->addChild(button);
      m_pageButtons->addObject(button);

      float gap = 6.f;
      bool breakLine = i == vanillaPageCount - 1;
      if (i == firstDeniedPage && firstDeniedPage && iconKitState.settings.separateAcceptedFromDenied && !iconKitState.settings.noGapBetweenAcceptedAndDenied)
        // if there was another button between those 2 buttons, and that button was invisible, this is what the gap between those 2 buttons would look like
        gap = button->getContentWidth() + 2*gap;

      button->setLayoutOptions(AxisLayoutOptions::create()->setPrevGap(gap)->setBreakLine(breakLine));
    }
    m_navDotMenu->updateLayout();
    m_navDotMenu->setPositionY(25.f);

    // more icons tries getting the first page, since that always exists in vanilla
    // it may not exist with this mod, however, so put a fake one in, it won't be visible anyway
    // since it's not in the actual menu
    if (m_pageButtons->count() == 0 && Loader::get()->isModLoaded("hiimjustin000.more_icons")) {
      m_pageButtons->addObject(CCMenuItemSpriteExtra::create(CCSprite::create(), nullptr, nullptr));
    }
  }
  
  void onPlayerOptions(CCObject *) {
    OptionsPopup::createWithPlayerOptions()->show();
  }
  
  void onFilterAndSort(CCObject *) {
    FilterAndSortPopup::create()->show();
  }

  void createPlayerOptionsButton() {
    CCNode* backMenu = getChildByID("back-menu");
    if (!backMenu) return;
    CCMenuItemSpriteExtra* playerOptionsButton = CCMenuItemSpriteExtra::create(
      CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png"),
      this,
      menu_selector(HookedGJGarageLayer::onPlayerOptions)
    );
    playerOptionsButton->setID("player-options-button"_spr);
    backMenu->addChild(playerOptionsButton);
    backMenu->updateLayout();
  }

  void createFilterAndSortButton() {
    CCNode* shardsMenu = getChildByID("shards-menu");
    if (!shardsMenu) return;

    CCSprite* filterIcon = CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png");
    CCSprite* sortIcon = CCSprite::createWithSpriteFrameName("GJ_sortIcon_001.png");
    filterIcon->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits(1/3.0f, {}));
    sortIcon->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits(1/3.0f, {}));
    CCNode* filterAndSortCollection = CCNode::create();

    filterAndSortCollection->setLayout(RowLayout::create()->setAutoScale(true)->setGap(0));
    filterAndSortCollection->addChild(filterIcon);

    filterAndSortCollection->addChild(sortIcon);

    // ensure we have a content size before creating the based button sprite, as it rescales stuff automatically to fit into the button
    filterAndSortCollection->setContentSize({30, 30});
    filterAndSortCollection->setAnchorPoint({0.5, 0.5});
    filterAndSortCollection->updateLayout();
    
    CircleButtonSprite* filterAndSortSprite = CircleButtonSprite::create(
        filterAndSortCollection, CircleBaseColor::Green, CircleBaseSize::Small
    );

    CCMenuItemSpriteExtra* filterAndSort = CCMenuItemSpriteExtra::create(filterAndSortSprite, this, menu_selector(HookedGJGarageLayer::onFilterAndSort));
    filterAndSort->setID("filter-and-sort-button"_spr);
    
    shardsMenu->addChild(filterAndSort);
    shardsMenu->updateLayout();
  }

  void removeDiamondShards() {
    if (CCNode* diamondShardsIcon = getChildByID("diamond-shards-icon")) diamondShardsIcon->setVisible(false);
    if (CCNode* diamondShardsLabel = getChildByID("diamond-shards-label")) diamondShardsLabel->setVisible(false);
    CCNode* statMenu = getChildByID("capeling.garage-stats-menu/stats-menu");
    if (!statMenu) return;
    CCNode* shardsContainer = statMenu->getChildByID("diamond-shards-container");
    if (!shardsContainer) return;
    shardsContainer->removeFromParent();
    statMenu->updateLayout();
  }

  bool init() override {
    recalculateIconOrder();

    iconKitState.shouldChangeIcons += 1;

    if (!GJGarageLayer::init()) { iconKitState.shouldChangeIcons -= 1; return false; }

    recalculateNavdotMenu(m_iconPages[m_iconType], m_iconType);
    defaultToggleNavigationMenus(m_iconType);

    iconKitState.shouldChangeIcons -= 1;

    if (!Mod::get()->getSettingValue<bool>("hide-player-options-button")) createPlayerOptionsButton();
    if (!Mod::get()->getSettingValue<bool>("hide-filter-and-sort-button")) createFilterAndSortButton();
    if (Mod::get()->getSettingValue<bool>("remove-diamond-shards")) removeDiamondShards();
    
    return true;
  }
};
