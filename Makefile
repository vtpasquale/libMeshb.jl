# Usage:
# make	# compile
# make clean  # remove object files

.PHONY = all

CC = gcc

LBM_SRCS := $(wildcard ./libMeshb/*.c)
LBM_OBJ := $(LBM_SRCS:%.c=%.o)

C_SRCS := $(wildcard *.c)
C_OBJ := $(C_SRCS:%.c=%.o)

# Build Gamma class object
all: ${LBM_OBJ} ${C_OBJ}
	${CC} -o readTriMesh readTriMesh.o libMeshb/libmeshb7.o
	${CC} -shared -fPIC -o readTriMesh readTriMesh.c libMeshb/libmeshb7.c

# Build Gamma class object from source
C_OBJ: ${C_SRCS}
	${CC} -c $<

# Build Gamma member objects from source
LBM_OBJ: ${LBM_SRCS}
	${CC} -c $<

clean:
	@echo "Cleaning up..."
	rm -rvf ./libMeshb/*.o
	rm -rvf *.o
	rm -rvf *.so
	rm -rvf readTriMesh