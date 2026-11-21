# Tinker

**A plethora of editor changes, additions, and improvements to make editing levels feel just a little bit better.** 

<c-dddddd>This mod provides a fully customizable and modular feature set for you to use the editor how you want to!</c>

**Don't like a feature?** 

<c-dddddd>Well you can toggle anything you want on and off. All disabled features are **fully** disabled, meaning they won't be hiding in the background affecting performance without you realizing.</c>

<c-dddddd>Please report any bugs or crashes on the mod's [GitHub Issues](https://github.com/Alphalaneous/Tinker/issues) page.</c> 

<c-dddddd>**View the full feature list [here](https://alphalaneous.dev/tinker)!**</c>

---

*Support banner art by [yoxnol](https://twitter.com/yoxnol)*

---

## API
Tools to interface with this mod as a mod developer.

---

### ​  UI Scaling

- <c-dddddd>Listen for when the UI scale is changed.</c>

```cpp
#include <alphalaneous.tinker/include/UIScaling.hpp>

// scale will be 1 if the setting is turned off while in the editor
tinker::ui_scaling::UIScaleUpdated().listen([] (float scale, bool scaleToolbars, bool topAlign) {
    // editor UI scaled
});

tinker::ui_scaling::PauseUIScaleUpdated().listen([] (float scale) {
    // editor pause UI scaled
});

```

---

## Credits
Cool people that helped with the making of this mod.

---

### ​  Mat
- <c-dddddd>Original Reference Image concept</c>

---

### ​  TerminumGD
- <c-dddddd>Idea for Hide IDs in the Edit Level screen</c>

---

## Support

---

- <c-dddddd>If you like this mod and what I make, feel free to donate to my [Ko-fi](https://ko-fi.com/alphalaneous).</c>
