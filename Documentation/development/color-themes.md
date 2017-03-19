KiCad Color Themes
==================

Theme Files
-----------

A theme file is a JSON file that stores colors for one or more KiCad applications.
Each application can choose a theme to use (they don't have to be the same across applications), and users can edit themes from inside the application.

The default theme is a special case -- it is stored in the code, and cannot be edited by users.
If users try to edit the default theme, a new theme is automatically created (copy-on-write) and the user is prompted to save it with a new name.

Here is an abbreviated theme file:

```
{
    "kicad-theme-version": 1,
    "name": "My Custom Theme",
    "eeschema": {
        "wire": "rgb(166, 226, 46)",
        "bus": "rgb(174, 129, 255)",
        /* ... */
    },
    "pcbnew": {
        "F.Cu": "rgb(194, 0, 0)",
        "In1.Cu": "rgb(194, 194, 0)",
        /* ... */
        "Ratsnest": "rgb(255, 255, 255)",
        /* ... */
    },
    "gerbview": {
        /* ... */
    }
}
```

Theme files must contain at minimum the following keys:

* `kicad-theme-version`: This should be set to `1` for now.

* `name`: This is the name of the theme displayed in the GUI

* One or more of `eeschema`, `pcbnew`, `gerbview`: Dictionaries containing color settings for the
various KiCad applications.

COLOR_THEME class
-----------------

The `COLOR_THEME` class stores an in-memory representation of a theme file.  It also has a static mapping of layer IDs to keys in the JSON theme file format.  Each "layer" in KiCad internal terms is a set of drawn objects that share certain properties, including color, visibility, and draw order.  So, each color setting maps to exactly one layer.  Most layers are unique to each application, but some layers are shared between applications, such as LAYER_GRID and LAYER_GP_OVERLAY which are used in GAL-enabled applications to draw the grid and various overlays and preview objects.  Some layers do not have color settings (such as LAYER_GP_OVERLAY) but most do.  Many layer colors are settable by the user, but some are currently not shown in the GUI (but can be edited in a theme file).

