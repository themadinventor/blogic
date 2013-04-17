CC = sdcc -mmcs51
BASENAME = blogic
CFLAGS = --disable-warning 197

$(BASENAME).hex: $(BASENAME).c
	$(CC) $(CFLAGS) -o $@ $(BASENAME).c

clean:
	bash -c "rm -f $(BASENAME).{asm,hex,lnk,lst,map,mem,rel,rst,sym,lk,cdb,omf}"
