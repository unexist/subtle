
 /**
  * @package subtle
  *
  * @file Header file
  * @copyright Copyright (c) 2005-2008 Christoph Kappel <unexist@dorfelite.net>
  * @version $Id$
  *
  * This program can be distributed under the terms of the GNU GPL.
  * See the file COPYING.
  **/

#ifndef SUBTLE_H
#define SUBTLE_H 1

/* Includes {{{ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <regex.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>

#include "config.h"

#ifdef HAVE_SYS_INOTIFY_H
#include <sys/inotify.h>
#endif /* HAVE_SYS_INOTIFY */
/* }}} */

/* Macros {{{ */
#define WINNEW(parent,x,y,width,height,border,mask) \
  XCreateWindow(subtle->disp, parent, x, y, width, height, border, CopyFromParent, \
    InputOutput, CopyFromParent, mask, &attrs);                   ///< Shortcut

#define FLAGS int                                                 ///< Flags

#define WINWIDTH(c)  (c->rect.width - 2 * subtle->bw)             ///< Get real width
#define WINHEIGHT(c) (c->rect.height - subtle->th - subtle->bw)   ///< Get real height

#define SUBTLE(s) ((SubSubtle *)s)                                ///< Cast to SubSubtle
#define ARRAY(a)  ((SubArray *)a)                                 ///< Cast to SubArray
#define VIEW(v)   ((SubView *)v)                                  ///< Cast to SubView
#define LAYOUT(l) ((SubLayout *)l)                                ///< Cast to SubLayout
#define CLIENT(c) ((SubClient *)c)                                ///< Cast to SubClient
#define TAG(t)    ((SubTag *)t)                                   ///< Cast to SubTag
#define SUBLET(s) ((SubSublet *)s)                                ///< Cast to SubSublet
#define KEY(k)    ((SubKey *)k)                                   ///< Cast to SubKey
#define RECT(r)   ((XRectangle *)r)                               ///< Cast to XRectangle
/* }}} */

/* Flags {{{ */
/* Data types */
#define SUB_TYPE_CLIENT        (1L << 1)                          ///< Client
#define SUB_TYPE_TAG           (1L << 2)                          ///< Tag
#define SUB_TYPE_VIEW          (1L << 3)                          ///< View
#define SUB_TYPE_LAYOUT        (1L << 4)                          ///< Layout
#define SUB_TYPE_SUBLET        (1L << 5)                          ///< Sublet
#define SUB_TYPE_KEY           (1L << 6)                          ///< Key

/* Tile modes */
#define SUB_TILE_VERT          (1L << 7)                          ///< Tile vert
#define SUB_TILE_HORZ          (1L << 8)                          ///< Tile horz
#define SUB_TILE_SWAP          (1L << 9)                          ///< Tile swap

/* Client states */
#define SUB_STATE_SHADE        (1L << 12)                         ///< Shaded window
#define SUB_STATE_FLOAT        (1L << 13)                         ///< Floated window
#define SUB_STATE_FULL         (1L << 14)                         ///< Fullscreen window
#define SUB_STATE_RESIZE       (1L << 15)                         ///< Resized window
#define SUB_STATE_STACK        (1L << 16)                         ///< Stacked tiling window
#define SUB_STATE_TRANS        (1L << 17)                         ///< Transient window
#define SUB_STATE_DEAD         (1L << 18)                         ///< Dead window
#define SUB_STATE_TILED        (1L << 19)                         ///< Tiled client

/* Client preferences */
#define SUB_PREF_INPUT         (1L << 20)                         ///< Active/passive focus-model
#define SUB_PREF_FOCUS         (1L << 21)                         ///< Send focus message
#define SUB_PREF_CLOSE         (1L << 22)                         ///< Send close message

/* Drag states */
#define SUB_DRAG_START         (1L << 1)                          ///< Drag start
#define SUB_DRAG_ABOVE         (1L << 2)                          ///< Drag above
#define SUB_DRAG_BELOW         (1L << 3)                          ///< Drag below
#define SUB_DRAG_BEFORE        (1L << 4)                          ///< Drag before
#define SUB_DRAG_AFTER         (1L << 5)                          ///< Drag after
#define SUB_DRAG_LEFT          (1L << 6)                          ///< Drag left
#define SUB_DRAG_RIGHT         (1L << 7)                          ///< Drag right
#define SUB_DRAG_TOP           (1L << 8)                          ///< Drag top
#define SUB_DRAG_BOTTOM        (1L << 9)                          ///< Drag bottom
#define SUB_DRAG_MOVE          (1L << 10)                         ///< Drag move
#define SUB_DRAG_SWAP          (1L << 11)                         ///< Drag swap

/* Keys */
#define SUB_KEY_VIEW_JUMP      (1L << 10)                         ///< Jump to view
#define SUB_KEY_VIEW_MNEMONIC  (1L << 11)                         ///< Jump to view
#define SUB_KEY_EXEC           (1L << 12)                         ///< Exec an app

/* Data types */
#define SUB_DATA_STRING        (1L << 10)                         ///< String data
#define SUB_DATA_FIXNUM        (1L << 11)                         ///< Fixnum data
#define SUB_DATA_NIL           (1L << 12)                         ///< Nil data

/* ICCCM */
#define SUB_EWMH_WM_NAME                       0                  ///< Name of window
#define SUB_EWMH_WM_CLASS                      1                  ///< Class of window
#define SUB_EWMH_WM_STATE                      2                  ///< Window state
#define SUB_EWMH_WM_PROTOCOLS                  3                  ///< Supported protocols 
#define SUB_EWMH_WM_TAKE_FOCUS                 4                  ///< Send focus messages
#define SUB_EWMH_WM_DELETE_WINDOW              5                  ///< Send close messages
#define SUB_EWMH_WM_NORMAL_HINTS               6                  ///< Window normal hints
#define SUB_EWMH_WM_SIZE_HINTS                 7                  ///< Window size hints

/* EWMH */
#define SUB_EWMH_NET_SUPPORTED                 8                  ///< Supported states
#define SUB_EWMH_NET_CLIENT_LIST               9                  ///< List of clients
#define SUB_EWMH_NET_CLIENT_LIST_STACKING     10                  ///< List of clients
#define SUB_EWMH_NET_NUMBER_OF_DESKTOPS       11                  ///< Total number of views
#define SUB_EWMH_NET_DESKTOP_NAMES            12                  ///< Names of the views
#define SUB_EWMH_NET_DESKTOP_GEOMETRY         13                  ///< Desktop geometry
#define SUB_EWMH_NET_DESKTOP_VIEWPORT         14                  ///< Viewport of the view
#define SUB_EWMH_NET_CURRENT_DESKTOP          15                  ///< Number of current view
#define SUB_EWMH_NET_ACTIVE_WINDOW            16                  ///< Focus window
#define SUB_EWMH_NET_WORKAREA                 17                  ///< Workarea of the views
#define SUB_EWMH_NET_SUPPORTING_WM_CHECK      18                  ///< Check for compliant window manager
#define SUB_EWMH_NET_VIRTUAL_ROOTS            19                  ///< List of virtual destops
#define SUB_EWMH_NET_CLOSE_WINDOW             20

#define SUB_EWMH_NET_WM_PID                   21                  ///< PID of client
#define SUB_EWMH_NET_WM_DESKTOP               22                  ///< Desktop client is on

#define SUB_EWMH_NET_WM_STATE                 23                  ///< Window state
#define SUB_EWMH_NET_WM_STATE_MODAL           24                  ///< Modal window
#define SUB_EWMH_NET_WM_STATE_SHADED          25                  ///< Shaded window
#define SUB_EWMH_NET_WM_STATE_HIDDEN          26                  ///< Hidden window
#define SUB_EWMH_NET_WM_STATE_FULLSCREEN      27                  ///< Fullscreen window

#define SUB_EWMH_NET_WM_WINDOW_TYPE           28
#define SUB_EWMH_NET_WM_WINDOW_TYPE_DESKTOP   29
#define SUB_EWMH_NET_WM_WINDOW_TYPE_NORMAL    30
#define SUB_EWMH_NET_WM_WINDOW_TYPE_DIALOG    31

#define SUB_EWMH_NET_WM_ALLOWED_ACTIONS       32
#define SUB_EWMH_NET_WM_ACTION_MOVE           33
#define SUB_EWMH_NET_WM_ACTION_RESIZE         34
#define SUB_EWMH_NET_WM_ACTION_SHADE          35
#define SUB_EWMH_NET_WM_ACTION_FULLSCREEN     36
#define SUB_EWMH_NET_WM_ACTION_CHANGE_DESKTOP 37
#define SUB_EWMH_NET_WM_ACTION_CLOSE          38

/* Misc */
#define SUB_EWMH_UTF8                         39                  ///< String encoding

/* subtle */
#define SUB_EWMH_SUBTLE_CLIENT_TAG            40                  ///< subtle client tag
#define SUB_EWMH_SUBTLE_CLIENT_UNTAG          41                  ///< subtle client untag
#define SUB_EWMH_SUBTLE_CLIENT_TAGS           42                  ///< subtle client tags
#define SUB_EWMH_SUBTLE_TAG_NEW               43                  ///< subtle tag new
#define SUB_EWMH_SUBTLE_TAG_KILL              44                  ///< subtle tag kill
#define SUB_EWMH_SUBTLE_TAG_LIST              45                  ///< subtle tag list
#define SUB_EWMH_SUBTLE_VIEW_NEW              46                  ///< subtle view new
#define SUB_EWMH_SUBTLE_VIEW_KILL             47                  ///< subtle view kill
#define SUB_EWMH_SUBTLE_VIEW_LIST             48                  ///< subtle view list
#define SUB_EWMH_SUBTLE_VIEW_TAG              49                  ///< subtle view tag
#define SUB_EWMH_SUBTLE_VIEW_UNTAG            50                  ///< subtle view untag
#define SUB_EWMH_SUBTLE_VIEW_TAGS             51                  ///< subtle view tags
/* }}} */

/* Typedefs {{{ */
typedef struct subsubtle_t /* {{{ */
{
  Display            *disp;                                       ///< Subtle Xorg display
  int                th, bw, fx, fy;                              ///< Subtle tab height, border width, font metrics
  XFontStruct        *xfs;                                        ///< Subtle font

  Window             focus;                                       ///< Subtle focus window

  struct subsublet_t *sublet;                                     ///< Subtle first sublet
  struct subview_t   *cv;                                         ///< Subtle current view
  
  struct subarray_t  *keys;                                       ///< Subtle keys
  struct subarray_t  *tags;                                       ///< Subtle tags
  struct subarray_t  *views;                                      ///< Subtle views
  struct subarray_t  *clients;                                    ///< Subtle clients
  struct subarray_t  *sublets;                                    ///< Subtle sublets

#ifdef HAVE_SYS_INOTIFY_H
  int                notify;                                      ///< Subtle inotify descriptor
#endif

  struct
  {
    Window           win, views, sublets;                  
  } bar;                                                          ///< Subtle bar windows

  struct
  {
    unsigned long    font, border, norm, focus, cover, bg;                            
  } colors;                                                       ///< Subtle colors

  struct
  {
    GC               font, border, invert;                  
  } gcs;                                                          ///< Subtle graphic contexts

  struct
  {
    Cursor           square, move, arrow, horz, vert, resize;                                
  } cursors;                                                      ///< Subtle cursors

  struct
  {
    int              click;
  } hooks;                            
} SubSubtle; /* }}} */

typedef struct subarray_t /* {{{ */
{
  int    ndata;                                                   ///< Array data count
  void  **data;                                                   ///< Array data
} SubArray; /* }}} */

typedef struct subclient_t /* {{{ */
{
  FLAGS               flags;                                      ///< Client flags
  XRectangle          rect;                                       ///< Client rect
  int                 size, tags;                                 ///< Client size, tags
  char                *name;                                      ///< Client name
  Colormap            cmap;                                       ///< Client colormap
  Window              frame, caption, title, win;                 ///< Client decoration windows
  Window              left, right, bottom;                        ///< Client border windows
  struct subclient_t  *prev, *next, *down;                        ///< Client prev/next/down siblings
} SubClient; /* }}} */

typedef struct subtag_t /* {{{ */
{
  FLAGS    flags;                                                 ///< Tag flags
  char    *name;                                                  ///< Tag name
  regex_t *preg;                                                  ///< Tag regex
} SubTag; /* }}} */

typedef struct sublayout_t /* {{{ */
{
  FLAGS  flags;                                                   ///< Layout flags
  struct subclient_t *c1;                                         ///< Layout client1
  struct subclient_t *c2;                                         ///< Layout client2
} SubLayout; /* }}} */

typedef struct subview_t /* {{{ */
{
  FLAGS             flags;                                        ///< View flags
  int               tags, width;                                  ///< View tags, button width, layout
  Window            frame, button;                                ///< View frame, button
  char              *name;                                        ///< View name
  struct subarray_t *layout;                                      ///< View layout
} SubView; /* }}} */

typedef struct subsublet_t /* {{{ */
{
  FLAGS         flags;                                            ///< Sublet flags
  unsigned long recv;                                             ///< Sublet ruby receiver
  int           width;                                            ///< Sublet width
  time_t        time, interval;                                   ///< Sublet update time, interval time

  struct subsublet_t *next;                                       ///< Sublet next sibling

  union 
  {
    char *string;                                                 ///< Sublet data string
    int  fixnum;                                                  ///< Sublet data fixnum
  };
} SubSublet; /* }}} */

typedef struct subkey_t /* {{{ */
{
  FLAGS        flags;                                             ///< Key flags
  int          code;                                              ///< Key code
  unsigned int mod;                                               ///< Key modifier

  union
  {
    char       *string;                                           ///< Key data string
    int        number;                                            ///< Key data number
  };
} SubKey; /* }}} */

extern SubSubtle *subtle;
/* }}} */

/* array.c {{{ */
SubArray *subArrayNew(void);                                      ///< Create new array
void subArrayPush(SubArray *a, void *e);                          ///< Push element to array
void subArrayPop(SubArray *a, void *e);                           ///< Pop element from array
int subArrayFind(SubArray *a, void *e);                           ///< Find array id of element
void subArraySplice(SubArray *a, int idx, int len);               ///< Splice array at idx with len
void subArraySort(SubArray *a,                                    ///< Sort array with given compare function 
  int(*compar)(const void *a, const void *b));
void subArrayKill(SubArray *a, int clean);                        ///< Kill array with all elements
/* }}} */

/* client.c {{{ */
SubClient *subClientNew(Window win);                              ///< Create new client
void subClientConfigure(SubClient *c);                            ///< Send configure request
void subClientRender(SubClient *c);                               ///< Render client
void subClientFocus(SubClient *c);                                ///< Focus client
void subClientMap(SubClient *c);                                  ///< Map client  
void subClientUnmap(SubClient *c);                                ///< Unmap client
void subClientDrag(SubClient *c, int mode);                       ///< Move/drag client
void subClientToggle(SubClient *c, int type);                     ///< Toggle client state
void subClientFetchName(SubClient *c);                            ///< Fetch client name
void subClientSetWMState(SubClient *c, long state);               ///< Set client WM state
long subClientGetWMState(SubClient *c);                           ///< Get client WM state
void subClientPublish(void);                                      ///< Publish all clients
void subClientKill(SubClient *c);                                 ///< Kill client
/* }}} */

/* tag.c {{{ */
SubTag *subTagNew(char *name, char *regex);                       ///< Create tag
SubTag *subTagFind(char *name, int *id);                          ///< Find tag
int subTagMatch(char *string);                                    ///< Match tags
void subTagPublish(void);                                         ///< Publish tags
void subTagKill(SubTag *t);                                       ///< Delete tag
/* }}} */

/* layout.c {{{ */
SubLayout *subLayoutNew(SubClient *c1, SubClient *c2, 
  int mode);                                                      ///< Create new layout
void subLayoutKill(SubLayout *l);                                 ///< Kill layout
/* }}} */

/* view.c {{{ */
SubView *subViewNew(char *name, char *tags);                      ///< Create new view
void subViewConfigure(SubView *v);                                ///< Configure view
void subViewArrange(SubView *v, SubClient *c1, 
  SubClient *c2, int mode);                                       ///< Arrange view
void subViewUpdate(void);                                         ///< Update views
void subViewRender(void);                                         ///< Render views
void subViewJump(SubView *v);                                     ///< Jump to view
void subViewPublish(void);                                        ///< Publish views
void subViewSanitize(SubClient *c);                               ///< Sanitize views
void subViewKill(SubView *v);                                     ///< Kill view
/* }}} */

/* sublet.c {{{ */
SubSublet *subSubletNew(unsigned long ref, time_t interval, 
  char *watch);                                                   ///< Create new sublet
void subSubletConfigure(void);                                    ///< Configure sublet bar
void subSubletRender(void);                                       ///< Render sublet
int subSubletCompare(const void *a, const void *b);               ///< Compare two sublets
void subSubletKill(SubSublet *s);                                 ///< Kill sublet
/* }}} */

/* display.c {{{ */
void subDisplayInit(const char *display);                         ///< Create new display
void subDisplayScan(void);                                        ///< Scan root window
void subDisplayFinish(void);                                      ///< Delete display
/* }}} */

/* key.c {{{ */
void subKeyInit(void);                                            ///< Init keymap
SubKey *subKeyNew(const char *key,                                ///< Create key
  const char *value);
KeySym subKeyGet(void);                                           ///< Get key
SubKey *subKeyFind(int code, unsigned int mod);                   ///< Find key
void subKeyGrab(Window win);                                      ///< Grab keys for window
void subKeyUngrab(Window win);                                    ///< Ungrab keys for window
int subKeyCompare(const void *a, const void *b);                  ///< Compare keys
void subKeyKill(SubKey *k);                                       ///< Kill key
/* }}} */

/* ruby.c {{{ */
void subRubyInit(void);                                           ///< Init Ruby stack 
void subRubyLoadConfig(const char *path);                         ///< Load config file
void subRubyLoadSublets(const char *path);                        ///< Load sublets
void subRubyCall(SubSublet *s);                                   ///< Call Ruby script
void subRubyFinish(void);                                         ///< Kill Ruby stack
/* }}} */

/* event.c {{{ */
void subEventLoop(void);                                          ///< Event loop
/* }}} */

/* util.c {{{ */
#ifdef DEBUG
void subUtilLogSetDebug(void);
#define subUtilLogDebug(...)  subUtilLog(0, __FILE__, __LINE__, __VA_ARGS__);
#else
#define subUtilLogDebug(...)
#endif /* DEBUG */

#define subUtilLogError(...)  subUtilLog(1, __FILE__, __LINE__,  __VA_ARGS__);
#define subUtilLogWarn(...)    subUtilLog(2, __FILE__, __LINE__, __VA_ARGS__);

void subUtilLog(int type, const char *file,
  int line, const char *format, ...);                             ///< Print messages
int subUtilLogXError(Display *disp, XErrorEvent *ev);             ///< Print X error messages
void *subUtilAlloc(size_t n, size_t size);                        ///< Allocate memory
void *subUtilRealloc(void *mem, size_t size);                     ///< Reallocate memory
XPointer *subUtilFind(Window win, XContext id);                   ///< Find window data
time_t subUtilTime(void);                                         ///< Get the current time
regex_t * subUtilRegexNew(char *regex);                           ///< Create new regex
int subUtilRegexMatch(regex_t *preg, char *string);               ///< Check if string matches preg
void subUtilRegexKill(regex_t *preg);                             ///< Kill regex
/* }}} */

/* ewmh.c  {{{ */
void subEwmhInit(void);                                           ///< Init atoms/hints
Atom subEwmhFind(int hint);                                       ///< Find atom
char *subEwmhGetProperty(Window win, 
  Atom type, int hint, unsigned long *size);                      ///< Get property
void subEwmhSetWindows(Window win, int hint, 
  Window *values, int size);                                      ///< Set window properties
void subEwmhSetCardinals(Window win, int hint,
  long *values, int size);                                        ///< Set cardinal properties
void subEwmhSetString(Window win, int hint, 
  char *value);                                                   ///< Set string property
void subEwmhSetStrings(Window win, int hint,                      ///< Set string properties
  char **values, int size);
/* }}} */
#endif /* SUBTLE_H */
