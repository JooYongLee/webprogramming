import cv2
import numpy as np
import os

path = "siba.jpg"
with open(path, "rb") as f:
    data = f.read()
print("file size", len(data))
mystat = os.stat(path)
print("from os size ", mystat.st_size)
# cv2.imdecode()
np_data = np.asarray(data)
np_buffer = np.frombuffer(np_data, np.uint8)
image = img_np = cv2.imdecode(np_buffer,  cv2.IMREAD_UNCHANGED)
cv2.imshow("", image)
cv2.waitKey()