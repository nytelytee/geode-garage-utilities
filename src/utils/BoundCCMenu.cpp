#include <Geode/Geode.hpp>
#include <utils/BoundCCMenu.hpp>

using namespace geode::prelude;

void BoundCCMenu::ccTouchEnded(CCTouch *touch, CCEvent*) {
  this->retain();
  if (m_pSelectedItem) {
    m_pSelectedItem->unselected();
    if (m_bound.containsPoint(touch->getLocation())) m_pSelectedItem->activate();
  }
  m_eState = tCCMenuState::kCCMenuStateWaiting;
  this->release();
}

bool BoundCCMenu::ccTouchBegan(CCTouch *touch, CCEvent*) {
  if (m_eState != tCCMenuState::kCCMenuStateWaiting || !isVisible() || !isEnabled()) return false;
  for (CCNode *c = this->getParent(); c != nullptr; c = c->getParent()) if (!c->isVisible()) return false;
  m_pSelectedItem = itemForTouch(touch);
  if (!m_pSelectedItem || !m_bound.containsPoint(touch->getLocation())) return false;
  m_eState = tCCMenuState::kCCMenuStateTrackingTouch;
  m_pSelectedItem->selected();
  return true;
}

void BoundCCMenu::ccTouchMoved(CCTouch *touch, CCEvent*) {
  CCMenuItem *currentItem = itemForTouch(touch);
  CCPoint location = touch->getLocation();
  CCPoint previousLocation = touch->getPreviousLocation();
  if (!m_bound.containsPoint(location) && !m_bound.containsPoint(previousLocation)) return;
  else if (!m_bound.containsPoint(location) && m_bound.containsPoint(previousLocation)) {
    if (currentItem != m_pSelectedItem || !m_pSelectedItem) return;
    m_pSelectedItem->unselected();
    m_pSelectedItem = nullptr;
  } else if (m_bound.containsPoint(location)) {
    if (currentItem == m_pSelectedItem) return;
    if (m_pSelectedItem) m_pSelectedItem->unselected();
    m_pSelectedItem = currentItem;
    if (m_pSelectedItem) m_pSelectedItem->selected();
  }
}
