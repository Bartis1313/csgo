# Csgo
Very short and commented source aimed for legit gameplay

![Bartis internal CSGO](https://imgur.com/QwPgxMp.jpg)

# Information
In development, not every feature may work perfectly.
<br />
Menu key is insert, panic key is delete, you can change it in config if it matters.
<br />
**BEFORE anything, make sure you have it installed**: [c++ redistributables](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170) [directX](https://www.microsoft.com/en-us/download/details.aspx?id=35)

# FAQ
Crashing on inject
- You either have some missing files, or cheat was not updated. Open the issue giving enough information to help you.

How to compile
- Just open VS, don't change anything, you can try change enhanced instruction set (valid for your CPU).

I can't compile
- Install runtimes for it.

# Features: 
<details>

 * BunnyHop
 * Chams (Normal, Xyz)
 * ESP (2D, 2D Filled, 3D, 3D Filled)
 * Glow
 * Sound ESP
 * Fov (Changing your local FOV)
 * Third Person (no bypass for sv_cheats included yet)
 * Aimbot (Nearest, Head, Chest)
 * Fov Aimbot
 * Smooth Aimbot
 * Triggerbot (delay in ms)
 * Rcs (With one value representing both yaw and punch)
 * Crosshair (Static, Recoil, Engine)
 * Backtrack (manipulation in ms)
 * Radar 2D (Representing on small overlay where enemies are, including their view)
 * Local Info (Some information about local player)
 * Esp Flags (Some information about enemies, their hp, armor etc...)
 * Esp Info (For now only name and C4 holder)
 * Esp Skeleton (Skeleton drawn by lines, working with backtrack too)
 * Backtrack Chams (Static, Rainbow, Last Tick)
 * DLight (Uses engine to highlight the player)
 * Nigtmode (Chnages materials to be darker including chnage of the sky)
 * Esp Lines (They will only show on selected target by aimbot)
 * Plots (Represent current FPS and Velocity in stored records and then drawn using lines and difference between them)
 * Arm Chams (Color, no arms)
 * Weapon Chams (Color, no weapon)
 </details>

# Keybinds
 - ARROW UP/DOWN (Set current index of feature, it will highlight at as grey color)
 - ARROW RIGHT/LEFT (Vector - switches index of vector, Int - add/minus, Bool - ON/OFF)
 - INSERT (Close menu)

# Issues or requests
For now the cheat may have them many as during development my main goal is to learn how to write optimized and simple code

About requests, when I personally think the feature provided is alright,
I will try to implement it in a short time perioid, eventually people who commit prs will be contributors

# Ending Goals
- More features, especially visual friendly
- <del>Fix performance, to keep it very stable </del> done
- Create custom UI, with quake looking set, starting from icons, ending on killfeed
- <del>Add option to either use x88 style menu or dynamic gui wiritten with game's engine or even ImGui</del> done, not ImGui part

# Log
<details>

* 1.0.0 - initial commit
* 1.0.1 - small fixes in code + rainbow chams
* 1.0.2 - improved a bit performance, changed render text function to modern wstring, aimbot big drops fixed, aimbot now uses ranged loop for hitboxes
* 1.0.3 - added PlayerResource class which contain kills deaths and ping. Improved some of the features again
* 1.0.4 - fixed typo for backtrack and added in comments ticks option to backtrack, I personally don't need it, hence why it's commented
* 1.0.5 - cleaned up the source in visuals and render. Added new player visuals. Fixed padding in one SDK class, added skeleton ESP. In utils there is a new conventer function which you can very easily use to get distance to enemy in meters (unit to meters)
* 1.0.6 - small update with removing fps drops while menu is opened
* 1.0.7 - big update for performance and cleanup in code, a lot of useless defines have been removed. Changed console::log function to take only one string and log type, string can be easily used with std::format
* 1.0.8 - small update with making menu usage more easier, items are now in map and features can be passed as name for vars::names map
* 1.0.9 - big update for performance, recode, and code style. All functions are camel case. I seperated visuals to be in few files since it's better to read and move in code. Added even more visuals once again. Fixed prediction, thanks: [kaposzta99](https://github.com/Bartis1313/csgo/issues/4)
* 1.1.0 - update came with fixes: triggerbot, backtrack records and SDK. Added Dlight and "nightmode" that just forces other skybox. Few very small addons to menu and code changes.
* 1.1.1 - small update with new planned feature and handling the error in case the directory can't be created.
* 1.1.2 - 2D plot added, for fps and velocity. From now if config fails to find file to save, it throws the error.
* 1.1.3 - Added hand/weapon chams, and rewrote chams code.
* 1.1.4 - Added global shots counter, which can result in getting valid accuracy per round, and hitmarker with sound got added.
* 1.1.5 - Added visibility check, in aimbot and triggerbot. Some visuals added.
* 1.1.6 - Changed aimbot, fixed visibility, aimbot now uses only one function to run. Added no scope overlay.
* 1.1.7 - fixed code, added more visuals. Added few more things to SDK.
* 1.1.8 - added image rendering, pretty simple as it does need only one argument. Console logging is a bit simplier now. Fixed the main thread call with handling critical errors correctly.
* 1.1.9 - very small update. Added check for paint hook, implemented idea of gui.
* 1.2.0 - update to gui and few fixes in random files. (broke the git again..., lol)
* 1.2.1 - GUI update with finishing ideas of everything, recode is still needed for automatic padding for GUI objects or add ideally add flags to choose.
* 1.2.2 - Big GUI update, it now can use auto padding, changed few comments to make it easier to understand in whole code. Still it needs few fixes.
* 1.2.3 - Almost GUI is finished, few things left.
* 1.2.4 - Update on new config system using type safe checks, menu is finally ready to work with variables and multiple configs. Fixed, removed bunch of functions. Deleted singleton as it's a very bad choice to work with single class objects. Reduced many calls. Made return address more easier.
* 1.2.5 - Made surface GUI example to be working correctly. Added ImGui + freetype libraries, as it's next goal to use this GUI framework as example + solid rendering. Fixed code in few places, and removed oftently changed files from precompilation, which could lead to errors.
* 1.2.6 - Added ImGui example, added ghetto fix for sendpacket, although it's not used. It's a temp fix. Made hooking even easier to use now.
* 1.2.7 - Added ImGuiRender class which has a rich amount of functions, arguments are almost everywhere same as in surface rendering. GUI made with surface has been deleted aswell as x88 menu, if you need to know how was it done, search old repo commits. Cleaned up whole sdk and code in few places.
</details>

# Dependencies
* [minhook](https://github.com/TsudaKageyu/minhook)
* [lazy importer](https://github.com/JustasMasiulis/lazy_importer)
* [xorstr](https://github.com/JustasMasiulis/xorstr)
* [lodepng](https://github.com/lvandeve/lodepng)
* [json](https://github.com/nlohmann/json)
* [ImGui](https://github.com/ocornut/imgui)

# People who helped
* [KittenPopo](https://github.com/KittenPopo) - answering questions I have been confused how something works and for cs15 leak which made reversing even easier.
* [Pawel Gozdur](https://github.com/carlos-konewka) - helping me with math, especially plot logic.
* [UC Wiki](https://www.unknowncheats.me/wiki/UnKnoWnCheaTs_Game_Hacking_Wiki) - for very helpful sources.
