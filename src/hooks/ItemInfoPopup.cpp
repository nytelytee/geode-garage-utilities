#include <Geode/Geode.hpp>
#include <Geode/modify/ItemInfoPopup.hpp>
#include <Geode/modify/GJPathPage.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>


// getChildOfType crashes on negative indices if the
// thing you are looking for does not exist; this is a fixed version
template <class Type = cocos2d::CCNode>
Type* getChildOfTypeFixed(CCNode* node, int index) {
  const size_t childSize = node->getChildrenCount();
  bool isBackwards = index < 0;
  index = isBackwards ? -index - 1 : index;
  for (size_t i = 0u; i < childSize; ++i) {
    size_t actualI = isBackwards ? childSize - i - 1 : i;
    Type* child = typeinfo_cast<Type*>(node->getChildren()->objectAtIndex(actualI));
    if (child && !index) return child;
    else if (child) index--;
  }
  return nullptr;
}

using namespace geode::prelude;

enum ItemInfoPopupButtonAction { Previous, Next };

struct HookedItemInfoPopup : Modify<HookedItemInfoPopup, ItemInfoPopup> {

  bool init(int icon, UnlockType unlockType) {
    if (!ItemInfoPopup::init(icon, unlockType)) return false;
    if (!Mod::get()->getSettingValue<bool>("disable-item-info-popup-animation-name-changes"))
      if (unlockType == UnlockType::GJItem && m_itemID >= 18 && m_itemID <= 20) {
        CCLabelBMFont *title = getChildOfType<CCLabelBMFont>(m_mainLayer, 0);
        TextArea *description = getChildOfType<TextArea>(m_mainLayer, 0);
        title->setString(fmt::format("Animation {}", m_itemID - 17).c_str());
        description->setString("You can <cl>buy</c> this <cg>Animation</c> at the <cp>Mechanic</c>!");
      }
    if (Mod::get()->getSettingValue<bool>("disable-item-info-popup-arrows")) return true;
    CCSprite *prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCSprite *nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    CCMenuItemSpriteExtra *prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(HookedItemInfoPopup::onPrevNext));
    CCMenuItemSpriteExtra *next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(HookedItemInfoPopup::onPrevNext));
    prev->setTag(ItemInfoPopupButtonAction::Previous);
    next->setTag(ItemInfoPopupButtonAction::Next);
    CCSize size = getChildOfType<CCScale9Sprite>(m_mainLayer, 0)->getContentSize();
    float pad = 25;
    float yOffset = 90;  // robtop moved the entire menu instead of just the ok button i think
    prev->setPosition({-size.width/2 - pad, yOffset});
    next->setPosition({size.width/2 + pad, yOffset});
    m_buttonMenu->addChild(prev);
    m_buttonMenu->addChild(next);
    return true;
  }

  void onPrevNext(CCObject *sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    ItemInfoPopupButtonAction action = static_cast<ItemInfoPopupButtonAction>(button->getTag());

    GJGarageLayer *garage = getChildOfTypeFixed<GJGarageLayer>(CCScene::get(), 0);
    ShardsPage *shards = nullptr;
    if (garage) shards = getChildOfTypeFixed<ShardsPage>(garage, -1);
    GJPathPage *pathPage = getChildOfTypeFixed<GJPathPage>(CCScene::get(), -1);
    GJPathSprite *pathSprite = nullptr;
    if (pathPage) pathSprite = getChildOfTypeFixed<GJPathSprite>(pathPage->m_mainLayer, -1);
    ProfilePage* profilePage = getChildOfTypeFixed<ProfilePage>(CCScene::get(), -1);
    
    std::vector<std::pair<unsigned, UnlockType>>* earlyIcons = nullptr;
    std::vector<std::pair<unsigned, UnlockType>> playerIcons;
    std::vector<std::pair<unsigned, UnlockType>> specialIcons;

    if (profilePage && m_unlockType != UnlockType::Col1 && m_unlockType != UnlockType::Col2) {
      playerIcons.push_back({profilePage->m_score->m_playerCube,   UnlockType::Cube});
      playerIcons.push_back({profilePage->m_score->m_playerShip,   UnlockType::Ship});
      playerIcons.push_back({profilePage->m_score->m_playerBall,   UnlockType::Ball});
      playerIcons.push_back({profilePage->m_score->m_playerUfo,    UnlockType::Bird});
      playerIcons.push_back({profilePage->m_score->m_playerWave,   UnlockType::Dart});
      playerIcons.push_back({profilePage->m_score->m_playerRobot,  UnlockType::Robot});
      playerIcons.push_back({profilePage->m_score->m_playerSpider, UnlockType::Spider});
      playerIcons.push_back({profilePage->m_score->m_playerSwing,  UnlockType::Swing});
      CCMenu *playerMenu = static_cast<CCMenu*>(profilePage->m_mainLayer->getChildByID("player-menu"));
      if (playerMenu && playerMenu->getChildByID("player-jetpack"))
        playerIcons.push_back({profilePage->m_score->m_playerJetpack, UnlockType::Jetpack});
      if (playerMenu && playerMenu->getChildByID("player-deathEffect"))
        playerIcons.push_back({profilePage->m_score->m_playerExplosion, UnlockType::Death});
      earlyIcons = &playerIcons;
    } else if (profilePage) {
      playerIcons.push_back({profilePage->m_score->m_color1,   UnlockType::Col1});
      playerIcons.push_back({profilePage->m_score->m_color2,   UnlockType::Col2});
      if (profilePage->m_score->m_glowEnabled)
        playerIcons.push_back({profilePage->m_score->m_color3,   UnlockType::Col2});
      earlyIcons = &playerIcons;
    } else if (pathSprite) {
      PathType path = static_cast<PathType>(pathSprite->m_pathNumber);
      earlyIcons = &PATHS_UNLOCK_ORDER[path];
    } else if (shards) {
      earlyIcons = &SHARDS_OF_POWER_UNLOCK_ORDER;
    } else if (!SHOULD_CHANGE_UNLOCK_TYPE(m_unlockType)) {
      // lmao probably should have thought about this before
      for (unsigned icon : SPECIAL_UNLOCK_ORDER[m_unlockType]) specialIcons.push_back({icon, m_unlockType});
      earlyIcons = &specialIcons;
    }

    if (earlyIcons) {
      std::pair<unsigned, UnlockType> newIcon;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_first = earlyIcons->begin();
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_last = earlyIcons->end() - 1;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_current = std::find(p_first, p_last + 1, std::pair<unsigned, UnlockType>{m_itemID, m_unlockType});
      if (action == ItemInfoPopupButtonAction::Next) {
        if (p_current != p_last) newIcon = *(p_current+1);
        if (p_current == p_last) newIcon = *p_first;
      } else {
        if (p_current != p_first) newIcon = *(p_current-1);
        if (p_current == p_first) newIcon = *p_last;
      }
      m_buttonMenu->removeAllChildren();
      m_mainLayer->removeAllChildren();
      m_mainLayer->removeFromParent();
      ItemInfoPopup::init(newIcon.first, newIcon.second);
      return;
    }
    
    int newItemID = m_itemID;
    std::vector<unsigned>::iterator p_acceptedFirst = iconKitState.acceptedIcons[m_unlockType].begin();
    std::vector<unsigned>::iterator p_deniedFirst = iconKitState.deniedIcons[m_unlockType].begin();
    std::vector<unsigned>::iterator p_acceptedLast = iconKitState.acceptedIcons[m_unlockType].end() - 1;
    std::vector<unsigned>::iterator p_deniedLast = iconKitState.deniedIcons[m_unlockType].end() - 1;
    std::vector<unsigned>::iterator p_acceptedCurrent = std::find(p_acceptedFirst, p_acceptedLast+1, m_itemID);
    std::vector<unsigned>::iterator p_deniedCurrent = std::find(p_deniedFirst, p_deniedLast+1, m_itemID);
    bool isAcceptedEmpty = p_acceptedFirst == p_acceptedLast + 1;
    bool isDeniedEmpty = p_deniedFirst == p_deniedLast + 1;

    if (action == ItemInfoPopupButtonAction::Next) {
      if (p_acceptedCurrent <= p_acceptedLast) {
        if (p_acceptedCurrent != p_acceptedLast) newItemID = *(p_acceptedCurrent+1);
        if (p_acceptedCurrent == p_acceptedLast && (!iconKitState.settings.showDenied || isDeniedEmpty)) newItemID = *p_acceptedFirst;
        if (p_acceptedCurrent == p_acceptedLast && iconKitState.settings.showDenied && !isDeniedEmpty) newItemID = *p_deniedFirst;
      } else {
        if (p_deniedCurrent != p_deniedLast) newItemID = *(p_deniedCurrent+1);
        if (p_deniedCurrent == p_deniedLast && isAcceptedEmpty) newItemID = *p_deniedFirst;
        if (p_deniedCurrent == p_deniedLast && !isAcceptedEmpty) newItemID = *p_acceptedFirst;
      }
    } else {
      if (p_acceptedCurrent <= p_acceptedLast) {
        if (p_acceptedCurrent != p_acceptedFirst) newItemID = *(p_acceptedCurrent-1);
        if (p_acceptedCurrent == p_acceptedFirst && (!iconKitState.settings.showDenied || isDeniedEmpty)) newItemID = *p_acceptedLast;
        if (p_acceptedCurrent == p_acceptedFirst && iconKitState.settings.showDenied && !isDeniedEmpty) newItemID = *p_deniedLast;
      } else {
        if (p_deniedCurrent != p_deniedFirst) newItemID = *(p_deniedCurrent-1);
        if (p_deniedCurrent == p_deniedFirst && isAcceptedEmpty) newItemID = *p_deniedLast;
        if (p_deniedCurrent == p_deniedFirst && !isAcceptedEmpty) newItemID = *p_acceptedLast;
      }
    }
    
    m_buttonMenu->removeAllChildren();
    m_mainLayer->removeAllChildren();
    m_mainLayer->removeFromParent();
    ItemInfoPopup::init(newItemID, m_unlockType);
  }
};
