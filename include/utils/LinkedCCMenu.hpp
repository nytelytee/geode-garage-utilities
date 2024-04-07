#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// this can only be used for really really simple menus
// i did not bother to actually make it retain any of the
// links either, so if something that's linked gets freed
// and the loops in ccTouchWhatever use it, you will
// be acessing invalid memory.
// this doesn't concern my use case because, in places i
// use this, it gets freed together with the children

class LinkedCCMenu : public CCMenu {
protected:
  std::list<CCMenu *> m_links;
  CCMenu *m_touching = nullptr;

  bool ccTouchBegan(CCTouch *, CCEvent *) override;
  void ccTouchEnded(CCTouch *, CCEvent *) override;
  void ccTouchMoved(CCTouch *, CCEvent *) override;
  void ccTouchCancelled(CCTouch *, CCEvent *) override;

public:
  
  size_t size() { return m_links.size(); }

  void sortAndUpdatePriority();

  bool link(CCMenu *);
  bool unlink(CCMenu *);
  bool linkChildren(CCNode *, int = 1);
  bool unlinkChildren(CCNode *, int = 1);
  
  static LinkedCCMenu * create();
  static LinkedCCMenu * createLinked(CCNode *, int = 1);

};
