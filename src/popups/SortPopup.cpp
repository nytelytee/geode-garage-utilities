#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/SortPopup.hpp>
#include <popups/FilterAndSortPopup.hpp>

#include <utils/LinkedCCMenu.hpp>
#include <utils/IconButtonSpriteNoText.hpp>

using namespace geode::prelude;

void SortPopup::onClose(CCObject *sender) {
  Popup<FilterAndSortPopup *>::onClose(sender);
  parentPopup->refreshMenuState();
};

void SortPopup::onToggle(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  if (toggler->getTag() == 0) {
    iconKitState.pendingSettings.strictCategory = !toggler->isToggled();
    recalculateListItemBackgrounds();
  }
}

void SortPopup::recalculateListItemBackgrounds() {
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    CCLayerColor *listItemForeground = getChild<CCLayerColor>(m_sortingList, 5*i+4);
    if (iconKitState.pendingSettings.isSortedBy(iconKitState.pendingSettings.sortBy[i])) listItemForeground->setOpacity(0);
    else listItemForeground->setOpacity(75);
  }
}

void SortPopup::onArrow(CCObject *sender) {
  CCMenuItemSpriteExtra *item = static_cast<CCMenuItemSpriteExtra *>(sender);
  int listItem = item->getTag() >> 1;
  int action = 2*(item->getTag() & 1) - 1;

  if (listItem == 0 && action == -1) return;
  if (listItem == SORT_TYPE_COUNT-1 && action == 1) return;

  std::swap(iconKitState.pendingSettings.sortBy[listItem], iconKitState.pendingSettings.sortBy[listItem + action]);
  std::swap(iconKitState.pendingSettings.sortIsReverse[listItem], iconKitState.pendingSettings.sortIsReverse[listItem + action]);
  
  int labelIndex = 5*listItem + 2;
  int swapWithLabelIndex = labelIndex + 5*action;

  int menuIndex = 5*listItem + 3;
  int swapWithMenuIndex = menuIndex + 5*action;
  
  CCMenu* menu = getChild<CCMenu>(m_sortingList, menuIndex);
  CCMenu* swapWithMenu = getChild<CCMenu>(m_sortingList, swapWithMenuIndex);

  CCMenuItemSpriteExtra *sortOrder = getChild<CCMenuItemSpriteExtra>(menu, 2);
  CCMenuItemSpriteExtra *swapWithSortOrder = getChild<CCMenuItemSpriteExtra>(swapWithMenu, 2);

  std::string sortOrderState = getChild<IconButtonSpriteNoText>(sortOrder, 0)->getBGName();
  std::string swapWithOrderState = getChild<IconButtonSpriteNoText>(swapWithSortOrder, 0)->getBGName();
  getChild<IconButtonSpriteNoText>(sortOrder, 0)->setBG(swapWithOrderState.c_str(), false);
  getChild<IconButtonSpriteNoText>(swapWithSortOrder, 0)->setBG(sortOrderState.c_str(), false);

  CCLabelBMFont *label = getChild<CCLabelBMFont>(m_sortingList, labelIndex);
  CCLabelBMFont *swapWithLabel = getChild<CCLabelBMFont>(m_sortingList, swapWithLabelIndex);

  CCPoint tempPosition = swapWithLabel->getPosition();
  swapWithLabel->setPosition(label->getPosition());
  label->setPosition(tempPosition);

  m_sortingList->swapChildIndices(label, swapWithLabel);

  recalculateListItemBackgrounds();
}

void SortPopup::onReverseSortOrder(CCObject *sender) {
  CCMenuItemSpriteExtra *item = static_cast<CCMenuItemSpriteExtra *>(sender);
  int listItem = item->getTag();
  iconKitState.pendingSettings.sortIsReverse[listItem] = !iconKitState.pendingSettings.sortIsReverse[listItem];
  if (iconKitState.pendingSettings.sortIsReverse[listItem])
    getChild<IconButtonSpriteNoText>(item, 0)->setBG("GJ_button_01.png", false);
  else
    getChild<IconButtonSpriteNoText>(item, 0)->setBG("GJ_button_04.png", false);
}

bool SortPopup::setup(FilterAndSortPopup *parent) {
  parentPopup = parent;
  this->setTitle("Sort");
  float separator_height = 1;
  float title_margin = m_bgSprite->boundingBox().getMaxY() - m_title->boundingBox().getMaxY() - TOP_BORDER_SIZE;
  float title_height = m_title->getScaledContentSize().height;
  float real_title_height = 2*title_margin + title_height;

  CCSize contentSize = m_size - CCPoint{2*HORIZONTAL_BORDER_SIZE, real_title_height + BOTTOM_BORDER_SIZE + TOP_BORDER_SIZE};
  
  m_sortingList = CCNode::create();
  m_sortingList->setAnchorPoint({0, 1});
  m_sortingList->setPosition(HORIZONTAL_BORDER_SIZE, BOTTOM_BORDER_SIZE + contentSize.height);

  CCLayerColor  *listItemBackgrounds[SORT_TYPE_COUNT];
  CCLabelBMFont *listItemNumbers[SORT_TYPE_COUNT];
  CCLabelBMFont *listItemSortLabels[SORT_TYPE_COUNT];
  CCMenu* listItemMenus[SORT_TYPE_COUNT];
  CCLayerColor  *listItemForegrounds[SORT_TYPE_COUNT];
  
  float numberWidth = -1;
  float sortLabelWidth = -1;
  float textHeight = 32.5;
  float pad = 5;
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    listItemNumbers[i] = CCLabelBMFont::create(fmt::format("{}. ", i+1).c_str(), "bigFont.fnt");
    listItemNumbers[i]->setAnchorPoint({1, 0.5});
    numberWidth = std::max(numberWidth, listItemNumbers[i]->getContentWidth());

    listItemSortLabels[i] = CCLabelBMFont::create((SORT_TYPE_NAMES[iconKitState.pendingSettings.sortBy[i]] + " ").c_str(), "bigFont.fnt");
    listItemSortLabels[i]->setAnchorPoint({0, 0.5});
    sortLabelWidth = std::max(sortLabelWidth, listItemSortLabels[i]->getContentWidth());

    listItemMenus[i] = CCMenu::create();
    listItemMenus[i]->setAnchorPoint({0, 1});
    listItemMenus[i]->ignoreAnchorPointForPosition(false);
  }

  CCSize listItemSize{pad + numberWidth + sortLabelWidth + pad, pad + textHeight + pad};
  m_sortingList->setContentSize({0, SORT_TYPE_COUNT*listItemSize.height});
  
  listItemNumbers[0]->setPosition(CCPoint{pad + numberWidth, m_sortingList->getContentSize().height - pad - listItemNumbers[0]->getContentHeight()/2});
  listItemSortLabels[0]->setPosition(listItemNumbers[0]->getPosition());
  listItemMenus[0]->setPosition(CCPoint{listItemSize.width - pad, m_sortingList->getContentSize().height});

  for (int i = 1; i < SORT_TYPE_COUNT; i++) {
    listItemNumbers[i]->setPosition(listItemNumbers[i-1]->getPosition() - CCPoint{0, listItemSize.height});
    listItemSortLabels[i]->setPosition(listItemSortLabels[i-1]->getPosition() - CCPoint{0, listItemSize.height});
    listItemMenus[i]->setPosition(listItemMenus[i-1]->getPosition() - CCPoint{0, listItemSize.height});
  }
  
  float menuWidth = 0;
  float menuGap = 5;
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {

    CCSprite *moveUpSprite = CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
    IconButtonSpriteNoText *moveUpButtonSprite = IconButtonSpriteNoText::create("GJ_button_04.png", moveUpSprite);

    CCSprite *moveDownSprite = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    IconButtonSpriteNoText *moveDownButtonSprite = IconButtonSpriteNoText::create("GJ_button_04.png", moveDownSprite);

    CCSprite *reverseSortOrderSprite = CCSprite::createWithSpriteFrameName("GJ_sortIcon_001.png");
    reverseSortOrderSprite->setRotation(90);
    const char* bgname = iconKitState.pendingSettings.sortIsReverse[i] ? "GJ_button_01.png" : "GJ_button_04.png";
    IconButtonSpriteNoText *reverseSortOrderButtonSprite = IconButtonSpriteNoText::create(bgname, reverseSortOrderSprite);
    
    moveUpButtonSprite->setScale((listItemSize.height - 2*pad) / moveUpButtonSprite->getScaledContentSize().height);
    moveDownButtonSprite->setScale((listItemSize.height - 2*pad) / moveDownButtonSprite->getScaledContentSize().height);
    reverseSortOrderButtonSprite->setScale((listItemSize.height - 2*pad) / reverseSortOrderButtonSprite->getScaledContentSize().height);

    CCMenuItemSpriteExtra *moveUp = CCMenuItemSpriteExtra::create(moveUpButtonSprite, this, menu_selector(SortPopup::onArrow));
    CCMenuItemSpriteExtra *moveDown = CCMenuItemSpriteExtra::create(moveDownButtonSprite, this, menu_selector(SortPopup::onArrow));
    CCMenuItemSpriteExtra *reverseSortOrder = CCMenuItemSpriteExtra::create(reverseSortOrderButtonSprite, this, menu_selector(SortPopup::onReverseSortOrder));

    moveUp->setTag(2*i);
    moveDown->setTag(2*i + 1);
    reverseSortOrder->setTag(i);
    
    moveUp->setPosition(moveUp->getScaledContentSize().width/2, listItemSize.height/2);
    moveDown->setPosition(moveUp->getPosition() + CCPoint{moveUp->getScaledContentSize().width/2 + menuGap + moveDown->getScaledContentSize().width/2, 0});
    reverseSortOrder->setPosition(moveDown->getPosition() + CCPoint{moveDown->getScaledContentSize().width/2 + menuGap + reverseSortOrder->getScaledContentSize().width/2, 0});

    if (i == 0) {
      menuWidth += moveUp->getScaledContentSize().width;
      menuWidth += moveDown->getScaledContentSize().width;
      menuWidth += reverseSortOrder->getScaledContentSize().width;
      menuWidth += 2*menuGap;
    }
    listItemMenus[i]->setContentWidth(menuWidth);
    listItemMenus[i]->setContentHeight(listItemSize.height);

    listItemMenus[i]->addChild(moveUp);
    listItemMenus[i]->addChild(moveDown);
    listItemMenus[i]->addChild(reverseSortOrder);
  }

  listItemSize.width += menuWidth;
  m_sortingList->setContentWidth(listItemSize.width);
  m_sortingList->setScale(contentSize.width/listItemSize.width);


  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    CCPoint bottomLeft = CCPoint{0, m_sortingList->getContentHeight() - pad - textHeight - pad};

    listItemSortLabels[i]->limitLabelWidth(sortLabelWidth * 0.9f, 1, 0);

    listItemBackgrounds[i] = CCLayerColor::create({0, 0, 0, (GLubyte) (ODD_BACKGROUND_OPACITY * (i & 1) + EVEN_BACKGROUND_OPACITY * !(i & 1))}, listItemSize.width, listItemSize.height);
    listItemBackgrounds[i]->setPosition(bottomLeft - CCPoint{0, listItemSize.height}*i);

    listItemForegrounds[i] = CCLayerColor::create({0, 0, 0, 0}, listItemSize.width, listItemSize.height);
    listItemForegrounds[i]->setPosition(bottomLeft - CCPoint{0, listItemSize.height}*i);
  }
  
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    m_sortingList->addChild(listItemBackgrounds[i]); // 5n + 0
    m_sortingList->addChild(listItemNumbers[i]);     // 5n + 1
    m_sortingList->addChild(listItemSortLabels[i]);  // 5n + 2
    m_sortingList->addChild(listItemMenus[i]);       // 5n + 3
    m_sortingList->addChild(listItemForegrounds[i]); // 5n + 4
  }
  
  recalculateListItemBackgrounds();
  
  auto separator = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separator_height);
  auto separator2 = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separator_height);
  separator->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - title_margin);
  separator2->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - title_margin - m_sortingList->getScaledContentSize().height - separator_height);

  m_mainLayer->addChild(separator);
  m_mainLayer->addChild(m_sortingList);
  m_mainLayer->addChild(separator2);

  CCMenu *togglerMenu = CCMenu::create();
  togglerMenu->setContentSize(contentSize - CCPoint{0, m_sortingList->getScaledContentSize().height + separator_height});
  togglerMenu->setPosition(separator2->getPosition() - CCPoint{-separator2->getContentWidth()/2, togglerMenu->getContentHeight()/2});
  togglerMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Start));

  auto strictCategoryToggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(SortPopup::onToggle), 0.6f);
  strictCategoryToggler->setTag(0);
  strictCategoryToggler->toggle(iconKitState.pendingSettings.strictCategory);
  auto strictCategoryLabel = CCLabelBMFont::create("Strict Category", "bigFont.fnt");

  strictCategoryToggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
  strictCategoryLabel->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.375f));
  
  CCNode* spacer = CCNode::create();
  spacer->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false)->setNextGap(0));
  
  togglerMenu->addChild(spacer);
  togglerMenu->addChild(strictCategoryToggler);
  togglerMenu->addChild(strictCategoryLabel);

  togglerMenu->updateLayout();
  float delta = contentSize.height - m_sortingList->getScaledContentSize().height - separator_height - togglerMenu->getScaledContentSize().height;
  spacer->setContentWidth(delta/2);
  togglerMenu->updateLayout();

  CCMenu *infoMenu = CCMenu::create();
  infoMenu->setContentSize(m_size);
  infoMenu->setPosition(0, 0);
  InfoAlertButton *infoButton = InfoAlertButton::create(
    "Sort",

    "Customize the sorting order of the icons. Sorting methods that come first in the list get applied first.\n"
    "If a sorting method doesn't get used, it will be blacked out.\n"
    "The <co>flip</c> toggle reverses a sorting method's sorting order.\n"
    "<cy>Strict Category</c> - <cb>Category</c> additionally sorts icons inside categories by progression\n",

    0.5f
  );
  infoMenu->addChild(infoButton);
  infoButton->setPosition(m_size - CCPoint{HORIZONTAL_BORDER_SIZE, TOP_BORDER_SIZE} - infoButton->getScaledContentSize()/2 - infoButton->getScaledContentSize()/4);

  m_mainLayer->addChild(togglerMenu);
  m_mainLayer->addChild(infoMenu);

  LinkedCCMenu *linkedMenu = LinkedCCMenu::createLinked(m_mainLayer);
  linkedMenu->linkChildren(m_sortingList);
  m_mainLayer->addChild(linkedMenu);

  return true;
}

SortPopup* SortPopup::create(FilterAndSortPopup *parent) {
    auto ret = new SortPopup();
    if (ret && ret->initAnchored(250, 200, parent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
