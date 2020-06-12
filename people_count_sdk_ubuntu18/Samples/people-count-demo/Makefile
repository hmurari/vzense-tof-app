TARGET := PicoZensePeopleCount

SOURCE := PicoZensePeopleCount.cpp
        
FLAGS := -I../../Include -I../../Thirdparty/opencv-3.4.1/include -Wl,-rpath,./:../../Lib/:../../Thirdparty/opencv-3.4.1/lib -Wall -Wconversion -O3 -L../../Lib/ -lpicozense_api -L../../Thirdparty/opencv-3.4.1/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_calib3d -lopencv_imgcodecs -L. -lALG_PeopleCount -DDCAM_710

$(TARGET):$(SOURCE)
	g++ -std=c++11 $(FLAGS) $(SOURCE) -g -o $(TARGET) $(FLAGS)

clean:
	rm -rf *.o $(TARGET) *.txt
