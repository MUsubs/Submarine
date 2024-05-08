import cv2
import time

# File
informatiefile = open("informatie.txt", "a")

# Read video file
video_path = "Bovenkant_VID8.mp4" #Own video here.
cap = cv2.VideoCapture(video_path)

# Function to draw bounding box on image
def draw_bbox(image, bbox):
    x, y, w, h = [int(i) for i in bbox]
    cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)
    
# Get the first frame for tracking
ret, frame = cap.read()
if not ret:
    raise ValueError("Cannot read video file")

# Select ROI (Region of Interest) for tracking
bbox = cv2.selectROI("Select Object to Track", frame, False)

# Initialize tracker with selected ROI
tracker = cv2.TrackerCSRT_create()
tracker.init(frame, bbox)

# Process each frame
frame_counter = 0
start_time = time.time()
while True:
    ret, frame = cap.read()
    
    # Calculate elapsed time
    elapsed_time = time.time() - start_time
    
    # Capture frames at a rate of one per second
    if elapsed_time >= 1:
        cv2.imwrite("frame%d.jpg" % frame_counter, frame)
        start_time = time.time()  # Reset start time
        frame_counter += 1
         # Print text on screen and write to file
        bbox_str = f"Frame: {frame_counter}, x: {bbox[0]}, y: {bbox[1]}, w: {bbox[2]}, h: {bbox[3]}"
        informatiefile.write(bbox_str + "\n")
        informatiefile.flush()
        
    if not ret:
        break
    
    # Update tracker
    ret, bbox = tracker.update(frame)
    if ret:
       
        
        # Draw bounding box
        draw_bbox(frame, bbox)

    # Display frame
    cv2.imshow('Tracking', frame)

    # Exit if ESC pressed
    if cv2.waitKey(1) == 27:
        break

# Close the file
informatiefile.close()

# Release video capture
cap.release()
cv2.destroyAllWindows()
