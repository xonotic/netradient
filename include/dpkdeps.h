#ifndef __DPKDEPS_H__
#define __DPKDEPS_H__

#include <locale>
#include "string/string.h"

// Comparaison function for version numbers
// Implementation is based on dpkg's version comparison code (verrevcmp() and order())
// http://anonscm.debian.org/gitweb/?p=dpkg/dpkg.git;a=blob;f=lib/dpkg/version.c;hb=74946af470550a3295e00cf57eca1747215b9311
static int char_weight(char c){
	if (std::isdigit(c))
		return 0;
	else if (std::isalpha(c))
		return c;
	else if (c == '~')
		return -1;
	else if (c)
		return c + 256;
	else
		return 0;
}

int DpkPakVersionCmp(const char* a, const char* b){
	while (*a || *b) {
		int firstDiff = 0;

		while ((*a && !std::isdigit(*a)) || (*b && !std::isdigit(*b))) {
			int ac = char_weight(*a);
			int bc = char_weight(*b);

			if (ac != bc)
				return ac - bc;

			a++;
			b++;
		}

		while (*a == '0')
			a++;
		while (*b == '0')
			b++;

		while (std::isdigit(*a) && std::isdigit(*b)) {
			if (firstDiff == 0)
				firstDiff = *a - *b;
			a++;
			b++;
		}

		if (std::isdigit(*a))
			return 1;
		if (std::isdigit(*b))
			return -1;
		if (firstDiff)
			return firstDiff;
	}

	return false;
}

// release strings after using
bool DpkReadDepsLine( const char *line, char **pakname, char **pakversion ){
	const char* c = line;
	const char* p_name;
	const char* p_name_end;
	const char* p_version;
	const char* p_version_end;

	*pakname = 0;
	*pakversion = 0;

	while ( std::isspace( *c ) && *c != '\0' ) ++c;
	p_name = c;
	while ( !std::isspace( *c ) && *c != '\0' ) ++c;
	p_name_end = c;
	while ( std::isspace( *c ) && *c != '\0' ) ++c;
	p_version = c;
	while ( !std::isspace( *c ) && *c != '\0' ) ++c;
	p_version_end = c;

	if ( p_name_end - p_name > 0 ){
		*pakname = string_clone_range( StringRange( p_name, p_name_end ) );
	} else return false;

	if ( p_version_end - p_version > 0 ) {
		*pakversion = string_clone_range( StringRange( p_version, p_version_end ) );
	}
	return true;
}

#endif
