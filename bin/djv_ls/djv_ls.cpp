//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djv_ls.cpp

#include <djv_ls.h>

#include <djvError.h>
#include <djvErrorUtil.h>
#include <djvMath.h>
#include <djvMemory.h>
#include <djvStringUtil.h>
#include <djvSystem.h>
#include <djvTime.h>
#include <djvUser.h>

#include <QDir>

//------------------------------------------------------------------------------
// djvLsApplication
//------------------------------------------------------------------------------

djvLsApplication::djvLsApplication(int argc, char ** argv) throw (djvError) :
    djvCoreApplication("djv_ls", argc, argv),
    _info         (true),
    _filePath     (false),
    _sequence     (djvSequenceEnum::COMPRESS_RANGE),
    _recurse      (false),
    _hidden       (false),
    _columns      (djvSystem::terminalWidth()),
    _sort         (djvFileInfoUtil::SORT_NAME),
    _reverseSort  (false),
    _doNotSortDirs(false)
{
    //DJV_DEBUG("djvLsApplication::djvLsApplication");

    // Parse the command line.

    try
    {
        commandLine(_commandLineArgs);
    }
    catch (const djvError & error)
    {
        printError(djvError(QString(errorCommandLine).arg(error.string())));
        
        setExitValue(EXIT_VALUE_ERROR);
    }

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    // Convert the command line inputs:
    //
    // * Match wildcards
    // * Expand sequences

    djvFileInfoList list;

    for (int i = 0; i < _input.count(); ++i)
    {
        djvFileInfo file(_input[i], false);

        //DJV_DEBUG_PRINT("file = " << file);

        // Match wildcards.

        if (_sequence && file.isSequenceWildcard())
        {
            file = djvFileInfoUtil::sequenceWildcardMatch(
                file,
                djvFileInfoUtil::list(file.path(), _sequence));

            //DJV_DEBUG_PRINT("  wildcard match = " << file);
        }

        // Is this a sequence?

        if (_sequence && file.isSequenceValid())
        {
            file.setType(djvFileInfo::SEQUENCE);

            //DJV_DEBUG_PRINT("  sequence = " << file);
        }

        // Expand the sequence.

        QStringList tmp = djvFileInfoUtil::expandSequence(file);

        for (int j = 0; j < tmp.count(); ++j)
        {
            file = djvFileInfo(tmp[j], false);

            if (! file.stat())
            {
                printError(djvError(QString("Cannot open: %1").
                    arg(QDir::toNativeSeparators(file))));
                
                setExitValue(EXIT_VALUE_ERROR);
                
                continue;
            }

            if (_sequence && file.isSequenceValid())
            {
                file.setType(djvFileInfo::SEQUENCE);
            }

            list += file;
        }
    }

    //DJV_DEBUG_PRINT("list = " << list);

    // Process the inputs.

    process(list);

    //DJV_DEBUG_PRINT("process = " << list);

    // If there are no inputs list the current directory.

    if (! list.count() && exitValue() != EXIT_VALUE_ERROR)
    {
        list += djvFileInfo(".");
    }

    // Print the items.

    for (int i = 0; i < list.count(); ++i)
    {
        if (djvFileInfo::DIRECTORY == list[i].type())
        {
            if (! printDirectory(
                list[i],
                ((list.count() > 1) || _recurse) && ! _filePath))
            {
                printError(djvError(QString("Cannot open: %1").
                    arg(QDir::toNativeSeparators(list[i]))));
                
                setExitValue(EXIT_VALUE_ERROR);
            }
        }
        else
        {
            printItem(list[i], true, _info);
        }
    }
}

void djvLsApplication::commandLine(QStringList & in) throw (djvError)
{
    //DJV_DEBUG("djvLsApplication::commandLine");
    //DJV_DEBUG_PRINT("in = " << in);

    djvCoreApplication::commandLine(in);

    if (exitValue() != EXIT_VALUE_DEFAULT)
        return;

    QStringList args;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            // Parse the options.

            if ("-x_info" == arg || "-xi" == arg)
            {
                _info = false;
            }
            else if ("-file_path" == arg || "-fp" == arg)
            {
                _filePath = true;
            }
            else if ("-seq" == arg || "-q" == arg)
            {
                in >> _sequence;
            }
            else if ("-recurse" == arg || "-r" == arg)
            {
                _recurse = true;
            }
            else if ("-hidden" == arg)
            {
                _hidden = true;
            }
            else if ("-columns" == arg || "-c" == arg)
            {
                in >> _columns;
            }

            // Sorting options.

            else if ("-sort" == arg || "-s" == arg)
            {
                in >> _sort;
            }
            else if ("-reverse_sort" == arg || "-rs" == arg)
            {
                _reverseSort = true;
            }
            else if ("-x_sort_dirs" == arg || "-xsd" == arg)
            {
                _doNotSortDirs = false;
            }

            // Parse the arguments.

            else
            {
                _input += arg;
            }
        }
    }
    catch (const QString &)
    {
        throw djvError(arg);
    }
}

namespace
{

const QString commandLineHelpLabel =
"djv_ls\n"
"\n"
"    This application provides a command line tool for listing directories "
"with file sequences.\n"
"\n"
"    Example output:\n"
"\n"
"    el_cerrito_bart.1k.tiff   File 2.23MB darby rw Mon Jun 12 21:21:55 2006\n"
"    richmond_train.2k.tiff    File 8.86MB darby rw Mon Jun 12 21:21:58 2006\n"
"    fishpond.1-749.png       Seq 293.17MB darby rw Thu Aug 17 16:47:43 2006\n"
"\n"
"    Key:\n"
"\n"
"    (name)                        (type) (size) (user) (permissions) (time)\n"
"\n"
"    File types:                   Permissions:\n"
"\n"
"    * File - Regular file         * r - Readable\n"
"    * Seq  - File sequence        * w - Writable\n"
"    * Dir  - Directory            * x - Executable\n"
"\n"
"Usage\n"
"\n"
"    djv_ls [file|directory]... [option]...\n"
"\n"
"Options\n"
"\n"
"    -x_info, -xi\n"
"        Don't show information, only file names.\n"
"    -file_path, -fp\n"
"        Show file path names.\n"
"    -seq, -q (value)\n"
"        Set file sequencing. Options = %1. Default = %2.\n"
"    -recurse, -r\n"
"        Descend into sub-directories.\n"
"    -hidden\n"
"        Show hidden files.\n"
"    -columns, -c (value)\n"
"        Set the number of columns for formatting output. A value of zero "
"disables formatting.\n"
"\n"
"Sorting Options\n"
"\n"
"    -sort, -s (value)\n"
"        Set the sorting. Options = %3. Default = %4.\n"
"    -reverse_sort, -rs\n"
"        Reverse the sorting order.\n"
"    -x_sort_dirs, -xsd\n"
"        Don't sort directories first.\n"
"%5"
"\n"
"Examples\n"
"\n"
"    List the current directory:\n"
"    > djv_ls\n"
"\n"
"    List specific directories:\n"
"    > djv_ls ~/movies ~/pictures\n"
"\n"
"    Sort by time with the most recent first:\n"
"    > djv_ls -sort time -reverse_sort\n";

} // namespace

QString djvLsApplication::commandLineHelp() const
{
    return QString(commandLineHelpLabel).
        arg(djvSequenceEnum::compressLabels().join(", ")).
        arg(djvStringUtil::label(_sequence).join(", ")).
        arg(djvFileInfoUtil::sortLabels().join(", ")).
        arg(djvStringUtil::label(_sort).join(", ")).
        arg(djvCoreApplication::commandLineHelp());
}

void djvLsApplication::process(djvFileInfoList & in)
{
    //DJV_DEBUG("djvLsApplication::process");
    //DJV_DEBUG_PRINT("in = " << in);

    // Compress files into sequences.

    //djvFileInfoUtil::compressSeq(in, _seq);

    //DJV_DEBUG_PRINT("compress = " << in);

    // Remove hidden files.

    if (! _hidden)
    {
        djvFileInfoUtil::filter(in, djvFileInfoUtil::FILTER_HIDDEN);

        //DJV_DEBUG_PRINT("hidden = " << in);
    }

    // Sort.

    djvFileInfoUtil::sort(in, _sort, _reverseSort);

    //DJV_DEBUG_PRINT("sort = " << in);

    if (! _doNotSortDirs)
    {
        djvFileInfoUtil::sortDirsFirst(in);

        //DJV_DEBUG_PRINT("sort directories = " << in);
    }
}

void djvLsApplication::printItem(const djvFileInfo & in, bool path, bool info)
{
    //DJV_DEBUG("djvLsApplication::printItem");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("path = " << path);
    //DJV_DEBUG_PRINT("info = " << info);
    //DJV_DEBUG_PRINT("columns = " << _columns);

    QString name = in.fileName(-1, path);

    if (info)
    {
        const QString infoString =
#if defined(DJV_WINDOWS)
            QString("%1 %2 %3 %4").
#else
            QString("%1 %2 %3 %4 %5").
#endif
            arg(djvFileInfo::typeLabels()[in.type()], 4).
            arg(djvMemory::sizeLabel(in.size())).
#if ! defined(DJV_WINDOWS)
            arg(djvUser::uidToString(in.user())).
#endif // DJV_WINDOWS
            arg(djvFileInfo::permissionsLabel(in.permissions()), 3).
            arg(djvTime::timeToString(in.time()));

        // Elide the name if there isn't enough space.

        if (_columns)
        {
            const int length = djvMath::max(_columns - infoString.length() - 2, 0);

            if (name.length() > length)
            {
                name = name.mid(0, length);

                int i = name.length();
                int j = 3;

                while (i-- > 0 && j-- > 0)
                {
                    name[i] = '.';
                }
            }
        }

        print(
            QString("%1 %2").
                arg(QDir::toNativeSeparators(name)).
                arg(infoString, _columns - name.length() - 2));
    }
    else
    {
        print(name);
    }
}

bool djvLsApplication::printDirectory(const djvFileInfo & in, bool label)
{
    //DJV_DEBUG("djvLsApplication::printDirectory");
    //DJV_DEBUG_PRINT("in = " << in);

    // Read the directory contents.

    if (! QDir(in.path()).exists())
        return false;

    djvFileInfoList items = djvFileInfoUtil::list(in, _sequence);

    // Process the items.

    process(items);

    // Print the items.

    if (label)
    {
        print(QString("%1:").arg(QDir::toNativeSeparators(in)));
    }

    for (int i = 0; i < items.count(); ++i)
    {
        printItem(items[i], _filePath, _info);
    }

    if (label)
    {
        printSeparator();
    }

    // Recurse.

    bool r = true;

    if (_recurse)
    {
        djvFileInfoList items = djvFileInfoUtil::list(in, _sequence);

        djvFileInfoUtil::filter(
            items,
            djvFileInfoUtil::FILTER_FILE |
                (! _hidden ? djvFileInfoUtil::FILTER_HIDDEN : 0));

        for (int i = 0; i < items.count(); ++i)
        {
            r &= printDirectory(items[i], label);
        }
    }

    return r;
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------

int main(int argc, char ** argv)
{
    int r = 1;

    try
    {
        r = djvLsApplication(argc, argv).run();
    }
    catch (const djvError & error)
    {
        djvErrorUtil::print(error);
    }

    return r;
}
