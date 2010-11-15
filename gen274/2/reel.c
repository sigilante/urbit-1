/* j/2/reel.c
**
** This file is in the public domain.
*/
#include "all.h"
#include "../pit.h"

/* functions
*/
  u2_noun
  j2_mbc(Pit, reel)(u2_wire wir_r, u2_noun a, u2_noun b)
  {
    return u2_bl_bail(wir_r);
  }
  u2_noun
  j2_mb(Pit, reel)(u2_wire wir_r, u2_noun cor)
  {
    u2_noun a, b;

    u2_bl_yes(wir_r, u2_mean(cor, 8, &a, 9, &b, 0));
    {
      return j2_mbc(Pit, reel)(wir_r, a, b);
    }
  }

/* structures
*/
  u2_ho_jet 
  j2_mbj(Pit, reel)[] = {
    { ".3", j2_mb(Pit, reel), u2_yes, u2_none, u2_none },
    { }
  };
