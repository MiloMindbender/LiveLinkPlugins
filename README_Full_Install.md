# LiveLink plugins for Unreal and Bliss tracker, repository doc

Please note, I am not an employee of Bliss/Rassi Engineering LTD.  I developed this Plugin to learn LiveLink which is a mystery to many people because Epic/Unreal has almost no documentation on it.  The full source code is available for free to anyone who wants to use it for any purpose.  There are no warrantees or guarantees expressed or implied.

You do REALLY DO NOT want to build/install this from a full repository checkout/clone unless you are interested in messing with the source code.  MOST USERS DO NOT WANT THIS and should get the prebuilt versions from [releases page here](https://github.com/MiloMindbender/LiveLinkPlugins/releases), see the quick install docs [here](README.md)

On the [releases page](https://github.com/MiloMindbender/LiveLinkPlugins/releases) there are several ZIP files...here is what they are.

* The ones labeled "prebuilt" are binaries only built for specific versions of Unreal and ready to use.
* The ones labeled "code only" have ONLY the bliss plugin code by itself.
* The ones labeled just "source code zip" are the complete repository with all C++ code and the Unreal project used for development and testing of these plugins

All the zips on the releases page were made when the repository was at a point that was reasonablly stable and usable, most people will want to grab these.

If you do a clone/zip download of the repository from the green button on the main github page, you are getting an copy of work-in-progress which may not be stable or even usable, so I advise you always get the more stable code on the releases page unless you are trying to contribute to the project.

# For Unreal Engine 5.0.3

This project was originally authored for Unreal 5.0.3.  I am not sure what will happen if you try to convert the full C++ development project to something older than Unreal 5, however the plugins themselves can be built for earlier versions.  I have reports that they work on 4.26 and 4.27

Eventually I expect to have this available on the Epic/Unreal marketplace so it will be easier to install.

# To build it requires Microsoft Visual Studio Community Edition (free)

To build the C++ project you need at least the free Microsoft Visual Studio Community Edition.  The 2022 version is best, "paid" versions of Visual studio also work.  Check the Visual Studio installer and make sure you have the "workloads" for "desktop develpment with C++" and "game development with C++" or the build may fail.  Please let me know if you have trouble.

# Requires the right Bliss app

Requires Retracker-Bliss-0.8.4.exe Bliss available from here https://www.dropbox.com/s/ixz3tad4ujhrjus/Retracker-Bliss-0.8.4.exe?dl=0app It does NOT work with the original bliss app.

# This contains multiple plugins as test and example code

Inside this project there are several different plugin sources.  ONLY the LiveLinkBliss Source works with the Bliss tracker, the others are reference or test code for me and may not be fully functional.

# SETUP the test project

Since this is a test plugin with full source code, these instructions assume you are familiar with Unreal.  Eventually there will be a video tutorial and an easier to install version.

Either get a zip of this repository or clone it to your computer with git.  The repo should have everything you need to test the livelink plugin, it's a full Unreal Engine 5 test project.

The project has a setup with a lens distortion file, nodal offset and Virtual FIZ object so you can see how to hook them up.  These are for my camera and WILL BE WRONG for yours! Either supply your own or disconnect them.

1. Get REtracker Bliss (telemetry version) running as you usually do. Use the same RETracker settings file you usually do.  Make sure the render_osc_ip is set to the same address as renderer_freed_ip.  renderer_osc_port can be set to anything, the default setup is 7000.
2. Start unreal and load the sample project
3. If Unreal asks you to build plugins, say yes.  It may take a minute or two, it will appear nothing is happening.
4. Go to the plugins menu in Unreal, you should see 4 plugins listed under "project" at the top.  BlissTracker, LiveLinkBliss, LiveLinkFreeD and UniversalRig.  Only LiveLinkBliss needs to be checked.
5. If Unreal tells you to restart, do it.
6. Under Windows->Virtual Production open up LiveLink and TimedDataMonitor
7. In the LiveLink window, click +source" and choose "LiveLinkBliss Source"  If Bliss is running on the same machine as unreal, the defaults should be right, if it is on a remote PC you will need to change it.  The default port is 7000 unless you changed it in the Bliss settings file.  Press "add" when done.
8. If Bliss is running you should see Camera 1 with a green light next to it. In the upper part of the livelink window, click on the Bliss source.  Expand all the categories in the right hand window.
9. Scroll down in this window, and set your "max number of frames to buffer" to 500.  The MINIMUM setting for this is your tracking delay multiplied by 500 plus about 25 for safety. It does not hurt to set it a bit high.
10. The project is currently setup for an AJA video input card, if you are using something else set it up as a source in the way you usually do.  Remember to go into the media plate and set the media source there for your card.
11. Once your video source is setup and running you should see it and the Bliss in the Timed Data Monitor with a green "synchronized" light.
12. Go into "CineCameraActor" and set this up to match your real world camera.  Sensor size, and focal length must match your lens.  You may also need to adjust focus and Aperture to get a clear image.  If you aren't using a lens distortion file, disconnect it in the LiveLinkComponentController.
13. Put an apriltag on the floor close to the camera and do a worldpose in Bliss.  After this if you look at 0010_comp in the UE project you should see your video source.  There will be a skeletal box with one corner over the center of the apriltag.  To the right of that you will see a CG apriltag and a bit further over a mannequin.  You may have to pan around to see this, your studio setup is probably different from mine.

The test project has a lens distortion file and is setup for my camera's nodal offset.  If you already have a lens distortion file with an offset, just copy it into this project, go to the CineCameraActor's LiveLinkComponentController and change it to your file.

# To use with an existing Unreal Virtual production project.

After you have built the whole project, you can go into the Plugins folder and install LiveLinkBliss into any other project using the instructions {here](README.md)




