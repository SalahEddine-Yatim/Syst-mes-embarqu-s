###############################################################################
# Binary to build

PRG            = convoi
OBJ            = convoi.o uart.o

###############################################################################
# Path to AVR toolchain

ifneq (,$(wildcard /etc/redhat-release))
# Red Hat and CentOS have /etc/redhat-release
	AVR_TOOLCHAIN = /usr/local/arduino-1.8.19/hardware/tools/avr
else
# Debian and others
	AVR_TOOLCHAIN = /usr
endif

###############################################################################
# GCC parameters

CC             = $(AVR_TOOLCHAIN)/bin/avr-gcc
OBJCOPY        = $(AVR_TOOLCHAIN)/bin/avr-objcopy
OBJDUMP        = $(AVR_TOOLCHAIN)/bin/avr-objdump
SIZE           = $(AVR_TOOLCHAIN)/bin/avr-size

MCU_TARGET     = atmega328
OPTIMIZE       = -Os

DEFS           = -DF_CPU=16000000L
LIBS           =

###############################################################################
# avrdude parameters

CONFIG_FILE    = /etc/avrdude.conf
CPU_TYPE       = ATMEGA328
SERIAL_PORT    = /dev/ttyUSB0
BAUDRATE       = 115200
PROGRAMMER_TYPE= arduino

###############################################################################
# You should not have to change anything below here.

CFLAGS         = -g -Wall -Werror $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS) --std=c99
LDFLAGS        = -Wl,-Map,$(PRG).map

all: hex

upload: hex
	$(AVR_TOOLCHAIN)/bin/avrdude -C $(CONFIG_FILE) -p $(CPU_TYPE) \
	-P $(SERIAL_PORT) \
	-c $(PROGRAMMER_TYPE) -b $(BAUDRATE) -F -u -U flash:w:$(PRG).hex

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(SIZE) $@

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(PRG).elf $(PRG).bin *.hex *.bak
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
	rm -rf *~ *eeprom*

distclean: clean
	rm -rf $(PRG).hex

# Rules for building the rom images

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# serial terminal connexion

terminal:
	screen /dev/ttyUSB0 115200

###############################################################################
# Packaging of the lab

STUDENT_DIR=arduino


dist: package_student

package_student:
	-rm -rf $(STUDENT_DIR)
	mkdir $(STUDENT_DIR)
	cp Makefile $(STUDENT_DIR)
	-unifdef -D__STUDENT__ pid.c > $(STUDENT_DIR)/pid.c
	-unifdef -D__STUDENT__ blink.c > $(STUDENT_DIR)/blink.c
	tar zcvf $(STUDENT_DIR).tar.gz $(STUDENT_DIR)
