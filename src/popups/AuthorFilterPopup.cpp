#include <Geode/Geode.hpp>
#include <algorithm>

#include <constants.hpp>
#include <iconkit.hpp>
#include <popups/FilterPopup.hpp>
#include <popups/AuthorFilterPopup.hpp>
#include <utils/CCMenuItemTogglerSpoof.hpp>


using namespace geode::prelude;

void AuthorFilterPopup::onTopMenuButton(CCObject *sender) {
  CCMenuItemSpriteExtra *button = static_cast<CCMenuItemSpriteExtra *>(sender);
  int tag = button->getTag();
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount(); i += 2) {
    CCMenuItemToggler *toggler = getChild<CCMenuItemToggler>(m_buttonMenu, int(i));
    toggler->toggle(tag == 0 ? true : tag == 1 ? false : !toggler->isToggled());
    iconKitState.pendingSettings.authors[AUTHORS_IN_ORDER[size_t(toggler->getTag())]] = toggler->isToggled();
  }
}

void AuthorFilterPopup::toggleAuthor(CCObject *sender) {
  CCMenuItemToggler *toggler = static_cast<CCMenuItemToggler *>(sender);
  iconKitState.pendingSettings.authors[AUTHORS_IN_ORDER[size_t(toggler->getTag())]] = !toggler->isToggled(); 
}

void AuthorFilterPopup::goToAuthorProfile(CCObject *sender) {
  CCMenuItemSpriteExtra *label_button = static_cast<CCMenuItemSpriteExtra *>(sender);
  int accountID = AUTHORS_IN_ORDER[size_t(label_button->getTag())];
  ProfilePage::create(accountID, accountID == GJAccountManager::get()->m_accountID)->show();
}

void AuthorFilterPopup::addAuthor(int tag, int accountID) {

  CCMenuItemToggler *toggler = createTogglerWithStandardSpritesSpoofOn(this, menu_selector(AuthorFilterPopup::toggleAuthor));
  toggler->setTag(tag);
  toggler->setScale(0.6f);
  toggler->setLayoutOptions(AxisLayoutOptions::create()->setAutoScale(false));

  bool nextEven = !((m_buttonMenu->getChildrenCount() / 2 + 1) & 1);

  gd::string username = !accountID ? "RobTop" : GameLevelManager::get()->tryGetUsername(accountID);
  const char* font = accountID ? "goldFont.fnt" : "bigFont.fnt";
  ccColor3B color = accountID < 0 ? ccColor3B{90, 255, 255} : ccColor3B{255, 255, 255};
  CCLabelBMFont* label = CCLabelBMFont::create(username.c_str(), font);

  label->setColor(color);

  // make the gold font the same initial size as the big font
  float maxScale = 0.375f * (font[0] == 'g' ? 1.24096385542169f : 1.f);

  float gap = static_cast<AxisLayout*>(m_buttonMenu->getLayout())->getGap();
  float textWidth = label->getContentSize().width;
  // technically, one of the gaps is not always the same as the other, it may get extended later to put the
  // toggler and label exactly on the second half of the menu, but if that happens, then that means that the
  // normal minimum of 0.375 (plus the gold font scaling) succeeded, so this is fine
  float maximumTextWidth = m_buttonMenuSize.width/2 - toggler->getScaledContentSize().width - gap - (!nextEven)*gap;
  if (textWidth * maxScale > maximumTextWidth) maxScale = maximumTextWidth/textWidth;

  CCMenuItemSpriteExtra *labelButton = CCMenuItemSpriteExtra::create(label, this, menu_selector(AuthorFilterPopup::goToAuthorProfile));
  if (accountID <= 0) labelButton->setEnabled(false);
  labelButton->setTag((int) tag);
  
  labelButton->setLayoutOptions(
    AxisLayoutOptions::create()->
    setBreakLine(nextEven)->
    setSameLine(nextEven)->
    setScaleLimits(0.f, maxScale)
  );
  toggler->toggle(iconKitState.pendingSettings.authors[accountID]);

  m_buttonMenu->addChild(toggler);
  m_buttonMenu->addChild(labelButton);

}

void AuthorFilterPopup::customSetup() {
  
  this->setTitle("Author Filter");
  
  for (size_t i = 0; i < AUTHORS_IN_ORDER.size(); i++)
    addAuthor(int(i), AUTHORS_IN_ORDER[i]);

  m_buttonMenu->updateLayout();
  
  for (size_t i = 0; i < m_buttonMenu->getChildrenCount()/2; i += 2) {
    CCMenuItemSpriteExtra *labelButton = getChild<CCMenuItemSpriteExtra>(m_buttonMenu, 2*int(i)+1);
    float gap = m_buttonMenuSize.width/2 - labelButton->boundingBox().getMaxX();
    labelButton->setLayoutOptions(static_cast<AxisLayoutOptions *>(labelButton->getLayoutOptions())->setNextGap(gap));
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
