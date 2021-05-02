
#include <stdio.h>
#include <errno.h>

#include "stdiocpp.h"
#include "util.h"
#include "translation.h"
#include "os.h"

stdiocpp::QExcStdio::QExcStdio
(QString text, const FILE *file, bool collectErrno, bool collectStacktrace) :
    QExcCommon(text, false)
{
    m_descrip += (file == nullptr) ? ""
                                   : " - flags: " + QString::number(file->_flags);
    if(collectErrno){
        m_descrip += " (" + QString::number(errno) +
                "): " + translation::strerror_l(errno);
    }

    if(collectStacktrace){
        appendStacktraceToDescrip();
    }

}

FILE *stdiocpp::fopen(const char *pathname, const char *mode)
{
    FILE* f = ::fopen(pathname, mode);
    if(f == nullptr ){
        throw QExcStdio(QString("Cannot open %1 with mode %2: ")
                        .arg(pathname, mode), nullptr, true);
    }
    return f;
}

void stdiocpp::fclose(FILE *stream)
{
    if(::fclose(stream) != 0){
        throw QExcStdio("fclose failed: ", nullptr, true);
    }
}


int stdiocpp::fgetc_unlocked(FILE *stream)
{
    return ::fgetc_unlocked(stream);
}


size_t stdiocpp::fwrite_unlocked(const void *ptr, size_t size, size_t n_items, FILE *stream)
{
    size_t items_written = ::fwrite_unlocked(ptr , size, n_items, stream);
    if( items_written != n_items){
       throw QExcStdio(QString("fwrite_unlocked failed (only %1 of %2 items written): ")
                       .arg(items_written).arg(n_items),
                       stream);
    }
    return items_written;
}


void stdiocpp::fflush(FILE *stream)
{
    if(::fflush(stream) != 0){
        throw QExcStdio("fflush failed: ", nullptr, true);
    }
}


size_t stdiocpp::fread_unlocked(void *ptr, size_t size, size_t n, FILE *stream)
{
    return ::fread_unlocked(ptr, size, n, stream);
}


int stdiocpp::fseek(FILE *stream, long offset, int whence)
{
    int new_offset = ::fseek(stream, offset, whence);
    if(new_offset == -1){
        throw QExcStdio("fseek failed: ", nullptr, true);
    }
    return new_offset;
}


/// Warning: not threadsafe.
/// stdio.h does not provide such a functionality, so we must take care
/// that buffer is flushed before using the raw OS-ftruncate
void stdiocpp::ftruncate_unlocked(FILE *stream)
{
    stdiocpp::fflush(stream);
    stdiocpp::fseek(stream, 0, SEEK_SET);
    if(::ftruncate(fileno(stream), 0) == -1) {
        throw QExcStdio("POSIX ftruncate failed", stream, true);
    }
}



