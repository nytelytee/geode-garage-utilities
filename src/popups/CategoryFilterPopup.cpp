#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/FilterPopup.hpp>
#include <popups/CategoryFilterPopup.hpp>


using namespace geode::prelude;

void CategoryFilterPopup::onTopMenuButton(CCObject *sender) {
  CCMenuItemSpriteExtra *button = static_cast<CCMenuItemSpriteExtra *>(sender);
  int tag = button->getTag();
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount(); i += 2) {
    CCMenuItemToggler *toggler = getChild<CCMenuItemToggler>(m_buttonMenu, i);
    toggler->toggle(tag == 0 ? true : tag == 1 ? false : !toggler->isToggled());
    iconKitState.pendingSettings.categories[CATEGORIES_IN_ORDER[toggler->getTag()]] = toggler->isToggled();
  }
}

void CategoryFilterPopup::toggleCategory(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  iconKitState.pendingSettings.categories[CATEGORIES_IN_ORDER[toggler->getTag()]] = !toggler->isToggled(); 
}

void CategoryFilterPopup::addCategory(int tag, std::string category) {

  CCMenuItemToggler *toggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(CategoryFilterPopup::toggleCategory), 1);
  toggler->setTag(tag);
  toggler->setScale(0.6f);
  toggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));

  bool nextEven = !((m_buttonMenu->getChildrenCount() / 2 + 1) & 1);

  CCLabelBMFont *label = CCLabelBMFont::create(category.c_str(), "bigFont.fnt");
  
  float maxScale = 0.375f;
  if (label->getContentSize().width * maxScale > (m_scrollLayerSize.width/2 - toggler->getContentSize().width)) {
    maxScale = (m_scrollLayerSize.width/2 - toggler->getContentSize().width) / label->getContentSize().width;
  }
  
  label->setLayoutOptions(
    AxisLayoutOptions::create()->
    setBreakLine(nextEven)->
    setSameLine(nextEven)->
    setMaxScale(maxScale)
  );
  toggler->toggle(iconKitState.pendingSettings.categories[category]);

  m_buttonMenu->addChild(toggler);
  m_buttonMenu->addChild(label);

}

void CategoryFilterPopup::customSetup() {
  
  this->setTitle("Category Filter");
  
  for (size_t i = 0; i < CATEGORIES_IN_ORDER.size(); i++)
    addCategory(i, CATEGORIES_IN_ORDER[i]);

  m_buttonMenu->updateLayout();
  
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount()/2; i += 2) {
    //CCMenuItemToggler *toggler = getChild<CCMenuItemToggler>(m_buttonMenu, 2*i);
    CCLabelBMFont *label = getChild<CCLabelBMFont>(m_buttonMenu, 2*i+1);
    float gap = m_scrollLayerSize.width/2 - label->boundingBox().getMaxX();
    log::info("{} {}", label->getString(), gap);
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
