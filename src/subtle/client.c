
 /**
  * @package subtle
  *
  * @file Client functions
  * @copyright (c) 2005-2008 Christoph Kappel <unexist@dorfelite.net>
  * @version $Id$
  *
  * This program can be distributed under the terms of the GNU GPL.
  * See the file COPYING.
  **/

#include <X11/Xatom.h>
#include "subtle.h"

/* ClientMask {{{ */
static void
ClientMask(int type,
  SubClient *c,
  XRectangle *r)
{
  /* @todo Put modifiers into a static table? */
  switch(type)
    {
      case SUB_DRAG_START: 
        XDrawRectangle(subtle->disp, ROOT, subtle->gcs.invert, r->x - 1, r->y - 1,
          r->width - 3, r->height - 3);
        break;
      case SUB_DRAG_TOP:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, 5, 5, c->rect.width - 10, 
          c->rect.height * 0.5 - 5);
        break;
      case SUB_DRAG_BOTTOM:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, 5, c->rect.height * 0.5, 
          c->rect.width - 10, c->rect.height * 0.5 - 5);
        break;
      case SUB_DRAG_LEFT:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, 5, 5, c->rect.width * 0.5 - 5, 
          c->rect.height - 10);
        break;
      case SUB_DRAG_RIGHT:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, c->rect.width * 0.5, 5, 
          c->rect.width * 0.5 - 5, c->rect.height - 10);
        break;
      case SUB_DRAG_SWAP:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, c->rect.width * 0.35, 
          c->rect.height * 0.35, c->rect.width * 0.3, c->rect.height * 0.3);
        break;
      case SUB_DRAG_BEFORE:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, 5, 5, c->rect.width * 0.1 - 5,
          c->rect.height - 10);
        break;
      case SUB_DRAG_AFTER:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, c->rect.width * 0.9, 5, 
          c->rect.width * 0.1 - 5, c->rect.height - 10);
        break;
      case SUB_DRAG_ABOVE:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, 5, 5, c->rect.width - 10, 
          c->rect.height * 0.1 - 5);
        break;
      case SUB_DRAG_BELOW:
        XFillRectangle(subtle->disp, c->win, subtle->gcs.invert, 5, c->rect.height * 0.9, 
          c->rect.width - 10, c->rect.height * 0.1 - 5);
        break;
    }
} /* }}} */

/* ClientSnap {{{ */
static void
ClientSnap(SubClient *c,
  XRectangle *r)
{
  assert(c && r);

  if(SNAP > r->x) r->x = subtle->bw;
  else if(r->x > (SCREENW - WINW(c) - SNAP))
    r->x = SCREENW - c->rect.width + subtle->bw;
  if(SNAP > r->y) r->y = subtle->bw;
  else if(r->y > (SCREENH - WINH(c) - SNAP))
    r->y = SCREENH - c->rect.height + subtle->bw;
} /* }}} */

 /** subClientNew {{{
  * @brief Create new client
  * @param[in]  win  Main window of the new client
  * @return Returns a #SubClient or \p NULL
  **/

SubClient *
subClientNew(Window win)
{
  int i, n = 0;
  long vid = 1337;
  long supplied = 0;
  Window propwin = 0;
  XWMHints *hints = NULL;
  XWindowAttributes attrs;
  Atom *protos = NULL;
  SubClient *c = NULL;

  assert(win);
  
  c = CLIENT(subUtilAlloc(1, sizeof(SubClient)));
  c->flags = SUB_TYPE_CLIENT;
  c->win   = win;

  /* Dimensions */
  c->rect.x      = 0;
  c->rect.y      = subtle->th;
  c->rect.width  = SCREENW;
  c->rect.height = SCREENH - subtle->th;

  /* Update client */
  XAddToSaveSet(subtle->disp, c->win);
  XSelectInput(subtle->disp, c->win, SubstructureRedirectMask|SubstructureNotifyMask|
    EnterWindowMask|FocusChangeMask|KeyPressMask|ButtonPressMask|PropertyChangeMask);

  XSetWindowBorderWidth(subtle->disp, c->win, subtle->bw);
  XSaveContext(subtle->disp, c->win, CLIENTID, (void *)c);

  /* Window attributes */
  XGetWindowAttributes(subtle->disp, c->win, &attrs);
  c->cmap = attrs.colormap;
  subClientFetchName(c);

  /* Size hints */
  c->hints = XAllocSizeHints();
  if(!c->hints) subUtilLogError("Can't alloc memory. Exhausted?\n");
  XGetWMNormalHints(subtle->disp, c->win, c->hints, &supplied);
  if(0 < supplied) c->flags |= SUB_PREF_HINTS;
  else XFree(c->hints);

  /* Window manager hints */
  hints = XGetWMHints(subtle->disp, c->win);
  if(hints)
    {
      subClientSetWMState(c, NormalState);
      if(hints->input) c->flags |= SUB_PREF_INPUT;
      if(hints->flags & XUrgencyHint) c->tags |= SUB_TAG_URGENT;
      XFree(hints);
    }
  
  /* Protocols */
  if(XGetWMProtocols(subtle->disp, c->win, &protos, &n))
    {
      for(i = 0; i < n; i++)
        {
          if(protos[i] == subEwmhFind(SUB_EWMH_WM_TAKE_FOCUS))
            c->flags |= SUB_PREF_FOCUS;
          else if(protos[i] == subEwmhFind(SUB_EWMH_WM_DELETE_WINDOW))
            c->flags |= SUB_PREF_CLOSE;
        }
      XFree(protos);
    }

  /* Tags */
  if(c->name)
    for(i = 0; i < subtle->tags->ndata; i++)
      {
        SubTag *t = TAG(subtle->tags->data[i]);

        if(t->preg && subUtilRegexMatch(t->preg, c->name)) c->tags |= (1L << (i + 1));
      }

  /* Special tags */
  XGetTransientForHint(subtle->disp, win, &propwin); ///< Check for dialogs
  if(c->tags & SUB_TAG_FLOAT) subClientToggle(c, SUB_STATE_FLOAT);
  if(c->tags & SUB_TAG_FULL)  subClientToggle(c, SUB_STATE_FULL);
  if(c->tags & SUB_TAG_URGENT || propwin) subClientToggle(c, SUB_STATE_URGENT);
  if(!c->tags) c->tags |= SUB_TAG_DEFAULT; ///< Ensure that there's at least on tag

  /* EWMH: Tags and desktop */
  subEwmhSetCardinals(c->win, SUB_EWMH_SUBTLE_CLIENT_TAGS, (long *)&c->tags, 1);
  subEwmhSetCardinals(c->win, SUB_EWMH_NET_WM_DESKTOP, &vid, 1);

  printf("Adding client (%s)\n", c->name);
  subUtilLogDebug("new=client, name=%s, win=%#lx, supplied=%ld\n", c->name, win, supplied);

  return c;
} /* }}} */

 /** subClientConfigure {{{
  * @brief Send a configure request to client
  * @param[in]  c  A #SubClient
  **/

void
subClientConfigure(SubClient *c)
{
  XRectangle r = { 0 };
  XConfigureEvent ev;

  assert(c);

  /* Client size */
  r = c->rect;
  r.width  = WINW(c);
  r.height = WINH(c);

  if(c->flags & SUB_STATE_FULL) 
    {
      r.x      = 0;
      r.y      = 0;
      r.width  = SCREENW;
      r.height = SCREENH;
    }

  /* Tell client new geometry */
  ev.type              = ConfigureNotify;
  ev.event             = c->win;
  ev.window            = c->win;
  ev.x                 = r.x;
  ev.y                 = r.y;
  ev.width             = r.width;
  ev.height            = r.height;
  ev.above             = None;
  ev.override_redirect = 0;
  ev.border_width      = 0;

  XMoveResizeWindow(subtle->disp, c->win, r.x, r.y, r.width, r.height);
  XSendEvent(subtle->disp, c->win, False, StructureNotifyMask, (XEvent *)&ev);

  subUtilLogDebug("client=%#lx, state=%c, x=%03d, y=%03d, width=%03d, height=%03d\n",
    c->win, c->flags & SUB_STATE_FLOAT ? 'f' : c->flags & SUB_STATE_FULL ? 'u' : 'n',
    r.x, r.y, r.width, r.height);
} /* }}} */

 /** subClientRender {{{
  * @brief Render client and redraw titlebar and borders
  * @param[in]  c  A #SubClient
  **/

void
subClientRender(SubClient *c)
{
  XSetWindowAttributes attrs;

  assert(c);

  attrs.border_pixel = subtle->windows.focus == c->win ? subtle->colors.focus : subtle->colors.norm;
  XChangeWindowAttributes(subtle->disp, c->win, CWBorderPixel, &attrs);

  /* Caption */
  XResizeWindow(subtle->disp, subtle->windows.caption, TEXTW(c->name), subtle->th);
  XClearWindow(subtle->disp, subtle->windows.caption);
  XftDrawString8(subtle->draws.caption, &subtle->colors.font, subtle->xft, 3, subtle->fy - 1,
    (XftChar8 *)c->name, strlen(c->name));
} /* }}} */

 /** subClientFocus {{{
  * @brief Set or unset focus to client
  * @param[in]  c  A #SubClient
  **/

void
subClientFocus(SubClient *c)
{
  assert(c);

  /* Check if client wants to take focus by itself */
  if(c->flags & SUB_PREF_FOCUS)
    {
      XEvent ev;
  
      ev.type                 = ClientMessage;
      ev.xclient.window       = c->win;
      ev.xclient.message_type = subEwmhFind(SUB_EWMH_WM_PROTOCOLS);
      ev.xclient.format       = 32;
      ev.xclient.data.l[0]    = subEwmhFind(SUB_EWMH_WM_TAKE_FOCUS);
      ev.xclient.data.l[1]    = CurrentTime;
      
      XSendEvent(subtle->disp, c->win, False, NoEventMask, &ev);
      
      subUtilLogDebug("Focus: win=%#lx, input=%d, send=%d\n", c->win,
        !!(c->flags & SUB_PREF_INPUT), !!(c->flags & SUB_PREF_FOCUS));
    }   
  else XSetInputFocus(subtle->disp, c->win, RevertToNone, CurrentTime);
} /* }}} */

 /** subClientDrag {{{
  * @brief Move and/or drag client
  * @param[in]  c     A #SubClient
  * @param[in]  mode  Drag/move mode
  **/

void
subClientDrag(SubClient *c,
  int mode)
{
  XEvent ev;
  Window win, unused;
  unsigned int mask = 0;
  int loop = True, wx = 0, wy = 0, rx = 0, ry = 0, state = 0, lstate = 0;
  XRectangle r;
  SubClient *c2 = NULL, *lc = NULL;
  short *dirx = NULL, *diry = NULL, minx = 10, miny = 10, maxx = SCREENW, maxy = SCREENH,
    stepx = subtle->step, stepy = subtle->step;

  assert(c);
 
  /* Get window position on root window */
  XQueryPointer(subtle->disp, c->win, &win, &win, &rx, &ry, &wx, &wy, &mask);
  r.x      = rx - wx;
  r.y      = ry - wy;
  r.width  = c->rect.width;
  r.height = c->rect.height; 

  /* Directions and steppings  {{{ */
  switch(mode)
    {
      case SUB_DRAG_MOVE:
        dirx = &r.x;
        diry = &r.y;
        break;
      case SUB_DRAG_RESIZE:
        dirx = (short *)&r.width;
        diry = (short *)&r.height;

        /* Respect size hints */
        if(c->flags & SUB_PREF_HINTS)
          {
            if(c->hints->flags & PResizeInc) ///< Resize hints
              {
                stepx = c->hints->width_inc;
                stepy = c->hints->height_inc;
              }
            if(c->hints->flags & PMinSize) ///< Min. size
              {
                minx = BETWEEN(c->hints->min_width, MINW, 0);
                miny = BETWEEN(c->hints->min_height, MINH, 0);
              }
            if(c->hints->flags & PMaxSize) ///< Max. size
              {
                maxx = c->hints->max_width;
                maxy = c->hints->max_height;
              }
         }
    } /* }}} */

  subUtilLogDebug("minx=%02d, miny=%02d, stepx=%02d, stepy=%02d\n", minx, miny, stepx, stepy);

  if(XGrabPointer(subtle->disp, c->win, True, 
    ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None,
    SUB_DRAG_MOVE == mode ? subtle->cursors.move : subtle->cursors.resize, 
    CurrentTime)) return;

  XGrabServer(subtle->disp);
  if(mode & (SUB_DRAG_MOVE|SUB_DRAG_RESIZE)) ClientMask(SUB_DRAG_START, c, &r);

  while(loop) ///< Event loop
    {
      XMaskEvent(subtle->disp, 
        PointerMotionMask|ButtonReleaseMask|KeyPressMask|EnterWindowMask, &ev);
      switch(ev.type)
        {
          case EnterNotify:   win = ev.xcrossing.window; break; ///< Find destination window
          case ButtonRelease: loop = False;              break;
          case KeyPress: /* {{{ */
            if(mode & (SUB_DRAG_MOVE|SUB_DRAG_RESIZE))
              {
                KeySym sym = XKeycodeToKeysym(subtle->disp, ev.xkey.keycode, 0);
                ClientMask(SUB_DRAG_START, c, &r);

                switch(sym)
                  {
                    case XK_Left:   *dirx -= stepx; break;
                    case XK_Right:  *dirx += stepx; break;
                    case XK_Up:     *diry -= stepy; break;
                    case XK_Down:   *diry += stepy; break;
                    case XK_Return: loop = False;   break;
                  }

                *dirx = BETWEEN(*dirx, minx, maxx);
                *diry = BETWEEN(*diry, miny, maxy);
              
                ClientMask(SUB_DRAG_START, c, &r);
              }
            break; /* }}} */
          case MotionNotify: /* {{{ */
            if(mode & (SUB_DRAG_MOVE|SUB_DRAG_RESIZE))
              {
                ClientMask(SUB_DRAG_START, c, &r);
          
                /* Calculate selection rect */
                switch(mode)
                  {
                    case SUB_DRAG_MOVE:
                      r.x = (rx - wx) - (rx - ev.xmotion.x_root);
                      r.y = (ry - wy) - (ry - ev.xmotion.y_root);

                      ClientSnap(c, &r);
                      break;
                    case SUB_DRAG_RESIZE: 
                      if(c->rect.width + ev.xmotion.x_root >= rx)
                        {
                          r.width = BETWEEN(c->rect.width + (ev.xmotion.x_root - rx),
                            minx, maxx);
                          r.width -= r.width % stepx;
                        }
                      if(c->rect.height + ev.xmotion.y_root >= ry)
                        {
                          r.height = BETWEEN(c->rect.height + (ev.xmotion.y_root - ry),
                            miny, maxy);
                          r.height -= r.height % stepy;
                        }
                      break;
                  }  

                ClientMask(SUB_DRAG_START, c, &r);
              }
            else if(win != c->win && SUB_DRAG_SWAP == mode)
              {
                if(!c2 ) c2 = CLIENT(subUtilFind(win, CLIENTID));
                if(c2)
                  {
                    XQueryPointer(subtle->disp, win, &unused, &unused,
                      &rx, &ry, &wx, &wy, &mask);
                    r.x = rx - wx;
                    r.y = ry - wy;

                    /* Change drag state */
                    if(wx > c2->rect.width * 0.35 && wx < c2->rect.width * 0.65)
                      {
                        if(state != SUB_DRAG_TOP && wy > c2->rect.height * 0.1 &&
                          wy < c2->rect.height * 0.35)
                          state = SUB_DRAG_TOP;
                        else if(state != SUB_DRAG_BOTTOM && wy > c2->rect.height * 0.65 &&
                          wy < c2->rect.height * 0.9)
                          state = SUB_DRAG_BOTTOM;
                        else if(state != SUB_DRAG_SWAP && wy > c2->rect.height * 0.35 &&
                          wy < c2->rect.height * 0.65)
                          state = SUB_DRAG_SWAP;
                      }
                    if(state != SUB_DRAG_ABOVE && wy < c2->rect.height * 0.1)
                      state = SUB_DRAG_ABOVE;
                    else if(state != SUB_DRAG_BELOW && wy > c2->rect.height * 0.9)
                      state = SUB_DRAG_BELOW;
                    if(wy > c2->rect.height * 0.1 && wy < c2->rect.height * 0.9)
                      {
                        if(state != SUB_DRAG_LEFT && wx > c2->rect.width * 0.1 &&
                          wx < c2->rect.width * 0.35)
                          state = SUB_DRAG_LEFT;
                        else if(state != SUB_DRAG_RIGHT && wx > c2->rect.width * 0.65 &&
                          wx < c2->rect.width * 0.9)
                          state = SUB_DRAG_RIGHT;
                        else if(state != SUB_DRAG_BEFORE && wx < c2->rect.width * 0.1)
                          state = SUB_DRAG_BEFORE;
                        else if(state != SUB_DRAG_AFTER && wx > c2->rect.width * 0.9)
                          state = SUB_DRAG_AFTER;
                      }

                    if(lstate != state || lc != c2) 
                      {
                        if(SUB_DRAG_START != lstate) ClientMask(lstate, lc, &r);
                        ClientMask(state, c2, &r);

                        lc     = c2;
                        lstate = state;
                      }
                    }
                }
            break; /* }}} */
        }
    }

  ClientMask(state, c2, &r); ///< Erase mask

  if(win != c->win && c && c2) ///< Except same window
    {
      if(state & (SUB_DRAG_LEFT|SUB_DRAG_RIGHT))
        {
          subViewArrange(subtle->cv, c, c2, SUB_TILE_HORZ);
          subViewConfigure(subtle->cv);
        }
      else if(state & (SUB_DRAG_TOP|SUB_DRAG_BOTTOM))
        {
          subViewArrange(subtle->cv, c, c2, SUB_TILE_VERT);
          subViewConfigure(subtle->cv);
        }
      else if(SUB_DRAG_SWAP == state)
        {
          subViewArrange(subtle->cv, c, c2, SUB_TILE_SWAP);
          subViewConfigure(subtle->cv);
        }
    }
  else ///< Move/Resize
    {
      if(c->flags & (SUB_STATE_FLOAT|SUB_STATE_URGENT)) 
        {
          r.y -= (subtle->th + subtle->bw); ///< Border and bar height
          r.x -= subtle->bw;
          c->rect = r;

          subClientConfigure(c);
        }
      else if(SUB_DRAG_RESIZE >= mode)
        {
          /* Get size ratios */
          if(SUB_DRAG_RIGHT == mode || SUB_DRAG_LEFT == mode)
            c->size = r.width * 100 / WINW(c);
          else c->size = r.height * 100 / WINH(c);

          if(90 > c->size) c->size = 90;      ///< Max. 90%
          else if(10 > c->size) c->size = 10; ///< Min. 10%

          if(!(c->flags & SUB_STATE_RESIZE)) c->flags |= SUB_STATE_RESIZE;
          subViewConfigure(subtle->cv);        
        }
    }

  XUngrabPointer(subtle->disp, CurrentTime);
  XUngrabServer(subtle->disp);
} /* }}} */

 /** subClientToggle {{{
  * @brief Toggle various states of client
  * @param[in]  c     A #SubClient
  * @param[in]  type  Toggle type
  **/

void
subClientToggle(SubClient *c,
  int type)
{
  assert(c);

  if(c->flags & type) 
    {
      c->flags &= ~type;

      switch(type)
        {   
          case SUB_STATE_FULL: 
            XSetWindowBorderWidth(subtle->disp, c->win, subtle->bw);
            XReparentWindow(subtle->disp, c->win, subtle->cv->frame, 0, 0);
        }            
    }
  else 
    {
      int width = SCREENW, height = SCREENH;

      c->flags |= type;

      switch(type)
        {
          case SUB_STATE_URGENT:
          case SUB_STATE_FLOAT: /* {{{ */
            if(c->flags & SUB_PREF_HINTS)
              {
                if(c->hints->flags & (USSize|PSize)) ///< User/program size
                  {
                    c->rect.width  = BETWEEN(c->hints->width, MINW, 0) + 2 * subtle->bw;
                    c->rect.height = BETWEEN(c->hints->height, MINW, 0) + 2 * subtle->bw;
                  }
                else if(c->hints->flags & PBaseSize) ///< Base size
                  {
                    c->rect.width  = BETWEEN(c->hints->base_width, MINW, 0) + 2 * subtle->bw;
                    c->rect.height = BETWEEN(c->hints->base_height, MINW, 0) + 2 * subtle->bw;
                  }
                else if(c->hints->flags & PMinSize) ///< Min size
                  {
                    c->rect.width  = BETWEEN(c->hints->min_width, MINW, 0) + 2 * subtle->bw;
                    c->rect.height = BETWEEN(c->hints->min_height, MINH, 0) + 2 * subtle->bw;
                  }
                else ///< Fallback
                  {
                    c->rect.width  = MINW + 2 * subtle->bw;
                    c->rect.height = MINH + 2 * subtle->bw;
                  }
                if(c->hints && c->hints->flags & (USPosition|PPosition)) ///< User/program pos
                  {
                    c->rect.x = c->hints->x + 2 * subtle->bw;
                    c->rect.y = c->hints->y + 2 * subtle->bw;
                  }
                else if(c->hints->flags & PAspect) ///< Aspect size
                  {
                    c->rect.x = (c->hints->min_aspect.x - c->hints->max_aspect.x) / 2;
                    c->rect.y = (c->hints->min_aspect.y - c->hints->max_aspect.y) / 2;
                  }
                else ///< Fallback
                  {
                    c->rect.x = (width - c->rect.width) / 2;
                    c->rect.y = (height - c->rect.height) / 2;
                  }
              }
            else
              {
                c->rect.width  = MINW + 2 * subtle->bw;
                c->rect.height = MINH + 2 * subtle->bw;
                c->rect.x      = (width - c->rect.width) / 2;
                c->rect.y      = (height - c->rect.height) / 2;
              }
            break; /* }}} */
          case SUB_STATE_FULL: /* {{{ */
            XSetWindowBorderWidth(subtle->disp, c->win, 0);
            XReparentWindow(subtle->disp, c->win, ROOT, 0, 0);
          break; /* }}} */
        }

      subClientConfigure(c);
    }

  subClientFocus(c);
} /* }}} */

  /** subClientFetchName {{{
   * @brief Fetch client name and store it
   * @param[in]  c  A #SubClient
   **/

void
subClientFetchName(SubClient *c)
{
  assert(c);

  if(c->name) 
    {
      XFree(c->name);
      c->name = NULL;
    }

  XFetchName(subtle->disp, c->win, &c->name);
  if(!c->name) c->name = strdup("subtle");

  subClientRender(c);
} /* }}} */

 /** subClientSetWMState {{{
  * @brief Set WM state for client
  * @param[in]  c      A #SubClient
  * @param[in]  state  New state for the client
  **/

void
subClientSetWMState(SubClient *c,
  long state)
{
  CARD32 data[2];
  data[0] = state;
  data[1] = None; /* No icons */

  assert(c);

  XChangeProperty(subtle->disp, c->win, subEwmhFind(SUB_EWMH_WM_STATE), 
    subEwmhFind(SUB_EWMH_WM_STATE), 32, PropModeReplace, (unsigned char *)data, 2);
} /* }}} */

 /** subClientGetWMState {{{
  * @brief Get WM state from client
  * @param[in]  c  A #SubClient
  * @return Returns client WM state
  **/

long
subClientGetWMState(SubClient *c)
{
  Atom type;
  int format;
  unsigned long unused, bytes;
  long *data = NULL, state = WithdrawnState;

  assert(c);

  if(XGetWindowProperty(subtle->disp, c->win, subEwmhFind(SUB_EWMH_WM_STATE), 0L, 2L, False,
      subEwmhFind(SUB_EWMH_WM_STATE), 
      &type, &format, &bytes, &unused, (unsigned char **)&data)== Success && bytes)
    {
      state = *data;
      XFree(data);
    }
  return state;
} /* }}} */

 /** subClientPublish {{{
  * @brief Publish clients
  **/

void
subClientPublish(void)
{
  if(0 < subtle->clients->ndata )
    {
      int i;
      Window *wins = (Window *)subUtilAlloc(subtle->clients->ndata, sizeof(Window));

      for(i = 0; i < subtle->clients->ndata; i++) 
        wins[i] = CLIENT(subtle->clients->data[i])->win;

      /* EWMH: Client list and client list stacking */
      subEwmhSetWindows(DefaultRootWindow(subtle->disp), SUB_EWMH_NET_CLIENT_LIST,
        wins, subtle->clients->ndata);
      subEwmhSetWindows(DefaultRootWindow(subtle->disp), SUB_EWMH_NET_CLIENT_LIST_STACKING,
        wins, subtle->clients->ndata);

      subUtilLogDebug("publish=client, clients=%d\n", subtle->clients->ndata);

      free(wins);
    }
} /* }}} */

 /** subClientKill {{{
  * @brief Send interested clients the close signal and/or kill it
  * @param[in]  c  A #SubClient
  **/

void
subClientKill(SubClient *c)
{
  assert(c);

  printf("Killing client (%s)\n", c->name);

  /* Ignore further events and delete context */
  XSelectInput(subtle->disp, c->win, NoEventMask);
  XDeleteContext(subtle->disp, c->win, CLIENTID);

  if(subtle->windows.focus == c->win) subtle->windows.focus = 0; ///< Unset focus
  if(!(c->flags & SUB_STATE_DEAD))
    {
      subArrayPop(subtle->clients, (void *)c->win);

      /* EWMH: Update Client list and client list stacking */
      subEwmhSetWindows(DefaultRootWindow(subtle->disp), SUB_EWMH_NET_CLIENT_LIST,
        (Window *)subtle->clients->data, subtle->clients->ndata);
      subEwmhSetWindows(DefaultRootWindow(subtle->disp), SUB_EWMH_NET_CLIENT_LIST_STACKING,
        (Window *)subtle->clients->data, subtle->clients->ndata);
    
      /* Honor window preferences */
      if(c->flags & SUB_PREF_CLOSE)
        {
          XEvent ev;

          ev.type                 = ClientMessage;
          ev.xclient.window       = c->win;
          ev.xclient.message_type = subEwmhFind(SUB_EWMH_WM_PROTOCOLS);
          ev.xclient.format       = 32;
          ev.xclient.data.l[0]    = subEwmhFind(SUB_EWMH_WM_DELETE_WINDOW);
          ev.xclient.data.l[1]    = CurrentTime;

          XSendEvent(subtle->disp, c->win, False, NoEventMask, &ev);
        }
      else XKillClient(subtle->disp, c->win);
    }

  subViewSanitize(c);
  subArrayPop(subtle->clients, (void *)c);

  if(c->flags & SUB_PREF_HINTS) XFree(c->hints);
  if(c->name) XFree(c->name);
  free(c);

  subUtilLogDebug("kill=client\n");
} /* }}} */

