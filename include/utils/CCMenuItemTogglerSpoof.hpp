#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

CCMenuItemToggler* createTogglerWithStandardSpritesSpoofOn(CCObject* target, SEL_MenuHandler callback);
CCMenuItemToggler* createTogglerWithStandardSpritesSpoofOff(CCObject* target, SEL_MenuHandler callback);
