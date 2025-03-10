import time
import os
from picamera2 import *
from picamera2.encoders import *
from picamera2.outputs import *
from datetime import datetime


RECORDING_TIME: int = 60
MAX_RECORDING_CYCLES: int = 420 # 7hrs * 60 sec


def setup_camera(camera_index: int):

	success_flag: int = 0

	try:
		camera = Picamera2(camera_index)
		video_config = camera.create_video_configuration()
		camera.configure(video_config)

		return camera

	except Exception as e:
		print(f"Error initializing camera #{camera_index}: {e}")
		logfile.write(f"[{get_time()}][ERROR] Error initializing camera #{camera_index}: {e}")
		return None


def record(name: str):
	try:
		output_0 = FfmpegOutput(f"files/{name}-cam0.mp4")
		output_1 = FfmpegOutput(f"files/{name}-cam1.mp4")

		picam_0.start_recording(encoder_0, output_0)
		picam_1.start_recording(encoder_1, output_1)

		time.sleep(RECORDING_TIME)

		picam_0.stop_recording()
		picam_1.stop_recording()
		

	except Exception as e:
		print(f"Error during recording: {e}")
		logfile.write(f"[{get_time()}][ERROR] Error during recording: {e}")

	finally:
		try:
		picam_0.stop_recording()
		picam_1.stop_recording()


def get_time() -> str:
	now = datetime.now()
	current_time = now.strftime("%H:%M:%S")

	return current_time



picam_0 = setup_camera(0)
picam_1 = setup_camera(1)


encoder_0 = H264Encoder(10000000)
encoder_1 = H264Encoder(10000000)


logfile = open("logfile.log", "w")
logfile.write(f"[{get_time()}][INFO] Created logfile")


if __name == "__main__":

	if (picam_0 == None) or (picam_1 == None):
		exit(0)
		
	recording_cycles = 0
	while True:
		try:
			record(i)
			recording_cycles += 1

			if recording_cycles >= MAX_RECORDING_CYCLES:
				print("Reached final recording cycle. Exiting gracefully.")
				logfile.write(f"[{get_time()}][INFO] Reached final recording cycle. Exiting gracefully.")
				logfile.close()
				break;

		except KeyboardInterrupt:
			print("\nUser requested termination. Exiting gracefully.")
			logfile.write(f"[{get_time()}][INFO] User requested termination. Exiting gracefully.")
			logfile.close()
			break

		except Exception as e:
			print(f"Unexpected error: {e}")
			logfile.write(f"[{get_time()}][ERROR] Unexpected error: {e}")
			time.sleep(5)
