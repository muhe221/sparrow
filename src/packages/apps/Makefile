
SUB_MODULE_DIRS = Signal

all:
	@for module in ${SUB_MODULE_DIRS}; \
	do \
	${MAKE} -C $${module} all; \
	done


include ${MAKE_INCLUDE}

