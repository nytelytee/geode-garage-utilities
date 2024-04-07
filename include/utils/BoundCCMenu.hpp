#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class BoundCCMenu : public CCMenu {
protected:
  CCRect m_bound;

  bool initWithRect(CCRect worldSpaceBound) { m_bound = worldSpaceBound; return CCMenu::init(); }

  bool ccTouchBegan(CCTouch *, CCEvent *) override;
  void ccTouchEnded(CCTouch *, CCEvent *) override;
  void ccTouchMoved(CCTouch *, CCEvent *) override;

public:
  CCRect getBound() const { return m_bound; }
  void setBound(CCRect newBound) { m_bound = newBound; }

  static BoundCCMenu *create(CCRect worldSpaceBound) {
  	auto a = new BoundCCMenu();
    if (a && a->initWithRect(worldSpaceBound)){ a->autorelease(); return a; }
    CC_SAFE_DELETE(a);
    return nullptr;
  }
};
