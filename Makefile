TARGET		:= Minicraft
TITLE		:= CRFT00001
SOURCES		:= source source/minizip
INCLUDES	:= include
DATA		:= data
LIBS = -lvita2d -lSceAppMgr_stub -lSceLibKernel_stub -lSceSysmodule_stub -lSceCtrl_stub -lSceTouch_stub \
	-lm -lSceAppUtil_stub -lScePgf_stub -lSceAudio_stub \
	-ljpeg -lfreetype -lc -lScePower_stub -lSceCommonDialog_stub -lpng16 -lz \
	-lSceGxm_stub -lSceDisplay_stub

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
BINFILES	:= $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.png)))
OBJS     := $(BINFILES) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = -fcommon -g -Wl,-q -O3
CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-make-fself -s $< build/eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) "$(TARGET)" build/sce_sys/param.sfo
	cp -f build/sce_sys/param.sfo param.sfo
	cp -f build/eboot.bin eboot.bin
	vita-pack-vpk -s build/sce_sys/param.sfo -b build/eboot.bin \
		--add build/sce_sys/icon0.png=sce_sys/icon0.png \
		--add build/sce_sys/livearea=sce_sys/livearea $@

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@
	
%.png:
	@echo $@
	@$(bin2o)
	
$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS)
	
send: $(TARGET).vpk
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/CRFT00001/eboot.bin
	@echo "Sent."