########## User-definable stuff ##########
#
###Compiler and compilation options
COMP_SER = gcc
COMP_MPI = mpicc
OPTIONS = -Wall -O3
#
### Behavioural flags
#Use double precision integer (enable in general)
DEFINEFLAGS += -D_LONGIDS
#Generate debug help. Only useful for development
DEFINEFLAGS += -D_DEBUG
#Use double precision floating point? Set to "yes" or "no"
USE_SINGLE_PRECISION = yes
#Compile with HDF5 capability? Set to "yes" or "no"
USE_HDF5 = yes
#Compile with FITS capability? Set to "yes" or "no"
USE_FITS = yes
#Use OMP parallelization? Set to "yes" or "no"
USE_OMP = yes
#Use MPI parallelization? Set to "yes" or "no"
USE_MPI = yes
#
###Path to libraries and headers
###If two or more of the dependencies reside in the same paths, only
###one instance is necessary.
#GSL
GSL_INC =
GSL_LIB =
#FFTW
FFTW_INC =
FFTW_LIB =
#cfitsio
FITS_INC =
FITS_LIB =
#cfitsio
HDF5_INC =
HDF5_LIB =
#
########## End of user-definable ##########

ifeq ($(strip $(USE_OMP)),yes)
OPTIONS += -fopenmp
DEFINEFLAGS += -D_HAVE_OMP
endif #OMP

ifeq ($(strip $(USE_MPI)),yes)
DEFINEFLAGS += -D_HAVE_MPI
COMP_PAR = $(COMP_MPI)
else #MPI
COMP_PAR = $(COMP_SER)
endif #MPI

ifeq ($(strip $(USE_SINGLE_PRECISION)),yes)
DEFINEFLAGS += -D_SPREC

ifeq ($(strip $(USE_OMP)),yes)
LIB_FFTW += -lfftw3f_omp
endif #OMP
ifeq ($(strip $(USE_MPI)),yes)
LIB_FFTW += -lfftw3f_mpi
endif #MPI
LIB_FFTW += -lfftw3f

else #SINGLE_PRECISION

ifeq ($(strip $(USE_OMP)),yes)
LIB_FFTW += -lfftw3_omp
endif #OMP
ifeq ($(strip $(USE_MPI)),yes)
LIB_FFTW += -lfftw3_mpi
endif #MPI
LIB_FFTW += -lfftw3

endif #SINGLE_PRECISION


OPTIONS += $(DEFINEFLAGS)

INC_ALL = -I./src $(GSL_INC) $(FFTW_INC) $(FITS_INC) $(HDF5_INC)
LIB_ALL = $(GSL_LIB) $(FFTW_LIB) $(FITS_LIB) $(HDF5_LIB) -lgsl -lgslcblas $(LIB_FFTW)
ifeq ($(strip $(USE_HDF5)),yes)
DEFINEFLAGS += -D_HAVE_HDF5
LIB_ALL += -lhdf5 -lhdf5_hl -lz
endif #HDF5
ifeq ($(strip $(USE_FITS)),yes)
DEFINEFLAGS += -D_HAVE_FITS
LIB_ALL += -lcfitsio
endif #FITS
LIB_ALL += -lm

COMMONO = src/common.o
COSMOMADO = src/cosmo_mad.o
COSMOO = src/cosmo.o
FOURIERO = src/fourier.o
GRIDO = src/grid_tools.o
IOO = src/io.o
MAIN = src/main.c
OFILES = $(COMMONO) $(COSMOMADO) $(COSMOO) $(FOURIERO) $(GRIDO) $(IOO)

EXEC = CoLoRe

default : $(EXEC)

%.o : %.c
	$(COMP_CC) $(OPTIONS) $(INC_ALL) -c $< -o $@

$(OFILES) : COMP_CC := $(COMP_PAR)

$(EXEC) : $(OFILES)
	$(COMP_PAR) $(OPTIONS) $(INC_ALL) $(OFILES) $(MAIN) -o $(EXEC) $(LIB_ALL)

clean :
	rm -f src/*.o

cleaner :
	rm -f *~ src/*.o src/*~  $(EXEC)
