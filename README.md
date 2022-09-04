# LiveLink Plugins for Bliss tracker

This contains 4 different plugins, all of them are EXPERIMENTAL and may have bugs!!  No guarantees, don't use for production work and save frequently when using.

If you are an expert please, BE SURE to at least read the "How To Test" section below.

Requires Retracker-Bliss-0.9.3-LiveLink.exe Bliss app which is available from Marwan Rassi if you have a bliss sensor.  It does NOT work with the regular bliss app.

# The test app contains 3 LiveLink Plugins, use the right one!

When you click +source in LiveLink, you should see all of these.

## LiveLinkBliss Source (Use this!)

This is my second attempt at a LiveLink driver and appears to be working correctly.  Since there is no documentation on ANY of the LiveLink functions I had to start from something known working and guess at what changes were needed, this is based on the Epic FreeD LiveLink plugin.  As I go I'm trying to add documentation to the code so other people can learn how to write LiveLink drivers from it.

Documentation on how to setup is below

## BlissTracker  DO NOT USE

First attempt at LiveLink, didn't work right here only for my reference purposes only, don't use it.

This code will be removed when LiveLinkBliss is confirmed stable.

## LiveLinkFreeD  SAME AS UE5 FreeD

This is the Epic LiveLinkFreeD plugin from the unreal repo, here only for reference purposes, it is unchanged.  The test project builds with it instead of the one shipped with unreal.

This code will be removed when LiveLinkBliss is confirmed stable.

# UniversalRig

This is a plugin with some parts used to adjust the relationship between camera and tracker.  It is similar to the unreal "camera rig crain" and lets you setup tweakable relationships between the tracker and camera to represent the nodal offset.  Eventually this will be unnecessary as all this will be handled automatically by the UE lens file and calibrator.

Currently the test project doesn't use this, if I don't find a need for it this code will be removed.

# What LiveLinkBliss Source does

This brings the bliss data in through Unreal's LiveLink system.  Don't expect big differences, the goal here is to see if it is stable before we add features.  Let me know if you notice any improvements or problems compared to using Bliss over FreeD.

There is one new feature that changes the way time is handled, this can be turned on and off for testing.

Currently this only works with ONE bliss sensor, multiple sensors will be added soon.

# SETUP the test project

Since this is a test plugin, these instructions assume you are familiar with Unreal.  Eventually there will be a video tutorial

AT THE MOMENT, you may need a copy of Visual Studio (community edition is fine, and it's free) installed to build this.  When I get further along I will try to provide built versions so you won't need this.

This is built for Unreal Engine 5.0.3  It may work with older versions but I haven't tested it so you're on your own there.

To use this you need a copy of "Retracker-Bliss-0.9.3-LiveLink.exe", available from https://www.dropbox.com/s/dhzpba6a1vuli4i/Retracker-Bliss-0.9.3-livelink.exe?dl=0

You can use whatever RETracker settings file has worked for you in the past.  Make sure the render_osc_ip is set to the same address as renderer_freed_ip.  renderer_osc_port can be set to anything, the default setup is 7000.

Either get a zip of this repository or clone it to your computer with git.  The repo should have everything you need to test the livelink plugin, it's a full Unreal Engine 5 test project.

1. Get REtracker Bliss (telemetry version) running as you usually do.
2. Start unreal and load the project
3. If it asks you to build plugins, let it.
4. Go to the plugins menu in Unreal, you should see 4 plugins listed under "project" at the top.  Make sure they are all checked.
	BlissTracker, LiveLinkBliss, LiveLinkFreeD and UniversalRig
5. If it tells you to restart, do it.
6. Under Windows->Virtual Production open up LiveLink and TimedDataMonitor
7. In the LiveLink window, click +source" and choose "LiveLinkBliss Source"  If bliss is running on the same machine as unreal, the defaults should be right, if it is on a remote PC you will need to change it.  Be sure to use port 7000!  Press "add" when done.
8. If bliss is running you should see Camera 0 with a green light next to it.9. In the upper part of the livelink window, click on the bliss source.  Expand all the categories in the right hand window.  Turn off isValid for focal length and focus distance.
9. At the end of this window, set your "max number of frames to buffer" to 600 (you may be able to use less, start with this) and set "offset" to the tracking delay you usually use for bliss.
10. The project is currently setup for an AJA video input card, if you are using something set it up as a source in the way you usually do.  Remember to go into the media plate and set the media source there for your card.
11. Once your video source is setup and running you should see it and the bliss in the Timed Data Monitor with a green "synchronized" light.
12. Go into "CineCameraActor" and set this up to match your real world camera.  Sensor size, and focal length must match your lens.  You may also need to adjust focus and Aperture to get a clear image.
13. Put an apriltag on the floor close to the camera and do a worldpose in bliss.  After this if you look at 0010_comp in the UE project you should see your video source.  There will be a skeletal box with one corner over the center of the apriltag.  To the rigit of that you will see a CG apriltag and a bit further over a mannequin.  You may have to pan around to see this, your studio setup is probably different from mine.

The test project has an uncalibrated camera and no lens file.  To apply a nodal point offset, go into CineCameraActor's "details" click the camera component and put your offsets into the location and rotation section.  You can add a lens file to correct lens distortion and add a nodal point offset if you have one.  If your lens file contains nodal ponint data, be sure to go into CineCameraActor's "details" click the camera component and zero out the location and rotation section

# To use with an existing Unreal Virtual production project.

Go into your project folder and create a "Plugins" folder.  Copy the "LiveLinkBliss" folder from the Plugins folder of the sample project to your project's Plugins folder.  You should be able to use the same steps above to turn on the LiveLinkBliss plugin.  This should work with any project setup to use LiveLink, just remember to point the LiveLinkComponentController in your camera at the LiveLinkBliss source.

# What is different compared to Bliss on FreeD

When you use bliss with FreeD, errors or delays in your network can cause tracking problems like jumps or wiggles.  These are usually only visible if you are moving very slowly or if your network is having problems and losing or delaying data.  This is because Unreal uses the tracking data based on when it arrives, so lost or delayed data can cause problems.  This new LiveLink driver can evaluate the data based on the time it was SENT from Bliss which should reduce errors from networking delays.

Right now I'm not certain this LiveLink code is 100% correct because Epic publishes NO DOCUMENTATION on it.  Till Epic gets back to me with some details, I'm guessing on a few things.  So we need to test this code to see if it is an improvement.

This new LiveLink can handle Bliss data the old way, or use this new timing method.  Assuming I did things right, using the old timing method should work exactly the same as using Bliss through FreeD.

# How to test (read carefully!)

When you setup "LiveLinkBliss Source" it will be using the NEW way of handling time which hopefully will be better.  We need to test this to see if it really helps and that I've done it right.

Please let me know if you see any changes in how your Bliss works, good or bad.

## Switching between old and new timing

Click on the bliss source in the LiveLink window.  Expand the "source" area in the right hand window.  Under "Focus Distance Encoder Data" if IsValid is checked, you are using the new timing system.  Clear this box to test the old timing system.  If you have Unreal's "output log" window open you will see a message confirming which timing system is used.

Try using Bliss as you normally would, also try moving the camera very slowly and using a handheld camera.  Please let me know if you see any improvements or changes.  In "old" timing mode it should work the same as it did under FreeD.

>>IMPORTANT<< The "offset/time correction" (tracking delay) might be a little from the numbers you used with FreeD.  Be sure to check when using BlissLiveLink Source to make sure the delay is still right.  Delay MIGHT be different for the old/new timing too, so be sure to check it when you switch.

## Switching between LiveLinkBliss and FreeD

You can open the LiveLinkFreeD source and have it connected at the same time as LiveLinkBliss source.  Both should display a green light in the LiveLink window.

Once you have both open you can go into the LiveLinkComponentController in your camera and switch the subject between the freed and LiveLink source.

## Logging some test data for me

LiveLinkBliss Source can also log some data about your network that would be helpful for me to see.  Please capture a bit of it and send it to me.

Open up the Unreal Output Log window.

Click on the bliss source in the LiveLink window.  Expand the "source" area in the right hand window.  Under "User Defined Encoder Data" check IsValid, after about a second, uncheck it again.  In the unreal output window you should see a bunch of lines appear that look like this

LogLiveLinkBliss: Warning: LiveLinkBlissSource: Sensor Time 3141.635328 sensordelta 0.001280 hostdelta 0.000000 unrealdelta 0.001531 delivery jitter -0.000251

Please copy about a dozen lines of this output and send them to me.

# Coming soon

Instructions on how to use an Unreal Lens file.

I will also be adding information on how to setup your worldpose, floor position and tracking center using an Aruco tag.

We are also working on support to do a full lens calibration including finding the nodal point offset using Unreal.  







