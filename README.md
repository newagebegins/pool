Circle collision physics, based on the article [Pool Hall Lessons: Fast, Accurate Collision Detection Between Circles or Spheres by Joe van den Heuvel, Miles Jackson](http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php).

Building (Windows): run `build.bat` from a command prompt, `main.exe` will appear in the `build` directory.

NOTE: Visual Studio's `cl.exe` must be available in the command prompt in order to build. For example, if VS2015 is installed you can:
* run `"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64` in any `cmd.exe` shell to make `cl.exe` available;
* run "Developer Command Prompt for VS2015" via Start menu and run `build.bat` inside it.

Demo GIF:  
![Demo GIF](demo.gif)
