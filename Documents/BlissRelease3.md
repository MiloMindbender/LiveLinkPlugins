For install instructions, go [here](INSTALLATION.md)

Release Notes

REQUIRES RETracker Bliss Hardware

REQUIRES Retracker-Bliss-0.8.7.3 or higher software to support all the new features!
REQUIRES Unreal Engine 5 or higher

What's new

* In REtracker Bliss settings.text file you can set the camera number that will appear in LiveLink, recommend using 1 to 100
* You can track an AprilTag and use it to measure your nodal offset and sensor position, the tag tracking shows up as Camera 101 in LiveLink.
* Added an AprilTag actor that has a "tag size" you should set to the size of your printed tag
* Added an ArucoTag actor that has a "tag size" you should set to the size of your printed tag
* Added a skeletal 3d box reference object, you can set this to any size and have the pivot point be either in the center or the corner of the box
* Added an adjustable "measuring scale".  You can set the scale to any length in meters and have marks every 10cm.  You can also turn on text labels every 10cm and minor marks every cm.
* None of this should require any recompiling for UE 5

The only file you need is LiveLinkBliss_Release_3.zip, this includes the plugin, calibration actors, source code and a small sample project.  This was only slightly larger than the plugin by itself.

You DO NOT need the files labeled "Source Code" they archives of the repository and NOT READY TO USE
 
If you run into any problems or need a version for an older Unreal Engine, please post an issue [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues) to let me know what you need 

