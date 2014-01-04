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
Current Version: 1.03 Pre-Alpha (1/3/2014)
==========================================

Changes to this version included the integration of force and field caculations based on magnetism and electric current 
has been added to the Polaris Engine. Point Charge struct has been added as well for use in magnetic calculations. 

-- Note *Using test seed #24 *

==================
Previoius Versions
==================

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
