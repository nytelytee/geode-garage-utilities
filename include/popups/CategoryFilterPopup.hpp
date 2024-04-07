#pragma once

#include <Geode/Geode.hpp>
#include <popups/FilterPopup.hpp>

using namespace geode::prelude;

class CategoryFilterPopup : public FilterPopup {
protected:
  
  void toggleCategory(CCObject *sender);
  void addCategory(int, std::string);

  void onTopMenuButton(CCObject *) override;
  void customSetup() override;

public:
  static CategoryFilterPopup* create(FilterAndSortPopup *);
};
