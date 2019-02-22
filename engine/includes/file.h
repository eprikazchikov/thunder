/*
    This file is part of Thunder Next.

    Thunder Next is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Thunder Next is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Thunder Next.  If not, see <http://www.gnu.org/licenses/>.

    Copyright: 2008-2014 Evgeny Prikazchikov
*/

#ifndef FILEIO_H
#define FILEIO_H

#include <stdint.h>
#include <string>
#include <list>

#include "engine_export.h"

using namespace std;

typedef void            _FILE;
typedef	uint64_t        _size_t;
typedef list<string>    StringList;

class ENGINE_EXPORT IFile {
public:
    void                finit           (const char *argv0);
    void                fsearchPathAdd  (const char *path, bool isFirst = false);

    virtual StringList  _flist          (const char *path);

    virtual bool        _mkdir          (const char *path);

    virtual bool        _delete         (const char *path);

    virtual bool        _exists         (const char *path);

    virtual bool        _isdir          (const char *path);

    virtual int         _fclose         (_FILE *stream);

    virtual _size_t     _fseek          (_FILE *stream, long int offset, int origin);

    virtual _FILE      *_fopen          (const char *path, const char *mode);

    virtual _size_t     _fread          (void *ptr, _size_t size, _size_t count, _FILE *stream);

    virtual _size_t     _fwrite         (const void *ptr, _size_t size, _size_t count, _FILE *stream);

    virtual _size_t     _fsize          (_FILE *stream);

    virtual _size_t     _ftell          (_FILE *stream);

    virtual const char *baseDir         () const;

    virtual const char *userDir         () const;
};

#endif // FILEIO_H
