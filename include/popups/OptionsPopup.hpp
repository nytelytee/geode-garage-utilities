#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;
class FilterAndSortPopup;


class OptionsPopup : public Popup<bool, FilterAndSortPopup *> {
protected:

  CCMenu *m_buttonMenu;
  FilterAndSortPopup *parentPopup = nullptr;
  
  void toggleOption(CCObject *sender);

  void addOption(const char*, int, bool);
  void addOptionInfo(int, const char*);
  bool setup(bool, FilterAndSortPopup *) override;

  void onClose(CCObject *) override;

public:
  static OptionsPopup* createWithDisplayOptions(FilterAndSortPopup *);
  static OptionsPopup* createWithPlayerOptions();
};

