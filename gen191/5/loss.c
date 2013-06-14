/* j/5/loss.c
**
** This file is in the public domain.
*/
#include "all.h"
#include "../pit.h"

/* functions
*/
  typedef struct _u2_loss {                 //  loss problem
    u2_noun hel;                            //  a as a list
    c3_w lel_w;                             //  length of a 
    c3_w lev_w;                             //  length of b
    u2_noun* hev;                           //  b as an array
    u2_noun sev;                            //  b as a set of lists
    c3_w kct_w;                             //  candidate count
    u2_noun* kad;                           //  candidate array
  } u2_loss;

  //  free loss object
  //
  static void 
  _flem(u2_loss* loc_u)
  {
    u2z(loc_u->sev);
    {
      c3_w i_w;

      for ( i_w = 0; i_w < loc_u->kct_w; i_w++ ) {
        u2z(loc_u->kad[i_w]);
      }
    }
    free(loc_u->hev);
    free(loc_u->kad);
  }

  //  extract lcs  -  XX don't use the stack like this
  //
  static u2_noun
  _lext(u2_loss* loc_u, u2_noun kad)
  {
    if ( u2_nul == kad ) {
      return u2_nul;
    } else {
      return u2nc(loc_u->hev[u2_cr_word(0, u2h(kad))], _lext(loc_u, u2t(kad)));
    }
  }

  //  extract lcs 
  //
  static u2_noun
  _lexs(u2_loss* loc_u)
  {
    if ( 0 == loc_u->kct_w ) {
      return u2_nul;
    } else return u2_ckb_flop(_lext(loc_u, loc_u->kad[loc_u->kct_w - 1]));
  }

  //  initialize loss object
  //
  static void
  _lemp(u2_loss* loc_u, 
        u2_noun  hel,                                             //  retain 
        u2_noun  hev)                                             //  retain
  {
    loc_u->hel = hel;
    loc_u->lel_w = u2_ckb_lent(u2k(hel));

    //  Read hev into array.
    {
      c3_w i_w;

      loc_u->hev = malloc(u2_ckb_lent(u2k(hev)) * sizeof(u2_noun));

      for ( i_w = 0; u2_nul != hev; i_w++ ) {
        loc_u->hev[i_w] = u2h(hev);
        hev = u2t(hev);
      }
      loc_u->lev_w = i_w;
    }
    loc_u->kct_w = 0;
    loc_u->kad = malloc(
                              (1 + c3_min(loc_u->lev_w, loc_u->lel_w)) *
                              sizeof(u2_noun));

    //  Compute equivalence classes.
    //
    loc_u->sev = u2_nul;
    {
      c3_w i_w;

      for ( i_w = 0; i_w < loc_u->lev_w; i_w++ ) {
        u2_noun how = loc_u->hev[i_w];
        u2_weak hav = u2_ckd_by_get(u2k(loc_u->sev), u2k(how));
        u2_noun teg = u2nc(u2_ci_words(1, &i_w), 
                          (hav == u2_none) ? u2_nul : hav);

        loc_u->sev = u2_ckd_by_put(loc_u->sev, u2k(how), teg);
      }
    }
  }

  //  apply
  //
  static void
  _lune(u2_loss* loc_u,
        c3_w     inx_w,
        c3_w     goy_w)
  {
    u2_noun kad;

    kad = u2nc(u2_ci_words(1, &goy_w),
               (inx_w == 0) ? u2_nul
                            : u2k(loc_u->kad[inx_w - 1]));
    if ( loc_u->kct_w == inx_w ) {
      loc_u->kct_w++;
    } else {
      u2z(loc_u->kad[inx_w]);
    }
    loc_u->kad[inx_w] = kad;
  } 

  //  apply list
  //
  static void
  _lunz(u2_loss* loc_u,                 
        u2_noun zes)                                              //  retain
  {
    while ( u2_nul != zes ) {
      u2_noun i_zes = u2h(zes);

      _lune(loc_u, u2_cr_word(0, u2h(i_zes)), u2_cr_word(0, u2t(i_zes)));
      zes = u2t(zes);
    }
  }

  //  extend fits top
  //
  static u2_bean
  _hink(u2_loss* loc_u,
        c3_w     inx_w,
        c3_w     goy_w)
  {
    return u2_say
         ( (loc_u->kct_w == inx_w) ||
           (u2_cr_word(0, u2h(loc_u->kad[inx_w])) > goy_w) );
  }

  //  extend fits bottom
  //
  static u2_bean
  _lonk(u2_loss* loc_u,
        c3_w     inx_w,
        c3_w     goy_w)
  {
    return u2_say
      ( (0 == inx_w) ||
        (u2_cr_word(0, u2h(loc_u->kad[inx_w - 1])) < goy_w) );
  }

  //  last index >= inx_w, <= max_w, that lonks goy_w.  or u2_no if none.
  //
  static u2_bean 
  _lunk(u2_loss* loc_u,
        c3_w     *inx_w,
        c3_w     max_w, 
        c3_w     goy_w)
  {
    while ( 1 ) {
      if ( max_w <= (3 + *inx_w) ) {
        u2_noun ret   = u2_no;
        c3_w    rex_w, cur_w;
     
        rex_w = cur_w = *inx_w;
        while ( (cur_w <= max_w) && (u2_yes == _lonk(loc_u, cur_w, goy_w)) ) {
          ret = u2_yes;
          rex_w = cur_w; 
          cur_w++;
        }
        *inx_w = rex_w;
        return ret;
      } else {
        c3_w dif_w = (max_w - *inx_w);
        c3_w med_w = *inx_w + (dif_w / 2);

        if ( u2_yes == _lonk(loc_u, med_w, goy_w) ) {
          *inx_w = med_w;
        } else {
          max_w = med_w;
        }
      }
    }
  }

  //  first index >= inx_w, <= max_w, that hinks goy_w.  or u2_no if none.
  //
  static u2_bean
  _hunk(u2_loss* loc_u,
        c3_w*    inx_w,
        c3_w     max_w,
        c3_w     goy_w)
  {
    while ( 1 ) {
      if ( max_w <= (3 + *inx_w) ) {
        while ( *inx_w <= max_w ) { 
          if ( u2_yes == _hink(loc_u, *inx_w, goy_w) ) {
            return u2_yes;
          }
          else ++*inx_w;
        }
        return u2_no;
      } else {
        c3_w dif_w = (max_w - *inx_w);
        c3_w med_w = *inx_w + (dif_w / 2);

        if ( u2_yes == _hink(loc_u, med_w, goy_w) ) {
          max_w = med_w;
        } else {
          *inx_w = med_w;
        }
      }
    }
  }
  
  //  search for first index >= inx_w and <= max_w that fits
  //  the hink and lonk criteria.
  //
  static u2_bean
  _bink(u2_loss* loc_u,
        c3_w*    inx_w,
        c3_w     max_w,
        c3_w     goy_w)
  {
#if 0
    while ( *inx_w <= max_w ) {
      if ( u2_no == _lonk(loc_u, *inx_w, goy_w) ) {
        return u2_no;
      }
      if ( u2_yes == _hink(loc_u, *inx_w, goy_w) ) {
        return u2_yes;
      }
      else ++*inx_w;
    }
    return u2_no;
#else
    u2_noun bot;
    c3_w    box_w;

    bot = _lunk(loc_u, &box_w, max_w, goy_w);
    if ( u2_no == bot ) {
      return u2_no;
    } else {
      return _hunk(loc_u, inx_w, box_w, goy_w);
    }
#endif
  }

#if 0
  //  this should work perfectly but doesn't for some reason
  //
  static void
  _grim(u2_loss* loc_u,
        c3_w     inx_w,
        u2_noun  gay)
  {
    if ( (u2_nul == gay) || (inx_w > loc_u->kct_w) ) {
      return;
    }
    else {
      u2_noun i_gay = u2h(gay); 
      c3_w    goy_w = u2_cr_word(0, i_gay);
      u2_noun bik;

      bik = _bink(loc_u, &inx_w, loc_u->kct_w, goy_w);
      
      if ( u2_yes == bik ) {
        _grim(loc_u, inx_w + 1, u2t(gay));
        _lune(loc_u, inx_w, goy_w);
      }
      else {
        _grim(loc_u, inx_w, u2t(gay));
      }
    }
  }
#endif

  //  merge
  //
  static void
  _merg(u2_loss* loc_u,
        u2_noun  gay)                                             //  retain
  {
    u2_noun zes = u2_nul;
    c3_w    inx_w = 0;

    while ( (u2_nul != gay) && (inx_w <= loc_u->kct_w) ) {
      u2_noun i_gay = u2h(gay); 
      c3_w    goy_w = u2_cr_word(0, i_gay);
      u2_noun bik;

      bik = _bink(loc_u, &inx_w, loc_u->kct_w, goy_w);
      
      if ( u2_yes == bik ) {
        zes = u2nc(u2nc(u2_ci_words(1, &inx_w), u2k(i_gay)), zes);
        inx_w++;
      }
      gay = u2t(gay);
    }
    _lunz(loc_u, zes);
    u2z(zes);
  }

  
  //  compute lcs
  //
  static void
  _loss(u2_loss* loc_u)
  {
    while ( u2_nul != loc_u->hel ) {
      u2_noun i_hel = u2h(loc_u->hel);
      u2_weak guy   = u2_ckd_by_get(u2k(loc_u->sev), u2k(i_hel));

      if ( u2_none != guy ) {
        u2_noun gay = u2_ckb_flop(u2k(guy));
         
        _merg(loc_u, gay);
        // _grim(loc_u, gay, 0);
        u2z(gay);
      }

      loc_u->hel = u2t(loc_u->hel);
    }
  }

  u2_noun                                                         //  produce
  j2_mbc(Pt5, loss)(u2_wire wir_r, 
                    u2_noun hel,                                  //  retain
                    u2_noun hev)                                  //  retain
  {
    u2_loss loc_u;
    u2_noun lcs;
 
    _lemp(&loc_u, hel, hev);
    _loss(&loc_u);
    lcs = _lexs(&loc_u);

    _flem(&loc_u);
    return lcs;
  }

  static u2_bean                                                
  _listp(u2_noun lix)                                             //  retain
  {
    while ( 1 ) { 
      if ( u2_nul == lix ) return u2_yes;
      if ( u2_no == u2du(lix) ) return u2_no;
      lix = u2t(lix);
    }
  }

  u2_weak                                                         //  produce
  j2_mb(Pt5, loss)(u2_wire wir_r, 
                   u2_noun cor)                                   //  retain
  {
    u2_noun hel, hev;

    if ( (u2_none == (hel = u2_frag(u2_cv_sam_2, cor))) ||
         (u2_none == (hev = u2_frag(u2_cv_sam_3, cor))) ||
         (u2_no == _listp(hel)) ||
         (u2_no == _listp(hev)) ) 
    {
      return u2_bl_bail(wir_r, c3__fail);
    } else {
      return j2_mbc(Pt5, loss)(wir_r, hel, hev);
    }
  }


/* structures
*/
  u2_ho_jet 
  j2_mbj(Pt5, loss)[] = { 
    { ".2", c3__lite, j2_mb(Pt5, loss), u2_jet_live | u2_jet_test, u2_none, u2_none },
    { }
  };
