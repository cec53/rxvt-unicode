/*
 * rxvttoolkit.h - provide toolkit-functionality for rxvt.
 */
#ifndef RXVT_TOOLKIT_H
#define RXVT_TOOLKIT_H

#include <X11/Xlib.h>

#if XFT
# include <X11/Xft/Xft.h>
#endif

#include "iom.h"

#include "rxvtlib.h"
#include "rxvtutil.h"

#include "callback.h"

// see rxvttoolkit.C:xa_names, which must be kept in sync
enum {
  XA_TEXT,
  XA_COMPOUND_TEXT,
  XA_UTF8_STRING,
  XA_MULTIPLE,
  XA_TARGETS,
  XA_TIMESTAMP,
  XA_VT_SELECTION,
  XA_INCR,
  XA_WM_PROTOCOLS,
  XA_WM_DELETE_WINDOW,
  XA_CLIPBOARD,
#if ENABLE_FRILLS
  XA_MOTIF_WM_HINTS,
#endif
#if ENABLE_EWMH
  XA_NET_WM_PID,
  XA_NET_WM_NAME,
  XA_NET_WM_ICON_NAME,
  XA_NET_WM_PING,
#endif
#if USE_XIM
  XA_WM_LOCALE_NAME,
  XA_XIM_SERVERS,
#endif
#if TRANSPARENT
  XA_XROOTPMAP_ID,
  XA_ESETROOT_PMAP_ID,
#endif
#if ENABLE_XEMBED
  XA_XEMBED,
  XA_XEMBED_INFO,
#endif
  NUM_XA
};

struct rxvt_term;
struct rxvt_display;

struct im_watcher;
struct xevent_watcher;

struct refcounted {
  int referenced;
  char *id;

  refcounted (const char *id);
  bool ref_init () { return false; }
  void ref_next () { }
  ~refcounted ();
};

template<class T>
struct refcache : vector<T *> {
  T *get (const char *id);
  void put (T *obj);
  void clear ();

  ~refcache ()
  {
    clear ();
  }
};

/////////////////////////////////////////////////////////////////////////////

#ifdef USE_XIM
struct rxvt_xim : refcounted {
  void destroy ();
  rxvt_display *display;

//public
  XIM xim;

  rxvt_xim (const char *id) : refcounted (id) { }
  bool ref_init ();
  ~rxvt_xim ();
};
#endif

struct rxvt_display : refcounted {
  io_manager_vec<xevent_watcher> xw;

  io_watcher x_ev; void x_cb (io_watcher &w, short revents);

#ifdef USE_XIM
  refcache<rxvt_xim> xims;
  vector<im_watcher *> imw;

  void im_change_cb ();
  void im_change_check ();
#endif

//public
  Display   *display;
  int       depth;
  int       screen;
  Visual    *visual;
  Colormap  cmap;
  Window    root;
  rxvt_term *selection_owner;
  Atom      xa[NUM_XA];
#ifndef NO_SLOW_LINK_SUPPORT
  bool      is_local;
#endif
#ifdef POINTER_BLANK
  Cursor    blank_cursor;
#endif

  rxvt_display (const char *id);
  XrmDatabase get_resources ();
  bool ref_init ();
  void ref_next ();
  ~rxvt_display ();

  void flush ();
  Atom atom (const char *name);
  void set_selection_owner (rxvt_term *owner);

  void reg (xevent_watcher *w);
  void unreg (xevent_watcher *w);

#ifdef USE_XIM
  void reg (im_watcher *w);
  void unreg (im_watcher *w);

  rxvt_xim *get_xim (const char *locale, const char *modifiers);
  void put_xim (rxvt_xim *xim);
#endif
};

#ifdef USE_XIM
struct im_watcher : watcher, callback0<void> {
  template<class O1, class O2>
  im_watcher (O1 *object, void (O2::*method) ())
  : callback0<void> (object,method)
  { }

  void start (rxvt_display *display)
  {
    display->reg (this);
  }
  void stop (rxvt_display *display)
  {
    display->unreg (this);
  }
};
#endif

struct xevent_watcher : watcher, callback1<void, XEvent &> {
  Window window;

  template<class O1, class O2>
  xevent_watcher (O1 *object, void (O2::*method) (XEvent &))
  : callback1<void, XEvent &> (object,method)
  { }

  void start (rxvt_display *display, Window window)
  {
    this->window = window;
    display->reg (this);
  }
  void stop (rxvt_display *display)
  {
    display->unreg (this);
  }
};

extern refcache<rxvt_display> displays;

/////////////////////////////////////////////////////////////////////////////

typedef unsigned long Pixel;

struct rxvt_color {
#if XFT
  XftColor c;
  operator Pixel () const { return c.pixel; }
#else
  Pixel p;
  operator Pixel () const { return p; }
#endif

  bool operator == (const rxvt_color &b) const { return Pixel (*this) == Pixel (b); }
  bool operator != (const rxvt_color &b) const { return Pixel (*this) != Pixel (b); }

  void get (rxvt_display *display, unsigned short &cr, unsigned short &cg, unsigned short &cb);
 
  bool set (rxvt_display *display, Pixel p);
  bool set (rxvt_display *display, const char *name);
  bool set (rxvt_display *display, unsigned short cr, unsigned short cg, unsigned short cb);

  rxvt_color fade (rxvt_display *, int percent); // fades to black
  rxvt_color fade (rxvt_display *, int percent, rxvt_color &fadeto);

  void free (rxvt_display *display);
};

#endif

