# RETracker Bliss Installation Instructions

Everything is in one small file, you should be able to use it without doing any compiling but the source code is there if you need it.

A video tutorial on how to do nodal point calibration will be available soon.  Please subscribe to [my YouTube Channel](https://www.youtube.com/user/GregCorson) to be notified!

## Download the Requirements 

Download the latest LiveLinkBliss_Release.zip from the [releases page here](https://github.com/MiloMindbender/LiveLinkPlugins/releases)

You should already have Unreal Engine 5, RETracker Bliss Hardware and Retracker-Bliss-0.8.7.3 or higher software

## Print the Calibration images

The zip includes some calibration images as PDF files you can print out if you don't have them already. Print these as large as possible and measure their size as accurately as you can.  Measure only the black part of the tags.  The AprilTag PDF contains tags 0-19, you only need to print out tag #1.

# Install and Setup Everything

You should NOT see messages about things needind to be compiled, if you do please report it [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues)

Unzip LiveLinkBliss_Release_3.zip to a convenient location.  That's it!

## Setup sample project for use

You need to setup your video input, the sample is setup for my AJA Kona HDMI, your setup will be different.  Make sure your source is connected to the media_plate in the sample and you will be ready to do a lens calibration with the sample.

Also your Bliss needs to be setup to send LiveLink tracking to your machine running unreal.  The default setup is to use port 50000, see your Bliss documentation on how to set this up.  There are some new things in Bliss-0.8.7.3

## Add Plugins into your own projects

1. In your own project, create a folder called "Plugins" if it isn't already there
2. Go to the LiveLinkBliss_Release_3 folder and copy Plugins/LiveLinkBliss to your project's Plugins folder
3. When you launch your project, go to Edit->Plugins, find the Bliss plugin and turn it on.

## Add calibration Actors to your own projects

The sample project contains some actors used for lens calibration.  Open Maps/AssetExamples to see examples of them. They are useful for aligning and measuring things so you might want to put them in your own projects.  In the sample project these are in the AprilTag, ArucoTag and VPStudio core folders.  You can use the unreal "migrate" function to move any of these to your own projects, or just copy those three folders into your own project.

# I see TWO cameras in LiveLink!

With the standard setup you may see Camera 1 and Camera 101 appear in the LiveLink window.

Camera 1 is your regular Bliss camera tracking like before
Camera 101 is Bliss tracking an AprilTag in the scene for Nodal Point Offset calibration.  If you are just using Bliss for camera tracking you can ignore this, it is ok for it to be yellow.

# How to use

There are many tutorials on [my YouTube Channel](https://www.youtube.com/user/GregCorson)

This one shows how to setup a [Virtual Production project from scratch](https://youtu.be/ebuLqk85VTQ)
A new tutorial on how to do the nodal point offset measurement will be available soon!

PLEASE report any problems [here](https://github.com/MiloMindbender/LiveLinkPlugins/issues)
