# LiveLink Plugins for Bliss tracker

This contains 4 different plugins, all of them are beta for test purposes only!!

You should not use any of this in a production project or even as a reference on how to write a good LiveLink plugin as nothing here is finished and may have bad bugs!

This requires the "telemetry" version of the Bliss app which is available from Marwan Rassi if you already have a bliss sensor.  It does NOT work with the regular bliss app.

# LiveLinkBliss Source

This is my second attempt at a LiveLink driver and appears to be working correctly.  Since there is no documentation on ANY of the LiveLink functions I had to start from something known working and guess at what changes were needed, this is based on the Epic FreeD LiveLink plugin.  As I go I'm trying to add documentation to the code so other people can learn how to write LiveLink drivers from it.

Documentation on how to setup is below

# BlissTracker  DO NOT USE

First attempt at LiveLink, didn't work right here only for reference purposes only.

This code will be removed when LiveLinkBliss is confirmed stable.

# LiveLinkFreeDE  SAME AS UE5 FreeD

This is the Epic LiveLinkFreeD plugin from the unreal repo, here only for reference purposes, it is unchanged.  The test project builds with it instead of the one shipped with unreal.

This code will be removed when LiveLinkBliss is confirmed stable.

# UniversalRig

This is a plugin with some parts used to adjust the relationship between camera and tracker.  It is similar to the unreal "camera rig crain" and lets you setup tweakable relationships between the tracker and camera to represent the nodal offset.  Eventually this will be unnecessary as all this will be handled automatically by the UE lens file and calibrator.

Currently the test project doesn't use this, if I don't find a need for it this code will be removed.

# What this does

This brings the bliss data in through Unreal's LiveLink system.  This version has the same features as using LiveLinkFreeD so right now we are just testing to make sure BlissLiveLink is stable.  Let me know if you notice any differences in the way it works.

In the near future we will add Time Stamps to the plugin which should allow it to keep Bliss data in better sync with video if it's being sent over longer networks or WiFi.  This may also include some kind of timecode support and other features.

Currently this only works with ONE bliss sensor, multiple sensors will be added soon.

# SETUP the test project

Since this is a test plugin, these instructions assume you are fairly familiar with Unreal.  Eventually there will be a video tutorial


AT THE MOMENT, you may need a copy of Visual Studio (community edition is fine, and it's free) installed to build this.  When I get further along I will try to provide built versions so you won't need this.

Currently this is built for Unreal Engine 5.0.3  It may work with older versions but I haven't tested it so you're on your own there.

First, setup your bliss and the Bliss app.  For now I recommend slowing the Bliss frame rate to 250hz by setting freed_timing_rate=0.003 in the settings.txt file.

Either get a zip of this repository or clone it to your computer with git.  The repo should have everything you need to test the livelink plugin, it's a full Unreal Engine 5 test project.

1. Get REtracker Bliss (telemetry version) running as you usually do.
2. Start unreal and load the project
3. If it asks you to build plugins, let it.
4. Go to the plugins menu in Unreal, you should see 4 plugins listed under "project" at the top.  Make sure they are all checked.
	BlissTracker, LiveLinkBliss, LiveLinkFreeD and UniversalRig
5. If it tells you to restart, do it.
6. Under Windows->Virtual Production open up LiveLink and TimedDataMonitor
7. In the LiveLink window, click +source" and choose "LiveLinkBliss Source"  If bliss is running on the same machine as unreal, the defaults should be right, if it is on a remote PC you will need to change it.  Be sure to use port 7000!  Press "add" when done.
8. If bliss is running you should see Camera 0 with a green light next to it.
9. In the upper part of the livelink window, click on the bliss source.  Expand all the categories in the right hand window.  Turn off isValid for focal length and focus distance.
10. At the end of this window, set your "max number of frames to buffer" to 600 (you may be able to use less, start with this) and set "offset" to the tracking delay you usually use for bliss.
11. The project is currently setup for an AJA video input card, if you are using something set it up as a source in the way you usually do.  Remember to go into the media plate and set the media source there for your card.
12. Once your video source is setup and running you should see it and the bliss in the Timed Data Monitor with a green "synchronized" light.
13. Go into "CineCameraActor" and set this up to match your real world camera.  Sensor size, and focal length must match your lens.  You may also need to adjust focus and Aperture to get a clear image.
14. Put an apriltag on the floor close to the camera and do a worldpose in bliss.  After this if you look at 0010_comp in the UE project you should see your video source.  There will be a skeletal box with one corner over the center of the apriltag.  To the rigit of that you will see a CG apriltag and a bit further over a mannequin.  You may have to pan around to see this, your studio setup is probably different from mine.
15. At this point you are running with an uncalibrated camera.  To apply a nodal point offset, go into CineCameraActor's "details" click the camera component and put your offsets into the location and rotation section.


# Coming soon

The project is structured so it can be used with an Unreal lens file.  I will add instructions on how to do this fairly soon, I have to test it.

If you have a lens file you can add it to the camera and it should work.  If your lens file contains nodal point information, remember to go into CineCameraActor's "details" click the camera component and zero out the location and rotation in the.

I will also be adding information on how to setup your worldpose, floor position and tracking center using an Aruco tag.

We are also working on support to do a full lens calibration including finding the nodal point offset using Unreal.  







