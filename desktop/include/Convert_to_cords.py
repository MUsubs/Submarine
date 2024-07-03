import cv2
import numpy as np

def convert_data_to_x_y(x,y,w,h,frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    #Red range
    lower_red = np.array([0, 70, 50])
    upper_red = np.array([10, 255, 255])
    #Yellow range
    lower_yellow = np.array([20, 100, 100])
    upper_yellow = np.array([30, 255, 255])
    #Create masks
    mask_red = cv2.inRange(hsv, lower_red, upper_red)
    mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
    #Calculate red coordinats
    contours_red, _ = cv2.findContours(mask_red, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours_red:
        largest_contour = max(contours_red, key=cv2.contourArea)
        M = cv2.moments(largest_contour)
        if M["m00"] != 0:
            red_x = int(M["m10"] / M["m00"])
            red_y = int(M["m01"] / M["m00"])
        else:
            red_x, red_y = 0,0
    else:
        red_x, red_y = 0,0

    #Calculate yellow coordinats
    contours_yellow, _ = cv2.findContours(mask_yellow, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours_yellow:
        largest_contour = max(contours_yellow, key=cv2.contourArea)
        M = cv2.moments(largest_contour)
        if M["m00"] != 0:
            yellow_x = int(M["m10"] / M["m00"])
            yellow_y = int(M["m01"] / M["m00"])
        else:
            yellow_x, yellow_y = 0,0
    else:
        yellow_x, yellow_y = 0,0     

    bx = int (x + (w/2))
    by = int (y + (h/2))
    bx = (bx - red_x)/(yellow_x-red_x)
    by = (by - red_y)/(yellow_y-red_y)
    rounded_x = round(bx,3)
    rounded_y = round(by,3)
    return rounded_x,rounded_y

def convert_data_to_z(x,y,w,h,frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    #Red range
    lower_red = np.array([0, 70, 50])
    upper_red = np.array([10, 255, 255])
    #Yellow range
    lower_yellow = np.array([20, 100, 100])
    upper_yellow = np.array([30, 255, 255])
    #Create masks
    mask_red = cv2.inRange(hsv, lower_red, upper_red)
    mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
    #Calculate red coordinats
    contours_red, _ = cv2.findContours(mask_red, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours_red:
        largest_contour = max(contours_red, key=cv2.contourArea)
        M = cv2.moments(largest_contour)
        if M["m00"] != 0:
            red_x = int(M["m10"] / M["m00"])
            red_y = int(M["m01"] / M["m00"])
        else:
            red_x, red_y = 0,0
    else:
        red_x, red_y = 0,0

    #Calculate yellow coordinats
    contours_yellow, _ = cv2.findContours(mask_yellow, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours_yellow:
        largest_contour = max(contours_yellow, key=cv2.contourArea)
        M = cv2.moments(largest_contour)
        if M["m00"] != 0:
            yellow_x = int(M["m10"] / M["m00"])
            yellow_y = int(M["m01"] / M["m00"])
        else:
            yellow_x, yellow_y = 0,0
    else:
        yellow_x, yellow_y = 0,0     

    bx = int (x + (w/2))
    bx = (bx - red_x)/(yellow_x-red_x)
    rounded_x = round(bx,3)
    return rounded_x

def read_current_location(x1,y1,w1,h1,frame1,x2,y2,w2,h2,frame2):
    x,y = convert_data_to_x_y(x1,y1,w1,h1,frame1)
    z = convert_data_to_z(x2,y2,w2,h2,frame2)
    return x,y,z

def main():
    frame_zijkant = cv2.imread("R2D2\Coordinaat-bepaling\images\Geel_2.jpg")
    frame_voorkant = cv2.imread("R2D2\Coordinaat-bepaling\images\Geel_3.jpg")
    x1,y1,w1,h1 = 240,146,112,47
    x2,y2,w2,h2 = 320,170,90,40
    print(read_current_location(x1,y1,w1,h1,frame_voorkant,x2,y2,w2,h2,frame_zijkant))

if __name__ == "__main__":
    main()