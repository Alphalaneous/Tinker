# Tinker

**A plethora of editor changes, additions, and improvements to make editing levels feel just a little bit better.** 

This mod provides a fully customizable and modular feature set for you to use the editor how you want to!

**Don't like a feature?** 

Well you can toggle anything you want on and off. All disabled features are **fully** disabled, meaning they won't be hiding in the background affecting performance without you realizing.

**<cy>This is a beta!**

Expect bugs, incompatibilities with other mods, etc. Please report any problems on the mod's [GitHub Issues](https://github.com/Alphalaneous/Tinker/issues) page. 

Some features here can affect the load time of the Editor. Do note that improvements are planned and actively being looked into.

## Interface
Customize different aspects of the editor's interface.

---

### ​  Editor Rotation
- Allows for you to rotate the editor by right clicking and dragging.

### ​  Improved Link Controls
- Fixes the link control buttons being clickable when toggled off, also fixes the size of them.

### ​  Show Length In Editor
- Shows the levels' length in time in the top left corner.

### ​  Live Colors
- Shows the levels' currently active colors on the bottom of the screen.

### ​  Preview Object Colors
- Allows for you to preview a color channel on the object buttons in the build tab.

### ​  Object Tooltips
- Tooltips with object names are shown while hovering over an object in the build tab.

### ​  Quick Extras
- Adds a shortcut button to edit extra properties of objects, replacing Edit Special when it isn't needed.

### ​  Reference Image
- Adds a button to import reference images into a level.

### ​  Scrollable Objects
- Allows for you to scroll through objects instead of navigating per page in the build tab.

### ​  Improved Group View
- Shows more than 20 groups at once in the Group View when selecting multiple objects with a scrollable list of the groups.

### ​  Joystick Navigation
- Navigate the editor with a Joystick. Most useful for mobile one handed use.

### ​  Negate Input
- Allows for you to press N and/or a button to negate any number input when focused.

### ​  Relocate Build Tools
- Relocates the build tools from the editor pause menu to the edit menu.

### ​  Repeating Editor Buttons
- Allows for you to hold certain buttons in the editor to repeatedly press them.

### ​  Paste Warnings
- Shows warnings for paste state and paste color.

---

## Objects
All things related to objects.

---

### ​  Duration Drag
- Adds the ability to drag on a duration line to change the duration without going into a trigger's edit menu.

### ​  Multi Text Edit
- Allows for you to edit multiple text objects at once.

### ​  Object Search
- Adds a new search tab for objects. In this tab, you will see every object and be able to search by name.
- You can fine tune your search and search by object ID by starting the search with `id:`, or by an exact name with `exact:`.

### ​  Object Summary
- Adds a button in the level settings to view how many of each object there are.

### ​  Old Color Triggers
- Brings back the old color trigger textures so you can view them when looking at old levels.

### ​  Improved Modifier Icons
- Improves the Modifier blocks (D/H/J/S/F) by adding icons for each of their actions instead of the letter.

### ​  Automatic "Build Helper"
- Adds a toggle to automate the Build Helper tool. Allows for you to copy and paste objects and/or place custom objects to automatically run Build Helper on them when the toggle is on.

### ​  Single Deselect
- Allows for you to hold a modifier to deselect objects within an existing selection.

---

## Fixes
Different fixes that you'll likely want to keep on to improve the editing experience.

---

### ​  Centered Object Buttons
- Makes the object sprites on the buttons in the build tab actually centered.

### ​  Editor Slider Fix
- Fixes the position slider not being based on the level length.

### ​  Transform Control Fixes
- Fixes the touch issues with Transform and Scale controls.

### ​  Text Object Transform Fix
- Fixes the transform bounds for text objects such that they are not always {0, 0}. Updates them properly when the text and/or kerning is changed.

---

## Miscellaneous
Stuff that just doesn't fit in any other category.

---

### ​  Hide IDs in the Edit Level screen
- Hides IDs in Edit Level screen, hold shift to show them.

---

## Other Mods
Changes Tinker can make to better integrate other mods with Tinker itself.

---

### ​  Alternate Lasso Select Location
- <cy>Requires Lasso Select Mod.
- Moves the Lasso Select button to the bottom right.

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

- Send events to update the group view when in SetGroupIDLayer.

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
- Automatic "Build Helper" sprite
- Relocate Build Tools sprites

---

### ​  Mat
- Original Reference Image concept

---

### ​  TerminumGD
- Idea for Hide IDs in the Edit Level screen

---

## Support

---

- If you like this mod and what I make, feel free to donate to my [Ko-fi](https://ko-fi.com/alphalaneous).
