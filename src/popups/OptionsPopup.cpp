#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/OptionsPopup.hpp>
#include <popups/FilterAndSortPopup.hpp>
#include <utils/LinkedCCMenu.hpp>


using namespace geode::prelude;

void OptionsPopup::onClose(CCObject *sender) {
  Popup<bool, FilterAndSortPopup *>::onClose(sender);
  if (parentPopup) parentPopup->refreshMenuState();
};

void OptionsPopup::toggleOption(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  int tag = toggler->getTag();
  if (tag & 1) {
    std::string gameVariableStr = fmt::format("{:04}", tag >> 1);
    GameManager::get()->setGameVariable(gameVariableStr.c_str(), !toggler->isToggled());
  } else switch (tag >> 1) {
    case 0: iconKitState.pendingSettings.showDenied = !toggler->isToggled(); break;
    case 1: iconKitState.pendingSettings.separateAcceptedFromDenied = !toggler->isToggled(); break;
    case 2: iconKitState.pendingSettings.dontRecolorNavigationButtons = !toggler->isToggled(); break;
    case 3: iconKitState.pendingSettings.noGapBetweenAcceptedAndDenied = !toggler->isToggled(); break;
    case 4: iconKitState.pendingSettings.hideNavigationMenuOnSinglePage = !toggler->isToggled(); break;
  }
}

void OptionsPopup::addOption(const char* name, int option, bool isGameVariable) {

  CCMenuItemToggler *toggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(OptionsPopup::toggleOption), 1);
  toggler->setTag(option << 1 | int(isGameVariable));
  toggler->setScale(0.8f);
  toggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));

  auto label = CCLabelBMFont::create(name, "bigFont.fnt");
  float maxScale = 0.375f;
  if (label->getContentSize().width * maxScale > (m_size.width - 2*HORIZONTAL_BORDER_SIZE - 40 - toggler->getContentSize().width/0.8f)) {
    maxScale = (m_size.width - 2*HORIZONTAL_BORDER_SIZE - 40 - toggler->getContentSize().width/0.8f) / label->getContentSize().width;
  }
  label->setLayoutOptions(AxisLayoutOptions::create()->setBreakLine(true)->setSameLine(true)->setMaxScale(maxScale));
  
  if (isGameVariable) {
    std::string gameVariableStr = fmt::format("{:04}", option);
    toggler->toggle(GameManager::get()->getGameVariable(gameVariableStr.c_str()));
  } else switch (option) {
    case 0: toggler->toggle(iconKitState.pendingSettings.showDenied); break;
    case 1: toggler->toggle(iconKitState.pendingSettings.separateAcceptedFromDenied); break;
    case 2: toggler->toggle(iconKitState.pendingSettings.dontRecolorNavigationButtons); break;
    case 3: toggler->toggle(iconKitState.pendingSettings.noGapBetweenAcceptedAndDenied); break;
    case 4: toggler->toggle(iconKitState.pendingSettings.hideNavigationMenuOnSinglePage); break;
  }

  m_buttonMenu->addChild(toggler);
  m_buttonMenu->addChild(label);
}

void OptionsPopup::addOptionInfo(int tag, const char* description) {
  InfoAlertButton *infoButton = InfoAlertButton::create("Info", description, 0.5f);
  CCMenuItemToggler *button = static_cast<CCMenuItemToggler *>(m_buttonMenu->getChildByTag(tag));
  auto [x, y] = button->getScaledContentSize() / 2;
  auto [bx, by] = infoButton->getContentSize() / 2;
  infoButton->setPosition(button->getPosition() - CCPoint{x, -y} - CCPoint{bx, -by});
  m_buttonMenu->addChild(infoButton);
}

bool OptionsPopup::setup(bool gameVar, FilterAndSortPopup *parent) {
  parentPopup = parent;

  auto winSize = CCDirector::sharedDirector()->getWinSize();

  this->setTitle(gameVar ? "Player Options" : "Icon Kit Display Options");
  
  float titleMargin = m_bgSprite->boundingBox().getMaxY() - m_title->boundingBox().getMaxY() - TOP_BORDER_SIZE;
  float titleHeight = m_title->getScale()*m_title->getContentSize().height;
  float realTitleHeight = 2*titleMargin + titleHeight;
  float separatorHeight = 1;
  CCSize contentSize = m_size - CCPoint{2*HORIZONTAL_BORDER_SIZE, realTitleHeight + BOTTOM_BORDER_SIZE + TOP_BORDER_SIZE};

  auto separator = CCLayerColor::create({ 0, 0, 0, 50 }, m_size.width - 2*HORIZONTAL_BORDER_SIZE, separatorHeight);
  separator->setPosition(HORIZONTAL_BORDER_SIZE, m_title->boundingBox().getMinY() - titleMargin);

  m_buttonMenu = CCMenu::create();
  m_buttonMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Start)->setGrowCrossAxis(true)->setAutoScale(true));
  m_buttonMenu->setContentSize({m_size.width - 2*HORIZONTAL_BORDER_SIZE - 40, 0});

  m_buttonMenu->setPosition(m_size/2 - CCPoint{0, realTitleHeight/2});

  if (gameVar) {
    addOption("Default Mini Icon", 60, true);
    addOption("Switch Spider Teleport Color", 61, true);
    addOption("Switch Dash Fire Color", 62, true);
    addOption("Switch Wave Trail Color", 96, true);
  } else {
    addOption("Show Denied Icons", 0, false);
    addOption("Separate Accepted\nFrom Denied", 1, false);
    addOption("Don't Recolor\nNavigation Buttons", 2, false);
    addOption("No Gap Between Accepted\nand Denied Navigation Buttons", 3, false);
    addOption("Hide Navigation Menu\non Single Page", 4, false);
  }

  m_buttonMenu->updateLayout();
  m_buttonMenu->setLayout(nullptr);
  float topTogglerMax = getChild<InfoAlertButton>(m_buttonMenu, 0)->boundingBox().getMaxY();
  
  float topInfoButtonMax;
  if (gameVar) {
    addOptionInfo(60 << 1 | 1, "Sets player icon in mini mode to default.");
    topInfoButtonMax = getChild<InfoAlertButton>(m_buttonMenu, -1)->boundingBox().getMaxY();
    addOptionInfo(61 << 1 | 1, "Toggles between main and secondary color for the\nteleport effect in spider mode.");
    addOptionInfo(62 << 1 | 1, "Toggles between main and secondary color for the\nfire effect from dash orbs.");
    addOptionInfo(96 << 1 | 1, "Toggles between main and secondary color for the\ntrail in wave mode.");
  } else {
    addOptionInfo(0 << 1, "Whether <cr>denied</c> icons will be shown after <cg>accepted</c> icons. If off, <cr>denied</c> icons are completely hidden.");
    topInfoButtonMax = getChild<InfoAlertButton>(m_buttonMenu, -1)->boundingBox().getMaxY();
    addOptionInfo(1 << 1, "Whether the <cr>denied</c> icons will start showing up on a separate page.");
    addOptionInfo(2 << 1, "Whether the navigation button (page) colors will be changed to\nshow which icons the current page contains.\n<cj>Blue</c> - only <cg>accepted</c> icons\n<cr>Red</c> - only <cr>denied</c> icons\n<cp>Magenta</c> - both <cg>accepted</c> and <cr>denied</c> icons\nIf off, the current page color is always <cj>blue</c>.");
    addOptionInfo(3 << 1, "If <cb>Separate Accepted From Denied</c> is active, there is a gap between <cg>accepted</c> and <cr>denied</c> pages. This removes the gap.");
    addOptionInfo(4 << 1, "In vanilla, if there is only one page, the navigation menu is not shown.\nSince this mod adds extra information to it, it makes sense to show it anyway.\nToggle to revert to vanilla behavior.");
  }
  
  m_buttonMenu->setPositionY(m_buttonMenu->getPositionY() - (topInfoButtonMax - topTogglerMax)/2);
   
  m_mainLayer->addChild(m_buttonMenu);
  m_mainLayer->addChild(separator);

  m_mainLayer->addChild(LinkedCCMenu::createLinked(m_mainLayer));

  return true;
}

OptionsPopup* OptionsPopup::createWithDisplayOptions(FilterAndSortPopup *parent) {
    auto ret = new OptionsPopup();
    if (ret && ret->initAnchored(260.f, 240.f, false, parent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

OptionsPopup* OptionsPopup::createWithPlayerOptions() {
    auto ret = new OptionsPopup();
    if (ret && ret->initAnchored(260.f, 210.f, true, nullptr)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
