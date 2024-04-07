#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <algorithm>

#include <utils/IconButtonSpriteNoText.hpp>
#include <utils/LinkedCCMenu.hpp>
#include <constants.hpp>
#include <iconkit.hpp>
#include <logic.hpp>
#include <popups/OptionsPopup.hpp>
#include <popups/FilterAndSortPopup.hpp>

using namespace geode::prelude;

struct HookedGJGarageLayer : Modify<HookedGJGarageLayer, GJGarageLayer> {
  
  static void onModify(auto& self) {
    // these 3 may sometimes skip running the original, so make sure they're the last function in the chain
    Result<> result = self.setHookPriority("GJGarageLayer::onArrow", INT_MAX-1);
    if (!result) log::error("Failed to set hook priority, some mods may not run their functions.");
    result = self.setHookPriority("GJGarageLayer::onSelect", INT_MAX-1);
    if (!result) log::error("Failed to set hook priority, some mods may not run their functions.");
    result = self.setHookPriority("GJGarageLayer::onToggleItem", INT_MAX-1);
    if (!result) log::error("Failed to set hook priority, some mods may not run their functions.");
    // this should run after Capeling's Demons In Garage 2.2
    // (and potentially other mods that mess with that area, but I'm not checking for those,
    //  if another mod messes with that area, and you would like me to account for it, please open an issue)
    // because it may remove the diamond shards section
    result = self.setHookPriority("GJGarageLayer::init", -1);
    if (!result) log::error("Failed to set hook priority, Demons In Garage 2.2 may be broken.");
  }


  void onArrow(CCObject *sender) {
    CCMenuItemSpriteExtra *arrow = static_cast<CCMenuItemSpriteExtra *>(sender);
    int pageOffset = 2*(arrow->getTag() == 1) - 1;
    int maxPage = (fakeCountForType(iconKitState.selectedIconType, INT_MAX)-1)/36;
    maxPage = maxPage == -1 ? 0 : maxPage;
    int currentPage = iconKitState.pageForIcon[iconKitState.selectedIconType];
    int page = currentPage + pageOffset;
    page = page > maxPage ? 0 : page < 0 ? maxPage : page;
    setupPage(page, iconKitState.selectedIconType);
  }

  void selectTab(IconType iconType) {
    iconKitState.selectedIconType = iconType;
    GJGarageLayer::selectTab(iconType);
    if (!SHOULD_CHANGE_ICON_TYPE(iconType))
      toggleNavigationMenus(false, false);
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
    CCMenuItemSpriteExtra *item = static_cast<CCMenuItemSpriteExtra *>(sender);
    int itemID = item ? item->getTag() : -1;

    UnlockType selectedUnlockType;
    IconType selectedIconType;
    if (iconKitState.selectedIconType == IconType::DeathEffect) {
      selectedUnlockType = UnlockType::Death;
      selectedIconType = IconType::DeathEffect;
    } else if (!SHOULD_CHANGE_ICON_TYPE(iconKitState.selectedIconType)) {
      // if someone reading this knows of a better way to fetch the icon type of a just clicked icon on a special page, please let me know, i hate this
      // these 2 are nested differently, Streaks have 6 parents up until they reach the GJGarageLayer, but ship fires have 7
      // animations have 3, but those don't matter since their callback is not onSelect, but onToggleItem
      if (item->getParent()->getParent()->getParent()->getParent()->getParent()->getParent() == this) {
        selectedUnlockType = UnlockType::Streak;
        selectedIconType = IconType::Special;
      } else {
        selectedUnlockType = UnlockType::ShipFire;
        selectedIconType = IconType::ShipFire;
      }
    } else {
      selectedUnlockType = ICON_TO_UNLOCK[iconKitState.selectedIconType];
      selectedIconType = iconKitState.selectedIconType;
    }
    
    // hold shift when clicking an icon to always open the icon popup, rather than selecting it, even if the icon is unlocked and unselected
    if (!Mod::get()->getSettingValue<bool>("disable-shift-clicking-on-icons"))
      if (itemID != -1 && CCDirector::get()->getKeyboardDispatcher()->getShiftKeyPressed()) {
        showUnlockPopup(itemID, selectedUnlockType);
        return;
      }

    if (!item || itemID == -1 || iconKitState.selectedIconType != IconType::Cube || GameManager::get()->isIconUnlocked(itemID, IconType::Cube))
      return GJGarageLayer::onSelect(sender);
    // only support unlocking the meltdown icon if moregames is installed because why not
    if (itemID != 13 && (itemID != 71 || !Loader::get()->isModLoaded("bitz.moregames")))
      return GJGarageLayer::onSelect(sender);
    if (Mod::get()->getSettingValue<bool>("disable-unlocking-free-icons"))
      return GJGarageLayer::onSelect(sender);

    GameManager::get()->unlockIcon(itemID, IconType::Cube);
    int itemPosition = displayToPosition(UnlockType::Cube, itemID);
    int itemPage = (itemPosition - 1)/36;
    recalculateIconOrder();
    int maxPage = (fakeCountForType(IconType::Cube, INT_MAX)-1)/36;
    if (itemPage > maxPage) itemPage = maxPage;
    setupPage(itemPage, IconType::Cube);
    showUnlockPopup(itemID, UnlockType::Cube);
    AchievementManager::sharedState()->reportAchievementWithID(itemID == 13 ? "geometry.ach.rate" : "geometry.ach.mdrate", 100, false);
  }
  
  CCArray* getItems(int maxIconCount, int page, IconType iconType, int activeIcon) {
    if (!SHOULD_CHANGE_ICON_TYPE(iconType))
      return GJGarageLayer::getItems(maxIconCount, page, iconType, activeIcon);

    // active icon is denied and we aren't showing denied, go to the first page instead
    if (!activeIcon && page == -1) page = 0;
    page = page == -1 ? (activeIcon - 1)/36 : page;
    
    // this is used to trick GJGarageLayer to stop drawing icons early if we are separating accepted from denied icons
    // see the comments in the definition of this function to actuallly see what it does
    maxIconCount = fakeCountForType(iconType, page);
    
    return GJGarageLayer::getItems(maxIconCount, page, iconType, activeIcon);
  }

  void setupPage(int page, IconType iconType) {
    if (!SHOULD_CHANGE_ICON_TYPE(iconType))
      return GJGarageLayer::setupPage(page, iconType);
    
    UnlockType unlockType = ICON_TO_UNLOCK[iconType];
    GameManager *gm = GameManager::get();

    iconKitState.shouldChangeIcons = true;

    page = iconKitState.overridePageForIcon[iconType] ? iconKitState.pageForIcon[iconType] : page;
    iconKitState.overridePageForIcon[iconType] = false;

    int activeIconPosition = gm->activeIconForType(iconType);
    log::info("{}", activeIconPosition);
    // active icon has been filtered out
    if (!activeIconPosition && page == -1) page = 0;
    page = page == -1 ? (activeIconPosition - 1)/36 : page;

    GJGarageLayer::setupPage(page, iconType);

    iconKitState.shouldChangeIcons = false;

    int fakeMaxIconCount = fakeCountForType(iconType, INT_MAX);
    int maxPage = (fakeMaxIconCount - 1)/36;
    
    int firstOnPage = page*36 + 1;
    CCMenu *menu = getChild<CCMenu>(getChild<ListButtonPage>(getChild<ExtendedLayer>(getChild<BoomScrollLayer>(getChildOfType<ListButtonBar>(this, 0), 0), 0), 0), 0);
    for (size_t i = 0; i < menu->getChildrenCount(); i++) {
      int selectedIconPosition = firstOnPage + i;
      int selectedIconDisplay = positionToDisplay(unlockType, selectedIconPosition);
      CCMenuItemSpriteExtra *icon = getChild<CCMenuItemSpriteExtra>(menu, i);
      icon->setTag(selectedIconDisplay);
    }
    recalculateNavdotMenu(page, iconType, unlockType);
    
    toggleNavigationMenus(iconKitState.settings.hideNavigationMenuOnSinglePage ? maxPage : fakeMaxIconCount, maxPage);

    iconKitState.pageForIcon[iconType] = page;
  }

  void toggleNavigationMenus(bool isNavdotMenuVisible, bool areArrowsVisible) {
    CCMenu *navdotMenu = static_cast<CCMenu *>(getChildByID("navdot-menu"));
    // no node ids, first setupPage is called before init is over
    if (!navdotMenu) navdotMenu = getChild<CCMenu>(this, 13);
    if (!navdotMenu) return;

    navdotMenu->setVisible(isNavdotMenuVisible);
    navdotMenu->setEnabled(isNavdotMenuVisible);

    // this is going to fail while in init, but i can't seem to find the
    // menus by indices at this point, so i manually handle them after init
    CCMenu *prevMenu = static_cast<CCMenu *>(getChildByID("prev-page-menu"));
    if (prevMenu) {
      getChild<CCMenuItemSpriteExtra>(prevMenu, 0)->setVisible(areArrowsVisible);
      getChild<CCMenuItemSpriteExtra>(prevMenu, 0)->setEnabled(areArrowsVisible);
    }

    CCMenu *nextMenu = static_cast<CCMenu *>(getChildByID("next-page-menu"));
    if (nextMenu) {
      getChild<CCMenuItemSpriteExtra>(nextMenu, 0)->setVisible(areArrowsVisible);
      getChild<CCMenuItemSpriteExtra>(nextMenu, 0)->setEnabled(areArrowsVisible);
    }

  }

  void recalculateNavdotMenu(size_t currentPage, IconType iconType, UnlockType unlockType) {

    int maxPage = (fakeCountForType(iconType, INT_MAX)-1)/36;
    maxPage = maxPage == -1 ? 0 : maxPage;

    CCMenu *navdotMenu = static_cast<CCMenu *>(getChildByID("navdot-menu"));
    if (!navdotMenu) navdotMenu = getChild<CCMenu>(this, 13);
    if (!navdotMenu) return;
    
    // add missing pages to the navdot menu
    for (int i = navdotMenu->getChildrenCount(); i <= maxPage; i++) {
      CCSprite *spriteOff = CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png");
      CCMenuItemSpriteExtra *button = CCMenuItemSpriteExtra::create(spriteOff, this, menu_selector(GJGarageLayer::onNavigate));
      button->setTag(i);
      navdotMenu->addChild(button);
    }
    // remove excess pages from the navdot menu
    for (int i = navdotMenu->getChildrenCount(); i > maxPage+1; i--) {
      navdotMenu->removeChild(getChild<CCMenuItemSpriteExtra>(navdotMenu, -1), true);
    }
    
    size_t lastAcceptedPage = iconKitState.acceptedIcons[unlockType].empty() ? 0 : (iconKitState.acceptedIcons[unlockType].size()-1)/36;
    size_t firstDeniedPage = lastAcceptedPage + 1;
    if (!iconKitState.settings.separateAcceptedFromDenied)
      firstDeniedPage = iconKitState.acceptedIcons[unlockType].size()/36;
    if (iconKitState.acceptedIcons[unlockType].empty()) lastAcceptedPage = UINT_MAX, firstDeniedPage = 0;
    if (!iconKitState.settings.showDenied) firstDeniedPage = UINT_MAX;

    for (size_t i = 0; i < navdotMenu->getChildrenCount(); i++) {
      CCMenuItemSpriteExtra *button = getChild<CCMenuItemSpriteExtra>(navdotMenu, i);
      if (i == firstDeniedPage && firstDeniedPage && iconKitState.settings.separateAcceptedFromDenied && !iconKitState.settings.noGapBetweenAcceptedAndDenied)
        button->setLayoutOptions(AxisLayoutOptions::create()->setPrevGap(25.f));
      else
        button->setLayoutOptions(AxisLayoutOptions::create()->setPrevGap({}));
      
      if (i == currentPage && firstDeniedPage == lastAcceptedPage && i == lastAcceptedPage && !iconKitState.settings.dontRecolorNavigationButtons) {
        CCSprite *spriteOnMixed = CCSprite::createWithSpriteFrameName("GU_navDotButton_mixed.png"_spr);
        spriteOnMixed->setScale(0.9f);
        button->setNormalImage(spriteOnMixed);
        button->setContentSize(button->getContentSize() * 0.9f);
      } else if (i == currentPage && i >= firstDeniedPage && !iconKitState.settings.dontRecolorNavigationButtons) {
        CCSprite *spriteOnDenied = CCSprite::createWithSpriteFrameName("GU_navDotButton_denied.png"_spr);
        spriteOnDenied->setScale(0.9f);
        button->setNormalImage(spriteOnDenied);
        button->setContentSize(button->getContentSize() * 0.9f);
      } else if (i == currentPage) {
        CCSprite *spriteOn = CCSprite::createWithSpriteFrameName("gj_navDotBtn_on_001.png");
        spriteOn->setScale(0.9f);
        button->setNormalImage(spriteOn);
        button->setContentSize(button->getContentSize() * 0.9f);
      } else {
        CCSprite *spriteOff = CCSprite::createWithSpriteFrameName("gj_navDotBtn_off_001.png");
        spriteOff->setScale(0.9f);
        button->setNormalImage(spriteOff);
        button->setContentSize(button->getContentSize() * 0.9f);
      }
    }
    navdotMenu->updateLayout();
  }
  
  void onPlayerOptions(CCObject *) {
    OptionsPopup::createWithPlayerOptions()->show();
  }
  
  void onFilterAndSort(CCObject *) {
    FilterAndSortPopup::create()->show();
  }

  bool init() {
    recalculateIconOrder();
    if (!GJGarageLayer::init()) return false;

    if (!Mod::get()->getSettingValue<bool>("hide-player-options-button")) {
      auto backMenu = static_cast<CCMenu *>(getChildByID("back-menu"));

      auto playerOptionsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
      auto playerOptions = CCMenuItemSpriteExtra::create(playerOptionsSprite, this, menu_selector(HookedGJGarageLayer::onPlayerOptions));
      
      backMenu->addChild(playerOptions);
      backMenu->updateLayout();
    }
    if (!Mod::get()->getSettingValue<bool>("hide-filter-and-sort-button")) {
      auto shardsMenu = static_cast<CCMenu *>(getChildByID("shards-menu"));
      auto filterIcon = CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png");
      auto sortIcon = CCSprite::createWithSpriteFrameName("GJ_sortIcon_001.png");
      filterIcon->setLayoutOptions(AxisLayoutOptions::create()->setMinScale(1/3.0f));
      sortIcon->setLayoutOptions(AxisLayoutOptions::create()->setMinScale(1/3.0f));
      auto filterAndSortCollection = CCNode::create();
      filterAndSortCollection->setLayout(RowLayout::create()->setAutoScale(true)->setGap(0));
      filterAndSortCollection->addChild(filterIcon);
      filterAndSortCollection->addChild(sortIcon);

      auto filterAndSortSprite = IconButtonSpriteNoText::create("GJ_button_01.png", filterAndSortCollection);
      auto filterAndSort = CCMenuItemSpriteExtra::create(filterAndSortSprite, this, menu_selector(HookedGJGarageLayer::onFilterAndSort));

      filterAndSortCollection->setContentSize(filterAndSortSprite->getContentSize() / sqrt(2));
      filterAndSortCollection->setAnchorPoint({0.5, 0.5});
      filterAndSortCollection->updateLayout();
      
      shardsMenu->addChild(filterAndSort);
      shardsMenu->updateLayout();
    }

    if (Mod::get()->getSettingValue<bool>("remove-diamond-shards")) {
      CCSprite *diamondShardsIcon = static_cast<CCSprite *>(getChildByID("diamond-shards-icon"));
      CCLabelBMFont *diamondShardsLabel = static_cast<CCLabelBMFont *>(getChildByID("diamond-shards-label"));
      CCSprite *demonsIcon = static_cast<CCSprite *>(getChildByID("demons-icon"));
      CCLabelBMFont *demonsLabel = static_cast<CCLabelBMFont *>(getChildByID("demons-label"));
      // very hardcoded, this part should really be made a layout so that it's easier to add whatever
      // you want here, which i initially wanted to do, but i decided against it since it's probably not
      // in scope for this mod, maybe some api mod? for now, i'll just account for Capeling's mod since
      // that's the only mod on the index I know that changes this area
      if (!demonsLabel) {
        diamondShardsLabel->removeFromParent();
        diamondShardsIcon->removeFromParent();
      } else {
        float spacing = diamondShardsLabel->boundingBox().getMaxY() - demonsLabel->boundingBox().getMaxY();
        demonsIcon->setPositionY(demonsIcon->getPositionY() + spacing);
        demonsLabel->setPositionY(demonsLabel->getPositionY() + spacing);
        diamondShardsLabel->removeFromParent();
        diamondShardsIcon->removeFromParent();
      }
    }

    int fakeCount = fakeCountForType(IconType::Cube, INT_MAX);
    int maxPage = (fakeCount-1)/36;
    toggleNavigationMenus(iconKitState.settings.hideNavigationMenuOnSinglePage ? maxPage : fakeCount, maxPage);

    return true;
  }
};
