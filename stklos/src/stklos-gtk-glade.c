/*
 * stklos-gtk-glade.c	-- STKLOS-GTK-GLADE Socket support
 * 
 * Copyright © 2005-2008 Erick Gallesio - I3S-CNRS/ESSI <eg@essi.fr>
 * 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, 
 * USA.
 * 
 *           Author: Erick Gallesio [eg@essi.fr]
 *    Creation date: 15-Mar-2005 22:00 (eg)
 * Last file update:  8-Apr-2008 22:54 (eg)
 */

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "stklos.h"

static SCM glade_module; 

/* GladeXMLConnectFunc:
 * @handler_name: the name of the handler function to connect.
 * @object: the object to connect the signal to.
 * @signal_name: the name of the signal.
 * @signal_data: the string value of the signal data given in the XML file.
 * @connect_object: non NULL if g_signal_connect_object should be used.
 * @after: TRUE if the connection should be made with g_signal_connect_after.
 * @user_data: the user data argument.
 */

/* This function is just a wrapper which calls the Scheme function "glade-connect"
 * with all the parameters it receives (except connect_object) 
 */
static void generic_handler(const gchar *handler_name,
			    GObject *object,
			    const gchar *signal_name,
			    const gchar *signal_data,
			    GObject *connect_object,
			    gboolean after,
			    gpointer user_data)
{
  SCM obj, ref, fct, glade_connect_symb = STk_intern("%glade-connect");

  /*
   fprintf(stderr, "** Generic name= %s object=%p signame=%s sigdata=%s ... connectobj=%p after=%d data=%p\n",
	  handler_name, object, signal_name, signal_data, connect_object, 
	  after, user_data);
  */
  obj = STk_make_Cpointer(object, STk_void, STk_false);
  fct = STk_lookup(glade_connect_symb, glade_module, &ref, TRUE);

  STk_C_apply(fct, 6, 
	           STk_intern((char*) handler_name),
	      	   obj, 
	      	   STk_Cstring2string((char*) signal_name),
	           (signal_data ? STk_Cstring2string((char*)signal_data) : 
		                  STk_false),
	      	   MAKE_BOOLEAN(after),
	           user_data);
}


DEFINE_PRIMITIVE("glade-xml-signal-autoconnect", gtk_autoconnect, subr1, 
		 (SCM object))
{
  if (!CPOINTERP(object))
    STk_error("bad object ~S", object);
  /* Use the full form instad of the simplified one */
  glade_xml_signal_autoconnect_full(CPOINTER_VALUE(object), generic_handler, 
				    STk_current_module());
  return STk_void;
}

/* ----------------------------------------------------------------------
 * 
 * 	Module stklos-gtk-glade starts here
 *
 * ---------------------------------------------------------------------- 
 */
#include "stklos-gtk-glade-inc.c"

MODULE_ENTRY_START("stklos-gtk-glade") {
  /* Create a new module named "stklos-gtk-glade" */
  glade_module = STk_create_module(STk_intern("stklos-gtk-glade"));
    
  /* Add new primitives */
  ADD_PRIMITIVE_IN_MODULE(gtk_autoconnect, glade_module);

  /* Execute Scheme code */
  STk_execute_C_bytecode(__module_consts, __module_code);
} MODULE_ENTRY_END

MODULE_ENTRY_INFO() {
   return STk_read_from_C_string(__module_infos);
}
