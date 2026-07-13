# 1.0.41
- <c-dddddd>Fix a crash when suspending the game while in the editor on iOS and Android</c>

# 1.0.40
- <c-dddddd>Fix a crash that could happen when typing in the rotation or scale inputs after an object was deleted.</c>

# 1.0.39
- <c-dddddd>Fix a potential crash with scale and rotate</c>
- <c-dddddd>Add node ID to hide UI button</c>

# 1.0.38
- <c-dddddd>Add option to disable screen safe area</c>
- <c-dddddd>Make Grid Size control position more dynamic, to account for mods adding to the settings menu and different aspect ratios</c>
- <c-dddddd>Fix crash on iOS when editing spawn remap</c>
- <c-dddddd>Fix negate input not working</c>

# 1.0.37
- <c-dddddd>Fix scale undo not working</c>
- <c-dddddd>Fix scaling multiple objects still being bounded</c>
- <c-dddddd>Add Higher Precision Scaling</c>
- <c-dddddd>Rotation Controls now rotate alongside camera rotation</c>
- <c-dddddd>Holding shift while trying to drag the rotation controls while the camera was rotated now works properly</c>
- <c-dddddd>Scale Controls now rotate alongside camera rotation</c>
- <c-dddddd>Added option for keeping the Scale control size constant no matter the editor zoom</c>
- <c-dddddd>Fix start pos switcher when Auto Pause is enabled in the editor</c>
- <c-dddddd>Fix ground scale with Zoom Ground Fix when switching start positions</c>
- <c-dddddd>Fix Area Trigger corruption with start positions (vanilla bug)</c>
- <c-dddddd>Prevent "Who Added This" mod from spamming notifications when pressing shift</c>

# 1.0.36
- <c-dddddd>Fix scale input not scaling Y</c>
- <c-dddddd>Fix scale ratio not being preserved when using the scale slider</c>
- <c-dddddd>Change default scale snap to 0.25</c>
- <c-dddddd>Remove Grid Controls from 32-bit android due to crashes</c>
- <c-dddddd>Improve Scale Slider performance</c>
- <c-dddddd>Hopefully fixes the disappearing tab issue once and for all. Please update EditorTab API alongside this</c>
- <c-dddddd>Fix some text input issues</c>

# 1.0.35
- <c-dddddd>Fix scale sliders being draggable while disabled</c>
- <c-dddddd>(Maybe) Fix Grid Controls crashing on 32-bit Android devices</c>
- <c-dddddd>Add Hide All option for Hide UI toggle, to hide the playtest and pause buttons as well</c>
- <c-dddddd>Increase hitbox of Hide UI toggle</c>
- <c-dddddd>You can now click anywhere when the UI is hidden, touch is not blocked at the bottom of the screen</c>
- <c-dddddd>Hiding UI and then selecting something no longer makes the tabs reappear</c>
- <c-dddddd>Fixed an issue where clicking on a text input would click what is behind it</c>
- <c-dddddd>Fixed some keybinds registering on release</c>
- <c-dddddd>Added some keybinds</c>
  - <c-dddddd>Toggle UI Visibility</c>
  - <c-dddddd>Increase Grid Size</c>
  - <c-dddddd>Decrease Grid Size</c>
  - <c-dddddd>Show Scale Controls</c>
  - <c-dddddd>Show Scale XY Controls</c>
  - <c-dddddd>Show Warp Controls</c>
  - <c-dddddd>Move Object Half</c>
  - <c-dddddd>Move Object Big</c>

# 1.0.34
- <c-dddddd>Disable pinch to rotate by default due to the following feedback:</c>
  - <c-dddddd>"EW EWWWW THE TINKER UPDATE MADE THE CAMERA TURN WHEN YOU ZOOM OUT"</c>
  - <c-dddddd>"Trash! Destory it"</c>
  - <c-dddddd>"Oh ye I downloaded that mod to try it out and deleted it in 5 mins"</c>
  - <c-dddddd>"There's no checkbox to disable it" (There is)</c>
  - <c-dddddd>"Same here but i deleted it in 1 mins, Trash mod"</c>
  - <c-dddddd>"thats vile thats disgusting"</c>
  - <c-dddddd>"Why would they added that shit"</c>
- <c-dddddd>Fix not being able to type precise scales close to 1</c>

# 1.0.33
- <c-dddddd>Fix a crash that happens when Editor Rotation was disabled</c>

# 1.0.32
- <c-dddddd>Fix the delete tab position being different levels of wonky for different people</c>
- <c-dddddd>Rework some UI scaling stuff to hopefully prevent that issue in the future</c>
- <c-dddddd>Cleanup some touch code</c>
- <c-dddddd>Replace some BE features altogether rather than disabling</c>
  - <c-dddddd>Improved Scale and Rotate controls</c>
  - <c-dddddd>Hide UI Toggle</c>
  - <c-dddddd>Grid Size Controls</c>
- <c-dddddd>Clean up remnants of old scale control code that broke bypass</c>

# 1.0.30 & 1.0.31
- <c-dddddd>Fix edit tab scale and delete tab height if free move is enabled and you tap the screen</c>

# 1.0.29
- <c-dddddd>Fix unfocusing while in full screen not pausing audio</c>

# 1.0.28
- <c-dddddd>Make SingleDeselect better when clicking the same position</c>
- <c-dddddd>Add a module logs toggle</c>
- <c-dddddd>Fix search tab position on mobile</c>
- <c-dddddd>Fix delete tab position</c>
- <c-dddddd>Fix some UI scaling issues</c>

### BetterEdit ports Part 1

*These will only show when BetterEdit is disabled*

---

- <c-dddddd>Add Hide UI Toggle</c>
- <c-dddddd>Add Improved Scale and Rotate controls</c>
- <c-dddddd>Add Zoom Text</c>
- <c-dddddd>Add Grid Size Controls</c>
- <c-dddddd>Add LDM Object Count</c>
- <c-dddddd>Add Copy object string to clipboard</c>
- <c-dddddd>Add Hide Trigger UI</c>
- <c-dddddd>Add Pinch to Zoom</c>

# 1.0.27
- <c-dddddd>Hackily fix toolbar tab position by setting it on a schedule (I still don't get what causes this rare bug)</c>
- <c-dddddd>Fix layer locking not being respected when selecting</c>
- <c-dddddd>Fix edit tab visibility when deselecting objects with single deselect</c>

# 1.0.26
- <c-dddddd>Fix some crashes</c>
- <c-dddddd>Clean up UI Scaling</c>
- <c-dddddd>Fix Preview Object Colors button being positioned off screen with UI scaling disabled</c>
- <c-dddddd>Fix ground and middleground colors not updating when toggling preview mode off</c>

# 1.0.25
- <c-dddddd>Add NaN/Infinity object position (`GJBaseGameLayer::addToSection`) crash fix for the Editor (Vanilla bug)</c>
- <c-dddddd>Add Improved Group View support for Sequence Triggers</c>
- <c-dddddd>Fix toggle trigger visibility when the trigger is meant to be hidden (Vanilla bug)</c>
- <c-dddddd>Remove an unneeded log</c>
- <c-dddddd>Scroll rewrite no longer interferes with DevTools</c>
- <c-dddddd>Add keybind to open search tab</c>
- <c-dddddd>Add keybind to set editor rotation back to 0</c>
- <c-dddddd>Improve object search algorithm some more</c>
- <c-dddddd>Fix blending not working if set by a color trigger before a start position (Vanilla bug)</c>
- <c-dddddd>Fix undo not working properly with Single Deselect</c>
- <c-dddddd>Maybe fix rare bug where the toolbar will be at a weird y position</c>
- <c-dddddd>Add option to skip disabled Start Positions when switching</c>

# 1.0.24
- <c-dddddd>Fix a crash when playtesting</c>

# 1.0.23
- <c-dddddd>Add editor touch fix, useful for mobile users where touch stops working when dragging the camera and pressing playtest at the same time (Vanilla bug)</c>
- <c-dddddd>Add no touch fix when pasting object state. Fixes no touch not working until reopening the editor for those objects (Vanilla bug)</c>
- <c-dddddd>Improved Group View now works for Spawn remap and Advanced Random Triggers</c>
- <c-dddddd>Add Mixed Edit Extras visualization, darker button signifies a mixed checkbox</c>

# 1.0.22
- <c-dddddd>Fix preview object colors inheriting color channel 1 opacity and blending when default is set</c>

# 1.0.21
- <c-dddddd>Actually fix the link controls position on mobile</c>
- <c-dddddd>Fix tooltips showing when dragging the camera on mobile</c>

# 1.0.20
- <c-dddddd>Fix some features no longer working when entering the editor</c>
- <c-dddddd>Fix hovering CreateMenuItems when alerts are visible</c>
- <c-dddddd>Fix CreateMenuItems being dimmed when dragging in the editor area</c>
- <c-dddddd>Toolbar blurring now applies to popups from Object Groups</c>
- <c-dddddd>Fixed opacity being wrong with Preview Object Colors enabled</c>

# 1.0.19
- <c-dddddd>Fix an issue with Editor start pos switcher not loading initial start positions</c>

# 1.0.18
- <c-dddddd>Fix editor slider not updating when camera moves from scroll and centering</c>
- <c-dddddd>Add button to lock camera to music playback</c>
- <c-dddddd>Add fix where the ground position is weird when zooming (Vanilla bug)</c>
- <c-dddddd>Clean up module system a bit</c>
- <c-dddddd>Rework Quick Extras to be cleaner</c>
- <c-dddddd>Add Object Groups version check</c>
- <c-dddddd>Fix link controls position on mobile</c>
- <c-dddddd>Fix Preview Object Colors affecting objects that are black by default when no color is set</c>
- <c-dddddd>Fixed opacity not working with Preview Object Colors</c>
- <c-dddddd>Object Search no longer renders objects separately</c>
- <c-dddddd>Preview Object Colors now works in the search tab</c>
- <c-dddddd>Improve Object Search to group objects by tab, tab with most results shows at front</c>
- <c-dddddd>Require editor restart when toggling toolbar blur due to potential crash</c>
- <c-dddddd>Smooth scroll speed now ignores speedhack</c>
- <c-dddddd>Fix Edit Extras not showing for text objects</c>

# 1.0.17
- <c-dddddd>Duration Drag no longer shows if duration lines are disabled</c>
- <c-dddddd>Fix dotted line and pulse line colors when duration drag is enabled</c>

# 1.0.16
- <c-dddddd>Fix a crash when you have one negative editor layer and one positive editor layer on an object</c>
- <c-dddddd>Remove editor pause escape keybind for MacOS due to weird crashes</c>

# 1.0.15
- <c-dddddd>Fix a crash</c>

# 1.0.14
- <c-dddddd>Account for iOS safe areas</c>
- <c-dddddd>Live colors now adjusts positions relative to the tab rather than floating so high</c>
- <c-dddddd>Zoom to cursor now zooms to the correct position when the editor is rotated</c>
- <c-dddddd>Disabling Editor Rotation should now properly update the rotation of the slider button</c>
- <c-dddddd>Add Grid Invert opacity setting</c>
- <c-dddddd>Fix GJDropDownLayers not blocking input properly, and also being skipped on the unpause check</c>
- <c-dddddd>Fix tooltips persisting when editor is paused</c>
- <c-dddddd>Fix tooltip highlight persisting when the tooltip is reset</c>
- <c-dddddd>Rework Search Tab positioning on mobile to be more consistent</c>
- <c-dddddd>Object summary's cosmetic random rotation no longer changes on filter change</c>
- <c-dddddd>Teleport portals in the object summary no longer cause Editor Sounds to play a sound</c>
- <c-dddddd>Teleport portals in the object summary can now be distinguished from unlinked teleport portals</c>
- <c-dddddd>Preview object colors button no longer shows on the search tab (as it is not functional there)</c>
- <c-dddddd>The object ID on tooltips (when enabled) no longer show if the ID is 0</c>
- <c-dddddd>Object Tooltips no longer show on custom objects</c>

# 1.0.13
- <c-dddddd>Fix Edit Extras not updating properly</c>

# 1.0.12
- <c-dddddd>Fix top align UI scaling setting not updating</c>
- <c-dddddd>Fix pause menu scaling not updating when UI scale is turned off </c>
- <c-dddddd>Fix Named Editor Layers mod alignment issues</c>
- <c-dddddd>Fix IDs not showing in search with ObjectIDDisplay mod</c>
- <c-dddddd>Add `tinker::api::ui_scaling::UIScaleUpdated` and `tinker::api::ui_scaling::PauseUIScaleUpdated` for other mods to listen to if they wish to account for UI scaling</c>
- <c-dddddd>Fix Awesome modifier icons letter showing on the Object Groups category when it shouldn't</c>
- <c-dddddd>Update about and readme</c>

# 1.0.11
- <c-dddddd>Fixes for Object Groups</c>
  - <c-dddddd>Scaling is correct for pinned groups now</c>
  - <c-dddddd>Pinned Groups no longer jitter when dragged</c>
  - <c-dddddd>Fixed crash when adding or deleting object groups</c>
  - <c-dddddd>Fixed crash when hovering over a deleted object in a group</c>
  - <c-dddddd>Reposition object groups settings and object find button</c>
  - <c-dddddd>Find button now scrolls to the object properly, and shows the circle wave animation</c>
- <c-dddddd>Add my own responsive UI scaling settings</c>
- <c-dddddd>Fix transform controls not updating when centering objects to camera</c>
- <c-dddddd>Fix Object Search lazy loading not working when a search result had less than `rows * columns` items</c>

# 1.0.10
- <c-dddddd>Fix a crash when selecting triggers with insane duration amounts</c>
- <c-dddddd>Fix align button alignment (lol) being wrong sometimes</c>
- <c-dddddd>Fix a mistake in the description</c>
- <c-dddddd>Fix scale of object groups popup</c>

# 1.0.9
- <c-dddddd>Object Groups compatibility fixes for Scrollable Toolbar, Object Tooltips, and Search</c>

# 1.0.8
- <c-dddddd>Add Blur Behind option for Custom Toolbar Background when Blur API is installed</c>
- <c-dddddd>Fix quick extras button being positioned weirdly after pausing a playtest</c>
- <c-dddddd>Fix another BetterEdit bug unrelated to Tinker</c>
- <c-dddddd>Revert icon</c>

# 1.0.7
- <c-dddddd>Fix a leak causing a crash when pressing space while the Gen Color Triggers mod is installed</c>

# 1.0.6
- <c-dddddd>Fix a crash when placing start positions while start position switcher is disabled</c>

# 1.0.5
- <c-dddddd>Change the icon so people stop complaining that it's too detailed, "liquid ass", looks bad, etc.</c>

# 1.0.4
- <c-dddddd>Fix a crash</c>

# 1.0.3
- <c-dddddd>Fix misnamed setting for Object Search</c>
- <c-dddddd>More performance fixes</c>
- <c-dddddd>Toggling Length In Editor now updates the time properly</c>
- <c-dddddd>Live Colors now properly updates with default color IDs when objects are placed</c>
- <c-dddddd>Add selection amount for Paste Confirmation</c>
- <c-dddddd>Add disabling Paste Confirmation for objects with no groups and color set</c>

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
