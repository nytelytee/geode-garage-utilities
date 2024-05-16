#include <Geode/Geode.hpp>

#include <utils/CCMenuItemTogglerSpoof.hpp>

using namespace geode::prelude;


CCMenuItemToggler* createTogglerWithStandardSpritesSpoofOn(CCObject* target, SEL_MenuHandler callback) {

  CCSize offSize{31.f, 31.f};
  CCSize onSize{35.f, 34.5f};

  CCSize delta = (onSize - offSize)/2;
  
  CCSprite* offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
  CCSprite* onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
  
  offSprite->setContentSize(onSize);
  onSprite->setContentSize(onSize);
  
  CCMenuItemToggler* ret = CCMenuItemToggler::create(offSprite, onSprite, target, callback);
  offSprite->setPosition(offSprite->getPosition() + delta);
  return ret;

}

CCMenuItemToggler* createTogglerWithStandardSpritesSpoofOff(CCObject* target, SEL_MenuHandler callback) {

  CCSize offSize{31.f, 31.f};
  CCSize onSize{35.f, 34.5f};

  CCSize delta = (offSize - onSize)/2;

  CCSprite* offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
  CCSprite* onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

  offSprite->setContentSize(offSize);
  onSprite->setContentSize(offSize);
  
  CCMenuItemToggler* ret = CCMenuItemToggler::create(offSprite, onSprite, target, callback);
  onSprite->setPosition(onSprite->getPosition() + delta);
  return ret;

}
