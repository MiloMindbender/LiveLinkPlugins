# Trougbleshooting LiveLinkBliss

## I get a "plugin was not compiled for your version of unreal" message

If you see this message you may have opened the 5.0 project with unreal 5.1, make sure you are opening the project with the version of unreal it's made for.

If you have multiple versions of Unreal installed, the first time you might have to run the right version of Unreal first, then browse to the project to open it.  After this it should show up in the Epic Games Launcher.

If you keep getting the plugin build message please report it[here](https://github.com/MiloMindbender/LiveLinkPlugins/issues) be sure to give the version of unreal you are running and if you built Unreal from source.

## I see TWO cameras in LiveLink!

With the standard setup you may see Camera 1 and Camera 101 appear in the LiveLink window.

Camera 1 is your regular Bliss camera tracking like before
Camera 101 is Bliss tracking an AprilTag in the scene for Nodal Point Offset calibration.  If you are just using Bliss for camera tracking you can ignore this, it is ok for it to be yellow.

## When I add the LiveLinkBlis Source I don't see a Camera Subject or the light is yellow instead of green

This means you have the wrong communications settings, or you bliss app has stopped running for some reason.

Make sure the bliss app is running, showing confidence 100 and a FPS of around 500.  Also check the cmd window for any errors.

The IP/Port should be shown in the RETracker Bliss window.  "LiveLink Bliss" should be visible and have the IP address of the computer running unreal (127.0.0.1 if unreal is on the same machine as bliss).  The port number should also match the setup in Unreal LiveLink, the default is 50000

If the light next to Camera 1 is yellow, it means communication with the bliss app was working but isn't now.  This is usually caused by the Bliss app being closed or some kind of communications error like an unplugged or bad cable.  Make sure your Bliss app is still running and there are no errors in the cmd window.  You can try closing the bliss app, optionally power cycling the Bliss sensor and restarting Bliss.  If your bliss is running on a different computer, make sure communications between your computers is still working

## Camera 101 is showing yellow

If you are not trying to track an AprilTag to calibrate your Nodal Offset, this is normal.

If you have an AprilTag in the scene and visible to the Bliss, this should be green.  If it is not, check the RETracker Bliss app to see if your "Detected Tag" shows the number of your tag.  If it doesn't check these things

1. Is the tag in view of the Bliss camera, you can use "run world pose" to check this.
2. The Bliss settings.txt file may not be setup to send tag updates or is set to track the wrong tag number.
3. The AprilTag may be poorly lit or have glare or reflections on it.


