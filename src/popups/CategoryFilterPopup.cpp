#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/FilterPopup.hpp>
#include <popups/CategoryFilterPopup.hpp>
#include <utils/CCMenuItemTogglerSpoof.hpp>


using namespace geode::prelude;

void CategoryFilterPopup::onTopMenuButton(CCObject *sender) {
  CCMenuItemSpriteExtra *button = static_cast<CCMenuItemSpriteExtra *>(sender);
  int tag = button->getTag();
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount(); i += 2) {
    CCMenuItemToggler *toggler = getChild<CCMenuItemToggler>(m_buttonMenu, int(i));
    toggler->toggle(tag == 0 ? true : tag == 1 ? false : !toggler->isToggled());
    iconKitState.pendingSettings.categories[CATEGORIES_IN_ORDER[size_t(toggler->getTag())]] = toggler->isToggled();
  }
}

void CategoryFilterPopup::toggleCategory(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  iconKitState.pendingSettings.categories[CATEGORIES_IN_ORDER[size_t(toggler->getTag())]] = !toggler->isToggled(); 
}



void CategoryFilterPopup::addCategory(int tag, std::string category) {

  CCMenuItemToggler *toggler = createTogglerWithStandardSpritesSpoofOn(this, menu_selector(CategoryFilterPopup::toggleCategory));
  toggler->setTag(tag);
  toggler->setScale(0.6f);

  toggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));

  bool nextEven = !((m_buttonMenu->getChildrenCount() / 2 + 1) & 1);

  CCLabelBMFont *label = CCLabelBMFont::create(category.c_str(), "bigFont.fnt");
  
  float maxScale = 0.375f;
  float gap = static_cast<AxisLayout*>(m_buttonMenu->getLayout())->getGap();
  float textWidth = label->getContentSize().width;
  // technically, one of the gaps is not always the same as the other, it may get extended later to put the
  // toggler and label exactly on the second half of the menu, but if that happens, then that means that the
  // normal minimum of 0.375 succeeded, so this is fine
  float maximumTextWidth = m_buttonMenuSize.width/2 - toggler->getScaledContentSize().width - gap - (!nextEven)*gap;
  if (textWidth*maxScale > maximumTextWidth) maxScale = maximumTextWidth/textWidth;
  
  label->setLayoutOptions(
    AxisLayoutOptions::create()->
    setBreakLine(nextEven)->
    setSameLine(nextEven)->
    setScaleLimits(0.f, maxScale)
  );
  toggler->toggle(iconKitState.pendingSettings.categories[category]);

  m_buttonMenu->addChild(toggler);
  m_buttonMenu->addChild(label);

}

void CategoryFilterPopup::customSetup() {
  
  this->setTitle("Category Filter");
  
  for (size_t i = 0; i < CATEGORIES_IN_ORDER.size(); i++)
    addCategory(int(i), CATEGORIES_IN_ORDER[i]);

  m_buttonMenu->updateLayout();
  
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount()/2; i += 2) {
    CCLabelBMFont *label = getChild<CCLabelBMFont>(m_buttonMenu, 2*int(i)+1);
    float gap = m_buttonMenuSize.width/2 - label->boundingBox().getMaxX();
    label->setLayoutOptions(static_cast<AxisLayoutOptions *>(label->getLayoutOptions())->setNextGap(gap));
  }

  m_buttonMenu->updateLayout();
}

CategoryFilterPopup* CategoryFilterPopup::create(FilterAndSortPopup *parent) {
    auto ret = new CategoryFilterPopup();
    if (ret && ret->initAnchored(300.f, 255.f, parent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
