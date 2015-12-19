TARGET = zygote
OBJECTS = properties.o fault_handler.o runtime.o signal_catcher.o zygote.o

DEPEND_LIB = ${PROJECT_PATH}/src/base/log.o ${PROJECT_PATH}/src/base/start.o ${PROJECT_PATH}/src/base/system_properties.o

all: ${OBJECTS} ${TARGET}

include ${MAKE_INCLUDE}

