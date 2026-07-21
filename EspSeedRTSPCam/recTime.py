import cv2
import datetime

# Replace with your RTSP URL
RTSP_URL = "rtsp://192.168.4.1:554"

# Open the RTSP stream
cap = cv2.VideoCapture(RTSP_URL)

# Check if stream opened successfully
if not cap.isOpened():
    print("Error: Cannot open RTSP stream")
    exit()

# Get stream properties
width  = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps    = cap.get(cv2.CAP_PROP_FPS)
fps    = fps if fps > 0 else 25  # Default to 25 if unknown

# Define video codec and output file
timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
out = cv2.VideoWriter(f"recorded_{timestamp}.mp4", cv2.VideoWriter_fourcc(*'mp4v'), fps, (width, height))

print("Recording started. Press 'q' to stop.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break

    # Get current timestamp
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # Set font and calculate position for top-right corner
    font = cv2.FONT_HERSHEY_SIMPLEX
    font_scale = 0.6
    font_color = (255, 255, 255)  # White
    thickness = 1

    # Get text size to align right
    (text_width, text_height), _ = cv2.getTextSize(now, font, font_scale, thickness)
    text_x = width - text_width - 10
    text_y = text_height + 10

    # Add background rectangle for better visibility
    cv2.rectangle(frame, (text_x - 5, text_y - text_height - 5), (text_x + text_width + 5, text_y + 5), (0, 0, 0), -1)

    # Put the timestamp text
    cv2.putText(frame, now, (text_x, text_y), font, font_scale, font_color, thickness)

    # Write and display the frame
    out.write(frame)
    cv2.imshow("RTSP Stream", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Cleanup
cap.release()
out.release()
cv2.destroyAllWindows()
print("Recording stopped.")
