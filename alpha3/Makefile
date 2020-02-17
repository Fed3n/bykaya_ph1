
default:
	@echo "Please specify a target"

umps:
	$(MAKE) -f umps_make
	
umps2:
	$(MAKE) -f umps_make

uarm:
	$(MAKE) -f uarm_make

clean:
	$(MAKE) -f uarm_make clean
	$(MAKE) -f umps_make clean

.PHONY: umps umps2 uarm clean default