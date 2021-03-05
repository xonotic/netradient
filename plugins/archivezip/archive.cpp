/*
   Copyright (C) 2001-2006, William Joseph.
   All Rights Reserved.

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
 */

#include "idatastream.h"
#include "cmdlib.h"
#include "bytestreamutils.h"

#include "modulesystem.h"
#include "iarchive.h"

#include <algorithm>
#include <glib.h>
#include "stream/filestream.h"
#include "container/array.h"
#include "archivelib.h"
#include "zlibstream.h"
#include "imagelib.h"

class DeflatedArchiveFile : public ArchiveFile
{
CopiedString m_name;
FileInputStream m_istream;
SubFileInputStream m_substream;
DeflatedInputStream m_zipstream;
FileInputStream::size_type m_size;
public:
typedef FileInputStream::size_type size_type;
typedef FileInputStream::position_type position_type;

DeflatedArchiveFile( const char* name, const char* archiveName, position_type position, size_type stream_size, size_type file_size )
	: m_name( name ), m_istream( archiveName ), m_substream( m_istream, position, stream_size ), m_zipstream( m_substream ), m_size( file_size ){
}

void release(){
	delete this;
}

size_type size() const {
	return m_size;
}

const char* getName() const {
	return m_name.c_str();
}

InputStream& getInputStream(){
	return m_zipstream;
}
};

class DeflatedArchiveTextFile : public ArchiveTextFile
{
CopiedString m_name;
FileInputStream m_istream;
SubFileInputStream m_substream;
DeflatedInputStream m_zipstream;
BinaryToTextInputStream<DeflatedInputStream> m_textStream;

public:
typedef FileInputStream::size_type size_type;
typedef FileInputStream::position_type position_type;

DeflatedArchiveTextFile( const char* name, const char* archiveName, position_type position, size_type stream_size )
	: m_name( name ), m_istream( archiveName ), m_substream( m_istream, position, stream_size ), m_zipstream( m_substream ), m_textStream( m_zipstream ){
}

void release(){
	delete this;
}

TextInputStream& getInputStream(){
	return m_textStream;
}
};

#include "pkzip.h"

#include <map>
#include "string/string.h"
#include "fs_filesystem.h"

class ZipArchive : public Archive
{
class ZipRecord
{
public:
enum ECompressionMode
{
	eStored,
	eDeflated,
};

ZipRecord( unsigned int position, unsigned int compressed_size, unsigned int uncompressed_size, ECompressionMode mode, bool is_symlink )
	: m_position( position ), m_stream_size( compressed_size ), m_file_size( uncompressed_size ), m_mode( mode ), m_is_symlink( is_symlink ){
}

unsigned int m_position;
unsigned int m_stream_size;
unsigned int m_file_size;
ECompressionMode m_mode;
bool m_is_symlink;
// Do not resolve more than 5 recursive symbolic links to
// prevent circular symbolic links.
int m_max_symlink_depth = 5;
};

typedef GenericFileSystem<ZipRecord> ZipFileSystem;
ZipFileSystem m_filesystem;
CopiedString m_name;
FileInputStream m_istream;

bool is_file_symlink( unsigned int filemode ){
	// see https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/stat.h
	// redefine so it works outside of Unices
	constexpr int RADIANT_S_IFMT = 00170000;
	constexpr int RADIANT_S_IFLNK = 0120000;
	// see https://trac.edgewall.org/attachment/ticket/8919/ZipDownload.patch
	constexpr int PKZIP_EXTERNAL_ATTR_FILE_TYPE_SHIFT = 16;
	unsigned long attr = filemode >> PKZIP_EXTERNAL_ATTR_FILE_TYPE_SHIFT;
	return (attr & RADIANT_S_IFMT) == RADIANT_S_IFLNK;
}

bool read_record(){
	zip_magic magic;
	istream_read_zip_magic( m_istream, magic );
	if ( !( magic == zip_root_dirent_magic ) ) {
		return false;
	}
	zip_version version_encoder;
	istream_read_zip_version( m_istream, version_encoder );
	zip_version version_extract;
	istream_read_zip_version( m_istream, version_extract );
	//unsigned short flags =
	istream_read_int16_le( m_istream );
	unsigned short compression_mode = istream_read_int16_le( m_istream );
	if ( compression_mode != Z_DEFLATED && compression_mode != 0 ) {
		return false;
	}
	zip_dostime dostime;
	istream_read_zip_dostime( m_istream, dostime );
	//unsigned int crc32 =
	istream_read_int32_le( m_istream );
	unsigned int compressed_size = istream_read_uint32_le( m_istream );
	unsigned int uncompressed_size = istream_read_uint32_le( m_istream );
	unsigned int namelength = istream_read_uint16_le( m_istream );
	unsigned short extras = istream_read_uint16_le( m_istream );
	unsigned short comment = istream_read_uint16_le( m_istream );
	//unsigned short diskstart =
	istream_read_int16_le( m_istream );
	//unsigned short filetype =
	istream_read_int16_le( m_istream );
	unsigned int filemode = istream_read_int32_le( m_istream );
	unsigned int position = istream_read_int32_le( m_istream );

	Array<char> filename( namelength + 1 );
	m_istream.read( reinterpret_cast<FileInputStream::byte_type*>( filename.data() ), namelength );
	filename[namelength] = '\0';

	bool is_symlink = is_file_symlink( filemode );

//	if ( is_symlink ) {
//		globalOutputStream() << "Warning: zip archive " << makeQuoted( m_name.c_str() ) << " contains symlink file: " << makeQuoted( filename.data() ) << "\n";
//	}

	m_istream.seek( extras + comment, FileInputStream::cur );

	if ( path_is_directory( filename.data() ) ) {
		m_filesystem[filename.data()] = 0;
	}
	else
	{
		ZipFileSystem::entry_type& file = m_filesystem[filename.data()];
		if ( !file.is_directory() ) {
			globalOutputStream() << "Warning: zip archive " << makeQuoted( m_name.c_str() ) << " contains duplicated file: " << makeQuoted( filename.data() ) << "\n";
		}
		else
		{
			file = new ZipRecord( position, compressed_size, uncompressed_size, ( compression_mode == Z_DEFLATED ) ? ZipRecord::eDeflated : ZipRecord::eStored, is_symlink );
		}
	}

	return true;
}

bool read_pkzip(){
	SeekableStream::position_type pos = pkzip_find_disk_trailer( m_istream );
	if ( pos != 0 ) {
		zip_disk_trailer disk_trailer;
		m_istream.seek( pos );
		istream_read_zip_disk_trailer( m_istream, disk_trailer );
		if ( !( disk_trailer.z_magic == zip_disk_trailer_magic ) ) {
			return false;
		}

		m_istream.seek( disk_trailer.z_rootseek );
		for ( unsigned int i = 0; i < disk_trailer.z_entries; ++i )
		{
			if ( !read_record() ) {
				return false;
			}
		}
		return true;
	}
	return false;
}

public:
ZipArchive( const char* name )
	: m_name( name ), m_istream( name ){
	if ( !m_istream.failed() ) {
		if ( !read_pkzip() ) {
			globalErrorStream() << "ERROR: invalid zip file " << makeQuoted( name ) << '\n';
		}
	}
}

~ZipArchive(){
	for ( ZipFileSystem::iterator i = m_filesystem.begin(); i != m_filesystem.end(); ++i )
	{
		delete i->second.file();
	}
}

bool failed(){
	return m_istream.failed();
}

void release(){
	delete this;
}

// The zip format has a maximum filename size of 64K
static const int MAX_FILENAME_BUF = 65537;

/* The symlink implementation is ported from Dæmon engine implementation by slipher which was a complete rewrite of one illwieckz did on Dæmon by taking inspiration from Darkplaces engine.

See:

- https://github.com/DaemonEngine/Daemon/blob/master/src/common/FileSystem.cpp
- https://gitlab.com/xonotic/darkplaces/-/blob/div0-stable/fs.c

Some words by slipher:

> Symlinks are a bad feature which you should not use. Therefore, the implementation is as
> slow as possible with a full iteration of the archive performed for each symlink.

> The symlink path `relative` must be relative to the symlink's location.
> Only supports paths consisting of "../" 0 or more times, followed by non-magical path components.
*/

static void resolveSymlinkPath( const char* base, const char* relative, char* resolved ){

	base = g_path_get_dirname( base );

	while( g_str_has_prefix( relative, "../" ) )
	{
		if ( base[0] == '\0' )
		{
			globalErrorStream() << "Error while reading symbolic link " << makeQuoted( base ) << ": no such directory\n";
			resolved[0] = '\0';
			return;
		}

		base = g_path_get_dirname( base );
		relative += 3;
	}

	snprintf( resolved, MAX_FILENAME_BUF, "%s/%s", base, relative);
}

ArchiveFile* readFile( const char* name, ZipRecord* file ){
	switch ( file->m_mode )
	{
		case ZipRecord::eStored:
			return StoredArchiveFile::create( name, m_name.c_str(), m_istream.tell(), file->m_stream_size, file->m_file_size );
		case ZipRecord::eDeflated:
		default: // silence warning about function not returning
			return new DeflatedArchiveFile( name, m_name.c_str(), m_istream.tell(), file->m_stream_size, file->m_file_size );
	}
}

void readSymlink( const char* name, ZipRecord* file, char* resolved ){
		globalOutputStream() << "Found symbolic link: " << makeQuoted( name ) << "\n";

		if ( file->m_max_symlink_depth == 0 ) {
		globalErrorStream() << "Maximum symbolic link depth reached\n";
			return;
		}

		file->m_max_symlink_depth--;

		ArchiveFile* symlink_file = readFile( name, file );
		ScopedArchiveBuffer buffer( *symlink_file );
		const char* relative = (const char*) buffer.buffer;

		resolveSymlinkPath( name, relative, resolved );
		globalOutputStream() << "Resolved symbolic link: " << makeQuoted( resolved ) << "\n";
}

ArchiveFile* openFile( const char* name ){
	ZipFileSystem::iterator i = m_filesystem.find( name );

	if ( i != m_filesystem.end() && !i->second.is_directory() ) {
		ZipRecord* file = i->second.file();

		m_istream.seek( file->m_position );
		zip_file_header file_header;
		istream_read_zip_file_header( m_istream, file_header );

		if ( file_header.z_magic != zip_file_header_magic ) {
			globalErrorStream() << "error reading zip file " << makeQuoted( m_name.c_str() );
			return 0;
		}

		if ( file->m_is_symlink ) {
			char resolved[MAX_FILENAME_BUF];

			readSymlink( name, file, resolved );

			// slow as possible full iteration of the archive
			return openFile( resolved );
		}

		return readFile( name, file );
	}

	return 0;
}

ArchiveTextFile* readTextFile( const char* name, ZipRecord* file ){
	switch ( file->m_mode )
	{
		case ZipRecord::eStored:
			return StoredArchiveTextFile::create( name, m_name.c_str(), m_istream.tell(), file->m_stream_size );
		case ZipRecord::eDeflated:
		default: // silence warning about function not returning
			return new DeflatedArchiveTextFile( name, m_name.c_str(), m_istream.tell(), file->m_stream_size );
	}
}

ArchiveTextFile* openTextFile( const char* name ){
	ZipFileSystem::iterator i = m_filesystem.find( name );

	if ( i != m_filesystem.end() && !i->second.is_directory() ) {
		ZipRecord* file = i->second.file();

		m_istream.seek( file->m_position );
		zip_file_header file_header;
		istream_read_zip_file_header( m_istream, file_header );

		if ( file_header.z_magic != zip_file_header_magic ) {
			globalErrorStream() << "error reading zip file " << makeQuoted( m_name.c_str() );
			return 0;
		}

		if ( file->m_is_symlink ) {
			char resolved[MAX_FILENAME_BUF];

			readSymlink( name, file, resolved );

			// slow as possible full iteration of the archive
			return openTextFile( resolved );
		}

		return readTextFile( name, file );
	}

	return 0;

}

bool containsFile( const char* name ){
	ZipFileSystem::iterator i = m_filesystem.find( name );
	return i != m_filesystem.end() && !i->second.is_directory();

}
void forEachFile( VisitorFunc visitor, const char* root ){
	m_filesystem.traverse( visitor, root );
}

};

Archive* OpenArchive( const char* name ){
	return new ZipArchive( name );
}

#if 0

class TestZip
{
class TestVisitor : public Archive::IVisitor
{
public:
void visit( const char* name ){
	int bleh = 0;
}
};
public:
TestZip(){
	testzip( "c:/quake3/baseq3/mapmedia.pk3", "textures/radiant/notex.tga" );
}

void testzip( const char* name, const char* filename ){
	Archive* archive = OpenArchive( name );
	ArchiveFile* file = archive->openFile( filename );
	if ( file != 0 ) {
		unsigned char buffer[4096];
		std::size_t count = file->getInputStream().read( (InputStream::byte_type*)buffer, 4096 );
		file->release();
	}
	TestVisitor visitor;
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eFilesAndDirectories, 0 ), "" );
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eFilesAndDirectories, 1 ), "" );
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eFiles, 1 ), "" );
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eDirectories, 1 ), "" );
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eFilesAndDirectories, 1 ), "textures" );
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eFilesAndDirectories, 1 ), "textures/" );
	archive->forEachFile( Archive::VisitorFunc( &visitor, Archive::eFilesAndDirectories, 2 ), "" );
	archive->release();
}
};

TestZip g_TestZip;

#endif
