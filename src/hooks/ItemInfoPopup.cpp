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

struct HookedItemInfoPopup : Modify<HookedItemInfoPopup, ItemInfoPopup> {
  bool init(int icon, UnlockType unlockType) {
    if (!ItemInfoPopup::init(icon, unlockType)) return false;
    CCLayer *layer = getChild<CCLayer>(this, 0);
    if (!Mod::get()->getSettingValue<bool>("disable-unlock-popup-animation-name-changes"))
      if (unlockType == UnlockType::GJItem && m_itemID >= 18 && m_itemID <= 20) {
        CCLabelBMFont *title = getChildOfType<CCLabelBMFont>(layer, 0);
        TextArea *description = getChildOfType<TextArea>(layer, 0);
        title->setString(fmt::format("Animation {}", m_itemID - 17).c_str());
        description->setString("You can <cl>buy</c> this <cg>Animation</c> at the <cp>Mechanic</c>!");
      }
    if (Mod::get()->getSettingValue<bool>("disable-unlock-popup-arrows")) return true;
    CCMenu *menu = getChildOfType<CCMenu>(layer, 0);
    CCSprite *prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCSprite *nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    CCMenuItemSpriteExtra *prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(HookedItemInfoPopup::onPrev));
    CCMenuItemSpriteExtra *next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(HookedItemInfoPopup::onNext));
    prev->setPosition({-175, 75});
    next->setPosition({175, 75});
    menu->addChild(prev);
    menu->addChild(next);
    return true;
  }
  void onPrev(CCObject *) {
    
    int previousItemID = m_itemID - 1;
    std::vector<unsigned>::iterator p_acceptedFirst;
    std::vector<unsigned>::iterator p_deniedFirst;
    std::vector<unsigned>::iterator p_acceptedLast;
    std::vector<unsigned>::iterator p_deniedLast;
    std::vector<unsigned>::iterator p_accepted;
    std::vector<unsigned>::iterator p_denied;
    bool isAcceptedEmpty;
    bool isDeniedEmpty;
    
    GJGarageLayer *garage = getChildOfTypeFixed<GJGarageLayer>(CCScene::get(), 0);
    ShardsPage *shards = nullptr;
    if (garage) shards = getChildOfTypeFixed<ShardsPage>(garage, -1);
    GJPathPage *pathPage = getChildOfTypeFixed<GJPathPage>(CCScene::get(), -1);
    GJPathSprite *pathSprite = nullptr;
    if (pathPage) pathSprite = getChildOfTypeFixed<GJPathSprite>(pathPage->m_mainLayer, -1);
    
    if (pathSprite) {
      PathType path = static_cast<PathType>(pathSprite->m_pathNumber);
      std::pair<unsigned, UnlockType> previousPathIcon;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_pathFirst = PATHS_UNLOCK_ORDER[path].begin();
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_pathLast = PATHS_UNLOCK_ORDER[path].end() - 1;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_path = std::find(p_pathFirst, p_pathLast + 1, std::pair<unsigned, UnlockType>{m_itemID, m_unlockType});
      if (p_path != p_pathFirst) previousPathIcon = *(p_path-1);
      if (p_path == p_pathFirst) previousPathIcon = *p_pathLast;
      getChild<CCLayer>(this, 0)->removeFromParent();
      ItemInfoPopup::init(previousPathIcon.first, previousPathIcon.second);
      return;
    }
    
    if (shards) {
      std::pair<unsigned, UnlockType> previousShardIcon;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_shardFirst = SHARDS_OF_POWER_UNLOCK_ORDER.begin();
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_shardLast = SHARDS_OF_POWER_UNLOCK_ORDER.end() - 1;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_shard = std::find(p_shardFirst, p_shardLast + 1, std::pair<unsigned, UnlockType>{m_itemID, m_unlockType});
      if (p_shard != p_shardFirst) previousShardIcon = *(p_shard-1);
      if (p_shard == p_shardFirst) previousShardIcon = *p_shardLast;
      getChild<CCLayer>(this, 0)->removeFromParent();
      ItemInfoPopup::init(previousShardIcon.first, previousShardIcon.second);
      return;
    }

    if (SHOULD_CHANGE_UNLOCK_TYPE(m_unlockType)) {
      p_acceptedFirst = iconKitState.acceptedIcons[m_unlockType].begin();
      p_deniedFirst = iconKitState.deniedIcons[m_unlockType].begin();
      p_acceptedLast = iconKitState.acceptedIcons[m_unlockType].end() - 1;
      p_deniedLast = iconKitState.deniedIcons[m_unlockType].end() - 1;
      isAcceptedEmpty = p_acceptedFirst == p_acceptedLast + 1;
      isDeniedEmpty = p_deniedFirst == p_deniedLast + 1;
      p_accepted = std::find(p_acceptedFirst, p_acceptedLast+1, m_itemID);
      p_denied = std::find(p_deniedFirst, p_deniedLast+1, m_itemID);
    } else {
      p_acceptedFirst = SPECIAL_UNLOCK_ORDER[m_unlockType].begin();
      p_acceptedLast = SPECIAL_UNLOCK_ORDER[m_unlockType].end() - 1;
      p_accepted = std::find(p_acceptedFirst, p_acceptedLast+1, m_itemID);
      isDeniedEmpty = true;
      isAcceptedEmpty = false;
    }
    
    if (p_accepted <= p_acceptedLast) {
      if (p_accepted != p_acceptedFirst) previousItemID = *(p_accepted-1);
      if (p_accepted == p_acceptedFirst && (!iconKitState.settings.showDenied || isDeniedEmpty)) previousItemID = *p_acceptedLast;
      if (p_accepted == p_acceptedFirst && iconKitState.settings.showDenied && !isDeniedEmpty) previousItemID = *p_deniedLast;
    } else {
      if (p_denied != p_deniedFirst) previousItemID = *(p_denied-1);
      if (p_denied == p_deniedFirst && isAcceptedEmpty) previousItemID = *p_deniedLast;
      if (p_denied == p_deniedFirst && !isAcceptedEmpty) previousItemID = *p_acceptedLast;
    }

    getChild<CCLayer>(this, 0)->removeFromParent();
    ItemInfoPopup::init(previousItemID, m_unlockType);
  }
  void onNext(CCObject *) {
    
    int nextItemID = m_itemID + 1;
    std::vector<unsigned>::iterator p_acceptedFirst;
    std::vector<unsigned>::iterator p_deniedFirst;
    std::vector<unsigned>::iterator p_acceptedLast;
    std::vector<unsigned>::iterator p_deniedLast;
    std::vector<unsigned>::iterator p_accepted;
    std::vector<unsigned>::iterator p_denied;
    bool isAcceptedEmpty;
    bool isDeniedEmpty;


    GJGarageLayer *garage = getChildOfTypeFixed<GJGarageLayer>(CCScene::get(), 0);
    ShardsPage *shards = nullptr;
    if (garage) shards = getChildOfTypeFixed<ShardsPage>(garage, -1);
    GJPathPage *pathPage = getChildOfTypeFixed<GJPathPage>(CCScene::get(), -1);
    GJPathSprite *pathSprite = nullptr;
    if (pathPage) pathSprite = getChildOfTypeFixed<GJPathSprite>(pathPage->m_mainLayer, -1);
    
    if (pathSprite) {
      PathType path = static_cast<PathType>(pathSprite->m_pathNumber);
      std::pair<unsigned, UnlockType> nextPathIcon;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_pathFirst = PATHS_UNLOCK_ORDER[path].begin();
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_pathLast = PATHS_UNLOCK_ORDER[path].end() - 1;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_path = std::find(p_pathFirst, p_pathLast + 1, std::pair<unsigned, UnlockType>{m_itemID, m_unlockType});
      if (p_path != p_pathLast) nextPathIcon = *(p_path+1);
      if (p_path == p_pathLast) nextPathIcon = *p_pathFirst;
      getChild<CCLayer>(this, 0)->removeFromParent();
      ItemInfoPopup::init(nextPathIcon.first, nextPathIcon.second);
      return;
    }

    if (shards) {
      std::pair<unsigned, UnlockType> nextShardIcon;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_shardFirst = SHARDS_OF_POWER_UNLOCK_ORDER.begin();
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_shardLast = SHARDS_OF_POWER_UNLOCK_ORDER.end() - 1;
      std::vector<std::pair<unsigned, UnlockType>>::iterator p_shard = std::find(p_shardFirst, p_shardLast + 1, std::pair<unsigned, UnlockType>{m_itemID, m_unlockType});
      if (p_shard != p_shardLast) nextShardIcon = *(p_shard+1);
      if (p_shard == p_shardLast) nextShardIcon = *p_shardFirst;
      getChild<CCLayer>(this, 0)->removeFromParent();
      ItemInfoPopup::init(nextShardIcon.first, nextShardIcon.second);
      return;
    }

    if (SHOULD_CHANGE_UNLOCK_TYPE(m_unlockType)) {
      p_acceptedFirst = iconKitState.acceptedIcons[m_unlockType].begin();
      p_deniedFirst = iconKitState.deniedIcons[m_unlockType].begin();
      p_acceptedLast = iconKitState.acceptedIcons[m_unlockType].end() - 1;
      p_deniedLast = iconKitState.deniedIcons[m_unlockType].end() - 1;
      p_accepted = std::find(p_acceptedFirst, p_acceptedLast+1, m_itemID);
      p_denied = std::find(p_deniedFirst, p_deniedLast+1, m_itemID);
      isAcceptedEmpty = p_acceptedFirst == p_acceptedLast + 1;
      isDeniedEmpty = p_deniedFirst == p_deniedLast + 1;
    } else {
      p_acceptedFirst = SPECIAL_UNLOCK_ORDER[m_unlockType].begin();
      p_acceptedLast = SPECIAL_UNLOCK_ORDER[m_unlockType].end() - 1;
      p_accepted = std::find(p_acceptedFirst, p_acceptedLast+1, m_itemID);
      isDeniedEmpty = true;
      isAcceptedEmpty = false;
    }
    
    if (p_accepted <= p_acceptedLast) {
      if (p_accepted != p_acceptedLast) nextItemID = *(p_accepted+1);
      if (p_accepted == p_acceptedLast && (!iconKitState.settings.showDenied || isDeniedEmpty)) nextItemID = *p_acceptedFirst;
      if (p_accepted == p_acceptedLast && iconKitState.settings.showDenied && !isDeniedEmpty) nextItemID = *p_deniedFirst;
    } else {
      if (p_denied != p_deniedLast) nextItemID = *(p_denied+1);
      if (p_denied == p_deniedLast && isAcceptedEmpty) nextItemID = *p_deniedFirst;
      if (p_denied == p_deniedLast && !isAcceptedEmpty) nextItemID = *p_acceptedFirst;
    }

    getChild<CCLayer>(this, 0)->removeFromParent();
    ItemInfoPopup::init(nextItemID, m_unlockType);
  }
};
