!:
::  /=main=/toy/cat/hoon
::
|=  *
|=  *
|=  ape=(list path)
:_  ~
:_  ~
:-  %%
!>  =|  tez=(list ,@t)
    |-  ^+  tez
    ?~  ape  (flop tez)
    =+  fix=((hard ,@) .^(%cx i.ape))
    |-  ^+  tez
    ?:  =(0 fix)
      ^$(ape t.ape)
    =+  ^=  meg
        =+  meg=0
        |-  ^-  @ud
        =+  gam=(cut 3 [meg 1] fix)
        ?:(|(=(10 gam) =(0 gam)) meg $(meg +(meg)))
    $(fix (rsh 3 +(meg) fix), tez [(end 3 meg fix) tez])
