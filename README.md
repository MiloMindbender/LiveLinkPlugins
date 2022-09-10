# LiveLink Plugins for Bliss tracker and Unreal Engine 5

These plugins are experimental and have NOT been heavily tested.  No guarantees, save frequently when using.

Please AT LEAST read the release section below when you update, how things work will be changing

This is an active development repository, The releases section here https://github.com/MiloMindbender/LiveLinkPlugins/releases/tag/release_2 has a zip with all the source code and a test project.   There is also a zip with the plugin only.

# RELEASE 2 changes

* Shows up as Camera 1 instead of zero.  You will need to change your LiveLinkSubjest in the camera.
* The way of switching time stamps and debug data on and off has changed.  It's now set in LiveLinkBliss source->default config.
* FocusDistance, focal length and user defined encoder data now do nothing.
* Added the Bliss icon to the plugin
* Updated/shortened documents
* Changes and cleanup of the code and project

# For Unreal Engine 5.0.3

It may work on other versions but I haven't tested them.  I have reports it works for 4.27 if you build it yourself.

# To build it requires Microsoft Visual Studio Community Edition (free)

I am working on doing releases that will not require this, but for now you need to have Microsoft Visual Studio Community Edition to build it.  The 2022 version is best, "paid" versions of Visual studio also work.  Check the Visual Studio installer and make sure you have the "workloads" for "desktop develpment with C++" and "game development with C++" or the build may fail.  Please let me know if you have trouble.

# Requires the right Bliss app

Requires Retracker-Bliss-0.8.4.exe Bliss available from here https://www.dropbox.com/s/ixz3tad4ujhrjus/Retracker-Bliss-0.8.4.exe?dl=0app It does NOT work with the original bliss app.

# This contains 3 LiveLink Plugins, use the right one!

When you click +source in LiveLink you will see 3 different LiveLink sources.  Please ONLY use LiveLinkBliss Source, the others are reference code for me and do not work.

# UniversalRig

This is a plugin used to adjust the relationship between camera and tracker.  At the moment it is not required to use the LiveLink plugin or the sample project.

# What LiveLinkBliss Source does

This LiveLink plugin uses a time stamp generated on the Bliss sensor.  Unreal will correct timing errors that can occur when tracking is sent over a network.  If the Bliss app is running on a different machine than unreal and communicating over wired ethernet or WiFi this should avoid errors due to delayed or lost packets.  If you run the Bliss app on the SAME machine as Unreal, you may not notice any change.

The time stamp feature can be turned on and off so you can see if it makes a difference in your tracking.

Currently only supports ONE Bliss sensor.

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

Go into your project folder and create a "Plugins" folder.  Copy the "LiveLinkBliss" folder from the Plugins folder of the sample project to your project's Plugins folder.  Go to settings->plugins and make sure LiveLinkBliss is turned on.  From this point LiveLinkBliss should function about the same as the old LiveLinkFreeD you had in your project for Bliss before.

# How to test (read carefully!)

When you setup "LiveLinkBliss Source" it will be using the NEW way of handling time which should be better.  We need to test this to see if it works better than the old way.

Please report any changes in how your Bliss works, good or bad.

## Switching between old and new timing

If you want to compare using time stamps (new way) to not using them (old way), here is how to switch.

Click on the Bliss source in the LiveLink window.  Expand the "source" area in the right hand window.  Under "Source" Default Config is near the top.  Set this to "Bliss" to get the latest version which uses time stamps.  Select "Bliss No Time Stamps" for the old version without them.

## Tests to try

Use Bliss as you normally would, try moving the camera fast, very slow and using a handheld camera.  If you want you can compare timestamps ON and timestamps OFF.  Please let me know if you see any improvements or problems when you have the new timing mode turned on.

You may NOT notice any difference, it will depend on how fast and how busy your network is, and if you are running Bliss and Unreal on the same computer.

--IMPORTANT-- The "offset/time correction" (tracking delay) might be a little different from the numbers you used with Bliss and FreeD.  Be sure to re-check your delay to make sure it is still right.

## Switching between LiveLinkBliss and FreeD

Bliss still sends LiveLink and FreeD data, so you can open the LiveLinkFreeD source and have it connected at the same time as LiveLinkBliss source.  Both should display a green light in the LiveLink window.

Once you have both open you can go into the LiveLinkComponentController in your camera and switch the subject between the freed and LiveLink source.

## Logging some test data for me

LiveLinkBliss Source can also log some data about your network that would be helpful for me to see.  Please capture a bit of it and send it to me.

Open up the Unreal Output Log window.

Click on the bliss source in the LiveLink window.  Expand the "source" area in the right hand window.  Under "Default Config" select "Bliss Print Statistics" briefly, then switch back to "Bliss"  In the unreal output window you should see a bunch of lines appear that look like this

LogLiveLinkBliss: Warning: LiveLinkBlissSource: Sensor Time 3141.635328 sensordelta 0.001280 hostdelta 0.000000 unrealdelta 0.001531 delivery jitter -0.000251

Please copy about a dozen lines of this output and send them to me.

# Coming soon

Instructions on how to use an Unreal Lens file.

I will also be adding information on how to setup your worldpose, floor position and tracking center using an Aruco tag.








