# Csgo
Very short and commented source aimed for legit gameplay

![Bartis internal CSGO](https://i.imgur.com/G3xVmB0.png)

# Information
In development, not every feature may work perfectly.

# Features: 
 - BUNNYHOP
 - CHAMS [Normal, Xyz, Backtrack Chams]
 - ESP [2D, 2D Filled, 3D, 3D Filled]
 - GLOW
 - SOUNDESP
 - FOV (Changing your local FOV)
 - THIRD PERSON (no bypass for sv_cheats included yet)
 - AIMBOT [NEAREST, HEAD, CHEST]
 - FOV AIMBOT
 - SMOOTH AIMBOT
 - TRIGGERBOT
 - RCS [With one value representing both yaw and punch]
 - CROSSHAIR [Static, Recoil, Engine]
 - LOCALPLAYER INFO
 - ESP FLAGS
 - RADAR 2D

# Keybinds
 - ARROW UP/DOWN (Set current index of feature, it will highlight at as grey color)
 - ARROW RIGHT/LEFT (Vector - switches index of vector, Int - add/minus, Bool - ON/OFF)
 - INSERT (Close text menu)

# Issues or requests
For now the cheat may have them many as during development my main goal is to learn how to write optimized and simple code

About requests, when I personally think the feature provided is alright,
I will try to implement it in a short time perioid, eventually people who commit prs will be contributors

# Ending Goals
- More features, especially visual friendly
- Fix performance, to keep it very stable
- Create custom UI, with quake looking set, staring from icons, ending on killfeed
- Add option to either use x88 style menu or dynamic gui wiritten with game's engine

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
</details>

# People who helped
* [KittenPopo](https://github.com/KittenPopo) - answering questions I have been confused how something works and for cs15 leak which made reversing even easier.
* [UC Wiki](https://www.unknowncheats.me/wiki/UnKnoWnCheaTs_Game_Hacking_Wiki) - for very helpful sources.
