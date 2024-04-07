#include <Geode/utils/cocos.hpp>
#include <cocos-ext.h>
#include <utils/IconButtonSpriteNoText.hpp>

using namespace geode::prelude;

bool IconButtonSpriteNoText::init(char const* bg, bool bgIsFrame, cocos2d::CCNode* icon) {
  if (!CCSprite::init()) return false;

  if (bgIsFrame) m_bg = CCScale9Sprite::createWithSpriteFrameName(bg);
  else m_bg = CCScale9Sprite::create(bg);
  this->addChild(m_bg);

  m_bgName = bg;

  if (icon) {
      m_icon = icon;
      icon->setZOrder(1);
      this->addChild(icon);
  }

  this->updateLayout();

  return true;
}

void IconButtonSpriteNoText::updateLayout() {
  auto size = CCSize { 25.f, 25.f };
  if (m_icon) limitNodeSize(m_icon, size, 1.f, .1f);
  size += CCSize{10.f, 10.f};

  this->setContentSize(size);
  m_bg->setContentSize(size / m_bg->getScale());
  m_bg->setPosition(m_obContentSize / 2);

  if (m_icon) m_icon->setPosition(size.height / 2, size.height / 2);
}

IconButtonSpriteNoText* IconButtonSpriteNoText::create(char const* bg, cocos2d::CCNode* icon) {
  auto ret = new IconButtonSpriteNoText();
  if (ret && ret->init(bg, false, icon)) {
      ret->autorelease();
      return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

IconButtonSpriteNoText* IconButtonSpriteNoText::createWithSpriteFrameName(char const* bg, cocos2d::CCNode* icon) {
  auto ret = new IconButtonSpriteNoText();
  if (ret && ret->init(bg, true, icon)) {
      ret->autorelease();
      return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

void IconButtonSpriteNoText::setBG(char const* bg, bool isFrame) {
  if (m_bg) m_bg->removeFromParent();
  if (isFrame) m_bg = CCScale9Sprite::createWithSpriteFrameName(bg);
  else m_bg = CCScale9Sprite::create(bg);
  this->addChild(m_bg);
  this->updateLayout();
  m_bgName = bg;
}

CCScale9Sprite* IconButtonSpriteNoText::getBG() const {
  return m_bg;
}

std::string IconButtonSpriteNoText::getBGName() const {
  return m_bgName;
}

void IconButtonSpriteNoText::setIcon(cocos2d::CCNode* icon) {
  if (m_icon)  m_icon->removeFromParent();
  m_icon = icon;
  m_icon->setZOrder(1);
  this->addChild(icon);
  this->updateLayout();
}

cocos2d::CCNode* IconButtonSpriteNoText::getIcon() const {
  return m_icon;
}

