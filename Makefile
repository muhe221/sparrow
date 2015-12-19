export PROJECT_PATH = ${shell pwd}
export MAKE_INCLUDE = ${PROJECT_PATH}/makeconfig/Makefile
#########################################################

#SUB_MODULE_DIRS = ${SRC}/base ${SRC}/utils ${SRC}/core
#SUB_MODULE_DIRS = ${SRC}/base ${SRC}/utils
#SUB_MODULE_DIRS = ${SRC}/base


all: DIR ALL_MODULE

DIR:
	-mkdir -p ${FinalOutput}

.PHONY : clean
clean :
	-rm -fr ${shell find ${SRC} -name '*.o'}
	-rm -fr ${Output}

include ${MAKE_INCLUDE}
