import cv2

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
count = 0
while True:
    ret, frame = cap.read()
    if not ret:
        break
    
    # Update tracker
    ret, bbox = tracker.update(frame)
    if ret:
        # Print text on screen and write to file
        bbox_str = f"Frame: {count}, x: {bbox[0]}, y: {bbox[1]}, w: {bbox[2]}, h: {bbox[3]}"
        print("Bounding box coordinates:", bbox_str)
        informatiefile.write(bbox_str + "\n")
        informatiefile.flush()
        
        # Draw bounding box
        draw_bbox(frame, bbox)
        
        # Save the frame with bounding box
        cv2.imwrite("frame%d.jpg" % count, frame)
        count += 1

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
