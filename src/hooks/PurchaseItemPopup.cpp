#include <Geode/Geode.hpp>
#include <Geode/modify/PurchaseItemPopup.hpp>

#include <iconkit.hpp>
#include <logic.hpp>

class $modify(HookedPurchaseItemPopup, PurchaseItemPopup) {
  
  static void onModify(auto& self) {
    // run before Better Unlock Info, so that Better Unlock Info gets to change the icon first
    Result<> result = self.setHookPriority("PurchaseItemPopup::onPurchase", -1);
    if (!result) log::error("Failed to set hook priority, may crash with Better Unlock Info.");
  }
  
  void onPurchase(CCObject* sender) {
    PurchaseItemPopup::onPurchase(sender);
    CCScene* scene = CCScene::get();
    GJGarageLayer* garage = getChildOfType<GJGarageLayer>(scene, 0);
    if (!garage) return;
    
    if (!SHOULD_CHANGE_ICON_TYPE(iconKitState.selectedIconType)) return;

    int itemPage = iconKitState.pageForIcon[iconKitState.selectedIconType];
    recalculateIconOrder();
    int maxPage = (fakeCountForType(IconType::Cube, INT_MAX)-1)/36;
    if (itemPage > maxPage) itemPage = maxPage;

    garage->setupPage(itemPage, iconKitState.selectedIconType);

  }

};
