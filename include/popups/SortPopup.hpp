#pragma once

#include <Geode/Geode.hpp>

class FilterAndSortPopup;

using namespace geode::prelude;

class SortPopup : public Popup<FilterAndSortPopup *> {
protected:

  CCNode *sortingList;
  CCMenu *sortingButtonMenu;
  FilterAndSortPopup *parentPopup;

  static const GLubyte EVEN_OPACITY = 32;
  static const GLubyte ODD_OPACITY = 16;
  static const GLubyte RED_EVEN_OPACITY = 64;
  static const GLubyte RED_ODD_OPACITY = 32;

  bool setup(FilterAndSortPopup *) override;

  void onArrow(CCObject *);
  void onReverseSortOrder(CCObject *);
  void onToggle(CCObject *);
  
  void onClose(CCObject *) override;
  
  void recalculateListItemBackgrounds();

public:
  static SortPopup* create(FilterAndSortPopup *);

};
