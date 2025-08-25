#include <Geode/Geode.hpp>

#pragma warning(push)
#pragma warning(disable: 4265)  // non-virtual destructor
#include <Geode/modify/ItemInfoPopup.hpp>
#pragma warning(pop)

#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>


using namespace geode::prelude;

enum ItemInfoPopupButtonAction { Previous, Next };

struct HookedItemInfoPopup : Modify<HookedItemInfoPopup, ItemInfoPopup> {

  static void onModify(auto& self) {
      (void) self.setHookPriority("ItemInfoPopup::init", -999);
  }
  
  // warning 4263 and 4264
  using CCLayerColor::init;

  bool init(int icon, UnlockType unlockType) {
    if (!ItemInfoPopup::init(icon, unlockType)) return false;
    if (!Mod::get()->getSettingValue<bool>("disable-item-info-popup-animation-name-changes"))
      if (unlockType == UnlockType::GJItem && m_itemID >= 18 && m_itemID <= 20) {
        CCLabelBMFont *title = m_mainLayer->getChildByType<CCLabelBMFont>(0);
        TextArea* description = m_mainLayer->getChildByType<TextArea>(0);
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
    CCSize size = m_mainLayer->getChildByID("background")->getContentSize();
    float bottomBG = m_mainLayer->getChildByID("background")->boundingBox().getMinY();
    float bottomMenu = m_buttonMenu->boundingBox().getMinY();
    float pad = 25;
    prev->setPosition({-size.width/2 - pad, size.height/2 - (bottomMenu - bottomBG)});
    next->setPosition({size.width/2 + pad, size.height/2 - (bottomMenu - bottomBG)});
    m_buttonMenu->addChild(prev);
    m_buttonMenu->addChild(next);
    return true;
  }

  void onPrevNext(CCObject *sender) {
    CCMenuItemSpriteExtra* button = static_cast<CCMenuItemSpriteExtra*>(sender);
    ItemInfoPopupButtonAction action = static_cast<ItemInfoPopupButtonAction>(button->getTag());
    
    CCScene* scene = CCScene::get();
    GJGarageLayer *garage = scene->getChildByType<GJGarageLayer>(0);
    ShardsPage *shards = nullptr;
    if (garage) shards = garage->getChildByType<ShardsPage>(-1);
    GJPathPage *pathPage = scene->getChildByType<GJPathPage>(-1);
    GJPathSprite *pathSprite = nullptr;
    if (pathPage) pathSprite = pathPage->m_mainLayer->getChildByType<GJPathSprite>(-1);
    ProfilePage* profilePage = scene->getChildByType<ProfilePage>(-1);
    
    std::vector<std::pair<int, UnlockType>>* earlyIcons = nullptr;
    std::vector<std::pair<int, UnlockType>> playerIcons;
    std::vector<std::pair<int, UnlockType>> specialIcons;

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
      for (int icon : SPECIAL_UNLOCK_ORDER[m_unlockType]) specialIcons.push_back({icon, m_unlockType});
      earlyIcons = &specialIcons;
    }

    if (earlyIcons) {
      std::pair<int, UnlockType> newIcon;
      std::vector<std::pair<int, UnlockType>>::iterator p_first = earlyIcons->begin();
      std::vector<std::pair<int, UnlockType>>::iterator p_last = earlyIcons->end() - 1;
      std::vector<std::pair<int, UnlockType>>::iterator p_current = std::find(p_first, p_last + 1, std::pair<int, UnlockType>{m_itemID, m_unlockType});
      if (action == ItemInfoPopupButtonAction::Next) {
        if (p_current != p_last) newIcon = *(p_current+1);
        if (p_current == p_last) newIcon = *p_first;
      } else {
        if (p_current != p_first) newIcon = *(p_current-1);
        if (p_current == p_first) newIcon = *p_last;
      }
      removeFromParent();
      ItemInfoPopup* popup = ItemInfoPopup::create(newIcon.first, newIcon.second);
      popup->m_noElasticity = true;
      popup->show();
      return;
    }
    
    int newItemID = m_itemID;
    std::vector<int> iconsInOrder(iconKitState.acceptedIcons[m_unlockType]);
    if (iconKitState.settings.showDenied)
      iconsInOrder.insert(iconsInOrder.end(), iconKitState.deniedIcons[m_unlockType].begin(), iconKitState.deniedIcons[m_unlockType].end());
    std::vector<int>::iterator current = std::find(iconsInOrder.begin(), iconsInOrder.end(), m_itemID);
    if (!iconsInOrder.empty()) {
      if (action == ItemInfoPopupButtonAction::Next) {
        if (current + 1 != iconsInOrder.end()) newItemID = *(current + 1);
        if (current + 1 == iconsInOrder.end()) newItemID = *iconsInOrder.begin();
      } else {
        if (current != iconsInOrder.begin()) newItemID = *(current - 1);
        if (current == iconsInOrder.begin()) newItemID = *(iconsInOrder.end() - 1);
      }
    }
    removeFromParent();
    ItemInfoPopup* popup = ItemInfoPopup::create(newItemID, m_unlockType);
    popup->m_noElasticity = true;
    popup->show();
    
  }
};
