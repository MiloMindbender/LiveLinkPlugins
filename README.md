# LiveLink Plugins for Bliss tracker and Unreal Engine

This plugin brings Bliss tracking data into Unreal's LiveLink system.  To use it you need a Bliss tracker from [Rassi Engineering LTD](https://www.retracker.co/) 

I am not an employee of Bliss/Rassi Engineering LTD.  I developed this Plugin to learn LiveLink which is a mystery to many people because Epic/Unreal has almost no documentation on it.  These plugins come with no warrantees or guarantees expressed or implied. You should try them and decide for yourself if they work well enough for your purposes.  Please report ANY problems to the github issues page [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues}

The full source code is available for free to anyone who wants to use it for any purpose, more info on that[here](README_Full_Install.md)

# Quick Install (DO NOT use the green CODE button)

This assumes you have an existing Virtual Production project of your own or know how to create one.  Later I will have a full video tutorial and sample unreal project showing how to set up a project from scratch.

Please go to the [releases page here](https://github.com/MiloMindbender/LiveLinkPlugins/releases) and get the latest "prebuilt" plugin for the version of unreal you are using.  If you need C++ source code for some reason, read [this](README_Full_Install.md) but most people will only need the "prebuilt" one.  If, at any time, you get a message saying the plugin was not compiled for your version of unreal, jump down to the "install problems" section.

!. To install in your own project, go to the project folder and create a new folder called "Plugins"
2. Unzip the "prebuilt" zip file into the plugins folder.  This will create a "LiveLinkBliss" folder inside "Plugins"
3. Launch Unreal  
4. Go to Edit->Plugins, search for LiveLinkBliss and check the box next to it to turn it on
5. Also search for "LiveLink" and "TimedDataMonitor" make sure they are checked also.
6. If unreal has asked you to restart, do that now.
7. If the Bliss app is not already running, please start it now.
8. Open up the Live Link and Timed Data Monitor windows in Unreal.
9. In the LiveLink window, click +source" and choose "LiveLinkBliss Source"  If Bliss is running on the same machine as unreal, the defaults should be right.  If bliss is running on a different machine or you have changed the bliss settings, you will need to enter the IP/port here.  For Bliss 0.8.5 or newer, the correct setting is shown on the app screen as "LiveLink Bliss"  For earlier versions of bliss, use port 7000. Press "add" when done.
9. You should see Camera 1 with a green light next to it in the LiveLink window.  In the upper part of the LiveLink window under "Source Type" click on Bliss.
10. In the right hand window, expand the "Buffer - Settings".  For now set "Max Number Of Frames to Buffered" to 500 and offset to .14 Later you will need to adjust the offset till the tracking and live camera are in sync.
11. Your existing Virtual Production project should already have a CineCameraActor setup in it that matche your real camera.  Find this in the world outliner and click it.
12. If your CineCameraActor does not already contain a LiveLinkControlerComponent, click the +add button and add one
13. Make sure the "subject representation" in the LiveLinkControllerComponent is pointed at your "Camera 1" LiveLinkBliss source.

At this point you should be able to move your camera in the real world and see it moving in Unreal.  

PLEASE report any problems [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues}

# How to test

It's possible to be run Bliss over FreeD (the old way) and through LiveLinkBliss AT THE SAME TIME.  You want to make sure you are using LiveLinkBliss.  The easiest way to do this is to go to your LiveLink window, if you see any "Source Type" FreeD, just close them so only Bliss sources remain.  If you're CineCameraActor stops tracking, make sure it's livelink component "Subject representation" is pointed at the bliss source.

If you want, you can leave FreeD and LiveLinkBliss running at the same time and switch between them by changing the "subject representation" in the livelink component of your CineCameraActor.

Test this plugin by using Bliss as you normally would.  Try moving the camera at different speeds from fast to very slow, even shaking it.  Try it on a tripod, motion control rig or handheld.  You should not see any jumps or unwanted jiggle as the camera moves.  If you move the camera suddenly and the CG and Video get out of sync, then line back up when you stop, the "offset" time needs to be adjusted to sync them up.  The offset with this plugin may be a little different than it was for Bliss on FreeD.

If you notice real objects and the CG ones are not staying aligned (foot slipping) as you pan the camera, this is an issue with your lens calibration and nodal offset, not Bliss or the plugin.

# Contact Us

For Virtual Production Tutorials, and updates [subscribe to my youtube channel.](https://www.youtube.com/user/GregCorson)

For issues with this LiveLink plugin, report them [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues}

For more iinformation on the Bliss tracker and to contact it's developer [Rassi Engineering LTD](https://www.retracker.co/)

You can also talk to Bliss users on [Discord](https://discord.gg/SsftMDMZfJ)

# Install Problems / Troubleshooting

## If you get a "plugin was not compiled for your version of unreal" message

If you get this error, please report it [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues} and be sure to include the version of Unreal you are running (ie: 5.0.3) and whether or not you built Unreal from source.  If you are running 5.0.3 you should NOT get this error, so be sure to report it.

The error usually means you are running some version of unreal other than 5.0.3.  I will be making prebuilt plugins available for other versions of unreal if you ask.  Also it is fairly easy to rebuild the plugin yourself, see "compiling yourself" below for more info.

## When I add the LiveLinkBlis Source I don't see a Camera 1 Subject or the light is yellow instead of green

This means you have the wrong communications settings, or you bliss app has stopped running for some reason.

Make sure the bliss app is running, showing confidence 100 and a FPS of around 500.  Also check the cmd window for any errors.

If you are running Bliss App 0.8.5 or newer, the correct IP/Port should be shown next to "LiveLink Bliss".  For older versions of Bliss the IP and port will be whatever is shown next to "OSC"  If your bliss app is older than 0.8.5 you should probably update it.  You need at least bliss 0.8.4 with "livelink" in it's name.

If the light next to Camera 1 is yellow, it means communication with the bliss app was working but isn't now.  This is usually caused by the Bliss app being closed or some kind of communications error like an unplugged or bad cable.  Make sure your Bliss app is still running and there are no errors in the cmd window.  You can try closing the bliss app, optionally power cycling the Bliss sensor and restarting Bliss.  If your bliss is running on a different computer, make sure communications between your computers is still working

# Compiling it yourself

You DON'T NEED TO DO THIS unless your unreal is complaining that the plugin "was not compiled for your version of unreal".  If this happens please let me know [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues}

I will try to get a version built for you, but you can also try building it yourself.  It is easy and completely automatic if you have the right software installed.  It only takes a couple of minutes.

## What you need to build

Get the "code only" zip file, follow steps 1 and 2 above to install it in your project.

You also need to install Microsoft Visual Studio.  If you don't have it, the "community edition" is free from Microsoft.  When you do the install, make sure the "workloads" for "desktop develpment with C++" and "game development with C++" are checked in the installer.  It may take awhile to download this.

## How to build

Once Visual Studio and the "code only" plugin is installed in your Unreal project, just start Unreal.  You should get the "plugin was not compiled for your version of unreal" message.  Click the box to tell it to build the plugin.  It may take a minute or two to build and there won't be any prograss dialog on the screen while it is happening, then Unreal should launch.

If you don't get the compile warning message when you launch unreal, go into edit->Plugins, find the bliss plugin and make sure it is checked.  When you restart unreal it should ask you to do a build.

If this process doesn't work for you, please tell me [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues}

# Coming Soon

Availibility from the Epic/Unreal marketplace for easier installs

Video tutorial on how to setup a Virtual Production project

A ready-to-use Unreal sample project.