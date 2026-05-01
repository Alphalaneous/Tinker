# 1.0.3
- <c-dddddd>Fix misnamed setting for Object Search</c>
- <c-dddddd>More performance fixes</c>
- <c-dddddd>Toggling Length In Editor now updates the time properly</c>
- <c-dddddd>Live Colors now properly updates with default color IDs when objects are placed</c>

# 1.0.2
- <c-dddddd>Fix a crash</c>
- <c-dddddd>Speed up Object Search loading</c>

# 1.0.1
- <c-dddddd>Rewrite zoom logic to hopefully resolve some bugs</c>
- <c-dddddd>Lazy load search tab</c>

# 1.0.0
- <c-dddddd>New visual identity</c>
- <c-dddddd>Updated mod information to include everything that is missing</c>
- <c-dddddd>Move out of Beta</c>
- <c-dddddd>Fix edit tab sorting with Scrollable Toolbar enabled</c>

# 1.0.0-beta.42
- <c-dddddd>Improve settings performance</c>
- <c-dddddd>Implement my own actually *good* search for settings</c>
- <c-dddddd>Fix scrolling working during playtest</c>
- <c-dddddd>Fix edit special being inaccessible for User Coins and Gold Coins</c>
- <c-dddddd>Add more keybinds</c>

# 1.0.0-beta.41
- <c-dddddd>Fix template blocks not having edit special when selected with something else</c>
- <c-dddddd>Fix settings not expanding properly when switching tabs (I think)</c>

# 1.0.0-beta.40
- <c-dddddd>Fix settings scroll layout not being updated properly</c>
- <c-dddddd>Fix a crash</c>

# 1.0.0-beta.39
- <c-dddddd>Rework Geode Settings</c>

# 1.0.0-beta.38
- <c-dddddd>Fix search height being too low for devices where the virtual keyboard height is misrepresented</c>
- <c-dddddd>Cleanup startpos switcher code some more</c>
- <c-dddddd>Default start position selection should now respect if a start pos is disabled, and will only disrespect it when it is used/switched to</c>

# 1.0.0-beta.37
- <c-dddddd>Improve Quick extras inclusions and exclusions</c>
- <c-dddddd>Fix rotating camera and dragging at same time</c>
- <c-dddddd>Add node ID to playtest without startpos button</c>

# 1.0.0-beta.36
- <c-dddddd>Add editor grid customization</c>
  - <c-dddddd>Color customization</c>
  - <c-dddddd>Grid Invert Behind option</c>
  - <c-dddddd>Effect lines match trigger color option</c>
  - <c-dddddd>Rainbow Music Line option :3</c>
- <c-dddddd>Fix crash when placing start position with Start Pos Tools disabled</c>

# 1.0.0-beta.35
- <c-dddddd>Fix a crash</c>

# 1.0.0-beta.34
- <c-dddddd>Fix duration drag being broken if the duration is negative</c>
- <c-dddddd>Fix start pos switching when there are no start positions</c>
- <c-dddddd>Adding positions via Clean Start Pos updates the label now</c>
- <c-dddddd>Added `restart` and `restart without start positions` keybinds for the editor</c>

# 1.0.0-beta.33
- <c-dddddd>Fix a crash</c>

# 1.0.0-beta.32
- <c-dddddd>Fix startpos switcher just not working consistently</c>
- <c-dddddd>Add playtest confirmation option</c>

# 1.0.0-beta.31
- <c-dddddd>Make playtest act like vanilla again, and turn the playtest from start pos button into a playtest without start pos button (Swapping the behavior)</c>
- <c-dddddd>Make scroll delegation an option for compatibility sake, off by default</c>

# 1.0.0-beta.30
- <c-dddddd>Fix typo in changelog</c>
- <c-dddddd>Only use scroll event to get points, trigger on scrollWheel call still</c>
- <c-dddddd>Add editor start pos switcher</c>

# 1.0.0-beta.29
- <c-dddddd>Add setting to make pressing enter start at a start position instead of the start of the level</c>
- <c-dddddd>Add customizing toolbar gradient and line colors</c>

# 1.0.0-beta.28
- <c-dddddd>Fix a crash when pressing enter to playtest</c>
- <c-dddddd>Fix playtest menu when BetterEdit is disabled</c>

# 1.0.0-beta.27
- <c-dddddd>Fix cycling start position selections not working consistently</c>
- <c-dddddd>Fix the start pos playtest button showing under the ground</c>
- <c-dddddd>Add always show negate button option</c>
- <c-dddddd>Better compatibility with Lasso Select (future update to Lasso Select needed)</c>
- <c-dddddd>Add ability to playtest from start, separate starting from start pos to a separate button</c>
- <c-dddddd>Fix D blocks not working with the ignore damage fix</c>
- <c-dddddd>Fix deleting all start positions still allowing you to start from a ghost start position</c>
- <c-dddddd>Fix color preview showing on bottom right of CustomizeObjectLayer when it shouldn't</c>
- <c-dddddd>Add node ID to tinker button in pause menu</c>

# 1.0.0-beta.26
- <c-dddddd>Fix preview object colors affecting object search permanently</c>
- <c-dddddd>Fix object search loading circle position</c>
- <c-dddddd>Add start playtest from startpos button</c>
- <c-dddddd>Allow wave to clip through slopes when ignore damage is enabled</c>
- <c-dddddd>Only show negate button when text field is focused</c>
- <c-dddddd>Make some changes to Editor Rotation to hopefully prevent objects being rotated when they shouldn't be</c>
- <c-dddddd>Placing custom objects should work properly now when the editor is rotated</c>
- <c-dddddd>Add node ID to live colors menu</c>

# 1.0.0-beta.25
- <c-dddddd>Disable scroll on tabs that don't have many buttons</c>
- <c-dddddd>Increase tab scroll delta on mobile</c>
- <c-dddddd>Check if slider in editor actually exists</c>

# 1.0.0-beta.24
- <c-dddddd>Free move now works when the camera is rotated</c>
- <c-dddddd>Added button and keybind to move selected objects to your editor position</c>
- <c-dddddd>Added button and keybind to move your editor position to selected objects</c>
- <c-dddddd>Make letting go of shift before mouse when swiping stopping swipe an option</c>
- <c-dddddd>Hackily make quick volume controls work with my scroll rewrite</c>
- <c-dddddd>Block scroll when paused</c>

# 1.0.0-beta.23
- <c-dddddd>Make object search items load while in the editor rather than on level load. Load rate can be customized in settings</c>

# 1.0.0-beta.22
- <c-dddddd>Adjust scrolling to account for natural scrolling on MacOS</c>
- <c-dddddd>Fix rotating objects not being snapped to grid when camera is rotated</c>
- <c-dddddd>Add alignment keybind for Editor Rotation</c>
- <c-dddddd>Fix offset objects not being rotated correctly when the editor is rotated</c>
- <c-dddddd>Add zoom bound settings</c>

# 1.0.0-beta.21
- <c-dddddd>Fix a BetterEdit bug with text inputs staying focused and blocking keybinds to prevent Tinker from being blamed for it</c>

# 1.0.0-beta.20
- <c-dddddd>Fix crash with Named Editor Groups</c>
- <c-dddddd>Fix crash with Joystick when Editor Rotation is disabled</c>
- <c-dddddd>Fix `control` keybind conflict with pasting objects with Single Deselect</c>
- <c-dddddd>Fix camera dragging with Editor Rotation when playtest is active but paused</c>
- <c-dddddd>Add Editor Rotation right click drag toggle, in case right click just isn't your thing but still want the fixed movement or to use the joystick.</c>

# 1.0.0-beta.19
- <c-dddddd>Fix missing bounds for scrolling</c>
- <c-dddddd>Fix potential divide by 0 issue with zoom</c>
- <c-dddddd>Add invert vertical and horizontal scroll settings</c>
- <c-dddddd>Add invert scroll setting for scrollable toolbars</c>
- <c-dddddd>Allow disabling zoom to cursor</c>
- <c-dddddd>Fix mac scrolling (I think)</c>
- <c-dddddd>Fix letting go of shift keeping swipe active</c>
- <c-dddddd>Fix selection bounds on rotated objects</c>
- <c-dddddd>Added repositioning check if the editor camera seems to be invalid</c>

# 1.0.0-beta.18
- <c-dddddd>Fix transform control buttons in Improved Transform Controls not being clickable</c>
- <c-dddddd>Fix a crash when Improved Transform Controls was clicked outside its bounds</c>
- <c-dddddd>Fix a crash when scrolling on the toolbar with scrollable toolbar disabled</c>
- <c-dddddd>Transform control buttons should now work with Canvas Rotation</c>

# 1.0.0-beta.17
- <c-dddddd>Fix some settings failing to apply</c>
- <c-dddddd>Fix feature names being wrong when applying</c>
- <c-dddddd>Fix another crash with Improved Transform Controls</c>

# 1.0.0-beta.16
- <c-dddddd>Fix modifier icons letter never being hidden when the setting is off (again)</c>

# 1.0.0-beta.15
- <c-dddddd>Tweak scroll delta in Improved Group View</c>
- <c-dddddd>Fix missing vanilla group offset button</c>
- <c-dddddd>Add scrollbar height and y offset settings for Scrollable Toolbar</c>
- <c-dddddd>Add settings button in the editor pause menu</c>
- <c-dddddd>Fix the editor not having scroll when the color select and hsv popup is visible</c>
- <c-dddddd>Fix locked layers not being respected</c>
- <c-dddddd>Reimplement editor scroll altogether to make it easier to deal with (trackpad scroll probably works on windows now, haven't tested)</c>
- <c-dddddd>Add option for smooth scroll and zoom in the editor</c>
- <c-dddddd>Add option for scroll and zoom multipliers</c>
- <c-dddddd>Make the selection box stick to the object layer</c>
- <c-dddddd>Fix Improved Transform Controls crash and button scale</c>
- <c-dddddd>Fix Modifier Icon letter never being hidden when the setting is off</c>
- <c-dddddd>Add keybind for Edit Extras</c>
- <c-dddddd>Add scroll modifier keybind and multiplier for the scrollable tabs</c>
- <c-dddddd>Allow scroll while mouse is hovered in the edit tab when BetterEdit's custom move menu is enabled, and delete tab</c>

# 1.0.0-beta.14
- <c-dddddd>Infinite zoom now works with BetterEdit again</c>
- <c-dddddd>Fix horizontal scroll breaking</c>
- <c-dddddd>Fix scrolling on Mac</c>
- <c-dddddd>Disable repeat on reference image button</c>

# 1.0.0-beta.13
- <c-dddddd>Hotfix for changing any setting causing a crash when opening the editor</c>

# 1.0.0-beta.12
- <c-dddddd>Hotfix for BetterEdit Edit Menu breaking</c>

# 1.0.0-beta.11
- <c-dddddd>Fix some broken sprites when not using high quality textures</c>
- <c-dddddd>Tooltips are now higher on mobile</c>
- <c-dddddd>Tooltips will now fade when button is scrolled outside of view</c>
- <c-dddddd>Fix tooltips persisting when switching modes while clicked</c>
- <c-dddddd>Fix tooltips instantly fading when not using scrollable objects on mobile</c>
- <c-dddddd>Fix tooltips causing button opacity to stay lowered on mobile</c>
- <c-dddddd>Fix tooltips height being inconsistent and changing if the button scales</c>
- <c-dddddd>Add a selection limit of 100 to duration dragging</c>
- <c-dddddd>Improve duration dragging performance</c>
- <c-dddddd>Fix duration dragging breaking when the editor is rotated</c>
- <c-dddddd>Fix duration drag slider not being visible after pasting objects</c>
- <c-dddddd>Fix repeating editor buttons not working with BetterEdit's move menu</c>
- <c-dddddd>Fix selection being broken when Canvas Rotate is enabled</c>
- <c-dddddd>Add missing node IDs for object search</c>
- <c-dddddd>Fix hold to swipe location being wrong when the editor is rotated</c>

## Known Issues
- <c-bfbf56>Scroll on Mac is inverted</c>
- <c-bfbf56>Rotate and Zoom gestures do not work on mobile</c>
- <c-bfbf56>Infinite Zoom hacks/mods do not work with Tinker currently</c>
- <c-bfbf56>Edit tab organization is terrible with scrollable tabs enabled</c>

# 1.0.0-beta.10
- <c-dddddd>Fix Duration Dragging lines showing when disabled</c>
- <c-dddddd>Fix Live Colors menu showing under search if you pause and unpause</c>
- <c-dddddd>Always force BetterEdit move tab reload to fix invisible buttons</c>
- <c-dddddd>Clean up some queued methods to hopefully fix a crash</c>
- <c-dddddd>Fix a typo in the settings</c>
- <c-dddddd>Fix link controls position and sizing on mobile</c>
- <c-dddddd>Add Object Tooltip Scale setting</c>
- <c-dddddd>Make Tooltips visible on mobile</c>
- <c-dddddd>Fix clicking on objects not bringing you to the right tab in the build menu</c>

# 1.0.0-beta.9
- <c-dddddd>Fix Auto "Build Helper" not working</c>
- <c-dddddd>Save Auto "Build Helper" toggle state</c>
- <c-dddddd>Fix Paste Warnings showing when clicking the buttons while they are disabled</c>
- <c-dddddd>Adjust scroll delta to prevent accidental scrolling when clicking an object in the build tab</c>
- <c-dddddd>Remove broken Split button with Multi Text Edit as it crashes. May reimplement in the future</c>

# 1.0.0-beta.8
- <c-dddddd>Depend on Better Touch Prio

# 1.0.0-beta.7
- <c-dddddd>Fix BetterEdit's edit menu buttons being invisible</c>
- <c-dddddd>Clean up Object Search UI on mobile</c>

# 1.0.0-beta.6
- <c-dddddd>Fix Platformer Control Touch in Editor when Canvas Rotation is enabled</c>
- <c-dddddd>Fix Edit Object not opening preview object colors selection when Scrollable Objects is disabled</c>
- <c-dddddd>Fix Group View touch not working</c>
- <c-dddddd>Fix Culling if row count was not 3</c>
- <c-dddddd>Fix Search Bar touch not working</c>
- <c-dddddd>Fix Edit Extras not showing when multiple objects were selected that cannot be edited with Edit Special</c>
- <c-dddddd>Fix Editing multiple objects</c>
- <c-dddddd>Fix Object Summary Description</c>
- <c-dddddd>Add setting to make Edit Extras always show</c>
- <c-dddddd>Add button to reset Joystick position in settings</c>
- <c-dddddd>Add setting to lock Joystick position</c>
- <c-dddddd>Disable automatic text focus in the Object Search tab on mobile</c>

## Known Issues
- <c-bfbf56>Something can cause missing buttons in the Edit tab when BetterEdit is enabled and some other unknown mod (looking into).</c>
- <c-bfbf56>Object Search is covered up by the keyboard on mobile.</c>
- <c-bfbf56>Horizontal Scroll on Mac is inverted</c>

# 1.0.0-beta.5
- <c-dddddd>Fix Negate Input sprite</c>

# 1.0.0-beta.4
- <c-dddddd>Fix a crash</c>
- <c-dddddd>Change Single Deselect to use Keybind</c>
- <c-dddddd>Fix Reference Image culling</c>

# 1.0.0-beta.3
- <c-dddddd>Fix search input memory leak and crash</c>

# 1.0.0-beta.2
- <c-dddddd>Fix a few Reference Image bugs</c>

# 1.0.0-beta.1
- <c-dddddd>Initial Release.</c>
