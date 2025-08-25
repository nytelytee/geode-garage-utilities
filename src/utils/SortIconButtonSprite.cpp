#include <Geode/utils/cocos.hpp>
#include <cocos-ext.h>
#include <utils/SortIconButtonSprite.hpp>

using namespace geode::prelude;

bool SortIconButtonSprite::init(char const* bg, bool bgIsFrame, cocos2d::CCNode* icon, cocos2d::CCSize buttonSize, cocos2d::CCSize buttonPadding) {
  if (!CCSprite::init()) return false;

  if (bgIsFrame) m_bg = CCScale9Sprite::createWithSpriteFrameName(bg);
  else m_bg = CCScale9Sprite::create(bg);
  this->addChild(m_bg);

  m_bgName = bg;
  m_buttonSize = buttonSize;
  m_buttonPadding = buttonPadding;

  if (icon) {
      m_icon = icon;
      icon->setZOrder(1);
      this->addChild(icon);
  }

  this->updateLayout();

  return true;
}

void SortIconButtonSprite::updateLayout() {
  if (m_icon) limitNodeSize(m_icon, m_buttonSize, 1.f, .1f);

  this->setContentSize(m_buttonSize + m_buttonPadding);
  m_bg->setContentSize((m_buttonSize + m_buttonPadding) / m_bg->getScale());
  m_bg->setPosition(m_obContentSize / 2);

  if (m_icon) m_icon->setPosition((m_buttonSize + m_buttonPadding) / 2);
}

SortIconButtonSprite* SortIconButtonSprite::create(char const* bg, cocos2d::CCNode* icon, cocos2d::CCSize buttonSize, cocos2d::CCSize buttonPadding) {
  auto ret = new SortIconButtonSprite();
  if (ret && ret->init(bg, false, icon, buttonSize, buttonPadding)) {
      ret->autorelease();
      return ret;
  }
  delete ret;
  return nullptr;
}

SortIconButtonSprite* SortIconButtonSprite::createWithSpriteFrameName(char const* bg, cocos2d::CCNode* icon, cocos2d::CCSize buttonSize, cocos2d::CCSize buttonPadding) {
  auto ret = new SortIconButtonSprite();
  if (ret && ret->init(bg, true, icon, buttonSize, buttonPadding)) {
      ret->autorelease();
      return ret;
  }
  delete ret;
  return nullptr;
}

void SortIconButtonSprite::setBG(char const* bg, bool isFrame) {
  if (m_bg) m_bg->removeFromParent();
  if (isFrame) m_bg = CCScale9Sprite::createWithSpriteFrameName(bg);
  else m_bg = CCScale9Sprite::create(bg);
  this->addChild(m_bg);
  this->updateLayout();
  m_bgName = bg;
}

CCScale9Sprite* SortIconButtonSprite::getBG() const {
  return m_bg;
}

std::string SortIconButtonSprite::getBGName() const {
  return m_bgName;
}

void SortIconButtonSprite::setIcon(cocos2d::CCNode* icon) {
  if (m_icon)  m_icon->removeFromParent();
  m_icon = icon;
  m_icon->setZOrder(1);
  this->addChild(icon);
  this->updateLayout();
}

cocos2d::CCNode* SortIconButtonSprite::getIcon() const {
  return m_icon;
}

void SortIconButtonSprite::setSize(cocos2d::CCSize size) {
  m_buttonSize = size;
  this->updateLayout();
}

void SortIconButtonSprite::setPadding(cocos2d::CCSize padding) {
  m_buttonPadding = padding;
  this->updateLayout();
}

cocos2d::CCSize SortIconButtonSprite::getSize() { return m_buttonSize; }

cocos2d::CCSize SortIconButtonSprite::getPadding() { return m_buttonPadding; }
