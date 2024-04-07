#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// partial comparison functions for sorting
std::optional<bool> compareByNumber(unsigned, unsigned, UnlockType);
std::optional<bool> compareByLockStatus(unsigned, unsigned, UnlockType);
std::optional<bool> compareByCategory(unsigned, unsigned, UnlockType);
std::optional<bool> compareByAuthor(unsigned, unsigned, UnlockType);
// total comparison function, uses partials in order
// until it finds the first one that doesn't return nullopt
bool compareIcons(unsigned, unsigned);

void recalculateIconOrder();

// input: the current icon position
// output: the actual icon that should be displayed in that position
//         0 if the position is invalid and won't display any icons (out of range)
unsigned positionToDisplay(UnlockType, unsigned);

// input: the icon that is displayed
// output: the position that displays this icon
//         0 if the icon is not displayed at any position (filtered out / invalid)
unsigned displayToPosition(UnlockType, unsigned);

int countForType(IconType iconType);
int fakeCountForType(IconType, unsigned);

