ProjectPolarity
===============

Polaris physics engine and game prototype Polarity

Project Polarity - HISTORY

Project Polarity started as an idea from a friend for a cool mechanic for a mobile game. It has over the years morphed and 
conformed into many different things, latest of which was as an entry for my Senior Innovation Project at University of 
Advancing Technology. The idea was to create a C++ library that supported physics based on magnetism and electro-magnetic
force. The testing of this physics library would be demostrated with a game that featured magnetism as its core mechanic.
The duo were dubbed Polarity (game) and Polaris (physics library). 
  
Polarity is a procedurally generated tunnel navigation game that uses magnetism to manipulate a player's avatar movement
through the tunnel. Each playthrough would be different and movement manuevers would get harder the longer the player has
played the current tunnel. 

==========================================
Current Version: 1.06 ALPHA (2/19/2014)
==========================================

This version brings about spacing in the obstacles throughout the level. Reduction in level overall size to support older PCs and a game start up sequence. The sequence now shows intro splash screen and has working basic menu to start and exit game. 

==================
Previoius Versions
==================
-1.05 ALPHA (2/9/2014)

Changes in this version finish up pre-alpha goals. Camera is implemented with full scrolling both left and right. NUM_POINTS for both top and bottom of tunnel have been shaved to fit maximum bitmap size for level. Collision Detection works perfectly throughout level for both top and bottom of tunnel. 

Main goals of version are:
1) Implement physical collision with ceiling and floor based on detection.
2) Implement forces on magnets to pull and push ship based on current polarity. Remove arrow movement.
3) Implement a spacing algorithm for both the magnets (top and bottom) and the obstacles. This will ensure that they are    not overly close or too far apart. As well as equally spread throughout the level.
4) Implement game startup screen (Splash, Game Menu, Options, Pause?)
      - This involves creating gameStates and a gameState Manager to control it. 
5) Implement end of level flash in polaric colors (1/2 screen - WHITE, 1/2 screen - RED).

- 1.04 Pre-Alpha (2/6/2014)

Changes in this version include the integration of collision for ceiling of random generated lines and for floor of randomly generated lines. Ship has been encapsulated, Point structure has been completely removed. All positions are now based on Vector3 structures. 

-- Note * Using test seed #64789 *

- 1.03 Pre-Alpha (1/3/2014)

Changes to this version included the integration of force and field caculations based on magnetism and electric current 
has been added to the Polaris Engine. Point Charge struct has been added as well for use in magnetic calculations. 

-- Note *Using test seed #24 *

- 1.02 Pre-Alpha (1/1/2014)

In its current state Project Polarity has the beginnings of the foundation for the physics library. The library supports 
the following data containers - Vector3, Quaternion, Martix4 and Matrix3. These will be manipulated through the physics
applied through electric force and magnetism. 

Polarity successfully draws the procedually genereated tunnel to a set length. The player is drawn and force movement is 
applied for testing. 

Main goals of this version are:
1) To get the remainder of the physics force calculations coded and integrated into the library.
2) Nail down collision with walls of tunnel, both top and bottom. 
3) Implement following camera. 
