AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
LIBGCC = `arm-none-eabi-gcc -print-libgcc-file-name`
CFLAGS = -Wall -O -fno-builtin -fomit-frame-pointer -finhibit-size-directive \
-fno-ident

OUTNAME = main
FILES = crt0.o ${OUTNAME}.o \
	BGtiles.o chr.o dsmanager.o  titlescr.o gamenote.o keyinput.o menulist.o songdata.o obj.o sfxwaves.o gamesheet.o sounddrv.o bgobjmgr.o


emu:	${OUTNAME}.gba
	mgba-qt ${OUTNAME}.gba

${OUTNAME}.gba: ${FILES}
	arm-none-eabi-ld -o ${OUTNAME}.out -T gcc.ls \
	${FILES} ${LIBGCC}
	arm-none-eabi-objcopy -O binary ${OUTNAME}.out ${OUTNAME}.bin
	mv ${OUTNAME}.bin ${OUTNAME}.gba

clean:
	rm -f *.o *.out *.sav *.gba

all:	clean emu

optusb:	${OUTNAME}.bin
	sudo optusb ${OUTNAME}.bin
