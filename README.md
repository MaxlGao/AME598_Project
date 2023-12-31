# User Guide

## Hardware
For reference, this is the full hardware stack used for the project.
- Raspberry Pi 4.0
    - External Keyboard and Monitor
    - Dedicated power supply
    - USB cable for USB tethering to phone
- USB camera
- x2 Miuzei SG90 servo on 3d-printed alt-azimuth assembly
- 5V 2A Servo power supply (mine was broken, I had to also use the right wall plug power supply)
- ESP32 devkit
- Laptop for programming ESP32 and connecting to EC2 instance. OS doesn't matter if running the face detection script on the pi.

![Full Setup](img_full_setup_4.jpg "Full Setup")



### For Webcam connected to PC (Use Ubuntu OS)
- Connect a USB webcam to either a PC running Ubuntu or a Raspberry Pi
- Install Python, at least version 3.10
- Run `pip3 install requests opencv-python numpy os platform`
- Open Face_Detector_PC.py and change IP address on line 159. (Finish "Startup EC2" steps below first)
- Run Face_Detector_PC.py and verify connection to server (Finish "Running the Server" steps below first)
- To quit: make 'capture' the active window, then press `q` to quit

### For Webcam connected to Raspberry Pi
- `git clone https://github.com/MaxlGao/AME598_Project.git`
- `cd AME598_Project`
- `sudo apt install python3-opencv` and other libraries
- `sudo nano Face_Detector_Pi.py`
- Change IP address on line 159. (Finish "Startup EC2" steps below first)
- Run Face_Detector_PC.py and verify connection to server (Finish "Running the Server" steps below first)
- To quit: Enter CTRL-C

## Startup EC2

- Create EC2 instance on AWS Academy, Ubuntu 20.04.
- Configure security settings to 
    1. SSH traffic to port 22 (default)
    2. All TCP traffic to port 8080
    3. TCP traffic to port 21017 from your IP (optional)
- Launch instance

## In the instance
### Initial
Run the following commands

`curl -s https://gist.githubusercontent.com/tejaswigowda/f289e9bff13d152876e8d4b3281142f8/raw/df37df2e16a3166e686357a045751a7c18bbeebe/installNodeMongoUbuntu16.04.sh | bash`

`sudo npm install -g n`

`sudo npm install -g http-server`

`git clone https://github.com/MaxlGao/AME598_Project.git`

`sudo service mongod start`

`sudo nano /etc/mongod.conf`

### In mongodb.conf:

Change bindIP to 0.0.0.0

### Running the Server:

If changing security settings, or if mongocompass isnt behaving: 

`sudo service mongod restart` 

Additionally:

`sudo npm install mongoskin`

`sudo npm install mongodb@^2.0`

`cd AME598_Project/server`

`node server.js`

To configure the server: 

`sudo nano server.js`

To fully shut down the server:

`ps`

Then

`kill -9 [NODE PROCESS ID]`

## On a Browser

Go to `http://[AWS-generated IP]:8080`

If the connection is functioning properly, you should see something like this. The 'capture' window only shows up if you are running 'Face_Detector_PC.py' on the same PC.

![First Test Results](screenshot_1.png "First Test Results")

If using the Pi, the image is aggressively downscaled as to avoid a "request entity is too large" error

![Second Test Results](screenshot_2.png "Second Test Results")


## On the ESP32

Open mC.ino and modify the fields for wifi SSID, password, and server IP address. This will pull motor commands from the server.

Modify pin attachments to servos as needed. 

Upload the sketch onto an ESP32 microcontroller. Verify that it is able to connect to the server.

Your output should look something like this:

![Arduino Output](screenshot_3.png "Arduino Output")
