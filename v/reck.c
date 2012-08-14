/* v/reck.c
**
**  This file is in the public domain.
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <setjmp.h>
#include <gmp.h>
#include <dirent.h>
#include <stdint.h>
#include <ev.h>

#include "all.h"
#include "f/coal.h"
#include "v/vere.h"

  /*  ::  |%  
  **  ::  ++  node
  **  ::    $%  [& p=time]                           ::  file/mtime
  **  ::        [| p=(map span node)]                ::  directory
  **  ::    ==
  **  ::  --
  */

/* _sync_load(): load file or bail.
*/
static u2_noun
_sync_load(c3_c* pas_c)
{
  struct stat buf_b;
  c3_i        fid_i = open(pas_c, O_RDONLY, 0644);
  c3_w        fln_w, red_w;
  c3_y*       pad_y;

  if ( (fid_i < 0) || (fstat(fid_i, &buf_b) < 0) ) {
    perror(pas_c);
    return u2_cm_bail(c3__fail);
  }
  fln_w = buf_b.st_size;
  pad_y = malloc(buf_b.st_size);

  red_w = read(fid_i, pad_y, fln_w);
  close(fid_i);

  if ( fln_w != red_w ) {
    free(pad_y);
    return u2_cm_bail(c3__fail);
  }
  else {
    u2_noun pad = u2_ci_bytes(fln_w, (c3_y *)pad_y); 
    free(pad_y);

    return pad;
  }
}

/* _sync_save(): save file or bail.
*/
static void
_sync_save(c3_c* pas_c, u2_noun tim, u2_atom pad)
{
  c3_i  fid_i = open(pas_c, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  c3_w  fln_w, rit_w;
  c3_y* pad_y;

  if ( fid_i < 0 ) {
    perror(pas_c);
    u2_cm_bail(c3__fail);
  }

  fln_w = u2_met(3, pad);
  pad_y = malloc(fln_w);
  u2_cr_bytes(0, fln_w, pad_y, pad);
  u2z(pad);

  rit_w = write(fid_i, pad_y, fln_w);
  close(fid_i);
  free(pad_y);

  if ( rit_w != fln_w ) {
    perror(pas_c);
    u2_cm_bail(c3__fail);
  }

  {
    struct timeval tim_tv[2];

    u2_time_out_tv(&tim_tv[0], u2k(tim));
    u2_time_out_tv(&tim_tv[1], tim);
 
    utimes(pas_c, tim_tv);
  }
}

/* _reck_root(): tool from boot.
*/
static u2_noun
_reck_root(const c3_c *txt_c, u2_noun ken)
{
  c3_c *ful_c = alloca(7 + strlen(txt_c));

  strcpy(ful_c, "=>  !%  ");
  strcat(ful_c, txt_c);

  return u2_cn_nock(0, u2_cn_nock(u2_ci_string(ful_c), u2k(ken)));
}

/* _reck_ream(): ream with toy.
*/
static u2_noun
_reck_ream(u2_reck* rec_u, u2_noun txt)
{
  return u2_cn_mung(u2k(rec_u->toy.ream), txt);
}

/* _reck_slam(): slam with toy.
*/
static u2_noun
_reck_slam(u2_reck* rec_u, u2_noun gat, u2_noun sam)
{
  return u2_cn_mung(u2k(rec_u->toy.slam), u2nc(gat, sam));
}

/* _reck_slap(): slap with toy.
*/
static u2_noun
_reck_slap(u2_reck* rec_u, u2_noun vax, u2_noun gen)
{
  return u2_cn_mung(u2k(rec_u->toy.slap), u2nc(vax, gen));
}

/* _reck_slop(): slop with toy.
*/
static u2_noun
_reck_slop(u2_reck* rec_u, u2_noun hed, u2_noun tal)
{
  return u2_cn_mung(u2k(rec_u->toy.slop), u2nc(hed, tal));
}

/* _reck_hard(): function against vase, producing noun.
*/
static u2_noun
_reck_hard(u2_reck* rec_u, u2_noun vax, const c3_c* txt_c, u2_noun sam)
{
  u2_noun gen = _reck_ream(rec_u, u2_ci_string(txt_c));
  u2_noun gat = _reck_slap(rec_u, vax, gen);
  u2_noun ret = u2_cn_mung(u2k(u2t(gat)), sam);

  u2z(gat);
  return ret;
}

/* _reck_soft(): function against vase, producing vase.
*/
static u2_noun
_reck_soft(u2_reck* rec_u, u2_noun vax, const c3_c* txt_c, u2_noun sam)
{
  u2_noun gen = _reck_ream(rec_u, u2_ci_string(txt_c));
  u2_noun gat = _reck_slap(rec_u, vax, gen);

  return _reck_slam(rec_u, gat, sam);
}

/* _reck_load(): layer file on vase -> vase.
*/
static u2_noun
_reck_load(u2_reck* rec_u, u2_noun vax, c3_c* pax_c)
{
  u2_noun txt = _sync_load(pax_c);
  u2_noun gen = _reck_ream(rec_u, txt);

  return _reck_slap(rec_u, vax, gen);
}

/* _reck_load_temp(): _reck_load() for old fs structure.
*/
static u2_noun
_reck_load_temp(u2_reck* rec_u, u2_noun vax, c3_w kno_w, c3_c* pax_c)
{
  c3_c ful_c[2048];
 
  sprintf(ful_c, "%s/sys/%d/%s", u2_Local, kno_w, pax_c);
  return _reck_load(rec_u, vax, ful_c);
}

/* _reck_time_set(): set the reck time.
*/
static void
_reck_time_set(u2_reck* rec_u)
{
  struct timeval tim_tv;

  gettimeofday(&tim_tv, 0);
  u2z(rec_u->now);
  rec_u->now = u2_time_in_tv(&tim_tv);

  u2z(rec_u->wen);
  rec_u->wen = _reck_hard
    (rec_u, 
     u2k(rec_u->syd), 
     "|=([a=@ta b=@] ~(rent co ~ a b))",
     u2nc(c3_s2('d','a'), u2k(rec_u->now)));

  {
    c3_c* dyt_c = u2_cr_string(rec_u->wen);

    printf("time: %s\n", dyt_c);
    free(dyt_c);
  }
}

/* u2_reck_init(): load the reck engine, from kernel.
*/
void
u2_reck_init(u2_reck* rec_u, c3_w kno_w, u2_noun ken)
{
  rec_u->kno_w = kno_w;
  rec_u->rno_w = 0;

  rec_u->ken = ken;
  rec_u->syd = _reck_root("seed", u2k(ken));

  rec_u->toy.ream = _reck_root("ream", u2k(ken));
  rec_u->toy.slam = _reck_root("slam", u2k(ken));
  rec_u->toy.slap = _reck_root("slap", u2k(ken));
  rec_u->toy.slop = _reck_root("slop", u2k(ken));

  _reck_time_set(rec_u);
  {
    u2_noun cer = _reck_load_temp(rec_u, u2k(rec_u->syd), kno_w, "reck.watt");
    u2_noun sam = u2nc(u2nc(c3__atom, 0), u2k(rec_u->now));

    rec_u->rec = _reck_slam(rec_u, cer, sam);
  }
}

static u2_noun
_reck_peek(u2_reck* rec_u, u2_noun hap)
{
  return _reck_hard(rec_u, u2k(rec_u->rec), "peek", u2nc(u2k(rec_u->now), hap));
}

/* _walk_in(): inner loop of _walk().
*/
static u2_noun
_walk_in(const c3_c* dir_c, c3_w len_w)
{
  DIR*    dir_d = opendir(dir_c);
  u2_noun map = u2_nul;

  if ( !dir_d ) {
    return u2_nul;
  }
  else while ( 1 ) {
    struct dirent  ent_n;
    struct dirent* out_n;

    if ( readdir_r(dir_d, &ent_n, &out_n) != 0 ) {
      perror(dir_c);
      break;
    } 
    else if ( !out_n ) {
      break;
    }
    else if ( !strcmp(out_n->d_name, ".") || !strcmp(out_n->d_name, "..") ) {
      continue;
    }
    else {
      c3_c* fil_c = out_n->d_name;
      c3_w  lef_w = len_w + 1 + strlen(fil_c);
      c3_c* pat_c = malloc(lef_w + 1);
      c3_c* dot_c;
   
      strcpy(pat_c, dir_c);
      pat_c[len_w] = '/';
      strcpy(pat_c + len_w + 1, fil_c);

      if ( (dot_c = strrchr(fil_c, '.')) ) {
        /*  all files must have extensions
        */
        c3_c*       nam_c = strdup(fil_c);
        c3_c*       ext_c = strdup(dot_c + 1);
        struct stat buf_b;

        nam_c[dot_c - fil_c] = 0;

        if ( 0 == stat(pat_c, &buf_b) ) {
          u2_noun tim = u2_time_in_ts(&buf_b.st_mtimespec);
          u2_noun nam = u2_ci_string(nam_c);
          u2_noun ext = u2_ci_string(ext_c);
          u2_noun get = u2_ckd_by_get(u2k(map), u2k(nam));

          if ( u2_none == get ) { get = u2_nul; }
         
          get = u2_ckd_by_put(get, ext, u2nc(u2_yes, tim));
          map = u2_ckd_by_put(map, nam, u2nc(u2_no, get));
        }
        free(nam_c);
        free(ext_c);
      }
      else {
        /*  all nonfiles are directories
        */
        u2_noun dir = _walk_in(pat_c, lef_w);

        if ( u2_nul != dir ) {
          map = u2_ckd_by_put(map, u2_ci_string(fil_c), u2nc(u2_no, dir));
        }
      }
      free(pat_c);
    }
  }
  return map;
}

/* _walk(): traverse `dir_c` to produce a node.
*/
static u2_noun
_walk(const c3_c* dir_c)
{
  return u2nc(u2_no, _walk_in(dir_c, strlen(dir_c)));
}

/* _sync_path(): cary path from raw path.
*/
static u2_noun
_sync_path(u2_reck* rec_u, u2_flag rey, c3_c how_c, u2_noun hap)
{
  u2_noun baz, bok, ven, lim;
  u2_noun cay;

  if ( u2_yes == rey ) {
    u2_cx_qual(hap, &baz, &bok, &ven, &lim);
    u2k(baz); u2k(bok); u2k(ven); u2k(lim);
  } else {
    u2_cx_trel(hap, &baz, &bok, &lim);
    u2k(baz); u2k(bok); u2k(lim);
    ven = u2k(rec_u->wen);
  }

  cay = u2nq(c3__cary, baz, bok, u2nt(ven, how_c, lim));
  u2z(hap);
  return cay;
}

/* _sync_peek_meta(): (unit meta) for cary-path.
*/
static u2_noun
_sync_peek_meta(u2_reck* rec_u, u2_flag rey, u2_noun hac)
{
  u2_noun pah = _sync_path(rec_u, rey, 'm', hac);

  return _reck_peek(rec_u, pah);
}

/* _sync_peek_data(): (unit @) for cary-path.
*/
static u2_noun
_sync_peek_data(u2_reck* rec_u, u2_flag rey, u2_noun hac)
{
  return _reck_peek(rec_u, _sync_path(rec_u, rey, 'd', hac));
}

/* _sync_unix(): unix path as by dir/file.ext or dir/subdir protocol.
*/
static c3_c*
_sync_unix(u2_flag dir, u2_flag rey, u2_noun hac)
{
  u2_noun baz, bok, rev, lim;
  c3_w len_w;
  c3_c *pas_c;

  //  normalize
  {
    u2_noun nor;

    if ( u2_no == rey ) {
      u2_cx_trel(hac, &baz, &bok, &lim);
      nor = u2nq(u2k(baz), u2k(bok), 'e', u2k(lim));
    } else {
      u2_cx_qual(hac, &baz, &bok, &rev, &lim);
      nor = u2nq(u2k(baz), u2k(bok), 'r', u2nc(u2k(rev), u2k(lim)));
    }
    u2z(hac);
    hac = nor;
  }

  //  measure
  //
  len_w = strlen(u2_Local) + 4;
  {
    u2_noun wiz = hac;

    while ( u2_nul != wiz ) {
      len_w += (1 + u2_cr_met(3, u2h(wiz)));
      wiz = u2t(wiz);
    }
  }

  //  cut
  //
  pas_c = malloc(len_w + 1);
  strcpy(pas_c, u2_Local);
  strcat(pas_c, "/car");
  {
    u2_noun wiz   = hac;
    c3_c*   waq_c = (pas_c + strlen(pas_c));

    while ( u2_nul != wiz ) {
      c3_w tis_w = u2_cr_met(3, u2h(wiz));

      if ( (u2_no == dir) && (u2_nul == u2t(wiz)) ) {
        *waq_c++ = '.';
      } else *waq_c++ = '/';

      u2_cr_bytes(0, tis_w, (c3_y*)waq_c, u2h(wiz));
      waq_c += tis_w;

      wiz = u2t(wiz);
    }
    *waq_c = 0;
  }
  u2z(hac);
  return pas_c;
}

/* _sync_file(): from path to C file.ext style path.
*/
static c3_c*
_sync_file(u2_flag rey, u2_noun hac)
{
  return _sync_unix(u2_no, rey, hac);
}

/* _sync_dire(): from path to C directory style path.
*/
static c3_c*
_sync_dire(u2_flag rey, u2_noun hac)
{
  return _sync_unix(u2_yes, rey, hac);
}

/* _sync_flat(): load unix file (or crash) from cary path.
*/
static u2_noun
_sync_flat(u2_flag rey, u2_noun hac)
{
  c3_c*   pas_c = _sync_file(rey, hac);
  u2_noun pad   = _sync_load(pas_c);

  free(pas_c);
  return pad;
}

/* _sync_make(): make sure directory is there.
*/
static void
_sync_make(u2_flag rey, u2_noun hac)
{
  c3_c* pas_c = _sync_file(rey, hac);

  mkdir(pas_c, 0777);
  free(pas_c);
}

/* _sync_push(): save unix file (or crash) from cary path, setting time.
*/
static void
_sync_push(u2_flag rey, u2_noun hac, u2_atom tim, u2_noun pad)
{
  c3_c* pas_c = _sync_file(rey, hac);

  _sync_save(pas_c, tim, pad);
  free(pas_c);
}

/* _sync_lose(): delete unix file.
*/
static void
_sync_lose(u2_flag rey, u2_noun hac)
{
  c3_c* pas_c = _sync_file(rey, hac);

  unlink(pas_c);
  free(pas_c);
}

/* _sync_give_m(): _sync_give() in map.
*/
static u2_noun _sync_give(u2_reck*, u2_noun, u2_noun, u2_noun);

static u2_noun
_sync_give_m(u2_reck* rec_u, u2_noun rah, u2_noun map, u2_noun det)
{
  if ( u2_nul == map ) {
    u2z(rah);
    return det;
  }
  else {
    u2_noun n_map, pn_map, qn_map, l_map, r_map;

    u2_cr_trel(map, &n_map, &l_map, &r_map);
    u2_cr_cell(n_map, &pn_map, &qn_map);

    det = _sync_give_m(rec_u, u2k(rah), u2k(l_map), det);
    det = _sync_give_m(rec_u, u2k(rah), u2k(r_map), det);
    det = _sync_give(rec_u, u2nc(u2k(pn_map), rah), u2k(qn_map), det);

    u2z(map);
    return det;
  }
}

/* _sync_give(): pure checkin (memory data does not exist).
*/
static u2_noun
_sync_give(u2_reck* rec_u, u2_noun rah, u2_noun nod, u2_noun det)
{
  if ( u2_yes == u2h(nod) ) {
    u2_noun hac = u2_ckb_flop(rah);
    u2_noun pad = _sync_flat(u2_no, u2k(hac));

    det = u2nc(u2nc(c3_s2('p','i'),
                    u2nt(hac, c3_s2('g', 'v'), pad)),
               det);
    u2z(nod);
    return det;
  }
  else {
    u2_noun map = u2k(u2t(nod));

    det = _sync_give_m(rec_u, rah, map, det);
    u2z(nod);
    return det;
  }
}

/* _sync_take(): pure checkout (fs data does not exist).
*/
static void
_sync_take(u2_reck* rec_u, u2_noun rey, u2_noun rah)
{
  u2_noun hac = u2_ckb_flop(rah);
  u2_noun mey = _sync_peek_meta(rec_u, u2_no, u2k(hac));

  if ( u2_nul == mey ) {
    u2_cm_bail(c3__fail);
  } else {
    u2_noun yem = u2k(u2t(mey));

    u2z(mey); mey = yem;
  }

  if ( u2_yes == u2h(mey) ) {
    u2_noun tim = u2k(u2t(mey));
    u2_noun pad   = _sync_peek_data(rec_u, rey, u2k(hac));

    _sync_push(rey, hac, tim, pad);
  } 
  else {
    u2_noun hac = u2_ckb_flop(u2k(rah));
    u2_noun n;

    _sync_make(rey, hac);

    for ( n=u2t(mey); u2_nul != n; n=u2t(n) ) {
      u2_noun i = u2h(n);   //  span

      _sync_take(rec_u, rey, u2nc(u2k(i), u2k(rah)));
    }
    u2z(rah);
  }
}

/* _sync_live(): sync in editable directory.
*/
static u2_noun
_sync_live(u2_reck* rec_u, u2_noun rah, u2_noun nod, u2_noun det)
{
  u2_noun hac = u2_ckb_flop(u2k(rah));
  u2_noun mey = _sync_peek_meta(rec_u, u2_no, u2k(hac));
  u2_flag end;

  //  simpler path for pure checkin
  {
    if ( u2_nul == mey ) {
      u2z(hac);
      return _sync_give(rec_u, rah, nod, det);
    }
    else {
      u2_noun yem = u2k(u2t(mey));

      u2z(mey); mey = yem;
    }
  }

  //  dir/file must match, please don't fsck this up for now
  {
    if ( u2h(nod) != u2h(mey) ) {
      //  XX  error
      printf("yo, sync mismatch\n");
      u2z(rah); u2z(hac); u2z(mey);
      return det;
    }
    else end = u2h(nod);
  }

  if ( u2_yes == end ) {
    c3_d nod_d = u2_cr_chub(0, u2t(nod));
    c3_d mey_d = u2_cr_chub(0, u2t(mey));
    
    if ( nod_d == mey_d ) {                             //  already in sync
      u2z(hac);
    }
    else if ( nod_d > mey_d ) {                         //  unix file is newer
      u2_noun pad = _sync_flat(u2_no, u2k(hac));

      det = u2nc(u2nc(c3_s2('p','o'),
                      u2nt(hac, c3_s2('g', 'v'), pad)),
                 det);
    }
    else {                                              //   cary is newer
      u2_noun pad = _sync_peek_data(rec_u, u2_no, u2k(hac));
        
      if ( u2_nul == pad ) {
        printf("%s missing & obsolete!\n", _sync_file(u2_no, u2k(hac))); 
      } else {
        printf("%s is obsolete!\n", _sync_file(u2_no, u2k(hac)));

        _sync_push(u2_no, u2k(hac), mey_d, u2k(u2t(pad)));
        u2z(pad);
      }
      u2z(hac);
    }
  }
  else {
    u2_noun n, yip, wam;

    //  we are only interested in the span list & map
    {
      u2_noun sil = u2k(u2t(u2t(mey)));
      u2_noun dun = u2k(u2t(nod));

      u2z(mey); u2k(nod);
      mey = sil;
      nod = dun;
    }

    //  we also be wanting a map - and a unix list
    // 
    yip = u2_ckd_in_tap(u2k(u2t(nod)), u2_nul);
    wam = u2_ckd_in_gas(u2k(mey), u2_nul);

    //  yip == (list ,[span node]) of unix files
    //  nod == map of unix files
    //  wam == set of cary files
    //  mey == (list span) of cary files

    //  iterate by unix files
    //
    for ( n=yip; u2_nul != n; n=u2t(n) ) {
      u2_noun i = u2h(n);   //  [span node]

      if ( u2_no == u2_ckd_in_has(u2k(wam), u2k(u2h(i))) ) {
        det = _sync_give
          (rec_u, u2nc(u2k(u2h(i)), u2k(rah)), u2k(u2t(i)), det);
      } else {
        det = _sync_live
          (rec_u, u2nc(u2k(u2h(i)), u2k(rah)), u2k(u2t(i)), det);
      }
    }

    //  iterate by cary files - should not be needed (but is).
    //
    for ( n=mey; u2_nul != n; n=u2t(n) ) {
      u2_noun i = u2h(n);

      if ( u2_no == u2_ckd_in_has(u2k(nod), u2k(i)) ) {
        printf("%s is missing!\n", 
              _sync_dire(u2_no, u2_ckb_flop(u2nc(u2k(i), u2k(rah)))));

        _sync_take(rec_u, u2_no, u2nc(u2k(i), u2k(rah)));
      }
    }
    u2z(yip);
    u2z(wam);
  }
  u2z(rah); u2z(hac); u2z(mey);
  return det;
}

/* _sync_form_m():
*/
static u2_noun
_sync_form_m(u2_reck* rec_u, u2_noun baz, u2_noun bok, u2_noun map, u2_noun det)
{
  if ( u2_nul == map ) {
    u2z(baz); u2z(bok);
    return det;
  }
  else {
    u2_noun n_map, pn_map, qn_map, l_map, r_map;
    u2_noun rah;

    u2_cr_trel(map, &n_map, &l_map, &r_map);
    u2_cr_cell(n_map, &pn_map, &qn_map);

    if ( ('e' != pn_map) ) {
      u2z(map); u2z(baz); u2z(bok);

      return det;               //  XX version checkout not yet supported
    }
   
    rah = u2nt(bok, baz, u2_nul);

    det = _sync_form_m(rec_u, u2k(baz), u2k(bok), u2k(l_map), det);
    det = _sync_form_m(rec_u, u2k(baz), u2k(bok), u2k(r_map), det);
    det = _sync_live(rec_u, rah, u2k(qn_map), det);

    u2z(map);
    return det;
  }
}

/* _sync_book(): sync `nod` as a project change to `baz`, `bok`.
*/
static u2_noun
_sync_book(u2_reck* rec_u, u2_noun baz, u2_noun bok, u2_noun nod, u2_noun det)
{
  if ( u2_yes == u2h(nod) ) {
    //  XX  user error
    u2z(nod); u2z(bok); u2z(baz);
    return det;
  }
  else {
    det = _sync_form_m(rec_u, baz, bok, u2k(u2t(nod)), det);
    u2z(nod);

    return det;
  }
}

/* _sync_book_m(): sync `map` to a change list `det`.
*/
static u2_noun
_sync_book_m(u2_reck* rec_u, u2_noun baz, u2_noun map, u2_noun det)
{
  if ( u2_nul == map ) {
    u2z(baz);
    return det;
  }
  else {
    u2_noun n_map, pn_map, qn_map, l_map, r_map;

    u2_cr_trel(map, &n_map, &l_map, &r_map);
    u2_cr_cell(n_map, &pn_map, &qn_map);

    det = _sync_book_m(rec_u, u2k(baz), u2k(l_map), det);
    det = _sync_book_m(rec_u, u2k(baz), u2k(r_map), det);
    det = _sync_book(rec_u, baz, u2k(pn_map), u2k(qn_map), det);

    u2z(map);
    return det;
  }
}

/* _sync_base(): sync `nod` as a base change.
*/
static u2_noun
_sync_base(u2_reck* rec_u, u2_noun baz, u2_noun nod, u2_noun det)
{
  if ( (u2_nul == nod) || (u2_yes == u2h(nod)) ) {
    u2z(baz);
    u2z(nod);
    return det;
  }
  else {
    det = _sync_book_m(rec_u, baz, u2k(u2t(nod)), det);
    u2z(nod);
    return det;
  }
}

/* _sync_base_m(): sync `map` to a change list `det`.
*/
static u2_noun
_sync_base_m(u2_reck* rec_u, u2_noun map, u2_noun det)
{
  if ( u2_nul == map ) {
    return det;
  }
  else {
    u2_noun n_map, pn_map, qn_map, l_map, r_map;

    u2_cx_trel(map, &n_map, &l_map, &r_map);
    u2_cx_cell(n_map, &pn_map, &qn_map);

    det = _sync_base(rec_u, u2k(pn_map), u2k(qn_map), det);
    det = _sync_base_m(rec_u, u2k(l_map), det);
    det = _sync_base_m(rec_u, u2k(r_map), det);

    u2z(map);
    return det;
  }
}

/* _reck_sync(): traverse filesystem to commit changes -> lamb
*/
static u2_noun
_reck_sync(u2_reck* rec_u)
{
  c3_c*   pas_c = malloc(strlen(u2_Local) + 1 + 3 + 1);
  u2_noun nod; 

  strcpy(pas_c, u2_Local);
  strcat(pas_c, "/car");
 
  nod = _walk(pas_c);
  free(pas_c);

  if ( (u2_nul == nod) || (u2_yes == u2h(nod)) ) {
    u2z(nod);
    return u2_nul;
  }
  else {
    u2_noun det = u2_nul;
    u2_noun map = u2t(nod);
    
    det = _sync_base_m(rec_u, u2k(map), det);
    u2z(nod);

    {
      u2_noun dut = det;
      u2_noun lam = u2_nul;

      while ( u2_nul != dut ) {
        lam = u2nc(u2nc(c3__cary, u2k(u2h(dut))), lam);
        dut = u2t(dut);
      }
      u2z(det);

      return u2nc('o', lam);
    }
  }
}

/* _reck_kiwi(): apply reck output.
*/
void
_reck_kiwi(u2_reck* rec_u, u2_noun veb)
{
  u2_noun p_veb, q_veb, r_veb;

  if ( u2_no == u2du(veb) ) {
    u2_err(u2_Wire, "kiwi", veb);
    u2_cm_bail(c3__fail);
  } 
  else {
    switch ( u2h(veb) ) {
      case 'p': {
        u2_cx_trel(u2t(veb), &p_veb, &q_veb, &r_veb);

        u2_ve_tank(u2k(q_veb), u2k(r_veb));
        break;
      }
      case 't': {
        u2_ve_sway(2, u2k(u2t(veb)));
        break;
      }
    }
  }
  u2z(veb);
}

/* _reck_kiwis(): apply a kiwi list.
*/
static void
_reck_kiwis(u2_reck *rec_u, u2_noun vyl)
{
  if ( u2_no != u2du(vyl) ) {
    _reck_kiwi(rec_u, u2k(u2h(vyl)));
    _reck_kiwis(rec_u, u2k(u2t(vyl)));
    u2z(vyl);
  }
}

/* _reck_gall(): apply a reck result.
*/
static void
_reck_gall(u2_reck* rec_u, u2_noun gax)
{
  u2_noun hix, pux;

  hix = _reck_slap(rec_u, u2k(gax), u2nc(c3__cnbc, 'p'));
  pux = _reck_slap(rec_u, gax, u2nc(c3__cnbc, 'q'));

  _reck_kiwis(rec_u, u2k(u2t(hix)));
  u2z(hix);

  u2z(rec_u->rec);
  rec_u->rec = pux; 
}

/* _reck_poke(): poke with raw lamb, apply result.
*/
static void
_reck_poke(u2_reck* rec_u, u2_noun lam)
{
  {
    u2_noun sam = _reck_slop(rec_u, u2nc(u2nc(c3__atom, 0), u2k(rec_u->now)),
                                    u2nc(c3__noun, lam));
    u2_noun gax;

    gax = _reck_soft(rec_u, u2k(rec_u->rec), "poke", sam);
    _reck_gall(rec_u, gax);
  }
}

/* u2_reck_line(): apply a reck line (protected).
*/
void
u2_reck_line(u2_reck* rec_u, u2_noun lin)
{
  static c3_w seq_w = 1;

  _reck_time_set(rec_u);
  {
    u2_noun lam = u2nq('l', u2nc(0, 0), seq_w, lin);

    _reck_poke(rec_u, lam);
    seq_w++;
  }
}

/* u2_reck_boot(): boot the reck engine (unprotected).
*/
void
u2_reck_boot(u2_reck* rec_u)
{
  u2_noun hoe;

  memset(rec_u, 0, sizeof *rec_u);
  u2_cm_trip();
  if ( 0 != (hoe = u2_cm_trap()) ) {
    u2_cm_purge();
    u2_ve_grab(hoe, 0);

    u2_ve_wine(u2k(u2h(hoe)));
    u2_ve_sway(2, u2_ckb_flop(u2k(u2t(hoe))));
    u2z(hoe);
  } 
  else {
    printf("loading reck...\n");
    u2_reck_init(rec_u, 
                 u2_Host.kno_w, 
                 u2k(u2_Host.ver_e[u2_Host.kno_w].ken));
    printf("loaded reck.\n");

    /* initial sync with filesystem
    */
    {
      u2_noun lam = _reck_sync(rec_u);

      _reck_poke(rec_u, lam);
    }

    u2_cm_done();
  
    u2_cm_purge();
    if ( (u2_yes == u2_Flag_Garbage) || (u2_no == u2_wire_lan(u2_Wire)) ) {
      u2_ve_grab(0);
    }
  }
  u2_cm_chin();
}
