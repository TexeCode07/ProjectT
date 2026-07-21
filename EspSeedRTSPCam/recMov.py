import cv2
import time
import datetime

# === CONFIG ===
RTSP_URL = "rtsp://192.168.0.111:554"  # Change to correct URL
MOTION_THRESHOLD = 5000               # Adjust sensitivity (number of changed pixels)
RECORD_DURATION_AFTER_MOTION = 5     # Seconds to keep recording after motion stops
SHOW_FRAMES = True                    # Set False to hide preview window

# === INIT ===
print(f"[INFO] Connecting to RTSP stream: {RTSP_URL}")
cap = cv2.VideoCapture(RTSP_URL)

if not cap.isOpened():
    print("[ERROR] Could not open video stream.")
    exit(1)

print("[INFO] Video stream opened.")

ret, prev_frame = cap.read()
if not ret:
    print("[ERROR] Could not read initial frame.")
    cap.release()
    exit(1)

prev_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)
prev_gray = cv2.GaussianBlur(prev_gray, (21, 21), 0)

fourcc = cv2.VideoWriter_fourcc(*'XVID')
recording = False
last_motion_time = 0
out = None
frame_count = 0

while True:
    ret, frame = cap.read()
    if not ret:
        print("[WARN] Failed to grab frame.")
        break

    frame_count += 1
    print(f"[DEBUG] Frame #{frame_count}")

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (21, 21), 0)

    frame_diff = cv2.absdiff(prev_gray, gray)
    thresh = cv2.threshold(frame_diff, 25, 255, cv2.THRESH_BINARY)[1]
    motion_score = cv2.countNonZero(thresh)

    print(f"[DEBUG] Motion score: {motion_score}")

    prev_gray = gray

    if motion_score > MOTION_THRESHOLD:
        print("[INFO] Motion detected!")
        last_motion_time = time.time()
        if not recording:
            timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"motion_{timestamp}.avi"
            out = cv2.VideoWriter(filename, fourcc, 20.0, (frame.shape[1], frame.shape[0]))
            print(f"[INFO] Started recording: {filename}")
            recording = True
    elif recording and (time.time() - last_motion_time) > RECORD_DURATION_AFTER_MOTION:
        print("[INFO] Motion ended. Stopping recording.")
        recording = False
        out.release()

    if recording and out:
        out.write(frame)

    if SHOW_FRAMES:
        cv2.imshow("Live Feed", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("[INFO] Exiting...")
            break

cap.release()
if out:
    out.release()
cv2.destroyAllWindows()
