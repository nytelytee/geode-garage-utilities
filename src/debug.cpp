#include <Geode/loader/GameEvent.hpp>

// i copied the header straight from the Prevter's PR, which hasn't been merged yet
// if you want to use this feature, use the build from that PR
#include <devtools-api.hpp>

#include <iconkit.hpp>
#include <constants.hpp>

$on_mod(Loaded) {
    devtools::waitForDevTools([] {
        devtools::registerNode<GJGarageLayer>([](GJGarageLayer* node) {
            devtools::label("Garage Utilities\n");
            devtools::label(fmt::format("Current page: {}", node->m_iconPages[node->m_iconType]).c_str());
            devtools::label(fmt::format("Current icon type: {}", ICON_NAMES[ICON_TO_UNLOCK[node->m_iconType]]).c_str());
            devtools::label(fmt::format("Accepted icons: {}", iconKitState.acceptedIcons[ICON_TO_UNLOCK[node->m_iconType]].size()).c_str());
            devtools::label(fmt::format("Denied icons: {}", iconKitState.deniedIcons[ICON_TO_UNLOCK[node->m_iconType]].size()).c_str());
            devtools::button("Reset Supporter Icons", []{
                GameManager::get()->resetAchievement("geometry.ach.rate");
                GameManager::get()->resetAchievement("geometry.ach.mdrate");
            });
        });
        devtools::registerNode<PurchaseItemPopup>([](PurchaseItemPopup* node) {
            devtools::label("Purchase\n");
            devtools::label(fmt::format("Index: {}", node->m_storeItem->m_index.value()).c_str());
            devtools::label(fmt::format("TypeID: {}", node->m_storeItem->m_typeID.value()).c_str());
            devtools::label(fmt::format("Unlock Type: {}", node->m_storeItem->m_unlockType.value()).c_str());
            devtools::label(fmt::format("Price: {}", node->m_storeItem->m_price.value()).c_str());
        });
    });
}

