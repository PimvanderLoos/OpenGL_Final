# OpenGL Contest submission: Tropical Island

### Pim van der Loos and Derk Pot


![full_scene](/screenshots/full_scene_crop.png)

![rhino_family](/screenshots/rhino_Family.png)

![rhino_hunt](/screenshots/hunt.png)

![elephant_hunt](/screenshots/elephant_hunt.png)


### Compilation
This branch has 2 repositories: 'master' and 'noBOOST'. 'master' depends on boost for model and texture serialization, which makes loading models MUCH faster. 'noBOOST', as the name implies works just fine without boost, but just a lot slower.

Requirements:
  * Qt5+ (we used 5.12.0)
  * C++17 compatible compiler (we used GCC9 20190113)
  * The resources files (which can be downloaded [here](https://drive.google.com/open?id=1f7YzkPjeW5uAEmZWoecBG_lP0qd_ZIdV))

We also provided a CMakeLists.txt in case you do not want to use Qt Creator.


### Scene building
To be able to easily build scenes, we added the following controls:

![controls](/screenshots/controls.png)

These are the entity-specific controls (there are global and model-specific controls as well). 
You can easily change the model of the selected entity, duplicate it, remove it etc.
We also added the shortcuts '.' and ',' to quickly switch between entities.

