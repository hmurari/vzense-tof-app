# People Count Demo using TOF Camera



## Download 
Clone this repository onto the device. 
```
# Use HTTP clone (if SSH keys are not configured)
git clone https://coolertech@dev.azure.com/coolertech/mp-on-shelf-availability/_git/tof-app

OR

# Use SSH clone (if SSH keys are configured)
git clone git@ssh.dev.azure.com:v3/coolertech/mp-on-shelf-availability/tof-app
```

## Install Dependencies
* Install Python3 and PIP if not installed already.
```
# Check if these are present.
python3 --version
pip3 --version

# If not present, then install these.
sudo apt-get install python3.6
sudo apt-get install python3-pip
```

* Install the latest OpenCV library.
```
sudo apt-get install python-opencv
```


## Build zense sdk environment & Demo application
* Build Zense SDK environment:
```
cd people_count_sdk_ubuntu18
./install.sh
```

* Build Demo application:
## Build Demo application.
```
cd Samples/people-count-demo
make
```

* Ensure that demo application is compiled. There should be an executable file `PicoZensePeopleCount `


## Run the People Count application
* Run application
```
./PicoZensePeopleCount
```
* The log file is available in `tof-log.txt` file.

