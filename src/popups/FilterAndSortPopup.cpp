#include <Geode/Geode.hpp>
#include <algorithm>
#include <random>

#include <constants.hpp>
#include <iconkit.hpp>
#include <logic.hpp>
#include <popups/FilterAndSortPopup.hpp>
#include <popups/SortPopup.hpp>
#include <popups/AuthorFilterPopup.hpp>
#include <popups/CategoryFilterPopup.hpp>
#include <popups/OptionsPopup.hpp>

#include <utils/LinkedCCMenu.hpp>

using namespace geode::prelude;

int randomNumber(int rangeStart, int rangeEnd) {
  std::random_device os_seed;
  const unsigned int seed = os_seed();
  std::mt19937 generator(seed);
  std::uniform_int_distribution<int> distribute(rangeStart, rangeEnd);
  return distribute(generator);
}

SpacerNode* createSpacerNodeWithBreakLine(size_t grow) {
  SpacerNode* node = SpacerNode::create(grow);
  node->setLayoutOptions(AxisLayoutOptions::create()->setBreakLine(true));
  return node;
}

enum ActionButtonType { Cancel, Apply };
enum MenuButtonType { AuthorFilterPopup, CategoryFilterPopup, SortPopup, IconKitDisplayOptionsPopup, ResetToDefault, SelectAll, DeselectAll, InvertFilter, ChangeFilterType };
enum TogglerType { Locked, Unlocked, Vanilla, Custom };

void FilterAndSortPopup::onActionButton(CCObject *sender) {
  CCMenuItemSpriteExtra *button = static_cast<CCMenuItemSpriteExtra *>(sender);
  if (button->getTag() == ActionButtonType::Cancel) return Popup<>::onClose(sender);
  iconKitState.settings = iconKitState.pendingSettings;
  recalculateIconOrder();
  // we are forcing every icon page to point to the page of the active icon
  // (or the first page if the active icon is filtered out) after
  // recalculating filters, to emulate what happens when you first
  // open the garage
  for (IconType iconType : ICON_TYPES_TO_CHANGE) {
    iconKitState.pageForIcon[iconType] = -1;
    iconKitState.overridePageForIcon[iconType] = true;
  }
  if (SHOULD_CHANGE_ICON_TYPE(iconKitState.selectedIconType))
    getChild<GJGarageLayer>(CCScene::get(), 0)->setupPage(-1, iconKitState.selectedIconType);
  Mod::get()->setSavedValue<IconKitSettings>("iconKitSettings", iconKitState.settings);
  Popup<>::onClose(sender);
}

void FilterAndSortPopup::refreshMenuState() {
  getChildOfType<CCMenuItemToggler>(m_buttonMenu, 0)->toggle(iconKitState.pendingSettings.locked);
  getChildOfType<CCMenuItemToggler>(m_buttonMenu, 1)->toggle(iconKitState.pendingSettings.unlocked);
  getChildOfType<CCMenuItemToggler>(m_buttonMenu, 2)->toggle(iconKitState.pendingSettings.vanilla);
  getChildOfType<CCMenuItemToggler>(m_buttonMenu, 3)->toggle(iconKitState.pendingSettings.custom);

  bool allTrue =   std::all_of(iconKitState.pendingSettings.authors.begin(), iconKitState.pendingSettings.authors.end(), [](const std::pair<int, bool> element){ return element.second; });
  bool noneTrue = std::none_of(iconKitState.pendingSettings.authors.begin(), iconKitState.pendingSettings.authors.end(), [](const std::pair<int, bool> element){ return element.second; });
  ButtonSprite *authorSprite   = getChild<ButtonSprite>(getChildOfType<CCMenuItemSpriteExtra>(m_buttonMenu, 0), 0);
  if (noneTrue)     authorSprite->updateBGImage(iconKitState.pendingSettings.invert ? "GJ_button_01.png" : "GJ_button_06.png");
  else if (allTrue) authorSprite->updateBGImage(iconKitState.pendingSettings.invert ? "GJ_button_06.png" : "GJ_button_01.png");
  else              authorSprite->updateBGImage("GU_button_01.png"_spr);

  allTrue =   std::all_of(iconKitState.pendingSettings.categories.begin(), iconKitState.pendingSettings.categories.end(), [](const std::pair<std::string, bool> &element){ return element.second; });
  noneTrue = std::none_of(iconKitState.pendingSettings.categories.begin(), iconKitState.pendingSettings.categories.end(), [](const std::pair<std::string, bool> &element){ return element.second; });
  ButtonSprite *categorySprite = getChild<ButtonSprite>(getChildOfType<CCMenuItemSpriteExtra>(m_buttonMenu, 1), 0);
  if (noneTrue)     categorySprite->updateBGImage(iconKitState.pendingSettings.invert ? "GJ_button_01.png" : "GJ_button_06.png");
  else if (allTrue) categorySprite->updateBGImage(iconKitState.pendingSettings.invert ? "GJ_button_06.png" : "GJ_button_01.png");
  else              categorySprite->updateBGImage("GU_button_01.png"_spr);
  
  ButtonSprite *sortSprite = getChild<ButtonSprite>(getChildOfType<CCMenuItemSpriteExtra>(m_buttonMenu, 2), 0);
  if (randomNumber(1, 100) == 7) {
    sortSprite->updateBGImage("geode.loader/GE_button_04.png");
    sortSprite->m_BGSprite->setCapInsets({5, 5, 30, 30});
  } else sortSprite->updateBGImage("GJ_button_02.png");

  ButtonSprite *displaySettingsSprite = getChild<ButtonSprite>(getChildOfType<CCMenuItemSpriteExtra>(m_buttonMenu, 3), 0);
  if (randomNumber(1, 100) == 7) {
    displaySettingsSprite->updateBGImage("geode.loader/GE_button_04.png");
    displaySettingsSprite->m_BGSprite->setCapInsets({5, 5, 30, 30});
  } else displaySettingsSprite->updateBGImage("GJ_button_02.png");

  getChild<CCSprite>(getChild<CCMenuItemSpriteExtra>(m_topMenu, 4), 0)->setColor(iconKitState.pendingSettings.invert ? ccColor3B{255, 255, 255} : ccColor3B{75, 75, 75});
}


void FilterAndSortPopup::onMenuButton(CCObject *sender) {
  CCMenuItemSpriteExtra *button = static_cast<CCMenuItemSpriteExtra *>(sender);
  switch (button->getTag()) {
    case MenuButtonType::AuthorFilterPopup:          return AuthorFilterPopup::create(this)->show();
    case MenuButtonType::CategoryFilterPopup:        return CategoryFilterPopup::create(this)->show();
    case MenuButtonType::SortPopup:                  return SortPopup::create(this)->show();
    case MenuButtonType::IconKitDisplayOptionsPopup: return OptionsPopup::createWithDisplayOptions(this)->show();
    case MenuButtonType::ResetToDefault:             iconKitState.pendingSettings.resetToDefault(); break;
    case MenuButtonType::SelectAll:                  iconKitState.pendingSettings.selectAll(); break;
    case MenuButtonType::DeselectAll:                iconKitState.pendingSettings.deselectAll(); break;
    case MenuButtonType::InvertFilter:               iconKitState.pendingSettings.invert = !iconKitState.pendingSettings.invert; break;
    case MenuButtonType::ChangeFilterType:           iconKitState.pendingSettings.changeFilterType(); break;
  }
  refreshMenuState();
}



void FilterAndSortPopup::onToggle(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  switch (toggler->getTag()) {
    case TogglerType::Locked:   iconKitState.pendingSettings.locked = !toggler->isToggled(); break;
    case TogglerType::Unlocked: iconKitState.pendingSettings.unlocked = !toggler->isToggled(); break;
    case TogglerType::Vanilla:  iconKitState.pendingSettings.vanilla = !toggler->isToggled(); break;
    case TogglerType::Custom:   iconKitState.pendingSettings.custom = !toggler->isToggled(); break;
  }
}

bool FilterAndSortPopup::setup() {
  iconKitState.pendingSettings = iconKitState.settings;
  this->setTitle("Filter & Sort");
  float separator_height = 1;
  float title_margin = m_bgSprite->boundingBox().getMaxY() - m_title->boundingBox().getMaxY() - TOP_BORDER_SIZE;
  float delta = 20;

  auto separator = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separator_height);
  separator->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - title_margin);
  m_mainLayer->addChild(separator);

  float layoutGap = 10.f;
  m_buttonMenu = CCMenu::create();
  m_buttonMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAxisAlignment(AxisAlignment::Start)->setGap(layoutGap));

  CCMenuItemToggler *lockedToggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(FilterAndSortPopup::onToggle), 0.6f);
  lockedToggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
  CCLabelBMFont *lockedLabel = CCLabelBMFont::create("Locked", "bigFont.fnt");
  lockedLabel->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.375));
  
  CCMenuItemToggler *unlockedToggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(FilterAndSortPopup::onToggle), 0.6f);
  unlockedToggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
  CCLabelBMFont *unlockedLabel = CCLabelBMFont::create("Unlocked", "bigFont.fnt");
  unlockedLabel->setLayoutOptions(AxisLayoutOptions::create()->setSameLine(true)->setBreakLine(true)->setMaxScale(0.375f));

  CCMenuItemToggler *vanillaToggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(FilterAndSortPopup::onToggle), 0.6f);
  vanillaToggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
  CCLabelBMFont *vanillaLabel = CCLabelBMFont::create("Vanilla", "bigFont.fnt");
  vanillaLabel->setLayoutOptions(AxisLayoutOptions::create()->setSameLine(false)->setBreakLine(false)->setMaxScale(0.375f));

  CCMenuItemToggler *customToggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(FilterAndSortPopup::onToggle), 0.6f);
  customToggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));
  CCLabelBMFont *customLabel = CCLabelBMFont::create("Custom", "bigFont.fnt");
  customLabel->setLayoutOptions(AxisLayoutOptions::create()->setSameLine(false)->setBreakLine(true)->setMaxScale(0.375f));

  ButtonSprite *authorSprite = ButtonSprite::create("Author");
  CCMenuItemSpriteExtra *author = CCMenuItemSpriteExtra::create(authorSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  author->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.5f));

  ButtonSprite *categorySprite = ButtonSprite::create("Category");
  CCMenuItemSpriteExtra *category = CCMenuItemSpriteExtra::create(categorySprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  category->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.5f));

  ButtonSprite *sortSprite = ButtonSprite::create("Sort");
  CCMenuItemSpriteExtra *sort = CCMenuItemSpriteExtra::create(sortSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  sort->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.5f));

  ButtonSprite *displaySettingsSprite = ButtonSprite::create("Display");
  CCMenuItemSpriteExtra *displaySettings = CCMenuItemSpriteExtra::create(displaySettingsSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  displaySettings->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.5f));


  lockedToggler->setTag(TogglerType::Locked);
  unlockedToggler->setTag(TogglerType::Unlocked);
  vanillaToggler->setTag(TogglerType::Vanilla);
  customToggler->setTag(TogglerType::Custom);

  author->setTag(MenuButtonType::AuthorFilterPopup);
  category->setTag(MenuButtonType::CategoryFilterPopup);
  sort->setTag(MenuButtonType::SortPopup);
  displaySettings->setTag(MenuButtonType::IconKitDisplayOptionsPopup);

  m_buttonMenu->setContentWidth(m_size.width - 2*HORIZONTAL_BORDER_SIZE - delta);

  m_buttonMenu->addChild(lockedToggler);
  m_buttonMenu->addChild(lockedLabel);
  m_buttonMenu->addChild(unlockedToggler);
  m_buttonMenu->addChild(unlockedLabel);
  
  m_buttonMenu->addChild(vanillaToggler);
  m_buttonMenu->addChild(vanillaLabel);
  m_buttonMenu->addChild(customToggler);
  m_buttonMenu->addChild(customLabel);
  
  m_buttonMenu->addChild(SpacerNode::create(1));
  m_buttonMenu->addChild(author);
  m_buttonMenu->addChild(SpacerNode::create(1));
  m_buttonMenu->addChild(category);
  m_buttonMenu->addChild(createSpacerNodeWithBreakLine(1));

  m_buttonMenu->addChild(SpacerNode::create(1));
  m_buttonMenu->addChild(sort);
  m_buttonMenu->addChild(SpacerNode::create(1));
  m_buttonMenu->addChild(displaySettings);
  m_buttonMenu->addChild(SpacerNode::create(1));

  m_buttonMenu->updateLayout();
 
  // i want everything to be aligned to center, but the togglers to be aligned horizontally in a grid as well
  // achieving this by aligning to start, setting up the gaps how i want them, and then adding fake nodes at
  // the start of each line to push the other nodes to the center
  float gapFirstLine = m_buttonMenu->getContentWidth()/2 - lockedToggler->m_offButton->getScaledContentSize().width - lockedLabel->getScaledContentSize().width - layoutGap;
  float gapSecondLine = m_buttonMenu->getContentWidth()/2 - vanillaToggler->m_offButton->getScaledContentSize().width - vanillaLabel->getScaledContentSize().width - layoutGap;
  lockedLabel->setLayoutOptions(static_cast<AxisLayoutOptions *>(lockedLabel->getLayoutOptions())->setNextGap(gapFirstLine));
  vanillaLabel->setLayoutOptions(static_cast<AxisLayoutOptions *>(vanillaLabel->getLayoutOptions())->setNextGap(gapSecondLine));
  m_buttonMenu->updateLayout();
  float minRemainingSpace = m_buttonMenu->getContentWidth() - std::max(unlockedLabel->boundingBox().getMaxX(), customLabel->boundingBox().getMaxX());
  CCNode *alignToCenter1 = CCNode::create();
  CCNode *alignToCenter2 = CCNode::create();
  alignToCenter1->setContentWidth(minRemainingSpace/2);
  alignToCenter2->setContentWidth(minRemainingSpace/2);
  alignToCenter1->setLayoutOptions(AxisLayoutOptions::create()->setNextGap(0));
  alignToCenter2->setLayoutOptions(AxisLayoutOptions::create()->setNextGap(0));
  m_buttonMenu->insertBefore(alignToCenter1, lockedToggler);
  m_buttonMenu->insertBefore(alignToCenter2, vanillaToggler);
  m_buttonMenu->updateLayout();
  m_buttonMenu->setPosition((m_size/2).width, separator->getPositionY() - m_buttonMenu->getContentHeight()/2 - delta/2);

  m_mainLayer->addChild(m_buttonMenu);
  
  m_actionMenu = CCMenu::create();
  m_actionMenu->setLayout(RowLayout::create()->setGrowCrossAxis(true)->setAxisAlignment(AxisAlignment::Center)->setGap(layoutGap));
  ButtonSprite *cancelSprite = ButtonSprite::create("Cancel");
  CCMenuItemSpriteExtra *cancel = CCMenuItemSpriteExtra::create(cancelSprite, this, menu_selector(FilterAndSortPopup::onActionButton));
  cancel->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.5f));
  ButtonSprite *applySprite = ButtonSprite::create("Apply");
  CCMenuItemSpriteExtra *apply = CCMenuItemSpriteExtra::create(applySprite, this, menu_selector(FilterAndSortPopup::onActionButton));
  apply->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(0.5f));
  m_actionMenu->setContentWidth(m_size.width - 2*HORIZONTAL_BORDER_SIZE - delta);
  cancel->setTag(ActionButtonType::Cancel);
  apply->setTag(ActionButtonType::Apply);
  m_actionMenu->addChild(SpacerNode::create(1));
  m_actionMenu->addChild(cancel);
  m_actionMenu->addChild(SpacerNode::create(2));
  m_actionMenu->addChild(apply);
  m_actionMenu->addChild(SpacerNode::create(1));
  m_actionMenu->updateLayout();
  m_actionMenu->setPosition((m_size/2).width, m_actionMenu->getContentHeight()/2 + delta/2 + BOTTOM_BORDER_SIZE);
  m_mainLayer->addChild(m_actionMenu);
  
  auto separator2 = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separator_height);
  separator2->setPosition(separator->getPositionX(), m_actionMenu->getPositionY() + m_actionMenu->getContentHeight()/2 + delta/2);
  m_mainLayer->addChild(separator2);

  m_closeBtn->removeFromParentAndCleanup(true);

  m_topMenu = CCMenu::create();
  m_topMenu->setLayout(RowLayout::create()->setGap(2.f)->setGrowCrossAxis(true));
  
  CCSprite *invertSprite = CCSprite::createWithSpriteFrameName("GJ_sortIcon_001.png");

  CCSprite *flipSprite = CCSprite::createWithSpriteFrameName("edit_ccwBtn_001.png");
  CCSprite *filterSprite = CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png");
  filterSprite->setScale(0.5f);
  CCNode *changeFilterTypeNode = CCNode::create();
  changeFilterTypeNode->addChild(flipSprite);
  changeFilterTypeNode->addChild(filterSprite);
  changeFilterTypeNode->setScale(0.45f);
  changeFilterTypeNode->setContentSize(flipSprite->getContentSize());
  flipSprite->setPosition(changeFilterTypeNode->getContentSize()/2);
  filterSprite->setPosition(changeFilterTypeNode->getContentSize()/2.25f);
  
  CircleButtonSprite *resetToDefaultSprite = CircleButtonSprite::createWithSpriteFrameName("geode.loader/reset.png", 1.f, CircleBaseColor::Green, CircleBaseSize::Tiny);
  resetToDefaultSprite->setScale(0.75f);
  resetToDefaultSprite->getTopNode()->setAnchorPoint({0.45f, 0.525f});

  CCLabelBMFont* allLabel = CCLabelBMFont::create("All", "bigFont.fnt");
  CircleButtonSprite *selectAllSprite = CircleButtonSprite::create(allLabel, CircleBaseColor::Green, CircleBaseSize::Tiny);
  selectAllSprite->setScale(0.75f);

  CCLabelBMFont* noneLabel = CCLabelBMFont::create("None", "bigFont.fnt");
  CircleButtonSprite *deselectAllSprite = CircleButtonSprite::create(noneLabel, CircleBaseColor::Green, CircleBaseSize::Tiny);
  deselectAllSprite->setScale(0.75f);

  invertSprite->setRotation(90);
  invertSprite->setScale(0.45f);
  CCMenuItemSpriteExtra *resetToDefault = CCMenuItemSpriteExtra::create(resetToDefaultSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  CCMenuItemSpriteExtra *selectAll = CCMenuItemSpriteExtra::create(selectAllSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  CCMenuItemSpriteExtra *deselectAll = CCMenuItemSpriteExtra::create(deselectAllSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));

  CCMenuItemSpriteExtra *invert = CCMenuItemSpriteExtra::create(invertSprite, this, menu_selector(FilterAndSortPopup::onMenuButton));
  invert->setLayoutOptions(AxisLayoutOptions::create()->setNextGap(5.f));
  CCMenuItemSpriteExtra *changeFilterType = CCMenuItemSpriteExtra::create(changeFilterTypeNode, this, menu_selector(FilterAndSortPopup::onMenuButton));
  changeFilterType->setLayoutOptions(AxisLayoutOptions::create()->setNextGap(5.f));

  resetToDefault->setTag(MenuButtonType::ResetToDefault);
  selectAll->setTag(MenuButtonType::SelectAll);
  deselectAll->setTag(MenuButtonType::DeselectAll);
  invert->setTag(MenuButtonType::InvertFilter);
  changeFilterType->setTag(MenuButtonType::ChangeFilterType);

  m_topMenu->addChild(resetToDefault);
  m_topMenu->addChild(selectAll);
  m_topMenu->addChild(deselectAll);
  m_topMenu->addChild(SpacerNode::create(1));
  m_topMenu->addChild(invert);
  m_topMenu->addChild(changeFilterType);
  m_topMenu->addChild(InfoAlertButton::create(
    "Info",

    "Allows you to change which icons appear and in what order.\n"
    "The <co>reset</c> button resets the filters and sort to default settings.\n"
    "The <co>all</c>/<co>none</c> buttons select/deselect all filters.\n"
    "The <co>flip</c> toggle inverts the entire filter.\n"
    "The <co>rotate filter</c> button inverts every individual filter and inverts the entire filter (effectively toggles the ALL to ANY and back).\n"
    "ALL filters must pass for an icon to be <cg>accepted</c>; the filters are:\n"
    "<cy>Lock State</c> - locked/unlocked and <cb>Locked</c>/<cb>Unlocked</c> is ticked\n"
    "<cy>Icon Type</c> - vanilla/custom and <cb>Vanilla</c>/<cb>Custom</c> is ticked\n"
    "<cy>Author</c> (vanilla only) - its author is ticked in <cb>Author</c>\n"
    "<cy>Category</c> (vanilla only) - its category is ticked in <cb>Category</c>",

    0.5
  ));
  m_topMenu->setContentWidth(m_size.width - 2*HORIZONTAL_BORDER_SIZE - delta/2);
  m_topMenu->updateLayout();
  m_topMenu->setPosition((m_size/2).width, m_size.height - TOP_BORDER_SIZE - m_topMenu->getContentHeight()/2);
  m_mainLayer->addChild(m_topMenu);

  m_mainLayer->addChild(LinkedCCMenu::createLinked(m_mainLayer));

  refreshMenuState();

  return true;
}

FilterAndSortPopup* FilterAndSortPopup::create() {
    auto ret = new FilterAndSortPopup();
    if (ret && ret->initAnchored(280, 200)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
