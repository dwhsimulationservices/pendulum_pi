
INSTALL_PATH?=	/usr
INSTALL=	/usr/bin/install -c
INSTALLDATA=	/usr/bin/install -c -m 644

CC=		gcc
CFLAGS= 	-Ofast -pipe -march=armv6zk -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard -DRPI_NO_X -Wall -fPIC

#-Winline -DDEBUG


# CFLAGS+=-DRPI_NO_X -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

default: pen

all: gl hw ui sol pen par

# parameters, configuration file input/output

PAR_OBJ= par.o

par: ${PAR_OBJ}
	@echo "making par"

par.o: par.c
	$(CC) ${CFLAGS} -c par.c -I/usr/include/libxml2 -I/usr/include/gsl

# basic console user interface

UI_OBJ= ui.o

ui: ${UI_OBJ}
	@echo "making ui"

ui.o: ui.c
	$(CC) ${CFLAGS} -c ui.c -I/usr/include/cdk

# interface to gpio ports

HW_OBJ= hw.o

hw: ${HW_OBJ}
	@echo "making hw"

hw.o: hw.c
	$(CC) ${CFLAGS} -c hw.c

hw-test: hw-test.c ${HW_OBJ}
	$(CC) ${CFLAGS} -o hw-test hw-test.c ${HW_OBJ}

# opengl es implementation

GL_INCS=	-I/opt/vc/include \
		-I/opt/vc/include/interface/vcos/pthreads \
		-I/opt/vc/include/interface/vmcs_host/linux \

GL_LIBS=	-L/opt/vc/lib -lEGL -lGLESv2 -lbcm_host -lvcos -lvchiq_arm

GL_OBJ= gl.o gl_geometries.o gl_tools.o

gl: ${GL_OBJ}
	@echo "making gl"

gl.o: gl.c gl_geometries.o gl_tools.o
	$(CC) ${CFLAGS} -c gl.c ${GL_INCS}

gl_geometries.o: gl_geometries.c
	$(CC) ${CFLAGS} -c gl_geometries.c

gl_tools.o: gl_tools.c
	$(CC) ${CFLAGS} -c gl_tools.c ${GL_INCS}

gl-test: gl-test.c ${GL_OBJ}
	$(CC) ${CFLAGS} -o gl-test gl-test.c ${GL_OBJ} ${GL_LIBS} -lm

# ode solver and equations

SOL_INCS= -I/usr/include/gsl
SOL_LIBS= -lgslcblas -lgsl -lm
SOL_OBJ= sol-equations.o sol.o

sol: ${SOL_OBJ}
	@echo "making sol"

sol.o: sol.c
	$(CC) ${CFLAGS} -c sol.c ${SOL_INCS}

sol-equations.o: sol-equations.c
	$(CC) ${CFLAGS} -c sol-equations.c ${SOL_INCS}

sol-test: sol-test.c ${PAR_OBJ} ${SOL_OBJ}
	$(CC) ${CFLAGS} -o sol-test sol-test.c ${PAR_OBJ} ${SOL_OBJ} \
		${SOL_INCS} ${SOL_LIBS} -lxml2

# main

pen: ${GL_OBJ} ${HW_OBJ} ${UI_OBJ} ${SOL_OBJ} ${PAR_OBJ} pen.o
	$(CC) ${CFLAGS} pen.o ${GL_OBJ} ${HW_OBJ} ${UI_OBJ} ${SOL_OBJ} ${PAR_OBJ} \
		${GL_LIBS} ${SOL_LIBS} -lcdk -lncursesw -lxml2 -o pen

pen.o: pen.c
	$(CC) ${CFLAGS} -c pen.c

# ...

clean:
	-rm *.o .depend pen

