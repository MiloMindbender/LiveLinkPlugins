How to use a lens file

The sample project can be used with or without a lens file and will work if the lens file contains only distortion information.

You do not need a tracker to calibrate lens distortion, so this can work for any camera setup.

You can find a video tutorial on this here https://youtu.be/H5t2BwUGqkI but please read the instructions below for new information.

# Tips

* Larger checkerboards with larger squares give the best results.  I am using a board with 5x7 squares that are 5.15 cm squares.
* Sice of the squares is IMPORTANT, do not trust your printer, get a good ruler and try to measure the size to fractions of a mm.
* From the point of view of your camera, make sure the board is well lit and doesn't have any shiny glares.
* Make sure the board is FLAT, without wrinkles.  I've attached mine to a sheet of 1/8'th plywood using binder clips.  Gluing it to art/foam board works well to.
* Put the board on something that won't move and have your camera on a tripod also.
* You need to be able to position the board facing the camera and also be able to turn it left and right 45 degrees.


# Setup

* Start by going through the setup portion of lens tutorial video, it is for UE 4.7 but UE5 works the same, it just looks slightly different in places.
* If you are not using VIVE, skip the parts about setting up vive trackers.  You just need the camera, checkerboard and lens file.
* After creating your CineCameraActor, create an actor called CameraParent and attach the CineCameraActor to it.  Zero out the location and rotation values in both of these actors after attaching them.

Before you start taking lens calibration images, check the "show coverage overlay" this will let you see where you have shot pictures of the board, you want these to cover the entire area of the lens.

Focus your camera on the board, then turn autofocus off.  Also be sure to turn off any stabalization/steadyshot features.




