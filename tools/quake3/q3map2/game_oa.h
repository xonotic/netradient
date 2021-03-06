/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* marker */
#ifndef GAME_OPENARENA_H
#define GAME_OPENARENA_H



/* -------------------------------------------------------------------------------

   content and surface flags

   ------------------------------------------------------------------------------- */

// reuses defines from game_quake3.h



/* -------------------------------------------------------------------------------

   game_t struct

   ------------------------------------------------------------------------------- */

{
	"oa",               /* -game x */
	"baseoa",           /* default base game data dir */
	".openarena",       /* unix home sub-dir */
	"openarena",        /* magic path word */
	"scripts",          /* shader directory */
	64,                 /* max lightmapped surface verts */
	999,                /* max surface verts */
	6000,               /* max surface indexes */
	qfalse,             /* enable per shader prefix surface flags and .tex file */
	qfalse,             /* flares */
	"flareshader",      /* default flare shader */
	qfalse,             /* wolf lighting model? */
	128,                /* lightmap width/height */
	1.0f,               /* lightmap gamma */
	qfalse,             /* lightmap sRGB */
	qfalse,             /* texture sRGB */
	qfalse,             /* color sRGB */
	0.0f,               /* lightmap exposure */
	1.0f,               /* lightmap compensate */
	1.0f,               /* lightgrid scale */
	1.0f,               /* lightgrid ambient scale */
	qfalse,             /* light angle attenuation uses half-lambert curve */
	qfalse,             /* disable shader lightstyles hack */
	qfalse,             /* keep light entities on bsp */
	8,                  /* default patchMeta subdivisions tolerance */
	qfalse,             /* patch casting enabled */
	qfalse,             /* compile deluxemaps */
	0,                  /* deluxemaps default mode */
	512,                /* minimap size */
	1.0f,               /* minimap sharpener */
	0.0f,               /* minimap border */
	qtrue,              /* minimap keep aspect */
	MINIMAP_MODE_GRAY,  /* minimap mode */
	"%s.tga",           /* minimap name format */
	MINIMAP_SIDECAR_NONE, /* minimap sidecar format */
	"IBSP",             /* bsp file prefix */
	46,                 /* bsp file version */
	qfalse,             /* cod-style lump len/ofs order */
	LoadIBSPFile,       /* bsp load function */
	WriteIBSPFile,      /* bsp write function */

	{
		/* name				contentFlags				contentFlagsClear			surfaceFlags				surfaceFlagsClear			compileFlags				compileFlagsClear */

		/* default */
		{ "default",        Q_CONT_SOLID,               -1,                         0,                          -1,                         C_SOLID,                    -1 },


		/* ydnar */
		{ "lightgrid",      0,                          0,                          0,                          0,                          C_LIGHTGRID,                0 },
		{ "antiportal",     0,                          0,                          0,                          0,                          C_ANTIPORTAL,               0 },
		{ "skip",           0,                          0,                          0,                          0,                          C_SKIP,                     0 },


		/* compiler */
		{ "origin",         Q_CONT_ORIGIN,              Q_CONT_SOLID,               0,                          0,                          C_ORIGIN | C_TRANSLUCENT,   C_SOLID },
		{ "areaportal",     Q_CONT_AREAPORTAL,          Q_CONT_SOLID,               0,                          0,                          C_AREAPORTAL | C_TRANSLUCENT,   C_SOLID },
		{ "trans",          Q_CONT_TRANSLUCENT,         0,                          0,                          0,                          C_TRANSLUCENT,              0 },
		{ "detail",         Q_CONT_DETAIL,              0,                          0,                          0,                          C_DETAIL,                   0 },
		{ "structural",     Q_CONT_STRUCTURAL,          0,                          0,                          0,                          C_STRUCTURAL,               0 },
		{ "hint",           0,                          0,                          Q_SURF_HINT,                0,                          C_HINT,                     0 },
		{ "nodraw",         0,                          0,                          Q_SURF_NODRAW,              0,                          C_NODRAW,                   0 },

		{ "alphashadow",    0,                          0,                          Q_SURF_ALPHASHADOW,         0,                          C_ALPHASHADOW | C_TRANSLUCENT,  0 },
		{ "lightfilter",    0,                          0,                          Q_SURF_LIGHTFILTER,         0,                          C_LIGHTFILTER | C_TRANSLUCENT,  0 },
		{ "nolightmap",     0,                          0,                          Q_SURF_VERTEXLIT,           0,                          C_VERTEXLIT,                0 },
		{ "pointlight",     0,                          0,                          Q_SURF_VERTEXLIT,           0,                          C_VERTEXLIT,                0 },


		/* game */
		{ "nonsolid",       0,                          Q_CONT_SOLID,               Q_SURF_NONSOLID,            0,                          0,                          C_SOLID },

		{ "trigger",        Q_CONT_TRIGGER,             Q_CONT_SOLID,               0,                          0,                          C_TRANSLUCENT,              C_SOLID },

		{ "water",          Q_CONT_WATER,               Q_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },
		{ "slime",          Q_CONT_SLIME,               Q_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },
		{ "lava",           Q_CONT_LAVA,                Q_CONT_SOLID,               0,                          0,                          C_LIQUID | C_TRANSLUCENT,   C_SOLID },

		{ "playerclip",     Q_CONT_PLAYERCLIP,          Q_CONT_SOLID,               0,                          0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
		{ "monsterclip",    Q_CONT_MONSTERCLIP,         Q_CONT_SOLID,               0,                          0,                          C_DETAIL | C_TRANSLUCENT,   C_SOLID },
		{ "nodrop",         Q_CONT_NODROP,              Q_CONT_SOLID,               0,                          0,                          C_TRANSLUCENT,              C_SOLID },

		{ "clusterportal",  Q_CONT_CLUSTERPORTAL,       Q_CONT_SOLID,               0,                          0,                          C_TRANSLUCENT,              C_SOLID },
		{ "donotenter",     Q_CONT_DONOTENTER,          Q_CONT_SOLID,               0,                          0,                          C_TRANSLUCENT,              C_SOLID },
		{ "botclip",        Q_CONT_BOTCLIP,             Q_CONT_SOLID,               0,                          0,                          C_TRANSLUCENT,              C_SOLID },

		{ "fog",            Q_CONT_FOG,                 Q_CONT_SOLID,               0,                          0,                          C_FOG,                      C_SOLID },
		{ "sky",            0,                          0,                          Q_SURF_SKY,                 0,                          C_SKY,                      0 },

		{ "slick",          0,                          0,                          Q_SURF_SLICK,               0,                          0,                          0 },

		{ "noimpact",       0,                          0,                          Q_SURF_NOIMPACT,            0,                          0,                          0 },
		{ "nomarks",        0,                          0,                          Q_SURF_NOMARKS,             0,                          C_NOMARKS,                  0 },
		{ "ladder",         0,                          0,                          Q_SURF_LADDER,              0,                          0,                          0 },
		{ "nodamage",       0,                          0,                          Q_SURF_NODAMAGE,            0,                          0,                          0 },
		{ "metalsteps",     0,                          0,                          Q_SURF_METALSTEPS,          0,                          0,                          0 },
		{ "flesh",          0,                          0,                          Q_SURF_FLESH,               0,                          0,                          0 },
		{ "nosteps",        0,                          0,                          Q_SURF_NOSTEPS,             0,                          0,                          0 },
		{ "nodlight",       0,                          0,                          Q_SURF_NODLIGHT,            0,                          0,                          0 },
		{ "dust",           0,                          0,                          Q_SURF_DUST,                0,                          0,                          0 },

		/* null */
		{ NULL, 0, 0, 0, 0, 0, 0 }
	}
}



/* end marker */
#endif
