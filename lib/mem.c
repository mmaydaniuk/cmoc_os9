#include <os.h>

int spare;

asm void *
sbrk(int increase)
{
	asm
	{
memend EXTERNAL
_mtop EXTERNAL
_stbot EXTERNAL

        ldd     memend,y 
        pshs    d 
        ldd     2+2,s           get parameter
        cmpd    _spare,y        compare to spare
        bcs     L0035           branch if less than
        pshs    y               else save off data ptr
        clra   
        clrb   
        os9     F$Mem           D = 0, get current size and upper boundaries
        addd    2+2+2,s         get parameter again
        os9     F$Mem           ask for new size in D
        tfr     y,d             put address of new memory area upper bound in D
        puls    y               recover data ptr
        bcc     L0027           branch if success
        ldd     #-1             else error
        leas    2,s             recover stack
        rts                     return
L0027   std     memend,y        save new memory area upper bound in memend
        addd    _spare,y        add to spare
        subd    ,s              subtract saved memend at start of call
        std     _spare,y        and update spare to that
L0035   leas    2,s           
        ldd     _spare,y        get spare into D
        pshs    d               save on stack
        subd    2+2,s           subtract parameter
        std     _spare,y        store in spare
* clear newly allocated area
        ldd     memend,y        get memend
        subd    ,s++            subtract spare saved on stack
        pshs    d               save D on stack
        clra                    clear A
        ldx     ,s              get 2 bytes on stack into X
clr@    sta     ,x+             write 0 to location and increment
        cmpx    memend,y        end of memory?
        bcs     clr@            branch if not
        puls    d,pc 
     }
}

asm void *
ibrk(int increase)
{
	asm
	{
        ldd     2,s             get parameter
        addd    _mtop,y         add to top of memory
        bcs     ibrkerr         if higher, error out
        cmpd    _stbot,y        compare to bottom of stack
        bcc     ibrkerr         branch if greater or equal
        pshs    d               save off D
        ldx     _mtop,y         get mtop in X
        clra   
clr@    cmpx    ,s 
        bcc     L0076 
        sta     ,x+ 
        bra     clr@
L0076   ldd     _mtop,y         get current _mtop
        puls    x               get earlier saved value 
        stx     _mtop,y         new _mtop
        rts    
ibrkerr ldd     #-1             error
        rts    
    }
}

asm void *
unbrk(int increase)
{
	asm
	{
        ldd     2,s           get parameter in D
        pshs    y 
        os9     F$Mem 
        bcc     L0093 
        ldd     #-1 
        puls    y,pc 
L0093   tfr     y,d 
        puls    y 
        std     memend,y 
        clra   
        clrb   
        std     _spare,y 
        rts 
    }
}   
