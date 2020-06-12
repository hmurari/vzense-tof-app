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
* The log file is available in `tof-log.txt` file. It looks like the following.
```
Fri Jun 12 13:39:44 2020 | People Count Changed    | People Count 1 | AD Count 0
Fri Jun 12 13:39:49 2020 | People Count Changed    | People Count 0 | AD Count 0
Fri Jun 12 13:39:52 2020 | People Count Changed    | People Count 1 | AD Count 0
Fri Jun 12 13:39:57 2020 | Residence Count Changed | People Count 1 | AD Count 1
Fri Jun 12 13:40:02 2020 | People Count Changed    | People Count 0 | AD Count 1
Fri Jun 12 13:40:03 2020 | People Count Changed    | People Count 1 | AD Count 1
Fri Jun 12 13:40:10 2020 | People Count Changed    | People Count 0 | AD Count 1
Fri Jun 12 13:40:13 2020 | People Count Changed    | People Count 1 | AD Count 1
Fri Jun 12 13:40:19 2020 | Residence Count Changed | People Count 1 | AD Count 2
Fri Jun 12 13:40:24 2020 | People Count Changed    | People Count 0 | AD Count 2
Fri Jun 12 13:40:26 2020 | People Count Changed    | People Count 1 | AD Count 2
Fri Jun 12 13:40:29 2020 | Residence Count Changed | People Count 1 | AD Count 3
Fri Jun 12 13:40:31 2020 | People Count Changed    | People Count 0 | AD Count 3
Fri Jun 12 13:40:38 2020 | People Count Changed    | People Count 1 | AD Count 3
Fri Jun 12 13:40:38 2020 | People Count Changed    | People Count 0 | AD Count 3
Fri Jun 12 13:40:38 2020 | People Count Changed    | People Count 1 | AD Count 3
Fri Jun 12 13:40:41 2020 | Residence Count Changed | People Count 1 | AD Count 4
Fri Jun 12 13:40:45 2020 | People Count Changed    | People Count 0 | AD Count 4
```


