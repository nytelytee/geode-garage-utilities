#pragma once

#include <Geode/Geode.hpp>

class FilterPopup;
class SortPopup;
class OptionsPopup;

using namespace geode::prelude;

class FilterAndSortPopup : public Popup<> {
protected:

  friend FilterPopup;
  friend SortPopup;
  friend OptionsPopup;

  CCMenu *m_buttonMenu;
  CCMenu *m_topMenu;
  CCMenu *m_actionMenu;

  bool setup() override;

  void onToggle(CCObject *);
  void onMenuButton(CCObject *);
  void onActionButton(CCObject *);
  void refreshMenuState();

public:
  static FilterAndSortPopup* create();

};
