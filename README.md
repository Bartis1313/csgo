# Csgo
Source aimed for visual aspects of the game.

![Bartis internal CSGO](https://user-images.githubusercontent.com/69613796/179428634-adf44410-4002-4e5d-9ac5-b794dcb1c35f.png)

## How to build

Requirements:

 * [git](https://git-scm.com/download/win)
 * [vs](https://visualstudio.microsoft.com/)
 * [c++ redistributables](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
 * [directX](https://www.microsoft.com/en-us/download/details.aspx?id=35)
 * [vcpkg](https://vcpkg.io/en/index.html)

Note that git is very optional, it's suggested option to stay up to date.
<br />
After every config change it is required to delete the entry and let it create a new default config.

### About dependencies

Dependencies folder IS NOT for dependencies themselves, if they are on vcpkg, do not include them manually! Otherwise add those
<br />
Some rare exceptions are edited libraries, but it is prefered to not.

## How to build
Please follow the short read [here](https://github.com/Microsoft/vcpkg#getting-started)
After installing, open the project and click build.
If there are any errors, please open the issue.

## How to run

Default menu key is ``INSERT`` more options will be self explanained by viewing menu.

## Information
Features marked ``(rebuild needed)`` need recode or better version.
<br />
This is a trianing software. No further actions taken by people are heeded.
<br />
<br />
As you know or not, this is not going to ensure your safety in 100%. To do that, use any good vac module dumper, and debug outputed files.
<br />
Good start is to: use any valid vac modules dumper (that will decode inputs), and understand all [this](https://github.com/danielkrupinski/VAC)

# Features list: 
<details>

 * Aimbot - smooth, delay, types, (mouse override)
 * RCS - control the recoil spray. Possible to smooth it. (mouse override)
 * Triggerbot - delay in ms.
 * Backtrack - manipulate ticks.
 * Fake latency - manipulate sequences in ms.
 * Bunnyhop - with chance%
 * Autostrafe - with few methods.
 * Callbacks - not really used, good example how to use them.
 * Events - listen game events, needed for some features for info. (Should avoid this if possible)
 * Thirdperson - 3rd person on key. Included with tracing, pretty much exactly what originally game does.
 * Crosshairs - custom crosshairs, 5 types.
 * Local info - ammo, pos etc... Bunch of local player informations.
 * Fps plot - collect fps and represent it in graphical resizable plot.
 * Velocity plot - collect local player speed and represent it in graphical resizable plot.
 * Hitmarker - ability to draw hitmarkers 2D/3D with dmg output.
 * No scope - removes scope black overlay.
 * Bullet tracer - draw bullets directions by line beams. Possible to also create own bullet tracer type.
 * Flashlight - use game effect flashlight.
 * Nade prediction - predicts endpoint of your nade throw. Also can predict incomming nades by same logic. Would need some rework for tracing specific things as water, glass, ents...
 * Engine prediction - useful to never skip tick and get correct server time.
 * Chams - uses engine's keyvalues to render it. 5 types and ability to draw backtrack records. Also possible to create your own.
 * Glow - highlight player ents, does not force memory glow vector, but controls it by own.
 * Sound Esp - apex legends like sound esp from passive skill of bloodhound character.
 * Esp - name, flags, boxes, health, armor, ammo, skeleton. 2D/3D.
 * DLight - highlights player by dlight. (Can't shortly fix the optimazation for them, it's due to overcomplication in how dlights are generated)
 * Enemy aiming at you warn. (Note that enemies using fov-changer hacks will not always be marked as "see you", it's physically impossible to gather fov from other clients by client)
 * 2D radar - overlay showing where enemy is, with background of the map texture. Renders rotated triangles that can be clipped with ranges or not.
 * Custom skybox - listed from game or custom, custom skybox is i 1st hierarchy.
 * Remove sky - removes props by forcing cvar, (rebuild needed - don't force, check retaddr or find if any function could be hooked for this)
 * Edit world colors - props/walls/sky (rebuild needed - transparency should be done in different way)
 * Movement trails - collect line frame by frame and draw from container
 * Bullet impacts - where the bullet has hit. Does not use events, instead gets client information of bullets list.
 * Custom smoke/molotov - color/remove/timer
 * Draw dropped weapons - box/icon/name/ammo. Fading alpha with distance
 * Bomb overlay with info. - draws infomation about bomb, where, what dmg, who planted.
 * Mirrorcam - show what is behind you on extra window (contains fixing setupbones occlusion problems)
 * Freecam - view any place without moving your model (dormant ents won't render)
 * Freelook - a copy of how it looks like in PUBG
 * Blacklist - blacklist bots and players, forces aimbot to fucus blacklisted players.
 * Playerlist - a playerlist that will give information of every player on the server, in one window. Does provide information for dormant ents.
 * Cache - not a feature, just escapes using same loops for(int i; 64)... everywhere.
 * Logger - log anything to left corner, with animated move + alpha when text duration reaches end.
 * Ambient - ambient light (colors are hard to set without convar - shaders api).
 * Fog - fog controller, color and intensity.
 * Weather - change weather, might cost a lot of performance drop, due to fact many particles will render.
 </details>

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
* 1.4.0 - After some break, added the grenade warning, lacks of visuals and custom choices of colors etc... No more errors with std::format, small config issue fixed.
* 1.4.1 - Nade prediction should output given dmg by nade, or inform if inferno is in range. Fonts contain pixel size at the end now. Input system will now only work when: we are not in key hotkey, when vgui console is off, when chat window is off.
* 1.4.2 - Rainbow colors can be an option from picker, remade color picker + logic in config for color.
* 1.4.3 - Finish nade prediction at all, warning for tracers will be scaled with distance to the nade.
* 1.4.4 - Sound esp is done. Credits to the guy who had the idea with this. Scaling included for sound circles.
* 1.4.5 - Changes in code, mainly to render low levels things such as images by resources. Few slight changes into imgui: add ability to load up image by memory, ability to draw multicolors with poly. Discord rpc with updated information from client.
* 1.4.6 - Texture rendered in the radar, I run on some weird problems there and unfortunately because of the dds file format it's impossible to use Resource class. Cleaned the project settings. Instead of ignoring the error of unsafe function, getting time is threadsafe now. Game color edits are now run per each instead of applying whole list.
* 1.4.7 - Easy cfg load on start option, setting load cfg won't save current variables but only name to load.
* 1.4.8 - Added mirrorcam (rendering by ImGui), freecam, freelook, fixed SEH catching allocation for symbols. Console logging is now locked, it's better to run them in mutex.
* 1.4.9 - Flashlight fixed and added it as a feature. Recoded rendering to take ImVec2 instead of vector2D when using imgui. Fixed/recoded drawing 3d boxes completely. Init localplayer once (and this method probably fixes crashes). Gather prediction move data from stack. Pass std::span as normal copy, not cref.
* 1.5.0 - World screen effects added: fog, tone control, draw screen effects.
* 1.5.1 - Add weather effect.
* 1.5.2 - Added 1:1 motion blur from source leak. Controlling thirdperson should be quite better as now it traces the distance to any objects, just like game originally does. Added extra options to it as suggested in: [#41](https://github.com/Bartis1313/csgo/issues/41). Added comments for sigs, I can't shortly explain how to use RTTI info in 3 lines, so this is just very basic string ref info.
* 1.5.3 - HUGE recode, cleanup in way that features are inited and run. Also general cleanup/fixes in features. Now every change will be much easier to make, without calling some run() method manually, it all gets called by static object vec, code itself explains it very well. Each class has one task to do, or very limited to more.
* 1.5.4 - Simplified constructors in base classes. If possible they should be constexpr.
* 1.5.5 - Config update for weapons selection in aimbot. Fixed crash with weather on new map. Cleaned up menu a bit.
* 1.5.6 - Few checks added to aimbot, triggerbot, backtrack and player visuals.
* 1.5.7 - Visual update for fading, players and dropped weapons. Cleanup in Resource class.
* 1.5.8 - Cache for ents done, when drawing something ent based in present hook you should stick to normal classic loop, as commented in radar file.
* 1.5.9 - Memory cleanup, static offsets were always a problem due to short lag while gathering the offset. Now it run once at the start + adding it should be very easy. Handled exports, mostly needed for sig scan and CreateInterface.
* 1.6.0 - Include paths.
* 1.6.1 - Include paths v2.
* 1.6.2 - Source cleanup, generic vector & matrix.
* 1.6.3 - Once again better generic vector. Config is now stored in nice-looking json + accessing values is not painful. Cleanup is still needed. Better smoothing included in aimbot, still needs some extensions.  
* 1.6.4 - Config fix again, added magic_enum to not repat same names in the selections namespace.
* 1.6.5 - Bit of clean up with memory, interfaces and valid entity cache that will work for any frame. And maybe crash fixes?
* 1.6.6 - Cleanup, base classes for features are now more abstract. Fixed crash due to wrong allocation for vector. Rebuild hook structs to be defined with macros.
* 1.6.7 - Wip menu.
* 1.6.8 - Finished menu animations/movelikes.
* 1.6.9 - Playerlist added (blacklist not done yet), changed render to take unique ptr.
* 1.7.0 - Blacklist finished, including crash fixes.
* 1.7.1 - Refactor of code. Fixes, and some addons, especially hooks. Deatailed in commit.
* 1.7.2 - Cleanup for lerping animation.
* 1.7.3 - visuals health/armor bug fix, more "clear" hooked functions definitions, accurate info gathered directly from the game about shots, not using events to count them manually (awful!). Prediction last edits. SDK cleanup and addons. Sound is finally not using events, wip to remove other events. Editor still not finished, almost... Plots reworked to work for last rendering change. Fixes to radar, eg: MapStruct, triangle enemies.
* 2.0.0 - Made it as a new version since it does change a lot. Provided very simple instruction for people who want to use this at long end. Bunch of very few refactors/renames in functions. 2.0 is now having a friendly folder structure. Replaced render from utils to own folder. As well as other things which were "overnamed" to be in utils. Very soon to finally end that project.
* 2.0.1 - Removed xorstr from here, it's a public source. Fixed hotkey with id detection, removed FORMAT wrapper since there is now no need. Removed console macros to few wrapping functions, removed macro for EXPORT, replaced in memory namespace as a template function. Added game punch which should be more "smoothed". Wip SEHCatch as well as features cleanup.
* 2.0.2 - Ending it soon... Finally got some free time. What is worth mentioning is fixing the aimbot smooth as well as RCS, they both now never set angles. Chams editor is finally added, current code is very far from clean, but the generation / updating / deleting / saving work well. Molotov now has a better way of drawing its range, added fancy triangulation with some poor workarounds to make it scaled.
* 2.0.3 - Added proper icon parsing, glow 1:1 to the game's code, constexpr signature to hex, less logging on release, fixed few crashes under visual features, and possible crashes with material system. What's left is cleaning up this code.
</details>

# People who helped
* [KittenPopo](https://github.com/KittenPopo) - answering questions I have been confused how something works and for cs18 leak which made reversing even easier.
* [Pawel Gozdur](https://github.com/carlos-konewka) - helping me with math, especially plot logic.
* [UC Wiki](https://www.unknowncheats.me/wiki/UnKnoWnCheaTs_Game_Hacking_Wiki) - for very helpful sources.
* [rollraw/qo0](https://github.com/rollraw) - swapchain render.
