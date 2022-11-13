# LiveLink plugins for Unreal and Bliss tracker, repository doc

If you are a regular Bliss user YOU DO NOT NEED TO READ THIS, everything you need to use the plugin is on the [releases page](https://github.com/MiloMindbender/LiveLinkPlugins/releases) with documentation on [the main page](https://github.com/MiloMindbender/LiveLinkPlugins).  

You REALLY DO NOT want to build/install everything from the repository yourself unless you are a coder or experimenter looking to see how a LiveLink plugin is made.

If you do a clone/zip download of the repository from the green CODE button on the main github page, you are getting an copy of work-in-progress which may not be stable or even usable, so I advise you always get the more stable code on the releases page unless you are trying to contribute to the project.

# For Unreal Engine 5.0.3

This project and the included blueprint actors were built for Unreal 5.0.3 and higher.  I would recommend switching to Unreal 5 as many bad bugs and performance issues have been solved. 

If you really need this plugin on Unreal Engine 4, you can rebuild it for that, I have reports of it working on 4.26 and 4.27.

Eventually I expect to have this available on the Epic/Unreal marketplace so it will be easier to install.

# The calibration assets are only available for UE5

The special Aruco, AprilTag and other blueprints are only available for UE5.  If you need them for UE 4 they will need to be re-created by hand.  Please let me know if you need them.

# What you need to rebuild the plugin from scratch

To build the plugin you need any version of Microsoft Visual Studio, 2019 or higher.  The free "Community Edition" should work fine. Check the Visual Studio installer and make sure you have the "workloads" for "desktop develpment with C++" and "game development with C++" or the build may fail.

You can build the plugin from either the repository (green CODE button) or from the zip on the [releases page](https://github.com/MiloMindbender/LiveLinkPlugins/releases)

# Don't use the included FreeD plugin.

The repository contains the LiveLink FreeD plugin source code from Epic.  This is just here for reference and should not be used as it may be out of date.

# Automatically building the plugin

1. Have your own project setup in whatever version of Unreal Engine you are using
2. Create a Plugins folder in your project if it doesn't exist already
3. Go to Plugins/LiveLinkBliss in your checkout of the repository and copy this entire folder to the Plugins file you just made in your project.
4. Start your project, you should get a message saying a plugin was not built for your version of unreal, tell it to build.
5. After a short delay, Unreal will rebuild the plugin and launch your project with it installed.

# Full Developer Build

If the above process fails or you are just a coder/experimenter who wants to use this as an example for how to write/debug live link plugins, you can build the whole thing from scratch.  The project is the one I actually use to develop and test this plugin, everything is there you need to run and debug it in Visual Studio.

1. Have my entire repository on your computer, you also need to have Unreal installed but you don't need Unreal source code.
2. Right click the LiveLinkPlugins.uproject file and choose "generate visual studio project files" 
3. You should now have a LiveLinkPlugins.sln file you can open in Visual Studio

From this point you can edit, build, run and debug the whole project in Visual Studio.  When you Run the project from inside Visual Studio it will start your installed version of Unreal with my plugin code installed, ready for debugging.  You can use the debugger to stop the code and examine how it works.

The built version of the plugin will be in plugins/LiveLinkBliss if you want to copy it to another project.



