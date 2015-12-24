# Directory and name settings.
SRC  = ./Code
OUT  = ./Output
RES  = ./Ressources
NAME = ALU_standalone.exe


#OBJS specifies which files to compile as part of the project.
OBJS = $(wildcard $(SRC)/Agents/*.cpp)\
       $(wildcard $(SRC)/Agents/Perception/*.cpp)\
       $(wildcard $(SRC)/Agents/Reasoning/*.cpp)\
       $(wildcard $(SRC)/Converter/*.cpp)\
       $(wildcard $(SRC)/Converter/Modules/*.cpp)\
       $(wildcard $(SRC)/Converter/Utils/*.cpp)\
       $(wildcard $(SRC)/Data/*.cpp)\
       $(wildcard $(SRC)/Data/Compression/*.cpp)\
       $(wildcard $(SRC)/Data/Math/*.cpp)\
       $(wildcard $(SRC)/Data/Textures/*.cpp)\
       $(wildcard $(SRC)/Environment/*.cpp)\
       $(wildcard $(SRC)/Execution/*.cpp)\
       $(wildcard $(SRC)/Input/*.cpp)\
       $(wildcard $(SRC)/Input/ControllerModules/*.cpp)\
       $(wildcard $(SRC)/Physics/*.cpp)\
       $(wildcard $(SRC)/Physics/Modules/*.cpp)\
       $(wildcard $(SRC)/UserInterface/*.cpp)\
       $(wildcard $(SRC)/Visualization/*.cpp)\

# Include paths.
INC_PATHS = -I$(SRC) -I$(RES)/SDL2-2.0.3/include

# Library paths.
LIB_PATHS = -L$(RES)/SDL2-2.0.3/lib_x86_mingw

# Compiler flags.
C_FLAGS = -Os -w -Wl,-subsystem,console

# Linker flags.
L_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -static

# Flags as suggested on StackOverflow for static linking: 
# http://stackoverflow.com/questions/17620884/static-linking-of-sdl2-libraries
# -lmingw32 -lSDL2main -lSDL2 -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32
# -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc

#This is the target that compiles our executable.
all : $(OBJS)
	g++ $(OBJS) $(INC_PATHS) $(LIB_PATHS) $(C_FLAGS) $(L_FLAGS) -o $(OUT)/$(NAME)