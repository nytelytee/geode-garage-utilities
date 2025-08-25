#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// partial comparison functions for sorting
std::optional<bool> compareByNumber(int, int, UnlockType);
std::optional<bool> compareByLockStatus(int, int, UnlockType);
std::optional<bool> compareByCategory(int, int, UnlockType);
std::optional<bool> compareByAuthor(int, int, UnlockType);
// total comparison function, uses partials in order
// until it finds the first one that doesn't return nullopt
bool compareIcons(int, int, UnlockType);

// applies pending settings and re-filters and sorts the icon kit icons, does no actions on the garage;
// if you need to do actions on the garage, use the functions below
void recalculateIconOrder();

// utilities for recalculateIconOrderAndRemainOnSameSectionPages
std::tuple<std::optional<int>, std::optional<int>, std::optional<int>> getPageInfoForType(UnlockType);
void remainOnSameSectionPageForIconType(
  GJGarageLayer*, IconType, std::optional<int>, std::optional<int>, std::optional<int>
);

// remain on the same page number after recalculating icon order, to be used when applying a new configuration
void recalculateIconOrderAndRemainOnSamePages(GJGarageLayer*);

// remain inside the same section's (accepted, denied, more icons) page number after recalculating icon order, to be used after unlocking an icon from within the icon kit
// and nowhere else, as it can be unintuitive if applying arbitrary filters; it works best when one icon is filtered at a time
void recalculateIconOrderAndRemainOnSameSectionPages(GJGarageLayer*);

// go to the active icon page after recalculating icon order, unused as of right now, old Garage Utilities behavior, maybe make it configurable to do this?
void recalculateIconOrderAndGoToActiveIconPages(GJGarageLayer*);

// if the provided icon is on the current page, flash it with a red cursor to give it attention
void giveIconAttention(GJGarageLayer*, UnlockType, int);

// if an icon is unlocked from within the icon kit, this can be used to go to the page the icon gets put on immediately
// if the icon gets filtered out, it acts the same as recalculateIconOrderAndRemainOnSameSectionPages
// returns true if the icon position changed, and false otherwise
bool recalculateIconOrderAndTrackIcon(GJGarageLayer*, UnlockType, int);

// input: the current icon position
// output: the actual icon that should be displayed in that position
//         0 if the position is invalid and won't display any icons (out of range)
int positionToDisplay(UnlockType, int);

// input: the icon that is displayed
// output: the position that displays this icon
//         0 if the icon is not displayed at any position (filtered out / invalid)
int displayToPosition(UnlockType, int);

// adjusted version of GameManager::countForType, which returns the count such that
// page calculations yield the correct maximum page, both for this mod and others
// (like More Icons)
int adjustedCountForType(IconType);

// returns the currently active icon's page, accounting for Separate Dual Icons and More Icons
int getActiveIconPage(IconType);
