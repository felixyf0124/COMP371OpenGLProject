
Please check the hot key for key actions.

=============================

Notice: it might take a few minutes to load all models, so please be patient.



=============================


Project progress report
=============================
-----------------------------

Project starts 
-----------------------
2017.06.06
=======================
Appartment User Draw GUI done with rectangle draw
-----------------------
2017.06.09
=======================
1. floor base insert, generate in 3D.
2. camera movement class & function added: camera move (W, A, S, D) & tilt (hold mouse right button)
3. giving up building our own room modeling tool
4. using 3ds Max for room building
5. group meeting with Edip 
-----------------------
2017.06.10
=======================
1. done modeling draft for room
2. clear useless code leftover from yesterday
-----------------------
2017.06.11
=======================

v7.06.13.19.53
---------------
- enable loading rooms with textures
- texture tiling value is not available to be applied easily to assimp
- hard coding tiling value is applied to remap texture size
? texture coefficients might need to adjust since the scene looks a bit awkward
- hard coded texture coefficients (ambient, diffuse, specular), the scene looks much better

v7.06.15.13.45
---------------
* mapping improvement - fixed UV mapping
- tiling value is removed since UV texture value is remapped directly through 
	modeling tool (3ds Max) without using tiling value
* Extra feature
- jumping function added
-----------------------
2017.06.15
=======================

v7.06.16.19.32
---------------
* lighting
- camera light changed to spot light
- living room light added
- light trigger added
- light coefficient adjusted
-----------------------
2017.06.16
=======================

V7.06.17.01.57
---------------
* model loading improvement
- adjust data passing through pointers or access through address, free up about 10 MB RAM with a bit faster loading
	(used to be 278 MB, currently 267 MB with 4 sofas + 1 toilet + rooms)

V7.06.17.01.57
---------------
- enable left click to select the item shown in the center of the screen and enable to move it through moving camera position
? bug - while pressing multiple camera movement keys, the selected object misses following camera position
      * completely solved - reset model matrix before really doing the movement in each direction
	
- added constructor overload for the shader class to include geometry shader for shadows
- algorithm for shadows added into the program
? bug - shadow algorithm completely overtakes the entire lighting of the scene and renders all objects completely black

-----------------------
2017.06.17
=======================

V7.06.17.01.57
---------------
- added function - enable to horizontally tilt obj based on camera position
	* fixed super bug - (MATRIX1 *= MATRIX2) != (MATRIX1 = MATRIX2*MATRIX1)
	* fixed last movement of obj was not recorded
- shadows still not able to be detected. Added renderScene function but did not solve the 
issue and created more bugs so it was removed. 


-----------------------
2017.06.18
=======================
- shadows shaders removed from code implementation and unused code removed to prevent unnecessary code clutter
- code comments added into the project.cpp class

V7.06.19
---------------
- all furniture set done
- camera-to-wall collision detection added
V7.06.20
---------------
- camera-to-object collision detection added
- object-to-object collision detection added
- object-to-wall collision detection added

-----------------------
Current solution version 7.2017.06.20