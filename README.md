# Csgo
Very short and commented source aimed for legit gameplay

![Bartis internal CSGO](https://user-images.githubusercontent.com/69613796/166804234-76aa01ff-36a8-4688-ad6a-b3b517413eb1.png)
Menu screens: [screens](https://imgur.com/a/FiTtSOT)

# Information
The development soon ends, it was very cool time for me to dig into that game for my needs. Features marked ``(rebuild needed)`` need recode or better version.
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
 * Chams (Fully possible to customize currently running 5 materials. Players, weapons, arms are rendered separately)
 * ESP (2D, 2D Filled, 3D, 3D Filled)
 * Glow
 * Sound ESP (rebuild needed - events are not always networked with the steps)
 * Fov (Changing your local FOV)
 * Third Person
 * Aimbot (Nearest, Head, Chest, delay in ms - no instant switch to new target in fov)
 * Fov Aimbot
 * Smooth Aimbot
 * Triggerbot (delay in ms)
 * Rcs (Customizable to decide both angle values)
 * Crosshair (Static, Recoil, Engine, Spread)
 * Backtrack (manipulation in ms)
 * Radar 2D (Representing on small overlay where enemies are, including their view)
 * Local Info (Some information about local player)
 * Esp Flags (Some information about enemies, their hp, armor etc...)
 * Esp Skeleton (Skeleton drawn by lines, working with backtrack too)
 * Backtrack Chams (Static, Rainbow, Last Tick)
 * DLight (Uses engine to highlight the player)e)
 * Nigtmode (Chnages materials to be darker including chnage of the sky)
 * Esp Lines (They will only show on selected target by aimbot)
 * Plots (Represent current FPS and Velocity in stored records and then drawn using lines and difference between them, fully custom)
 </details>

# Keybinds
 - INSERT (Close/open menu)
 - DELETE (Panic key - shutdowns whole hack thread)
 - HOME (Console key - shows the console window rendered with ImGui)

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
* 1.2.8 - Fixed few rendering bugs. Added basic imgui menu, if you care for styling etc... go make a pull request so people can use it as some replacement. Keep in mind - any imgui addons that are not included in the lib should be made in imguiaddons.hpp at all. Color now has a different type, due to making it easier for floating points. You will still be able to use it as 0-255 range.
* 1.2.9 - Few fixes in some places, nothing really big. I will start making commits comments a bit better from now on, more detailed.
* 1.3.0 - Huge recode, cleaned up most of bloat header includes. Features are now not in namespace but class. Improved code quality almost everywhere. Fixed unloading the cheat not causing any errors. Plots are totally rewritten. Fixed couple of features, although chams are temporiarly not possible. I am thinking of clean solution for them. Added console from imgui demo which will show current loggings. Still a lot of code needs rewrite.
* 1.3.1 - Added option "load on start", which will load selected config on the start.
* 1.3.2 - The hitmarker is now finished. Nothing more in this version.
* 1.3.3 - Fixed "Enemy is aiming at you", aimbot, and many more. And added working chams, still small fixes are needed to them.
* 1.3.4 - Fixed stupid math calcAngle basing on src - dst logic, made it same as calcAngleRelative with how delta is achieved. Few rendering improvements such as rendering font. Use cached bones for calculating the pos. Added posibility to render icons from the game. Added lines to radar.
* 1.3.5 - Fixes in visuals and addons. Addons as nade timers, editing molotov color, flags done with distance scaling.
* 1.3.6 - Fixes new update crash and config based crashes. Added many effects and edited code in many places. Everything is detailed in the commit.
* 1.3.7 - Fake latency, and I hope last backtrack. Make sure prediction is updated when deltatick is any value, just like game does. Added bomb overlay, not perfect for resizing because the size is always the same.
* 1.3.8 - Key detection made properly. Using 2 classes to detect it, as first using messages coming from wndproc is running 24/7 and other only listens to given key from config. Why making own detection? ImGui has it, but then you will have to deal with flickering cursor problems if want to run it 24/7.
* 1.3.9 - Reback x88 menu, this time with better types detection and code. It also has ability to change config variables by operating on arrow keys.
</details>

# Dependencies
* [minhook](https://github.com/TsudaKageyu/minhook)
* [lazy importer](https://github.com/JustasMasiulis/lazy_importer)
* [xorstr](https://github.com/JustasMasiulis/xorstr)
* [json](https://github.com/nlohmann/json)
* [ImGui](https://github.com/ocornut/imgui)

# People who helped
* [KittenPopo](https://github.com/KittenPopo) - answering questions I have been confused how something works and for cs18 leak which made reversing even easier.
* [Pawel Gozdur](https://github.com/carlos-konewka) - helping me with math, especially plot logic.
* [UC Wiki](https://www.unknowncheats.me/wiki/UnKnoWnCheaTs_Game_Hacking_Wiki) - for very helpful sources.
* [rollraw/qo0](https://github.com/rollraw) - swapchain render + icon font.
