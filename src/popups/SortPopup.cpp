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
    CCLayerColor *listBackground = getChild<CCLayerColor>(sortingList, 3*i);
    if (iconKitState.pendingSettings.isSortedBy(iconKitState.pendingSettings.sortBy[i])) {
      listBackground->setColor({0, 0, 0});
      listBackground->setOpacity(ODD_OPACITY * (i & 1) + EVEN_OPACITY * !(i & 1));
    }
    else {
      listBackground->setColor({255, 0, 0});
      listBackground->setOpacity(RED_ODD_OPACITY * (i & 1) + RED_EVEN_OPACITY * !(i & 1));
    }
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
  
  int labelIndex = 3*listItem + 2;
  int swapWithLabelIndex = labelIndex + 3*action;

  int sortOrderIndex = 3*listItem + 2;
  int swapWithSortOrderIndex = sortOrderIndex + 3*action;

  CCMenuItemSpriteExtra *sortOrder = getChild<CCMenuItemSpriteExtra>(sortingButtonMenu, sortOrderIndex);
  CCMenuItemSpriteExtra *swapWithSortOrder = getChild<CCMenuItemSpriteExtra>(sortingButtonMenu, swapWithSortOrderIndex);

  std::string sortOrderState = getChild<IconButtonSpriteNoText>(sortOrder, 0)->getBGName();
  std::string swapWithOrderState = getChild<IconButtonSpriteNoText>(swapWithSortOrder, 0)->getBGName();
  getChild<IconButtonSpriteNoText>(sortOrder, 0)->setBG(swapWithOrderState.c_str(), false);
  getChild<IconButtonSpriteNoText>(swapWithSortOrder, 0)->setBG(sortOrderState.c_str(), false);

  CCLabelBMFont *label = getChild<CCLabelBMFont>(sortingList, labelIndex);
  CCLabelBMFont *swapWithLabel = getChild<CCLabelBMFont>(sortingList, swapWithLabelIndex);

  CCPoint tempPosition = swapWithLabel->getPosition();
  swapWithLabel->setPosition(label->getPosition());
  label->setPosition(tempPosition);

  sortingList->swapChildIndices(label, swapWithLabel);

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
  
  sortingList = CCNode::create();
  sortingList->setAnchorPoint({0, 1});
  sortingList->setPosition(HORIZONTAL_BORDER_SIZE, BOTTOM_BORDER_SIZE + contentSize.height);

  CCLabelBMFont *numbers[SORT_TYPE_COUNT];
  CCLabelBMFont *sortLabels[SORT_TYPE_COUNT];
  CCLayerColor  *listItemBackgrounds[SORT_TYPE_COUNT];
  
  float numberWidth = -1;
  float sortLabelWidth = -1;
  float textHeight = 32.5;
  float pad = 5;
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    numbers[i] = CCLabelBMFont::create(fmt::format("{}. ", i+1).c_str(), "bigFont.fnt");
    numbers[i]->setAnchorPoint({1, 1});
    numberWidth = std::max(numberWidth, numbers[i]->getContentWidth());

    sortLabels[i] = CCLabelBMFont::create((SORT_TYPE_NAMES[iconKitState.pendingSettings.sortBy[i]] + " ").c_str(), "bigFont.fnt");
    sortLabels[i]->setAnchorPoint({0, 1});
    sortLabelWidth = std::max(sortLabelWidth, sortLabels[i]->getContentWidth());
  }

  CCSize listItemSize{pad + numberWidth + sortLabelWidth + pad, pad + textHeight + pad};
  sortingList->setContentSize({0, SORT_TYPE_COUNT*listItemSize.height});
  
  numbers[0]->setPosition(CCPoint{pad + numberWidth, sortingList->getContentSize().height - pad});
  sortLabels[0]->setPosition(numbers[0]->getPosition());

  for (int i = 1; i < SORT_TYPE_COUNT; i++) {
    numbers[i]->setPosition(numbers[i-1]->getPosition() - CCPoint{0, listItemSize.height});
    sortLabels[i]->setPosition(sortLabels[i-1]->getPosition() - CCPoint{0, listItemSize.height});
  }
  
  sortingButtonMenu = CCMenu::create();
  sortingButtonMenu->setContentSize({0, SORT_TYPE_COUNT * listItemSize.height});
  sortingButtonMenu->setAnchorPoint({0, 1});
  sortingButtonMenu->setPosition({pad + numberWidth + sortLabelWidth, sortingList->getContentHeight() - sortingButtonMenu->getContentHeight()});
  
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
    /*
    moveUpButtonSprite->setScale(0.75);
    moveDownButtonSprite->setScale(0.75);
    reverseSortOrderButtonSprite->setScale(0.75);
    */

    CCMenuItemSpriteExtra *moveUp = CCMenuItemSpriteExtra::create(moveUpButtonSprite, this, menu_selector(SortPopup::onArrow));
    CCMenuItemSpriteExtra *moveDown = CCMenuItemSpriteExtra::create(moveDownButtonSprite, this, menu_selector(SortPopup::onArrow));
    CCMenuItemSpriteExtra *reverseSortOrder = CCMenuItemSpriteExtra::create(reverseSortOrderButtonSprite, this, menu_selector(SortPopup::onReverseSortOrder));

    moveUp->setTag(2*i);
    moveDown->setTag(2*i + 1);
    reverseSortOrder->setTag(i);
    
    CCPoint topLeft = CCPoint{0, sortingButtonMenu->getContentHeight() - listItemSize.height*i};

    moveUp->setPosition(topLeft + CCPoint{moveUp->getScaledContentSize().width/2, -listItemSize.height/2});
    moveDown->setPosition(moveUp->getPosition() + CCPoint{moveUp->getScaledContentSize().width/2 + menuGap + moveDown->getScaledContentSize().width/2, 0});
    reverseSortOrder->setPosition(moveDown->getPosition() + CCPoint{moveDown->getScaledContentSize().width/2 + menuGap + reverseSortOrder->getScaledContentSize().width/2, 0});

    if (i == 0) {
      menuWidth += moveUp->getScaledContentSize().width;
      menuWidth += moveDown->getScaledContentSize().width;
      menuWidth += reverseSortOrder->getScaledContentSize().width;
      menuWidth += 2*menuGap;
    }

    sortingButtonMenu->addChild(moveUp);
    sortingButtonMenu->addChild(moveDown);
    sortingButtonMenu->addChild(reverseSortOrder);
  }

  sortingButtonMenu->setContentWidth(menuWidth);
  listItemSize.width += menuWidth;
  sortingList->setContentWidth(listItemSize.width);

  sortingList->setScale(contentSize.width/listItemSize.width);


  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    listItemBackgrounds[i] = CCLayerColor::create({0, 0, 0, 0}, listItemSize.width, listItemSize.height);
    CCPoint bottomLeft = CCPoint{0, sortingList->getContentHeight() - pad - textHeight - pad};
    listItemBackgrounds[i]->setPosition(bottomLeft - CCPoint{0, listItemSize.height}*i);
  }
  
  for (int i = 0; i < SORT_TYPE_COUNT; i++) {
    sortingList->addChild(listItemBackgrounds[i]);
    sortingList->addChild(numbers[i]);
    sortingList->addChild(sortLabels[i]);
  }
  
  recalculateListItemBackgrounds();
  
  auto separator = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separator_height);
  auto separator2 = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separator_height);
  separator->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - title_margin);
  separator2->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - title_margin - sortingList->getScaledContentSize().height - separator_height);

  m_mainLayer->addChild(separator);
  m_mainLayer->addChild(sortingList);
  m_mainLayer->addChild(separator2);

  sortingList->addChild(sortingButtonMenu);

  CCMenu *togglerMenu = CCMenu::create();
  togglerMenu->setContentSize(contentSize - CCPoint{0, sortingList->getScaledContentSize().height + separator_height});
  togglerMenu->setPosition(separator2->getPosition() - CCPoint{-separator2->getContentWidth()/2, -separator_height/2 + togglerMenu->getContentHeight()/2});
  togglerMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Center));

  auto strictCategoryToggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(SortPopup::onToggle), 0.6f);
  strictCategoryToggler->setTag(0);
  strictCategoryToggler->toggle(iconKitState.pendingSettings.strictCategory);
  auto strictCategoryLabel = CCLabelBMFont::create("Strict Category", "bigFont.fnt");

  strictCategoryToggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
  strictCategoryLabel->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.375f));

  togglerMenu->addChild(strictCategoryToggler);
  togglerMenu->addChild(strictCategoryLabel);
  togglerMenu->updateLayout();
  CCMenu *infoMenu = CCMenu::create();
  infoMenu->setContentSize(m_size);
  infoMenu->setPosition(0, 0);
  InfoAlertButton *infoButton = InfoAlertButton::create(
    "Sort",

    "Customize the sorting order of the icons. Sorting methods that come first in the list get applied first.\n"
    "If a sorting method won't get utilized, its background will become <cr>red</c>.\n"
    "The <co>flip</c> toggle reverses a sorting method's sorting order.\n"
    "<cy>Strict Category</c> - <cb>Category</c> additionally sorts icons inside categories by progression\n",

    0.5f
  );
  infoMenu->addChild(infoButton);
  infoButton->setPosition(m_size - CCPoint{HORIZONTAL_BORDER_SIZE, TOP_BORDER_SIZE} - infoButton->getScaledContentSize()/2 - infoButton->getScaledContentSize()/4);

  m_mainLayer->addChild(togglerMenu);
  m_mainLayer->addChild(infoMenu);

  LinkedCCMenu *linkedMenu = LinkedCCMenu::createLinked(m_mainLayer);
  linkedMenu->linkChildren(sortingList);
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
