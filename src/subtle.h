#ifndef SUBTLE_H
#define SUBTLE_H 1

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

/* Masks */
#define ENTER_MASK		(EnterWindowMask|LeaveWindowMask)
#define BUTTON_MASK		(ButtonPressMask|ButtonReleaseMask)
#define POINTER_MASK	(BUTTON_MASK|PointerMotionMask)
#define STRUCT_MASK		(StructureNotifyMask|SubstructureNotifyMask)
#define KEY_MASK			(KeyPressMask|KeyReleaseMask)
#define FRAME_MASK		(ENTER_MASK|BUTTON_MASK|KEY_MASK|FocusChangeMask| \
	VisibilityChangeMask|ExposureMask)

/* Macros */
#define SUBWINNEW(parent,x,y,width,height,border) \
	XCreateWindow(d->dpy, parent, x, y, width, height, border, CopyFromParent, \
		InputOutput, CopyFromParent, mask, &attrs);

/* win.c */
#define SUB_WIN_SCREEN	(1L << 1)											// Screen window
#define SUB_WIN_CLIENT	(1L << 2)											// Client window
#define SUB_WIN_TILEH		(1L << 3)											// Horizontal tiling window
#define SUB_WIN_TILEV		(1L << 4)											// Vertical tiling window
#define SUB_WIN_TRANS		(1L << 5)											// Transient window
#define SUB_WIN_FLOAT		(1L << 6)											// Floating window
#define SUB_WIN_SHADED	(1L << 7)											// Shaded window

struct subtile;
struct subclient;

typedef struct subwin
{
	int			x, y, width, height, prop;									// Window properties
	Window	icon, frame, title, win;										// Subwindows
	Window	left, right, bottom;												// Border windows
	struct subwin *parent;															// Parent window
	union
	{
		struct subtile *tile;
		struct subclient *client;
	};
} SubWin;

SubWin *subWinFind(Window win);												// Find a window
SubWin *subWinNew(Window win);												// Create a new window
void subWinDelete(SubWin *w);													// Delete a window
void subWinDrag(short mode, SubWin *w);								// Move/Resize a window
void subWinShade(SubWin *w);													// Shade/unshade a window
void subWinResize(SubWin *w);													// Resize a window
void subWinRender(short mode, SubWin *w);							// Render a window
void subWinRestack(SubWin *w);												// Restack a window
void subWinMap(SubWin *w);														// Map a window

/* client.c */
typedef struct subclient
{
	char			*name;																		// Client name
	Window		caption;																	// Client caption
	Colormap	cmap;																			// Client colormap	
} SubClient;

SubWin *subClientNew(Window win);											// Create a new client
void subClientDelete(SubWin *w);											// Delete a client
void subClientSetWMState(SubWin *w, int state);				// Set client WM state
long subClientGetWMState(SubWin *w);									// Get client WM state
void subClientSendConfigure(SubWin *w);								// Send configure request
void subClientSendDelete(SubWin *w);									// Send delete request
void subClientToggleShade(SubWin *w);									// Toggle shaded state
void subClientRender(short mode, SubWin *w);					// Render the window

/* tile.c */
typedef struct subtile
{
	unsigned int 		mw, mh, shaded;											// Tile min. width / height
	Window					newcol, delcol;											// Tile buttons
} SubTile;

#define subTileNewHoriz()	subTileNew(0)
#define subTileNewVert()	subTileNew(1)
SubWin *subTileNew(short mode);												// Create a new tile
void subTileAdd(SubWin *t, SubWin *w);								// Add a window to the tile
void subTileDelete(SubWin *w);												// Delete a tile
void subTileRender(short mode, SubWin *w);						// Render a tile
void subTileConfigure(SubWin *w);											// Configure a tile and it's children

/* screen.c */
typedef struct subscreen
{
	int n;																							// Number of screens
	SubWin **wins;																			// Root windows
	SubWin *active;																			// Active screen
	Window statusbar;																		// Statusbar
} SubScreen;

extern SubScreen *screen;

void subScreenNew(void);															// Create a new screen
void subScreenAdd(void);															// Add a screen
void subScreenDelete(SubWin *w);											// Delete a screen
void subScreenKill(void);															// Kill all screens
void subScreenShift(int dir);													// Shift current screen

/* display.c */
typedef struct subdisplay
{
	Display						*dpy;															// Connection to X server
	Window						focus;														// Focus window
	unsigned int			th, bw;														// Tab height, border
	unsigned int			fx, fy;														// Font metrics
	XFontStruct				*xfs;															// Font
	struct
	{
		unsigned long		font, border, norm, act, bg;			// Colors
	} colors;
	struct
	{
		GC							font, border, invert;							// Used graphic contexts (GCs)
	} gcs;
	struct
	{
		Atom						protos, delete, state, change;		// Used atoms
	} atoms;
	struct
	{
		Cursor					arrow, square, left, right, bottom, resize;				// Used cursors
	} cursors;
} SubDisplay;

extern SubDisplay *d;

int subDisplayNew(const char *display_string);				// Create a new display
void subDisplayKill(void);														// Delete a display

/* log.c */
#ifdef DEBUG
void subLogToggleDebug(void);
#define subLogDebug(...)	subLog(0, __FILE__, __LINE__, __VA_ARGS__);
#else
#define subLogDebug(...)
#endif /* DEBUG */

#define subLogError(...)	subLog(1, __FILE__, __LINE__, __VA_ARGS__);
#define subLogWarn(...)		subLog(2, __FILE__, __LINE__, __VA_ARGS__);

void subLog(short type, const char *file, 						// Print messages
	short line, const char *format, ...);

/* lua.c */
typedef void(*SubLuaFunction)(const char *, const char *);

int subLuaNew(void);																	// Open a Lua state
void subLuaKill(void);																// Close a Lua state
int subLuaLoad(const char *path, const char *file);		// Load a Lua script
void subLuaForeach(const char *table, 								// Do fearch field in table
	SubLuaFunction function);
int subLuaCall(const char *function, char **data);		// Call a Lua function
int subLuaGetNum(const char *table, 									// Get number from table
	const char *field, int fallback);
char *subLuaGetString(const char *table,			 				// Get string from table
	const char *field, char *fallback);

/* sublet.c */
typedef struct subsublet
{
	unsigned int time, interval;
	const char *function;
	char *data;
	Window win;
} SubSublet;

typedef struct subsubletlist
{
	int size;
	SubSublet **content;
} SubSubletList;

SubSublet *subSubletFind(Window win);									// Find a sublet window
SubSublet *subSubletGetRecent(void);									// Get the last recent sublet
void subSubletNew(void);															// Create a sublet
void subSubletAdd(const char *function,								// Add a new sublet item 
	unsigned int interval, unsigned int width);
void subSubletKill(void);															// Delete all sublet items

/* event.c */
int subEventGetTime(void);														// Get the current time
int subEventLoop(void);																// Event loop

#endif /* SUBTLE_H */
