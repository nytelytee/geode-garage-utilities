#pragma once

#include <Geode/Geode.hpp>
#include <popups/FilterPopup.hpp>

using namespace geode::prelude;

class AuthorFilterPopup : public FilterPopup {
protected:
  
  void toggleAuthor(CCObject *);
  void goToAuthorProfile(CCObject *);
  void addAuthor(int, int);

  void onTopMenuButton(CCObject *) override;
  void customSetup() override;

public:
  static AuthorFilterPopup* create(FilterAndSortPopup *parent);
};
