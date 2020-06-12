import cv2
import os

fourcc = cv2.VideoWriter_fourcc('M','J','P','G')

video = cv2.VideoWriter('tof-2.avi', fourcc, 14, (640, 480))
for idx in range(0,1000):
    print(idx)
    image = "{}.jpg".format(idx);
    if os.path.exists(image):
        frame = cv2.imread(image)
        print(frame.shape);
        video.write(frame)

video.release()

