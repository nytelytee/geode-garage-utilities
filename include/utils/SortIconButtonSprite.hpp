#pragma once

#include <cocos2d.h>

namespace cocos2d::extension {
  class CCScale9Sprite;
}

class SortIconButtonSprite : public cocos2d::CCSprite {
private:
  using CCSprite::init;
protected:
  cocos2d::extension::CCScale9Sprite* m_bg = nullptr;
  cocos2d::CCNode* m_icon = nullptr;
  std::string m_bgName;

  cocos2d::CCSize m_buttonSize;
  cocos2d::CCSize m_buttonPadding;

  bool init(char const*, bool, cocos2d::CCNode*, cocos2d::CCSize, cocos2d::CCSize);

  void updateLayout();

  SortIconButtonSprite() = default;
  SortIconButtonSprite(SortIconButtonSprite&&) = delete;
  SortIconButtonSprite& operator=(SortIconButtonSprite&&) = delete;
public:
  static SortIconButtonSprite* create(char const*, cocos2d::CCNode*, cocos2d::CCSize = {15.f, 15.f}, cocos2d::CCSize = {15.f, 15.f});
  static SortIconButtonSprite* createWithSpriteFrameName(char const*, cocos2d::CCNode*, cocos2d::CCSize = {15.f, 15.f}, cocos2d::CCSize = {15.f, 15.f});

  void setBG(char const*, bool);
  cocos2d::extension::CCScale9Sprite* getBG() const;

  std::string getBGName() const;

  void setIcon(cocos2d::CCNode*);
  cocos2d::CCNode* getIcon() const;

  void setSize(cocos2d::CCSize);
  void setPadding(cocos2d::CCSize);

  cocos2d::CCSize getSize();
  cocos2d::CCSize getPadding();

};
