
target = ftl-lite
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
	echo $< compile done

$(target):$(obj)
	$(CC) $^ $(hflag) -m32 -o $@

$(obj):%.o:%.c
	$(CC) $(cflag) $(hflag) $< -o $@


clean:
	rm -rf $(target) $(obj)
