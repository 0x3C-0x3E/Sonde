import time
from picamera2 import *
from picamera2.encoders import *
from picamera2.outputs import *

picam_0 = Picamera2(0)
picam_1 = Picamera2(1)

video_config = picam_0.create_video_configuration()

picam_0.configure(video_config)
picam_1.configure(video_config)

encoder_0 = H264Encoder(10000000)
encoder_1 = H264Encoder(10000000)

def record(name):
	output_0 = FfmpegOutput(f"{name}-cam0.mp4")
	output_1 = FfmpegOutput(f"{name}-cam1.mp4")

	picam_0.start_recording(encoder_0, output_0)
	picam_1.start_recording(encoder_1, output_1)

	time.sleep(60)

	picam_0.stop_recording()
	picam_1.stop_recording()



if __name__ == "__main__":
	i = 0
	while True:
		record(i)
		i += 1




