# Csgo

![Bartis internal CSGO](https://user-images.githubusercontent.com/69613796/179428634-adf44410-4002-4e5d-9ac5-b794dcb1c35f.png)

## Introduction

This project is and was a part of my c++ and game-hacking journey. The biggest focus of this cheat is on the visual aspects of the game Counter-Strike: Global Offensive (CSGO).

Please note that this cheat is for educational purposes only, provided as a training software. To ensure your safety while using cheats, it is recommended to use a reliable VAC module dumper and debug any outputted files. It is important to also look what client does to the game. You can start by understanding the information provided in the [VAC GitHub repository](https://github.com/danielkrupinski/VAC).

## Features

<details>
<summary>List of Features</summary>

* Aimbot: The aimbot feature provides various options such as smoothing, delay, and the ability to select hitboxes and define visibility rules. It sets the position of the mouse and retrieves data from the `createMove` function to ensure accuracy. It can be configured with a hotkey.
* Recoil Control System (RCS): The RCS feature allows you to control recoil by communicating with the game in a similar manner as the aimbot. It can be matched with custom pitch and yaw settings.
* Triggerbot: The triggerbot automatically shoots at targets when they are in your crosshair. It can be configured with a customizable delay.
* Backtrack: This feature manipulates the client to send a fake tickrate, enabling the storage of historical records. This allows you to shoot and hit targets as if they were in the past.
* Fake Latency: By sending fake latency to the server, this feature provides an additional tool for backtrack functionality.
* Bunnyhop: The bunnyhop feature enables automatic strafing and includes an autostrafe function to help accelerate sidemoves. It can be configured with a chance of successful bunnyhops.
* Thirdperson: This feature activates the third-person view in the game. It includes tracing to avoid clipping with walls. The position and hotkey can be customized.
* Crosshairs: This feature displays crosshairs in various styles or forces the game to display its own crosshair.
* Local Information: This feature provides information about the local player, such as ammunition, position, and more.
* Plots: The plots feature displays frames per second (FPS) and velocity using ImPlot. Velocity data is gathered from prediction.
* Hitmarker: When a hit occurs, this feature displays a hit position with a custom hitmark.
* No Scope: This feature nullifies the game's blur scope shader.
* Bullet Tracer: Bullet tracers use the game's engine beams to display the source and destination of bullets.
* Flashlight: This feature utilizes the game's flashlight effect.
* Nade Prediction: Predicts where grenades will land. A nade tracer displays the trajectory of grenades.
* Engine Prediction: Provides 1:1 prediction from the game, allowing accurate gathering of player data.
* Chams: Overrides the model with custom materials. It also allows the addition of custom materials at runtime using a user-friendly editor.
* Glow: This feature is a rebuild of game's build in glow system. And exposes visibility stencil done correctly.
* Sound Esp: A replica of the Apex Legends sound passive of the Bloodhound character.
* ESP (Extra Sensory Perception): Displays information about players, such as boxes, dlights, health, armor, flags, and more.
* Enemy Aiming at You Warning: Displays a small warning text when an enemy is aiming at you.
* 2D Radar: Displays a custom radar with a rotated background image, hiding the game's default radar to prevent overdraw.
* Skybox: Allows loading of any skybox into the game.
* Edit World Colors: Modulates the color of textures in the game world.
* Movement Trails: Displays lines representing recent player movement.
* Bullet Impacts: Displays cubes representing the positions where bullets hit.
* Edit Particle Color: Edits the color of effects such as molotov, smoke, and blood impacts.
* Draw Dropped Weapons: Displays dropped weapons with additional information.
* Bomb Overlay with Info: Displays bomb-related information as an overlay.
* Mirrorcam: Displays a mirror camera overlay.
* Freecam: Enables a freecam mode similar to Minecraft, allowing free movement.
* Blacklist: Sets priority for targeting specific entities.
* Playerlist: Stores entities in a centralized map.
* Logger: Logs game actions.
* Ambient: Forces the game to render ambient light.
* Fog: Control the fog effect in the game.
* Tone: Control the tone effect in the game.
* Weather: Renders precipitation effects in the game.
* Interpolation: Disables interpolation completely.
* Flashbang Reduce: Reduces the alpha of flashbang effects.
* Screen Effects: Forces the game to render selected effects.
* Motion Blur: Replicates the game's motion blur effect.
* Molotov and Smoke Overlays: Displays circles or polygons representing the radius of smoke or molotov effects.
* Zeus Display: Draws a circle representing the range of the Zeus taser and allows tracing of the polyline.
* World lights: Edit world lights directly, with possibility to restore original values.
</details>

<details>
<summary>Extra</summary>

* Threadsafe streamproof render wrapper, surface rendering is deprecated
* Icon extraction from the game to image
* Game callbacks
* Entity cache
* Discord Rich Presence (RPC)
* Hook-to-feature communication using handlers
* Logging console
* Game events (everything gathered from hooks, possible to create own though)
* Compile-time signature to optional bytes conversion
* Exports and memory class
* Netvars with dumping
* Interfaces with dumping
* Runtime shutdown with proper release of features
* Streamproof game's material rendering (some render states have problem)
</details>

## Building Instructions

### Requirements:

* [Git](https://git-scm.com/download/win)
* [Visual Studio](https://visualstudio.microsoft.com/)
* [C++ Redistributables](https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
* [DirectX](https://www.microsoft.com/en-us/download/details.aspx?id=35)
* [vcpkg](https://vcpkg.io/en/index.html)

Note: Git is optional but recommended for staying up to date with the project.

After making any configuration changes, delete the existing entry and let the program create a new default configuration.

### Dependencies

The dependencies folder is not for including dependencies themselves. If the dependencies are available in vcpkg, do not include them manually. Only include unsupported libraries or custom addons as rare exceptions.

## Building the Project

Please follow the steps outlined in the [Vcpkg Getting Started guide](https://github.com/Microsoft/vcpkg#getting-started). After installing the necessary dependencies, open the project in Visual Studio and click "Build". If any errors occur, please open an issue.

## Running the Cheat

The default key to open the cheat's menu is `INSERT`. Additional options within the menu will be self-explanatory.
For debugging the cheat please use injection method that lets you fully load pdb. As simpliest is better.
Debug build does allow exceptions, whereas release mode doesn't. Api will still take care to "simulate" the throws in the code.

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
* 2.0.4 - Dropped animations, it's useless to keep it updated all the time to newest lib version. Colorized the menu, and somewhat reworked look of it. render -> use namespace, and deprecate surface at all. Provided rendering backends for easier lookup. x88 menu now won't update keys if we don't want to. Control present / reset targets on own for streamproof method. Completely removed vectored handler - reason is because we can actually force memory leaks? That's happening eg: if you are the local host. Did some random cleanups.
* 2.0.5 - menu recode, I hope it's "clean" now. Recode of bunch of classes, WIP chams editor cleanup, because it annoys me how complicated it is.
* 3.0.0 - WIP refactor, making it as 3rd version due to the hard cleanup / refactor (not finished yet). I basically removed stupid OOP forcing to some real abstraction with hook and feature.
* 3.0.1 - Fixed UB stuff, detailed in commit message.
* 3.0.2 - Fixed the glow system, with better and more performant solution. I have also fixed the glow box lines with fading color. Added basic streamproof solution, which lacks some aspects and I couldn't force the game to understand some render states. Added interface names dumping.
* 3.0.3 - Streamproof chams added, world lights modulation added. Chams got somewhat better solutions to render them, especially see ignorez. Tried to mess with collide for weather particles, but failed to understand everything, code was left for testing usage only. Added alternative hooking method, in case anyone needs to mess with gameoverlay.dll. Fixed bug with round ends event. 
* 3.0.4 - Weather works for more types. Removed all listened events, instead gather information from another hook. Particle editing doesn't touch alpha channel anymore.
* 3.0.5 - Weather effects, and 2 bugs fixed. 1st is tone bug with default bloom, 2nd is wrapper for createMaterial missing loadFromBuffer. 
* 3.0.6 - Introduced simple api, to still be able to use ``throw`` instruction. [Manual map errors](https://github.com/Bartis1313/csgo/issues/77) are fixed. Fixed dumped vmt files. Few crashes with few forgotten checks where fixed. Removed discord RPC, because it is deprecated (discord-rpc lib). Introduced own ntdll functions for memory (not used, as currently lazy importer does great job with exports and modules). Cleaned up DllMain. Switched the timer class to use lower level functions for timers (abi didn't change). 
</details>

# People who helped
* [KittenPopo](https://github.com/KittenPopo) - answering questions I have been confused how something works and for cs18 leak which made reversing even easier.
* [Pawel Gozdur](https://github.com/carlos-konewka) - helping me with math, especially plot logic.
* [UC Wiki](https://www.unknowncheats.me/wiki/UnKnoWnCheaTs_Game_Hacking_Wiki) - for very helpful sources.
* [rollraw/qo0](https://github.com/rollraw) - swapchain render.
