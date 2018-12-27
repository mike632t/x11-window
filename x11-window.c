/*
 * x11-window.c
 *
 * Displays a window on the screen.
 *
 * This  program is free software: you can redistribute it and/or modify  it
 * under  the  terms of the GNU General Public License as published  by  the
 * Free  Software  Foundation, either version 3 of the License, or (at  your
 * option) any later version.
 *
 * This  program  is  distributed in the hope that it will  be  useful,  but
 * WITHOUT   ANY   WARRANTY;   without  even   the   implied   warranty   of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You  should have received a copy of the GNU General Public License  along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * To compile: 'gcc x11-window.c -o x11-window -lX11' - linux/gcc
 *             'cc x11-window' - VAX/VMS
 *             'link x11-window, sys$input/opt'
 *             'sys$share:decw$xlibshr/share'
 *
 * 28 Apr 13   0.1   - Initial version - MEJT
 *
 */

#define DEBUG True

#include <X11/Xlib.h> /* XOpenDisplay(), etc. */

#include <stdio.h> /* fprintf(), etc. */
#include <stdlib.h> /* getenv(), etc. */
#include <string.h> /* strlen(), etc. */

#ifndef debug /* Don't redefine macro if already defined. */
#define debug(code) do {if(DEBUG){code;}} while(0)
#endif

main(int argc, char *argv[]){

   unsigned int i_screen; /* Default screen number */
   unsigned int i_display_height, i_display_width; /* Screen height and width in pixels. */
   unsigned int i_window_width, i_window_height;   /* New window height and width in pixels. */
   unsigned int i_window_left, i_window_top;    /* Location of the window's top-left corner. */
   unsigned int i_position_left, i_position_top;   /* Location of the window's top-left corner. */
   unsigned int i_window_border; /* Window's border width. */
   unsigned int i_background_colour; /* Window's background colour. */
   unsigned int i_colour_depth; /* Window's colour depth. */
   unsigned int i_text_width;

   unsigned int i_red = 118;
   unsigned int i_green = 132;
   unsigned int i_blue = 143;

   char s_text[21]; /* Text message to display in midle of window */
   char *s_display_name = ""; /* Get pointer to the X display name. */

   Display *h_display; /* Pointer to X display structure. */
   Window x_application_window; /* Application window structure. */
   Window x_root_window; /* Root window structure. */
   Window x_child_window; /* Child window returned from XTranslateCoodinates */
   XEvent x_event;
   XFontStruct *h_font_info;

   /*   Open a display. */
   h_display = XOpenDisplay(s_display_name);

   /*   If successful create and display a new window. */
   if (h_display){

      /* Get the ID of the root window of the screen. */
      x_root_window = DefaultRootWindow(h_display);

      /* Get the default screen for our X server. */
      i_screen = DefaultScreen(h_display);

      /* Get the height and width of the default screen in pixels. */
      i_display_height = XDisplayHeight(h_display, i_screen);
      i_display_width = XDisplayWidth(h_display, i_screen);

      /* Set window size as 800x600 pixels. */
      i_window_width = 800;
      i_window_height = 600;

      /* Set the window's border width to 4 pixels wide. */
      i_window_border = 4;

      /* Set the window's backgound colour. */
      i_background_colour = i_blue | ((i_green | i_red << 8) << 8);

      /* Create the application window, as a child of the root window. */
      x_application_window = XCreateSimpleWindow(h_display, RootWindow(h_display, i_screen),
         i_window_width, i_window_height,  /* Window position - ignored ? */
         i_window_width, /* Window width */
         i_window_height, /* Window height */
         i_window_border, /* Border width - ignored ? */
         BlackPixel(h_display, i_screen), /* Preferred method to set border colour to black */
         i_background_colour); /* Background colour - RGB value. */

      XStoreName(h_display, x_application_window, ""); /* Set the window title. */
      
      /*   Show the window */
      XMapWindow(h_display, x_application_window);

      /* Flush all pending requests to the X server, and wait until they are processed by the X server. */
      XSync(h_display, False);

      /* Select kind of events we are interested in */
      XSelectInput(h_display, x_application_window, ExposureMask | KeyPressMask);

      XFlush(h_display);

      /* Main program event loop */
      while (True) {
         XNextEvent(h_display, &x_event);

         /* Draw or redraw the window */
         if (x_event.type == Expose) {

            /* Get window geometry */
            if (XGetGeometry(h_display, x_application_window,
               &RootWindow(h_display, i_screen),
               &i_window_left, &i_window_top,
               &i_window_width,
               &i_window_height,
               &i_window_border,
               &i_colour_depth) == False) {
               fprintf(stderr, "\nError: %s line : %d : %s: can't get window geometry\n", __FILE__, __LINE__, argv[0]);
               exit(1);
            }

            sprintf(s_text, "(%d x %d)",i_window_width, i_window_height);

            XTranslateCoordinates(h_display, x_application_window,
               RootWindow(h_display, i_screen),
               i_window_left, i_window_top,
               &i_position_left, &i_position_top, /* Returns position relative to root window */
               &x_child_window);

            /* Adjust position to allow for root window title bar and border */

            i_position_left = i_position_left - (i_window_left << 1);
            i_position_top = i_position_top - (i_window_top << 1);

            /* Print some debugging information to stderr. */
            debug(
               fprintf(stderr, "Debug: %s line : %d : \tDisplay \t: '%s'\n", __FILE__, __LINE__, s_display_name);
               fprintf(stderr, "Debug: %s line : %d : \tResolution \t: %d x %d\n", __FILE__, __LINE__, i_display_width, i_display_height);
               fprintf(stderr, "Debug: %s line : %d : \tBackground \t: #%X%X%X\n", __FILE__, __LINE__, i_red, i_green, i_blue);
               fprintf(stderr, "Debug: %s line : %d : \tSize \t\t: %s\n", __FILE__, __LINE__, s_text);
               fprintf(stderr, "Debug: %s line : %d : \tPosition \t: %d x %d\n", __FILE__, __LINE__, i_position_left, i_position_top);
               fprintf(stderr, "Debug: %s line : %d : \tDepth \t\t: %d \n", __FILE__, __LINE__, i_colour_depth);
            );
            
            h_font_info = XQueryFont(h_display, XGContextFromGC(DefaultGC(h_display, i_screen))); /* Get the default font properties */
           
            i_text_width = XTextWidth(h_font_info, s_text, strlen(s_text));
            i_position_left = (i_window_width - i_text_width) / 2;
            i_position_top = i_window_height / 2 + h_font_info->ascent / 2;
            if (i_position_left < i_window_border || i_position_left > i_window_width)
               i_position_left = i_window_border;
            if (i_position_top < h_font_info->ascent + i_window_border)
               i_position_top = h_font_info->ascent + i_window_border;

            /* Draw the message string in the middle of the window */
            XDrawString(h_display, x_application_window,
               DefaultGC(h_display, i_screen),
               i_position_left, i_position_top,
               s_text, strlen(s_text));
         }

         /* Wait for a key press event */
         if (x_event.type == KeyPress) break;
      }

      /* close connection to server */
      XCloseDisplay(h_display);
      }

   else {

   fprintf(stderr, "%s: cannot connect to X server '%s'\n", argv[0], s_display_name);
      exit(1);
   }

   exit(0);
}
