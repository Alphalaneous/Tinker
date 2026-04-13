# <c-004ba6>T</c><c-10629b>i</c><c-186f95>n</c><c-26848c>k</c><c-369c80>e</c><c-46b476>r</c>

**A plethora of editor changes, additions, and improvements to make editing levels feel just a little bit better.** 

<c-dddddd>This mod provides a fully customizable and modular feature set for you to use the editor how you want to!</c>

**Don't like a feature?** 

<c-dddddd>Well you can toggle anything you want on and off. All disabled features are **fully** disabled, meaning they won't be hiding in the background affecting performance without you realizing.</c>

**<cy>This is a beta!</c>**

<c-dddddd>Expect bugs, incompatibilities with other mods, etc. Please report any problems on the mod's [GitHub Issues](https://github.com/Alphalaneous/Tinker/issues) page.</c> 

<c-dddddd>Some features here can affect the load time of the Editor. Do note that improvements are planned and actively being looked into.</c>

---

## Recommendations

Sometimes, your experience with one mod can be improved by having other mods installed as well. I recommend the following mods alongside Tinker.

---

<mod:hjfod.betteredit>

<c-dddddd>When using Tinker, it is highly recommended to use BetterEdit's UI Scale setting at a value of 0.9 or lower.</c>

---

<mod:undefined0.lasso-select>

---

## Interface
Customize different aspects of the editor's interface.

---

### ​  Canvas Rotation
- <c-dddddd>Allows for you to rotate the editor canvas by right clicking and dragging.</c>

### ​  Improved Link Controls
- <c-dddddd>Fixes the link control buttons being clickable when toggled off, also fixes the size of them.</c>

### ​  Show Length In Editor
- <c-dddddd>Shows the levels' length in time in the top left corner.</c>

### ​  Live Colors
- <c-dddddd>Shows the levels' currently active colors on the bottom of the screen.</c>

### ​  Preview Object Colors
- <c-dddddd>Allows for you to preview a color channel on the object buttons in the build tab.</c>

### ​  Object Tooltips
- <c-dddddd>Tooltips with object names are shown while hovering over an object in the build tab.</c>

### ​  Quick Extras
- <c-dddddd>Adds a shortcut button to edit extra properties of objects, replacing Edit Special when it isn't needed.</c>

### ​  Reference Image
- <c-dddddd>Adds a button to import reference images into a level.</c>

### ​  Scrollable Objects
- <c-dddddd>Allows for you to scroll through objects instead of navigating per page in the build tab.</c>

### ​  Improved Group View
- <c-dddddd>Shows more than 20 groups at once in the Group View when selecting multiple objects with a scrollable list of the groups.</c>

### ​  Joystick Navigation
- <c-dddddd>Navigate the editor with a Joystick. Most useful for mobile one handed use.</c>

### ​  Negate Input
- <c-dddddd>Allows for you to press N and/or a button to negate any number input when focused.</c>

### ​  Relocate Build Tools
- <c-dddddd>Relocates the build tools from the editor pause menu to the edit menu.</c>

### ​  Repeating Editor Buttons
- <c-dddddd>Allows for you to hold certain buttons in the editor to repeatedly press them.</c>

### ​  Paste Warnings
- <c-dddddd>Shows warnings for paste state and paste color.</c>

---

## Objects
All things related to objects.

---

### ​  Duration Drag
- <c-dddddd>Adds the ability to drag on a duration line to change the duration without going into a trigger's edit menu.</c>

### ​  Multi Text Edit
- <c-dddddd>Allows for you to edit multiple text objects at once.</c>

### ​  Object Search
- <c-dddddd>Adds a new search tab for objects. In this tab, you will see every object and be able to search by name.</c>
- <c-dddddd>You can fine tune your search and search by object ID by starting the search with `id:`, or by an exact name with `exact:`.</c>

### ​  Object Summary
- <c-dddddd>Adds a button in the level settings to view how many of each object there are.</c>

### ​  Old Color Triggers
- <c-dddddd>Brings back the old color trigger textures so you can view them when looking at old levels.</c>

### ​  Improved Modifier Icons
- <c-dddddd>Improves the Modifier blocks (D/H/J/S/F) by adding icons for each of their actions instead of the letter.</c>

### ​  Automatic "Build Helper"
- <c-dddddd>Adds a toggle to automate the Build Helper tool. Allows for you to copy and paste objects and/or place custom objects to automatically run Build Helper on them when the toggle is on.</c>

### ​  Single Deselect
- <c-dddddd>Allows for you to hold a modifier to deselect objects within an existing selection.</c>

---

## Fixes
Different fixes that you'll likely want to keep on to improve the editing experience.

---

### ​  Centered Object Buttons
- <c-dddddd>Makes the object sprites on the buttons in the build tab actually centered.</c>

### ​  Editor Slider Fix
- <c-dddddd>Fixes the position slider not being based on the level length.</c>

### ​  Transform Control Fixes
- <c-dddddd>Fixes the touch issues with Transform and Scale controls.</c>

### ​  Text Object Transform Fix
- <c-dddddd>Fixes the transform bounds for text objects such that they are not always {0, 0}. Updates them properly when the text and/or kerning is changed.</c>

---

## Miscellaneous
Stuff that just doesn't fit in any other category.

---

### ​  Hide IDs in the Edit Level screen
- <c-dddddd>Hides IDs in Edit Level screen, hold shift to show them.</c>

---

## Other Mods
Changes Tinker can make to better integrate other mods with Tinker itself.

---

### ​  Alternate Lasso Select Location
- <cy>Requires Lasso Select Mod.</c>
- <c-dddddd>Moves the Lasso Select button to the bottom right.</c>

---

### ​  Scrolling
- <c-dddddd>Smooth scroll and scroll modifiers.</c>

---

### ​  Keybinds
- <c-dddddd>Quick Extras keybind</c>

---

## API
Tools to interface with this mod as a mod developer.

---

### ​  Improved Group View

- <c-dddddd>Send events to update the group view when in SetGroupIDLayer.</c>

```cpp
#include <alphalaneous.tinker/include/ImprovedGroupView.hpp>

// Sends an update event to the group view
tinker::improved_group_view::updateGroupView();
```

---

## Credits
Cool people that helped with the making of this mod.

---

### ​  Brift
- <c-dddddd>Automatic "Build Helper" sprite</c>
- <c-dddddd>Relocate Build Tools sprites</c>

---

### ​  Mat
- <c-dddddd>Original Reference Image concept</c>

---

### ​  TerminumGD
- <c-dddddd>Idea for Hide IDs in the Edit Level screen</c>

---

## Support

---

- <c-dddddd>If you like this mod and what I make, feel free to donate to my [Ko-fi](https://ko-fi.com/alphalaneous). I just really want a fursuit :3</c>
