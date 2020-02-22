
default:
	@echo "Compile for target processor with either 'make uarm' or 'make umps2'"

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