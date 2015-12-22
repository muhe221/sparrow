CC = g++

INCLUDES= -I ${PROJECT_PATH}/src/base

CFLAGS = -Wall -lpthread
#CFLAGS = -Wall -O2 -g

# output path
Output = out
FinalOutput := ${PROJECT_PATH}/${Output}

${TARGET}: 
	${CC} ${shell find ./ -name '*.o'} ${DEPEND_LIB} -o ${FinalOutput}/${TARGET}  ${CFLAGS}

#SUB_ALL_MAKE_FILES = ${shell find ${SRC} -name Makefile}
#SUB_MODULE_DIRS = ${foreach itera, ${SUB_ALL_MAKE_FILES}, $(itera:%/Makefile=%)}



#SUB_MODULE:
#	for module in ${SUB_MODULE_DIRS}; \
#	do \
#	${MAKE} -C $${module} all; \
#	done

%.o: %.cpp
	${CC} ${INCLUDES} -c $< -o $@  ${CFLAGS}
%.o: %.cc
	${CC} ${INCLUDES} -c $< -o $@  ${CFLAGS}
%.o: %.c
	${CC} ${INCLUDES} -c $< -o $@  ${CFLAGS}

###########################################################
ALL_MODULE: BASE_MODULE PROPERTY_SERVICE ZYGOTE PROPERTY_SERVICE INIT

BASE_MODULE:
	${MAKE} -C ${PROJECT_PATH}/src/base all

ZYGOTE: BASE_MODULE APPLICATION
	${MAKE} -C ${PROJECT_PATH}/src/core/service/zygote all

PROPERTY_SERVICE: BASE_MODULE
	${MAKE} -C ${PROJECT_PATH}/src/core/init/property_service all

INIT: PROPERTY_SERVICE
	${MAKE} -C ${PROJECT_PATH}/src/core/init all

APPLICATION:
	${MAKE} -C ${PROJECT_PATH}/src/packages/apps all
