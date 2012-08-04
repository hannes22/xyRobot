INJAR = *.class splash.png
#CFLAGS = -Xlint:unchecked
# Path to jni.h
ifdef SystemRoot
RM = del
INJAR += Serial.dll
PLATFORM = Win
else
ifdef SYSTEMROOT
# Looks like cygwin or mingw shell
RM = rm -rf
INJAR += Serial.dll
PLATFORM = Win
else
RM = rm -rf
UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
INJAR += libSerial.jnilib
PLATFORM = Mac
else
INJAR += libSerial.so
PLATFORM = Linux
endif
endif
endif

# All java files to be compiled
# List so it works as target
JAVAFILES = Remote.java HelperUtility.java SerialCommunicator.java PaintCanvas.java Registers.java CanvasWindow.java ImageTest.java

# --------------------------------------

ifdef SystemRoot
all: xyRobotRemote.jar
else ifdef SYSTEMROOT
all: xyRobotRemote.jar
else ifeq ($(UNAME),Darwin)
all: xyRobotRemote.app
else
all: xyRobotRemote.jar
endif

xyRobotRemote.app: xyRobotRemote.jar
	cp -r bundles/mac.app xyRobotRemote.app
	cp -r xyRobotRemoteMac.jar xyRobotRemote.app/Contents/Resources/Java/xyRobotRemoteMac.jar
	make cleanSmall

run: xyRobotRemote.jar
	java -jar xyRobotRemote$(PLATFORM).jar

doc: doc/index.html

xyRobotRemote.jar: HelperUtility.class manifest.txt libSerial
	jar -cmf manifest.txt "xyRobotRemote$(PLATFORM).jar" $(INJAR)

HelperUtility.class: $(JAVAFILES)
	javac $(CFLAGS) -g $(JAVAFILES)

doc/index.html: $(JAVAFILES)
	javadoc -d doc $(JAVAFILES)

ifdef SystemRoot
libSerial: Serial.dll
else
ifdef SYSTEMROOT
libSerial: Serial.dll
else
ifeq ($(UNAME),Darwin)
libSerial: libSerial.jnilib
else
libSerial: libSerial.so
endif
endif
endif

Serial.dll: libSerial/serialHelper.c libSerial/winSerial.c
	make -C libSerial
	mv libSerial/Serial.dll Serial.dll

libSerial.jnilib: libSerial/serialHelper.c libSerial/unixSerial.c
	make -C libSerial
	mv libSerial/libSerial.jnilib libSerial.jnilib

libSerial.so: libSerial/serialHelper.c libSerial/unixSerial.c
	make -C libSerial
	mv libSerial/libSerial.so libSerial.so

# Delete intermediate files
clean: cleanSmall
	$(RM) xyRobotRemote.app
cleanSmall:
	make -C libSerial clean
	$(RM) *.class
	$(RM) *.o
ifdef SystemRoot
	$(RM) *.dll
	$(RM) xyRobotRemoteWin.jar
else
ifdef SYSTEMROOT
	$(RM) *.dll
	$(RM) xyRobotRemoteWin.jar
else
ifeq ($(UNAME),Darwin)
	$(RM) *.jnilib
	$(RM) xyRobotRemoteMac.jar
else
	$(RM) *.so
	$(RM) xyRobotRemoteLinux.jar
endif
endif
endif
