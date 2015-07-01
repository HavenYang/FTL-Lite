
target = lite
rootdir = $(pwd)
dirs = common flash ftl test
cflag = -c -g -DSIM -m32
CC = gcc
hflag = $(addprefix -I$(rootdir),$(dirs))

src = $(foreach n,$(dirs),$(wildcard $(n)/*.c))
#src = $(wildcard test/*.c)

obj = $(patsubst %.c,%.o,$(src))

#all:
#	@echo $(src)

all:$(target)
	@echo done!!!

$(target):$(obj)
	@echo gen $@
	@$(CC) $^ $(hflag) -m32 -o $@

$(obj):%.o:%.c
	@echo compile $<
	@$(CC) $(cflag) $(hflag) $< -o $@


clean:
	rm -rf $(target) $(obj)
