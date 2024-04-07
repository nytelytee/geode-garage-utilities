#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/FilterPopup.hpp>
#include <popups/AuthorFilterPopup.hpp>


using namespace geode::prelude;

void AuthorFilterPopup::onTopMenuButton(CCObject *sender) {
  CCMenuItemSpriteExtra *button = static_cast<CCMenuItemSpriteExtra *>(sender);
  int tag = button->getTag();
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount(); i += 2) {
    CCMenuItemToggler *toggler = getChild<CCMenuItemToggler>(m_buttonMenu, i);
    toggler->toggle(tag == 0 ? true : tag == 1 ? false : !toggler->isToggled());
    iconKitState.pendingSettings.authors[AUTHORS_IN_ORDER[toggler->getTag()]] = toggler->isToggled();
  }
}

void AuthorFilterPopup::toggleAuthor(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  iconKitState.pendingSettings.authors[AUTHORS_IN_ORDER[toggler->getTag()]] = !toggler->isToggled(); 
}

void AuthorFilterPopup::goToAuthorProfile(CCObject *sender) {
  CCMenuItemSpriteExtra *label_button = static_cast<CCMenuItemSpriteExtra *>(sender);
  int accountID = AUTHORS_IN_ORDER[label_button->getTag()];
  ProfilePage::create(accountID, accountID == GJAccountManager::get()->m_accountID)->show();
}

void AuthorFilterPopup::addAuthor(int tag, int accountID) {

  CCMenuItemToggler *toggler = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(AuthorFilterPopup::toggleAuthor), 1.f);
  toggler->setTag(tag);
  toggler->setScale(0.6f);
  toggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));

  bool next_author_even = !((m_buttonMenu->getChildrenCount() / 2 + 1) & 1);

  gd::string username = !accountID ? "RobTop" : GameStatsManager::sharedState()->usernameForAccountID(accountID);
  const char* font = accountID ? "goldFont.fnt" : "bigFont.fnt";
  ccColor3B color = accountID < 0 ? ccColor3B{90, 255, 255} : ccColor3B{255, 255, 255};
  auto label = CCLabelBMFont::create(username.c_str(), font);

  label->setColor(color);

  float maxScale = 0.375f;
  // make the fonts the same size (the size of bigFont.fnt)
  if (font[0] == 'g') maxScale *= 1.24096385542169f;  // goldFont.fnt
  if (label->getContentSize().width * maxScale > (m_scrollLayerSize.width/2 - toggler->getContentSize().width)) {
    maxScale = (m_scrollLayerSize.width/2 - toggler->getContentSize().width) / label->getContentSize().width;
  }

  CCMenuItemSpriteExtra *label_button = CCMenuItemSpriteExtra::create(label, this, menu_selector(AuthorFilterPopup::goToAuthorProfile));
  if (accountID <= 0) label_button->setEnabled(false);
  label_button->setTag(tag);
  
  label_button->setLayoutOptions(
    AxisLayoutOptions::create()->
    setBreakLine(next_author_even)->
    setSameLine(next_author_even)->
    setMaxScale(maxScale)
  );
  toggler->toggle(iconKitState.pendingSettings.authors[accountID]);

  m_buttonMenu->addChild(toggler);
  m_buttonMenu->addChild(label_button);

}

void AuthorFilterPopup::customSetup() {
  
  this->setTitle("Author Filter");
  
  for (size_t i = 0; i < AUTHORS_IN_ORDER.size(); i++)
    addAuthor(i, AUTHORS_IN_ORDER[i]);

  m_buttonMenu->updateLayout();
  
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount()/2; i += 2) {
    //CCMenuItemToggler *toggler = getChild<CCMenuItemToggler>(m_buttonMenu, 2*i);
    CCMenuItemSpriteExtra *label_button = getChild<CCMenuItemSpriteExtra>(m_buttonMenu, 2*i+1);
    float gap = m_scrollLayerSize.width/2 - label_button->boundingBox().getMaxX();
    label_button->setLayoutOptions(static_cast<AxisLayoutOptions *>(label_button->getLayoutOptions())->setNextGap(gap));
  }

  m_buttonMenu->updateLayout();
}

AuthorFilterPopup* AuthorFilterPopup::create(FilterAndSortPopup *parent) {
    auto ret = new AuthorFilterPopup();
    if (ret && ret->initAnchored(300.f, 255.f, parent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
