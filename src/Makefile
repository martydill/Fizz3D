
# Top-level makefile for Fizz3d

PROGRAM = Fizz3d


# Platform-independent source files
OBJS = 	Math.o	\
			Vector.o	\
			Camera.o	\
			Config.o	\
			Entity.o	\
			Log.o	\
			Q3Map.o	\
			Q3Renderer.o	\
			MD3Model.o	\
			Main.o	\
			Engine.o	\
			GLDriver_Core.o	\
			GLDriver_TextureFactory.o


ifdef _WIN32

	OBJS += GLDriver_Win32.o Platform_Win32.o

else

	CPP = g++
	CPPFLAGS = -Wall -ansi -g -D_LINUX
	LDFLAGS = -L/usr/X11R6/lib -lstdc++ -lGL -lGLU -lXxf86vm -lIL -lILU -lILUT
	OBJS += glxWindow.o GLDriver_GLX.o XMouse.o XKeyboard.o Platform_Linux.o

endif


default: all


all: $(OBJS) program


$(OBJS): %.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $<


program: $(OBJS)
	$(CPP) $(OBJS) -o $(PROGRAM) $(LDFLAGS)


clean:
	-rm -f $(OBJS) $(PROGRAM)

