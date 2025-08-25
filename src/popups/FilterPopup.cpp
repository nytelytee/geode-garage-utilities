#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/FilterPopup.hpp>
#include <popups/FilterAndSortPopup.hpp>
#include <utils/BoundCCMenu.hpp>
#include <utils/LinkedCCMenu.hpp>

using namespace geode::prelude;

void FilterPopup::onClose(CCObject *sender) {
  Popup<FilterAndSortPopup *>::onClose(sender);
  parentPopup->refreshMenuState();
};

void FilterPopup::postCustomSetup() {
  LinkedCCMenu *linkedMenu = LinkedCCMenu::create();

  m_scrollLayer->m_contentLayer->setContentSize(m_buttonMenu->getContentSize());
  m_scrollLayer->m_contentLayer->addChild(m_buttonMenu);
  m_mainLayer->addChild(m_scrollLayer);
  m_mainLayer->addChild(m_actionButtonMenu);
  m_scrollLayer->moveToTop();
  
  linkedMenu->linkChildren(m_mainLayer);
  linkedMenu->link(m_buttonMenu);
  m_mainLayer->addChild(linkedMenu);
}

void FilterPopup::preCustomSetup() {
  this->setTitle("Filter");
  
  float separatorHeight = 1;
  
  // separators should span the popup size, excluding the borders, and they should be 1 unit high (i don't know if i need to adjust the units for different screen sizes, tell me if so)
  CCLayerColor* separatorTop = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separatorHeight);
  CCLayerColor* separatorBottom = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separatorHeight);
  
  // the margin of the title is the distance from the top of the text to the border, excluding the border itself, the same distance should be applied below the title, yielding realTitleHeight
  float titleMargin = m_bgSprite->boundingBox().getMaxY() - m_title->boundingBox().getMaxY() - VERTICAL_BORDER_SIZE;
  float titleHeight = m_title->getScaledContentSize().height;
  float realTitleHeight = 2*titleMargin + titleHeight;
  
  // the usable size is the size of the remainder of the popup, excluding the borders and the title
  CCSize usableSize = m_size - CCPoint{2*HORIZONTAL_BORDER_SIZE, realTitleHeight + 2*VERTICAL_BORDER_SIZE};
  
  // delta is the size of the empty space that the action button menu will take up
  // it is also the total empty space inside the scroll layer, horizontally
  float delta = 20;
  
  // the scroll layer should be centered on the x axis (the delta is not applied here, but in the button menu, as padding on the scroll layer rather than a margin)
  // center on x, flush to top on y, so empty space is delta/2 horizontally and delta on the bottom
  m_scrollLayerSize = usableSize - CCPoint{0, delta};
  m_buttonMenuSize = usableSize - CCPoint{delta, delta};
  
  m_actionButtonMenu = CCMenu::create();
  m_actionButtonMenu->setAnchorPoint({0, 0});
  m_actionButtonMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
  m_actionButtonMenu->setContentSize({usableSize.width, 0});

  ButtonSprite *topMenuAllButtonSprite = ButtonSprite::create("All", "goldFont.fnt", "GJ_button_01.png", 1);
  ButtonSprite *topMenuNoneButtonSprite = ButtonSprite::create("None", "goldFont.fnt", "GJ_button_06.png", 1);
  ButtonSprite *topMenuInvertButtonSprite = ButtonSprite::create("Invert", "goldFont.fnt", "GJ_button_03.png", 1);
  
  CCMenuItemSpriteExtra *topMenuAllButton = CCMenuItemSpriteExtra::create(topMenuAllButtonSprite, this, menu_selector(FilterPopup::onTopMenuButton));
  CCMenuItemSpriteExtra *topMenuNoneButton = CCMenuItemSpriteExtra::create(topMenuNoneButtonSprite, this, menu_selector(FilterPopup::onTopMenuButton));
  CCMenuItemSpriteExtra *topMenuInvertButton = CCMenuItemSpriteExtra::create(topMenuInvertButtonSprite, this, menu_selector(FilterPopup::onTopMenuButton));

  topMenuAllButton->setTag(0);
  topMenuNoneButton->setTag(1);
  topMenuInvertButton->setTag(2);

  topMenuAllButton->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits({}, (delta - separatorHeight - VERTICAL_BORDER_SIZE)/topMenuAllButton->getContentHeight()));
  topMenuNoneButton->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits({}, (delta - separatorHeight - VERTICAL_BORDER_SIZE)/topMenuNoneButton->getContentHeight()));
  topMenuInvertButton->setLayoutOptions(AxisLayoutOptions::create()->setScaleLimits({}, (delta - separatorHeight - VERTICAL_BORDER_SIZE)/topMenuInvertButton->getContentHeight()));
  
  m_actionButtonMenu->addChild(topMenuAllButton);
  m_actionButtonMenu->addChild(topMenuNoneButton);
  m_actionButtonMenu->addChild(topMenuInvertButton);

  m_actionButtonMenu->updateLayout();
  // offset necessary to place it in the center of the remaining space at the bottom
  float m_actionButtonMenuOffset = ((delta - separatorHeight) - m_actionButtonMenu->getContentHeight())/2;

  m_actionButtonMenu->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - titleMargin - m_scrollLayerSize.height - delta + VERTICAL_BORDER_SIZE - m_actionButtonMenuOffset);
  separatorTop->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - titleMargin);
  separatorBottom->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - titleMargin - m_scrollLayerSize.height - separatorHeight);
  
  m_mainLayer->addChild(separatorTop);
  m_mainLayer->addChild(separatorBottom);

  m_scrollLayer = ScrollLayer::create(m_scrollLayerSize);
  m_scrollLayer->setAnchorPoint({0, 0});
  m_scrollLayer->setPosition(HORIZONTAL_BORDER_SIZE, VERTICAL_BORDER_SIZE + delta);

  m_buttonMenu = BoundCCMenu::create({m_mainLayer->convertToWorldSpace(m_scrollLayer->getPosition()), m_scrollLayer->getContentSize()});
  m_buttonMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Start)->setGrowCrossAxis(true)->setAutoScale(true)->setGap(5.f));
  m_buttonMenu->setContentSize({m_buttonMenuSize.width, 0});
  m_buttonMenu->setAnchorPoint({0, 0});
  m_buttonMenu->setPosition({delta/2, 0});

}
