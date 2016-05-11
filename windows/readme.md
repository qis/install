# Windows
Install *Windows 10*. Disable all optional settings. It is a good idea to chose a single word username
and rename the user later to have a `%UserProfile%` path and MSYS2 username without spaces.

All command prompt scripts must be executed with Administrator privileges unless stated otherwise.


## Windows Updates
Disable automatic updates (optional).

```
gpedit.msc
+ Computer Configuration
  + Administrative Templates
    + Windows Components
      + Windows Update
        + Configure Automatic Updates
          Enabled
          Configure automatic updating: 2 - Notify for download and notify for install
          [ ] Install during automatic maintenance
          Scheduled install day: 7 - Every Saturday
          Scheduled install time: 01:00
          [OK]
[Reboot]
```

Verify the updates settings.

```
Settings
+ Update & security
  + Windows Update
    + Advanced options
      Never check for updates (not recommended)
```

Disable P2P updates.

```
Settings
+ Update & security
  + Windows Update
    + Advanced options
      + Choose how updates are delivered
        Updates from more than one place: PCs on my local network
```

NOTE: This setting is not available after automatic updates are disabled.

Get updates for other Microsoft products.

```
Settings
+ Update & security
  + Windows Update
    [✓] Give me updates for other Microsoft products when I update
[Reboot]
```


## Windows Defender
Disable the Windows Defender cloud-based protection and sample submission.

```
Settings
+ Update & security
  + Windows Defender
    Real-time protection: On (cannot be disabled permanently by this setting)
    Cloud-based Protection: Off
    Sample submission: Off
```

Disable the Windows Defender real-time protection.

```
gpedit.msc
+ Computer Configuration
  + Administrative Templates
    + Windows Components
      + Windows Defender
        + Real-time Protection
          Turn off real-time protection
          Enabled
```


## Wi-Fi
Disable Wi-Fi Sense.

```
Settings
+ Network & Internet
  + Wi-Fi
    + Manage Wi-Fi settings
      Connect to Wi-Fi hotspots: Off
      Exchange Wi-Fi network access with my contacts: Off
```
To prevent others from sharing the password, the SSID must end with "_output".


## Telemetry

```cmd
sc delete DiagTrack
sc delete dmwappushservice
echo "" > %programdata%\Microsoft\Diagnosis\ETLLogs\AutoLogger\AutoLogger-Diagtrack-Listener.etl
cacls "%programdata%\Microsoft\Diagnosis\ETLLogs\AutoLogger\AutoLogger-Diagtrack-Listener.etl" /d SYSTEM
```

```
gpedit.msc
+ Computer Configuration
  + Administrative Templates
    + Windows Components
      + Data Collection and Preview Builds
        Allow Telemetry: Disabled
```

```
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\DataCollection" /v "AllowTelemetry" /t REG_DWORD /d 0 /f
```

Disable the Application Experience tasks.

```
Task Scheduler
Task Scheduler Library
+ Microsoft
  + Windows
    + Application Experience
      Microsoft Compatibility Appraiser: Disabled
      ProgramDataUpdater: Disabled
```

## Cortana
Disable online search results.

```
Search the web and Windows (right to the start button)
+ Settings (Cog)
  Cortana...: Off
  Search online and include web results: Off
```

Disable cortana data gathering (if cortana was enabled).

```
Settings
+ Privacy
  + Speech, inking, & typing
    [Stop getting to know me]
```

Disable the search field.

```
Task Bar Context Menu
+ Search
  Hidden
```

## Lock Screen
Disable the lock screen so that the password input field is always visible.

```
gpedit.msc
+ Computer Configuration
  + Administrative Templates
    + Control Panel
      + Personalization
        Do not display the lock screen: Enabled
```

## OneDrive
Uninstall OneDrive.

```cmd
taskkill /f /im OneDrive.exe
%SYSTEMROOT%\SysWOW64\OneDriveSetup.exe /uninstall
rd "%USERPROFILE%\OneDrive" /Q /S
rd "C:\OneDriveTemp" /Q /S
shutdown -r -t 0
```

```cmd
rd "%LOCALAPPDATA%\Microsoft\OneDrive" /Q /S
rd "%PROGRAMDATA%\Microsoft OneDrive" /Q /S
reg delete "HKEY_CLASSES_ROOT\CLSID\{018D5C66-4533-4307-9B53-224DE2ED1FE6}" /f
reg delete "HKEY_CLASSES_ROOT\Wow6432Node\CLSID\{018D5C66-4533-4307-9B53-224DE2ED1FE6}" /f
shutdown -r -t 0
```

Execute the following in a user command prompt.

```cmd
rd "%USERPROFILE%\OneDrive" /Q /S
```


## Photos
Disable automatic photo enhancements and linked duplicates.

```
Photos
+ Settings
  Automatically enhance my photos: Off
  Linked duplicates: Off
  Show my cloud-only content from OneDrive: Off
```


## Privacy
Disable everything not absolutely necessary in `Settings / Privacy`.


## Settings
Use common sense in `Settings` and the `Control Panel`…


## OneNote
Right click the "Send to OneNote" tray icon and disable it in the settings.

```
Task Manager
+ Startup
  Send to OneNote Tool: Disabled
```


## Help & Support
To disable Help & Support (e.g. the F1 hotkey), rename the file `C:\Windows\HelpPane.exe`.


## Firewall
Disable everything in `Windows Firewall with Advanced Security / Inbound Rules` except:
- Core Networking - *
- Delivery Optimization
- File and Printer Sharing (Echo Request*)
- Network Discovery (NB-Datagram-In)
- Network Discovery (NB-Name-In)

Change the Echo Request rules to accept public connections.


## Services
Disable unwanted services.

```
services.msc
Certificate Propagation: Disabled
DataCollectionPublishingService: Disabled
Downloaded Maps Manager: Disabled
Geolocation Service: Disabled
Hyper-V *: Disabled (Optional)
Microsoft (R) Diagnostics Hub Standard Collector Service: Disabled
Offline Files: Disabled
Superfetch: Disabled (When no HDD is installed.)
Touch Keyboard and Handwriting Panel Service: Disabled
Windows Biometric Service: Disabled
Xbox Live \*: Disabled
```


## Startup
Disable graphical boot.

```
msconfig
+ Boot
  [✓] No GUI boot
```


## Notifications
```
Control Panel
+ System and Security
  + Security and Maintenance
    (Turn off all messages.)
```


## Skype
Disable ads after installing Skype.

```
Control Panel
+ Internet Options
  + Security
    Restricted sites
    + [Sites]
      Add this website to the zone: https://apps.skype.com
      [Add]
```

Edit `%APPDATA%\Skype\<username>\config.xml`
- Set the `<AdvertPlaceholder>` XML element to `0`.
- Set the file to read-only.


## Windows Libraries
Move unwanted Windows libraries.

1. Go to `%UserProfile%\Pictures`.
2. Right click and select `Properties` on `Camera Roll` and `Saved Pictures`.
3. Select the `Location` tab and enter a new location e.g. `%AppData%\Camera Roll`.

Delete the libraries (optional).

```
del "%APPDATA%\Microsoft\Windows\Libraries\SavedPictures.library-ms" /S
del "%APPDATA%\Microsoft\Windows\Libraries\CameraRoll.library-ms" /S
```

NOTE: Deleting the libraries or setting `"ThisPCPolicy"="Hide"` in the registry is not a permanent solution.


## Windows Mobile Device Center
Connect a Windows CE device and let the OS finish the WMDC installation.

```
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows CE Services" /v "GuestOnly" /t REG_DWORD /d 1 /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v "Windows Mobile Device Center" /t REG_SZ /d "%windir%\WindowsMobile\wmdcbase.exe" /f
```
