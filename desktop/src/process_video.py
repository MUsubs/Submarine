import cv2
import time
import numpy as np
import tensorflow as tf

# Load the pre-trained model (assuming it's fine-tuned for the submarine)
model = tf.keras.models.load_model('submarine_model.h5')

# Preprocess the image for the model
def preprocess_image(image):
    image = cv2.resize(image, (224, 224))
    image = tf.keras.applications.mobilenet_v2.preprocess_input(image)
    return np.expand_dims(image, axis=0)

# Function to get bounding box from the model prediction
def get_bounding_box(predictions, threshold=0.5):
    boxes = predictions['detection_boxes']
    scores = predictions['detection_scores']
    for i in range(len(scores)):
        if scores[i] > threshold:
            box = boxes[i]
            return box
    return None

# Function to process the video
def processVideoinformation(video_path):
    informatiefile = open("informatie.txt", "a")
    cap = cv2.VideoCapture(video_path)
    ret, frame = cap.read()
    if not ret:
        raise ValueError("Cannot read video file")

    # Detect submarine in the first frame
    preprocessed_frame = preprocess_image(frame)
    predictions = model.predict(preprocessed_frame)
    bbox = get_bounding_box(predictions[0])

    if bbox is None:
        raise ValueError("Submarine not detected in the first frame")

    # Convert bounding box to OpenCV format
    height, width, _ = frame.shape
    bbox = [bbox[1] * width, bbox[0] * height, (bbox[3] - bbox[1]) * width, (bbox[2] - bbox[0]) * height]

    # Create tracker and initialize it with the detected bounding box
    tracker = cv2.TrackerCSRT_create()
    tracker.init(frame, bbox)
    
    frameCounter = 0
    startTime = time.time()

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        elapsedTime = time.time() - startTime

        if elapsedTime >= 1:
            cv2.imwrite("frame%d.jpg" % frameCounter, frame)
            startTime = time.time()
            frameCounter += 1
            
            # Update tracker with the current frame
            ret, bbox = tracker.update(frame)
            if ret:
                # Draw bounding box on the frame
                p1 = (int(bbox[0]), int(bbox[1]))
                p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
                cv2.rectangle(frame, p1, p2, (0, 255, 0), 2, 1)
                
                bboxStr = f"Frame: {frameCounter}, x: {int(bbox[0])}, y: {int(bbox[1])}, w: {int(bbox[2])}, h: {int(bbox[3])}"
                informatiefile.write(bboxStr + "\n")
                informatiefile.flush()

        cv2.imshow('Tracking', frame)

        if cv2.waitKey(1) == 27:
            break

    informatiefile.close()
    cap.release()
    cv2.destroyAllWindows()

# Main function
def main():
    video_path = "Bovenkant_VID4.mp4"
    processVideoinformation(video_path)

if __name__ == "__main__":
    main()
