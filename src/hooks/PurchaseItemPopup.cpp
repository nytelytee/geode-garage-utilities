#include <Geode/Geode.hpp>

#pragma warning(push)
#pragma warning(disable: 4265)  // non-virtual destructor
#include <Geode/modify/PurchaseItemPopup.hpp>
#pragma warning(pop)

#include <iconkit.hpp>
#include <logic.hpp>

// Better Unlock Info compatibility

class $modify(HookedPurchaseItemPopup, PurchaseItemPopup) {
  
  static void onModify(auto& self) {
    // run before Better Unlock Info, so that Better Unlock Info gets to change the icon first
    Result<> result = self.setHookPriorityBefore("PurchaseItemPopup::onPurchase", "rynat.better_unlock_info");
    if (!result && *result.err() != "Mod not found") log::error("Failed to set hook priority, may crash with Better Unlock Info.");
  }
  
  void onPurchase(CCObject* sender) {
    PurchaseItemPopup::onPurchase(sender);
    GJGarageLayer* garage = CCScene::get()->getChildByType<GJGarageLayer>(0);
    if (!garage) return;
    UnlockType unlockType = static_cast<UnlockType>(m_storeItem->m_unlockType.value());
    if (!SHOULD_CHANGE_UNLOCK_TYPE(unlockType)) {
      if (unlockType == UnlockType::Death) {
        if (garage->m_iconPages[IconType::DeathEffect] != 0) {
          garage->m_iconPages[IconType::DeathEffect] = 0;
          garage->selectTab(IconType::DeathEffect);
          giveIconAttention(garage, unlockType, m_storeItem->m_typeID.value());
        }
      // you can't buy more icons icons, so give the bought icon attention if you happened to buy it while you weren't on
      // the actual vanilla page; the death effect handling above is future proofing in case MI ever decides to support death effects
      } else {
        if (garage->m_iconPages[IconType::Special] != 0) {
          garage->m_iconPages[IconType::Special] = 0;
          garage->selectTab(IconType::Special);
          giveIconAttention(garage, unlockType, m_storeItem->m_typeID.value());
        }
      }
      return;
    }
    int oldPage = garage->m_iconPages[UNLOCK_TO_ICON[unlockType]];
    bool positionChanged = recalculateIconOrderAndTrackIcon(garage, unlockType, m_storeItem->m_typeID.value());
    int newPage = garage->m_iconPages[UNLOCK_TO_ICON[unlockType]];
    garage->selectTab(UNLOCK_TO_ICON[unlockType]);
    if (positionChanged || oldPage != newPage) giveIconAttention(garage, unlockType, m_storeItem->m_typeID.value());
  }

};
