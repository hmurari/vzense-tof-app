# People Count Demo Guid



## Download 

Download people_count_sdk_ubuntu18.zip package 

## Build zense sdk environment

1. Unzip the package
2. run install.sh
3. Reference **people_count_sdk_ubuntu18/Document/SDK/PicoZenseSDK_User_Guide_Linux_en.pdf**, bulid  your software develop environment

## Compile demo app

1. enter **people_count_sdk_ubuntu18/Samples/people-count-demo**
2. run 'make'

## Run the demo

./PicoZensePeopleCount



## Configurations:

If user want to modify some parameters or code, you can reference below:

1. Algorithm parameters:

   In 'Alg_PCConfig.ini' files, user need config some paras based the circumstance.

   **CameraHeight** : 2250       the height value (mm) between camera and ground
   **RotationPitch** : -30		the horizontal angle of the camera 
   **StableSecond** : 3			reach the time value that people number will be count   

2. Demo modify 

   In 'PicoZensePeopleCount.cpp' files, user can change the demo code.

   ```c++
   // object instantiation
   ALG_PeopleCount* pPeopleCount = new ALG_PeopleCount();
   // init parameters
   pPeopleCount->Init(campara);
   //get frame from camera using zense base sdk api
   status = Ps2_GetFrame(deviceHandle, sessionIndex, PsDepthFrame, &depthFrame);
   //call algorithm 
   pPeopleCount->Run(RawDepth);
   
   ```

   The result of algorithm: people_number_statistics，residence_count， HighPoint，distance ....... 

   . 

