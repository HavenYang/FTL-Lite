
target = ftl-lite
rootdir = $(pwd)
dirs = common flash ftl test
cflag = -c -g
hflag = $(addprefix -I$(rootdir),$(dirs))

src = $(foreach n,$(dirs),$(wildcard $(n)/*.c))
#src = $(wildcard test/*.c)

obj = $(patsubst %.c,%.o,$(src))

#all:
#	@echo $(src)

all:$(target)
	echo $< compile done

$(target):$(obj)
	$(CC) $^ $(hflag) -o $@

$(obj):%.o:%.c
	$(CC) $(cflag) $(hflag) $< -o $@


clean:
	rm -rf $(target) $(obj)
