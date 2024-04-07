#include <Geode/Geode.hpp>

#include <utils/LinkedCCMenu.hpp>

using namespace geode::prelude;

bool LinkedCCMenu::ccTouchBegan(CCTouch *touch, CCEvent *event) {
  for (CCMenu *link : m_links) if (link->ccTouchBegan(touch, event)) { m_touching = link; return true; }
  m_touching = nullptr;
  return false;
}

void LinkedCCMenu::ccTouchEnded(CCTouch *touch, CCEvent *event) {
  if (m_touching) m_touching->ccTouchEnded(touch, event);
  m_touching = nullptr;
}

void LinkedCCMenu::ccTouchMoved(CCTouch *touch, CCEvent *event) {
  for (CCMenu *link : m_links) {
    if (link == m_touching) {
      m_touching->ccTouchMoved(touch, event);
      continue;
    } else if (link == m_touching) continue;
    if (link->ccTouchBegan(touch, event)) {
      m_touching->ccTouchCancelled(touch, event);
      m_touching = link;
      return;
    }
  }
}

void LinkedCCMenu::ccTouchCancelled(CCTouch *touch, CCEvent *event) {
  if (m_touching) m_touching->ccTouchCancelled(touch, event);
  m_touching = nullptr;
}

void LinkedCCMenu::sortAndUpdatePriority() {
  m_links.sort(
    [](CCMenu *a, CCMenu *b) {
      int atp = a->getTouchPriority();
      int btp = b->getTouchPriority();
      unsigned int aooa = a->getOrderOfArrival();
      unsigned int booa = b->getOrderOfArrival();
      if (atp != btp) return atp < btp;
      return aooa < booa;
    }
  );
  if (!m_links.empty()) setTouchPriority((*m_links.begin())->getTouchPriority() - 1);
}

bool LinkedCCMenu::link(CCMenu *menu) {
  if (!menu) return false;
  for (std::list<CCMenu *>::iterator p = m_links.begin(); p != m_links.end(); p++) {
    if (*p == menu) return false;
    if ((*p)->getTouchPriority() > menu->getTouchPriority()) {
      m_links.insert(p, menu);
      if (p == m_links.begin()) setTouchPriority(menu->getTouchPriority() - 1);
      return true;
    } else if ((*p)->getTouchPriority() == menu->getTouchPriority()) {
      if (menu->getOrderOfArrival() > (*p)->getOrderOfArrival()) m_links.insert(std::next(p), menu);
      else m_links.insert(p, menu);
      return true;
    }
  }
  m_links.push_back(menu);
  return true;
}

bool LinkedCCMenu::unlink(CCMenu *menu) {
  if (!menu) return false;
  for (std::list<CCMenu *>::iterator p = m_links.begin(); p != m_links.end(); p++) {
    if (*p == menu && p == m_links.begin()) {
      m_links.erase(p);
      if (!m_links.empty()) setTouchPriority((*m_links.begin())->getTouchPriority() - 1);
      return true;
    } else if (*p == menu) {
      m_links.erase(p);
      return true;
    }
  }
  return false;
}

bool LinkedCCMenu::linkChildren(CCNode *node, int depth) {
  if (!depth) return false;
  if (!node) return false;
  bool anyLinked = false;
  CCObject *childObject;
  CCARRAY_FOREACH(node->getChildren(), childObject) {
    if (CCMenu *childMenu = typeinfo_cast<CCMenu *>(childObject))
      anyLinked |= link(childMenu);
    anyLinked |= linkChildren(static_cast<CCNode *>(childObject), depth-1);
  }
  return anyLinked;
}

bool LinkedCCMenu::unlinkChildren(CCNode *node, int depth) {
  if (!node) return false;
  bool anyUnlinked = false;
  CCObject *childObject;
  CCARRAY_FOREACH(node->getChildren(), childObject) {
    if (CCMenu *childMenu = typeinfo_cast<CCMenu *>(childObject))
      anyUnlinked |= unlink(childMenu);
    anyUnlinked |= unlinkChildren(static_cast<CCNode *>(childObject), depth-1);
  }
  return anyUnlinked;
}

LinkedCCMenu* LinkedCCMenu::create() {
  auto a = new LinkedCCMenu();
  if (a && a->init()) { a->autorelease(); return a; }
  CC_SAFE_DELETE(a);
  return nullptr;
}

LinkedCCMenu* LinkedCCMenu::createLinked(CCNode *node, int depth) {
  auto a = LinkedCCMenu::create();
  if (!a) return nullptr;
  a->linkChildren(node, depth);
  return a;
}
