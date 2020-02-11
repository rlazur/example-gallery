# example-gallery
An example of gallery script from an analysis I did on ICARUS and SBND art-root files. The purpose of this repository is to provide some (hopefully) clear instructions/reference to getting started with gallery in the Fermilab GPVMs. There is a disturbing dearth of gallery resources online, hopefully this puts a few drops into the void of gallery resources.

# Instructions

In this directory there should be
 1. gallery c++ script (example_gallery.cpp)
 2. CMake file (CMakelists.txt)
 3. setup bash script (setupgallery.sh)

## 1. Setup environment on gpvm
If on the gpvm, first set up your environment so that you have access to art, larsoft, canvas, etc. resources; the setup.sh script should include these statements. You'll need to modify the contents to reflect your experiment-specific commands
   ```
   source setup.sh
   ```
## 2. Get 'compilable' gallery script
Okay, now get your gallery script in working order. Basically get it in a position where it will simply compile. This can be as simple as having include statements, opening some output file, filling it with trash, then closing it.

## 3. First compile/build
This is a multistep process of course.
First modify your CMake file. This basically means replacing every instance of "example_gallery" with <your gallery script name>. i.e.
  ```
  sed -e 's/example_gallery/new_gallery_name/g' CMakelists.txt
  ```
CMake is actually magic, I have no idea how any of the rest of it works so I wouldn't recommend touching anything else unless you run into compatability issues in the next few steps. This CMake file works on the SBND and ICARUS gpvms in late 2019 (icarus/sbndcode v08_##_##, gallery v1_13_01, cmake v3_10_01, -q e17:prof). What CMake does is build another file that you will then use to build your gallery code..

Next make a build directory to populate; i.e.
  ```
   mkdir build
   cd build
   ```

Now invoke CMake
```
  cmake ..
  ```

If all went well then a bunch of stuff should have shown up in your build directory. One of those things being the MakeFile. Now we use the MakeFile to compile/build the gallery script. Do this command in the build directory
  ```
  make
  ```
you'll have either succesfully compiled your gallery script, or gotten some error reports that you can follow and fix the gallery script. Once you succesfully compile your gallery script then a NEW file should appear in your build directory. That file should have the same name as <your gallery script> except without the .cpp at the end. This is an executable. To run the executable do this command
  ```
   ./<your executable name>
  ```

boom...
