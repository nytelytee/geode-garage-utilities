#pragma once

#include <cocos2d.h>

namespace cocos2d::extension {
    class CCScale9Sprite;
}

class IconButtonSpriteNoText : public cocos2d::CCSprite {
protected:
    cocos2d::extension::CCScale9Sprite* m_bg = nullptr;
    cocos2d::CCNode* m_icon = nullptr;
    std::string m_bgName;

    bool init(char const* bg, bool bgIsFrame, cocos2d::CCNode* icon);

    void updateLayout();

    IconButtonSpriteNoText() = default;
    IconButtonSpriteNoText(IconButtonSpriteNoText&&) = delete;
    IconButtonSpriteNoText& operator=(IconButtonSpriteNoText&&) = delete;

public:
    static IconButtonSpriteNoText* create(char const* bg, cocos2d::CCNode* icon);
    static IconButtonSpriteNoText* createWithSpriteFrameName(char const* bg, cocos2d::CCNode* icon);

    void setBG(char const* bg, bool isFrame);
    cocos2d::extension::CCScale9Sprite* getBG() const;

    std::string getBGName() const;

    void setIcon(cocos2d::CCNode* icon);
    cocos2d::CCNode* getIcon() const;

};
