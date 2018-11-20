# This Makefile assumes it's running on linux with the Arduino IDE and Teensyduino
# installed in ~/apps/arduino-1.8.7/
#
# See https://www.pjrc.com/teensy/td_download.html for install instructions.
#
# miniterm.py is provided by 'pip install pyserial'

ARDUINO_DIR = ${HOME}/apps/arduino-1.8.7
SKETCHBOOK	= $(HOME)/Arduino
SKETCH = $(notdir $(CURDIR)).ino
SKETCHDIRNAME = $(notdir $(CURDIR))
TARGET_DIR = $(CURDIR)/build-teensy32

all: build upload

serial: build upload monitor

build:
	@ mkdir -p $(TARGET_DIR)

	$(ARDUINO_DIR)/arduino-builder -compile -logger=human \
	-hardware $(ARDUINO_DIR)/hardware \
	-hardware $(HOME)/.arduino15/packages \
	-tools $(ARDUINO_DIR)/tools-builder \
	-tools $(ARDUINO_DIR)/hardware/tools/avr \
	-tools $(HOME)/.arduino15/packages \
	-built-in-libraries $(ARDUINO_DIR)/libraries \
	-libraries $(SKETCHBOOK)/libraries \
	-fqbn=teensy:avr:teensy31:usb=serialhid,speed=72,opt=o2std,keys=en-us \
	-ide-version=10805 \
	-build-path $(TARGET_DIR) \
	-warnings=none \
	-prefs=build.warn_data_percentage=75 \
	-verbose $(SKETCH)

upload:
	$(ARDUINO_DIR)/hardware/teensy/../tools/teensy_post_compile -test -file=$(SKETCH) -path=$(TARGET_DIR) -tools=$(ARDUINO_DIR)/hardware/teensy/../tools -board=TEENSY31 -reboot

monitor:
	sleep 1 && miniterm.py /dev/ttyACM0 115200

clean:
	rm -rf $(TARGET_DIR)
