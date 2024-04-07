#pragma once

#include <Geode/Geode.hpp>

class FilterAndSortPopup;

using namespace geode::prelude;

class SortPopup : public Popup<FilterAndSortPopup *> {
protected:

  CCNode *m_sortingList;
  FilterAndSortPopup *parentPopup;

  static const GLubyte EVEN_BACKGROUND_OPACITY = 32;
  static const GLubyte ODD_BACKGROUND_OPACITY = 16;

  bool setup(FilterAndSortPopup *) override;

  void onArrow(CCObject *);
  void onReverseSortOrder(CCObject *);
  void onToggle(CCObject *);
  
  void onClose(CCObject *) override;
  
  void recalculateListItemBackgrounds();

public:
  static SortPopup* create(FilterAndSortPopup *);

};
