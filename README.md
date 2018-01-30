# UnrealMosquitto

A [MQTT](http://mqtt.org) client with blueprint support for [Unreal Engine 4](http://unrealengine.com), based on [Mosquitto](http://mosquitto.org/).

![unrealmosquittoscreenshot](https://user-images.githubusercontent.com/45740/35557501-fd70a518-05ad-11e8-92bc-3dcf32978e2b.png)

# How to install

Because this has not been developed as a plugin, the installation process is slightly very cumbersome. Only Windows x64 is supported so far.

 0. Make sure your project is set-up for C++. If it's not already the case, add a simple C++ Actor in Unreal that you could remove later.
 1. Add the `ThirdParty` folder to the UnrealProject.
 2. Edit your project's build file, named `YourProjectName.Build.cs` in the `Source/YourProjectName` folder. The changes are based on the `Source/ProjectName/ProjectName.Build.cs` file.
  1. Add the `using System.IO;`
  2. Add the two new methods `ThirdPartyPath` and `LoadThirdPartyDLL`.
  3. Add the two `LoadThirdPartyDLL` at the end of the constructor.
 3. Add the source files in the `Source/ProjectName/Private` and `Source/ProjectName/Public` folders to your respective `Source/YourProjectName/Private` and `Source/YourProjectName/Public` folders.
 4. In these files, replace all occurences of `ProjectName` by `YourProjectName` and `PROJECTNAME_API` by `YOURPROJECTNAME_API`.
 5. Finally, add the `mosquitto.dll` and `mosquittopp.dll` in the `Binaries/Win64` folder. You can find these two DLLs in the `ThirdParty` subfolders, but you will have to to remove `.x64` from their pathname.
 6. Right-click on `YourProject.uproject` file,  and select `Generate Visual Studio Project files`.
 7. Open `YourProject.sln` and compile everything.
 8. (Re)Start the Unreal Engine Editor.
 
# How to use

To use the MQTT client, the simplest method is to create a blueprint child class from it.

 1. Find the C++ Unreal Mosquitto class in the content browser.
 2. Right click and select `Create Blueprint class based on UnrealMosquitto`.
 
 This new child class has access to the events in its event graph. You can also configure the client in the MQTT section in the details panel of the root element. The router can dispatch executions flows based on MQTT topics.
 
 
