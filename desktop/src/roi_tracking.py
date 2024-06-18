import cv2
import time

# Function drawing bounding box
def drawBbox(image, bbox):
    # Extracting coordinates and dimensions from bbox
    x, y, w, h = [int(i) for i in bbox]
    # Drawing rectangle around the object being tracked
    cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

# Function to process the video
def processVideoinformation(video_path):
    # Opening file to store information
    informatiefile = open("informatie.txt", "a")
    # Opening the video file
    cap = cv2.VideoCapture(video_path)
    # Reading the first frame
    ret, frame = cap.read()
    # Check if the frame was successfully read
    if not ret:
        raise ValueError("Cannot read video file")
    
    # Selecting ROI (Region of Interest) for tracking
    bbox = cv2.selectROI("Select Object to Track", frame, False)
    # Creating CSRT tracker
    tracker = cv2.TrackerCSRT_create()
    # Initializing the tracker with the selected ROI
    tracker.init(frame, bbox)
    
    # Initializing frame counter and start time for processing
    frameCounter = 0
    startTime = time.time()
    
    # Looping through each frame of the video
    while True:
        # Reading the next frame
        ret, frame = cap.read()
        # Calculating elapsed time
        elapsedTime = time.time() - startTime
        
        # Processing frames once per second
        if elapsedTime >= 1:
            # Saving frame
            cv2.imwrite("frame%d.jpg" % frameCounter, frame)
            # Updating start time and frame counter
            startTime = time.time() 
            frameCounter += 1
            # Writing bounding box information to file: (frame(nummer), x,y,w,h)
            bboxStr = f"Frame: {frameCounter}, x: {bbox[0]}, y: {bbox[1]}, w: {bbox[2]}, h: {bbox[3]}"
            informatiefile.write(bboxStr + "\n")
            informatiefile.flush()
            
        # Updating the tracker with the current frame
        ret, bbox = tracker.update(frame)
        # If tracking is successful, draw bounding box on the frame
        if ret:
            drawBbox(frame, bbox)

        # Displaying the frame with bounding box
        cv2.imshow('Tracking', frame)

        # Exiting the loop if ESC key is pressed
        if cv2.waitKey(1) == 27:
            break

    informatiefile.close()
    # Releasing the video capture
    cap.release()
    cv2.destroyAllWindows()

# Main function
def main():
    # Video file
    video_path = "Boot.mp4"
    # Calling the function to process the video
    processVideoinformation(video_path)

# Entry point of the script
if __name__ == "__main__":
    main()
