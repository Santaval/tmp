# Include common Makefile first
include ../common/Makefile

# Add OpenSSL libraries after including common Makefile
LIBS += -lssl -lcrypto

# Add FileSystem library and include path
LIBS += -L../file_system/lib -lfilesystem
INCLUDE += -I../file_system/include

# Build FileSystem library first
$(EXEFILE): ../file_system/lib/libfilesystem.a

# Rule to build FileSystem library
../file_system/lib/libfilesystem.a:
	$(MAKE) -C ../file_system

