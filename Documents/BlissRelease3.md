For install instructions, go [here](INSTALLATION.md)

REQUIRES RETracker Bliss Hardware

REQUIRES Retracker-Bliss-0.8.8.0 or higher software to support all the new features!
REQUIRES Unreal Engine 5 or higher
Works on UE 5.1 also

## Known Problems

* Measuring Scale Actor: If you go to "Details" and click/drag any of the Transform fields to move it, Unreal may freeze for a second or two, then come back. This does not happen if you click/drag on the actor in the 3d window and move it there.  This appears to be an Unreal bug, looking for a solution.

## What's new

* In REtracker Bliss settings.text file you can set the camera number that will appear in LiveLink, recommend using 1 to 100
* You can track an AprilTag and use it to measure your nodal offset and sensor position, the tag tracking shows up as Camera 101 in LiveLink.
* Added an AprilTag actor that has a "tag size" you should set to the size of your printed tag
* Added an ArucoTag actor that has a "tag size" you should set to the size of your printed tag
* Added a skeletal 3d box reference object, you can set this to any size and have the pivot point be either in the center or the corner of the box
* Added an adjustable "measuring scale".  You can set the scale to any length in meters and have marks every 10cm.  You can also turn on text labels every 10cm and minor marks every cm.
* None of this should require any recompiling for UE 5 or UE 5.1

## The files

### Ready to use

These are ready to use and should not require any compiling, just open them with the right release of unreal.

LiveLinkBliss_R3_for_UE55.0.zip  for Unreal Engine 5.0
LiveLinkBliss_R3_for_UE55.1.zip  for Unreal Engine 5.1

### Repository Snapshots

You DO NOT need the files labeled "Source Code" they archives of the repository as of Release 3.  They are not ready to use and require compiling with Visual Studio 2022

## Problems

If you have problems please check [troubleshooting](TROUBLESHOOTING.md) 

If this does not help or you need a version for an older Unreal Engine, please post an issue [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues)