
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

#ifndef SHARED_H
#define SHARED_H 1

/* Includes {{{ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <regex.h>
#include <getopt.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>
#include <X11/cursorfont.h>
/* }}} */

/* Typedefs {{{ */
typedef union submessagedata_t {
  char  b[20];
  short s[10];
  long  l[5];
} SubMessageData;

extern Display *display;
/* }}} */

/* Log {{{ */
#ifdef DEBUG
#define subSharedLogDebug(...)  subSharedLog(0, __FILE__, __LINE__, __VA_ARGS__);
#else
#define subSharedLogDebug(...)
#endif /* DEBUG */

#define subSharedLogError(...)  subSharedLog(1, __FILE__, __LINE__,  __VA_ARGS__);
#define subSharedLogWarn(...)   subSharedLog(2, __FILE__, __LINE__, __VA_ARGS__);

void subSharedLog(int type, const char *file, int line, const char *format, ...);
int subSharedLogXError(Display *display, XErrorEvent *ev);
/* }}} */

/* Alloc {{{ */
void *subSharedAlloc(size_t n, size_t size);
/* }}} */

/* Regex {{{ */
regex_t *subSharedRegexNew(char *regex);
int subSharedRegexMatch(regex_t *preg, char *string);
void subSharedRegexKill(regex_t *preg);
/* }}} */

/* Message {{{ */
void subSharedMessage(Window win, char *type, SubMessageData data);
/* }}} */

/* Property {{{ */
char *subSharedPropertyGet(Window win, Atom type, char *name, unsigned long *size);
char **subSharedPropertyList(Window win, char *name, int *size);
/* }}} */

/* Window {{{ */
char *subSharedWindowWMName(Window win);
char *subSharedWindowWMClass(Window win);
Window subSharedWindowSelect(void);
Window * subSharedWindowList(int *size);
/* }}} */

/* Client {{{ */
int subSharedClientFind(char *name, Window *win);
/* }}} */

/* Tag {{{ */
int subSharedTagFind(char *name);
/* }}} */

/* View {{{ */
int subSharedViewFind(char *name, Window *win);
/* }}} */
#endif /* SHARED_H */
