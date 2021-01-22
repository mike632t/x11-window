/*
 * x11-window.c
 *
 * Displays a window on the screen.
 *
 * This  program is free software: you can redistribute it and/or modify it
 * under  the terms of the GNU General Public License as published  by  the
 * Free  Software Foundation, either version 3 of the License, or (at  your
 * option) any later version.
 *
 * This  program  is distributed in the hope that it will  be  useful,  but
 * WITHOUT   ANY   WARRANTY;   without even   the   implied   warranty   of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You  should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 'gcc x11-window.c -o x11-window -lX11' - linux/gcc
 *
 * 'cc x11-window' - VAX/VMS
 * 'link x11-window, sys$input/opt'
 * 'sys$share:decw$xlibshr/share'
 *
 * 28 Apr 13   0.1   - Initial version - MT
 * 14 Mar 16         - Use window hints to position the window - MT
 *                   - Added  code  to handle window close events from  the
 *                     window manager - MT
 * 27 May 18         - Set background colour using named colour - MT
 * 28 May 18         - Trap key press and mouse button events - MT
 * 26 Dec 18         - Added a lookup table for event names, and debug code
 *                     code to print the event names - MT
 *                   - Moved debug macro defination to a seperate file  and
 *                     defined version number and build information - MT
 *                   - Now handles events when window is moved - MT
 * 28 Dec 18         - Changed  default display name to avoid  issues  when
 *                     compiled on VAX/VMS - MT
 * 16 Aug 20   0.2   - Created a seperate routine to (re)draw window - MT
 *                   - Added an error() and print() function - MT
 * 17 Aug 20         - Changes the background colour of the window when the
 *                     space bar is pressed - MT
 *                   - Displays the colour above the window size - MT
 * 
 * TO DO :           - Fix code so window is centered properly.
 *                   - Draw multiple colours on the window background...
 *
 */
 
#define NAME           "x11-window"
#define VERSION        "0.1"
#define BUILD          "013"
#define AUTHOR         "MT"
 
#define True           1
#define False          0

#define DEBUG          True

#include <X11/Xlib.h> /* XOpenDisplay(), etc. */
#include <X11/Xutil.h> /* XSizeHints */
#include <X11/keysym.h> /* XK_Escape, etc*/

#include <stdio.h> /* fprintf(), etc. */
#include <stdlib.h> /* getenv(), etc. */
#include <string.h> /* strlen(), etc. */
 
#include "x11-debug.h" /* Defines debug macros. */
#include "x11-colors.h" /* Defines a lookup table for X11 colours. */
#include "x11-eventnames.h" /* Defines a lookup table for event names. */
 
#define WIDTH 512
#define HEIGHT 320

unsigned int i_screen; /* Default screen number */
unsigned int i_display_height, i_display_width; /* Screen height and width in pixels. */
unsigned int i_window_left, i_window_top; /* Location of the window's top-left corner. */
unsigned int i_window_border; /* Window's border width. */
unsigned int i_background_colour; /* Window's background colour. */
unsigned int i_colour_depth; /* Window's colour depth. */
unsigned int i_text_top, i_text_left, i_text_width;
unsigned int i_count;
 
char s_text[80]; /* Used to display text in middle of window */

char *s_display_name = ""; /* Default X display name. */

Display *h_display; /* Pointer to X display structure. */
XColor x_background_color, x_true_color; /* Background color */
Window x_application_window; /* Application window structure. */
Window x_root_window; /* Root window structure. */
Window x_child_window; /* Child window returned from XTranslateCoodinates. */
XEvent x_event;
XFontStruct *h_font_info; /* Font to display text.  */
XSizeHints x_window_size; /* Window size hints structure. */
Atom wm_delete_window;
Colormap x_color_map; /* Colour map */
 
void v_draw_window();
   
int main(int argc, char *argv[]){

   /* Open a display. */
   h_display = XOpenDisplay(s_display_name);

   /* If successful create and display a new window. */
   if (h_display){

      /* Get the ID of the root window of the screen. */
      x_root_window = DefaultRootWindow(h_display);

      /* Get the default screen for our X server. */
      i_screen = DefaultScreen(h_display);

      /* Get the height and width of the default screen in pixels. */
      i_display_height = XDisplayHeight(h_display, i_screen);
      i_display_width = XDisplayWidth(h_display, i_screen);

      /* Tell the window manager we want to specify the position and size of
       * the window. */
      x_window_size.flags  = PPosition | PSize | USPosition | USSize;

      /* Set window size. */
      x_window_size.width = WIDTH; /* Set the desired size of the window. */
      x_window_size.height = HEIGHT;
      x_window_size.x = (i_display_width - WIDTH) / 2; /* Centre the window in the middle of the display. */
      x_window_size.y = (i_display_height - HEIGHT) / 2;

      /* Set the window's border width to 4 pixels wide. */
      i_window_border = 4;

      /* Set the window's backgound colour by name using the default color map. */
      x_color_map = DefaultColormap(h_display, i_screen);

      /* Create the application window, as a child of the root window. */
      x_application_window = XCreateSimpleWindow(h_display, /* Display handle. */
         RootWindow(h_display, i_screen),
         x_window_size.x, x_window_size.y,  /* Window position - ignored ? */
         x_window_size.width, /* Window width */
         x_window_size.height, /* Window height */
         i_window_border, /* Border width - ignored ? */
         BlackPixel(h_display, i_screen), /* Preferred method to set border colour to black */
         x_background_color.pixel); /* Background colour - RGB value. */

      XStoreName(h_display, x_application_window, ""); /* Set the window title. */
  
      /* Events are selected using a bit mask that corresponds the events we
       * want to receive. */
      XSelectInput(h_display, x_application_window, 
         ExposureMask | /* Window is redrawn. */
         ButtonPressMask | /* Mouse button is pressed. */
         KeyPressMask | /* Key is pressed. */
         StructureNotifyMask | /* Window resizing. */
         PropertyChangeMask /* Window position */
      );
 
      wm_delete_window = XInternAtom (h_display, "WM_DELETE_WINDOW", False);
      XSetWMProtocols (h_display, x_application_window, &wm_delete_window, 1);
 
      /*   Show the window */
      XMapWindow(h_display, x_application_window);
 
      /* Flush all pending requests to the X server, and wait until they are
       * processed by the X server. */
      XSync(h_display, False);
 
      i_count = 0;
      
      /* Update display and redraw window. */
      if (XAllocNamedColor(h_display, x_color_map, a_colors[i_count],
         &x_background_color, &x_true_color) == 0) {
         debug(print("%s line : %d : \tUnknown color '%s'.\n", __FILE__, __LINE__, a_colors[i_count]));
      } else {
         debug(print("%s line : %d : \tBackground color: %s.\n", __FILE__, __LINE__, a_colors[i_count]));
         XSetWindowBackground(h_display, x_application_window, x_background_color.pixel);
         XClearWindow(h_display, x_application_window);
         XSync(h_display, False); /* Flush all pending requests to the X server. */
         v_draw_window();
      }
      
      /* Main program event loop */
      while (True) {
         XNextEvent(h_display, &x_event);

         debug(print("%s line : %d : \tEvent \t\t: %s\n", __FILE__, __LINE__, x_event_names[x_event.type]));

         /* Draw or redraw the window */
         if (x_event.type == Expose || x_event.type == ConfigureNotify) {
            v_draw_window();
         } else if (x_event.type == ClientMessage) { /* Client messages */
            if ((Atom)x_event.xclient.data.l[0] == wm_delete_window) { /* Check the message type. */
               break; /* Exit if the window manager closes the application window. */
            }
         }
         else if (x_event.type == ButtonPress ) { /* Wait for a key press event */
            debug(
               print("%s line : %d : \tState \t\t: %d\n", __FILE__, __LINE__, x_event.xbutton.state);
               print("%s line : %d : \tButton \t\t: %d\n", __FILE__, __LINE__, x_event.xbutton.button)
            );
         }
         else if (x_event.type == KeyPress) { /* Wait for a key press event */
            debug(
               print("%s line : %d : \tState \t\t: %d\n", __FILE__, __LINE__, x_event.xkey.state);
               print("%s line : %d : \tKeycode \t: %d\n", __FILE__, __LINE__, x_event.xkey.keycode)
            );
            if (XLookupKeysym(&x_event.xkey, 0) == XK_Escape)
               break;
            else if (XLookupKeysym(&x_event.xkey, 0) == XK_space) {
               if (!a_colors[++i_count]) i_count = 0;
               if (XAllocNamedColor(h_display, x_color_map, a_colors[i_count],
                  &x_background_color, &x_true_color) == 0) {
                  debug(print("%s line : %d : \tunknown color '%s'.\n", __FILE__, __LINE__, a_colors[i_count]));
               } else {
                  debug(print("%s line : %d : \tBackground color: %s.\n", __FILE__, __LINE__, a_colors[i_count]));
                  XSetWindowBackground(h_display, x_application_window, x_background_color.pixel);
                  XClearWindow(h_display, x_application_window);
                  XSync(h_display, False); /* Flush all pending requests to the X server, and wait until they are processed by the X server. */
                  v_draw_window();
               }
            }
         }
      }

      /* close connection to server */
      XCloseDisplay(h_display);
      }
   else {
      error("cannot connect to X server '%s'.\n", s_display_name);
      exit(1);
   }

   exit(0);
}

void v_draw_window() {
   /* Get window geometry */
   if (XGetGeometry(h_display, x_application_window,
      &RootWindow(h_display, i_screen),
      &x_window_size.x, &x_window_size.y,
      &x_window_size.width,
      &x_window_size.height,
      &i_window_border,
      &i_colour_depth) == False) {
      error("can't get window geometry.\n");
      exit(1);
   }

   /* Get position relative to root window */
   XTranslateCoordinates(h_display, x_application_window,
      RootWindow(h_display, i_screen),
      x_window_size.x, x_window_size.y, 
      &i_window_left, &i_window_top,
      &x_child_window);

   /* Adjust position to allow for title bar and border */
   i_window_left = i_window_left - (x_window_size.x << 1);
   i_window_top = i_window_top - (x_window_size.y << 1);
   
   /* Print some debugging information to stderr. */
   debug(
      print("%s line : %d : \tDisplay \t: '%s'\n", __FILE__, __LINE__, s_display_name);
      print("%s line : %d : \tResolution \t: %d x %d\n", __FILE__, __LINE__, i_display_width, i_display_height);
      print("%s line : %d : \tBackground \t: #%06X\n", __FILE__, __LINE__, x_true_color);
      print("%s line : %d : \tSize \t\t: %s\n", __FILE__, __LINE__, s_text);
      print("%s line : %d : \tPosition \t: %d x %d\n", __FILE__, __LINE__, i_window_left, i_window_top);
      print("%s line : %d : \tDepth \t\t: %d \n", __FILE__, __LINE__, i_colour_depth); 
   );
   
   /* Get the default font properties */
   h_font_info = XQueryFont(h_display, XGContextFromGC(DefaultGC(h_display, i_screen))); 
 
   sprintf(s_text, "(%s)", a_colors[i_count]); /*
  
   /* Find font size and calculate position of text */
   i_text_width = XTextWidth(h_font_info, s_text, strlen(s_text));
   i_text_left = (x_window_size.width - i_text_width) / 2;
   i_text_top = x_window_size.height / 2 + h_font_info->ascent / 2 - (h_font_info->ascent * 1.5);
   if (i_text_left < i_window_border || i_text_left > x_window_size.width)
      i_text_left = i_window_border;
   if (i_text_top < h_font_info->ascent + i_window_border)
      i_text_top = h_font_info->ascent + i_window_border;
      
   /* Draw the message string in the middle of the window */
   XSetForeground(h_display, DefaultGC(h_display, i_screen),WhitePixel(h_display, i_screen));
   XDrawString(h_display, x_application_window, 
      DefaultGC(h_display, i_screen),
      i_text_left, i_text_top,
      s_text, strlen(s_text));

   sprintf(s_text, "(%d x %d)", x_window_size.width, x_window_size.height);
   
      /* Find font size and calculate position of text */
   i_text_width = XTextWidth(h_font_info, s_text, strlen(s_text));
   i_text_left = (x_window_size.width - i_text_width) / 2;
   i_text_top = x_window_size.height / 2 + h_font_info->ascent / 2;
   if (i_text_left < i_window_border || i_text_left > x_window_size.width)
      i_text_left = i_window_border;
   if (i_text_top < h_font_info->ascent + i_window_border)
      i_text_top = h_font_info->ascent + i_window_border;
      
   /* Draw the message string in the middle of the window */
   XSetForeground(h_display, DefaultGC(h_display, i_screen),BlackPixel(h_display, i_screen));
   XDrawString(h_display, x_application_window, 
      DefaultGC(h_display, i_screen),
      i_text_left, i_text_top,
      s_text, strlen(s_text));
}
