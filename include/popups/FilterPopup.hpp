#pragma once

#include <Geode/Geode.hpp>
#include <utils/BoundCCMenu.hpp>

class FilterAndSortPopup;

using namespace geode::prelude;

class FilterPopup : public Popup<FilterAndSortPopup *> {
protected:

  CCMenu *m_topButtonMenu;
  BoundCCMenu *m_buttonMenu;
  ScrollLayer *m_scrollLayer;
  CCSize m_scrollLayerSize;
  FilterAndSortPopup *parentPopup;

  virtual void onTopMenuButton(CCObject *) {};
  void onClose(CCObject *) override;

  bool setup(FilterAndSortPopup *parent) override { parentPopup = parent; preCustomSetup(); customSetup(); postCustomSetup(); return true; }

  void preCustomSetup();
  virtual void customSetup() {}
  void postCustomSetup();

};
