READINFILE=read-silo
CC=g++
SILODIR=${HOME}/scratch/silo-4.10.2
HDIR=${SILODIR}/include
LIBDIR=${SILODIR}/lib
LIBS= -L${LIBDIR} -I${HDIR} -lsiloh5 -lhdf5 -lm -lz


ARGS= ${LIBS} -O0 

all: ${READINFILE}

${READINFILE}: ${READINFILE}.c
	${CC} ${READINFILE}.c ${ARGS} -o ${READINFILE}

clean:
	rm ${READINFILE}

