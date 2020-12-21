#ifndef ENTRYCHARACTERSOPTIONS_H
#define ENTRYCHARACTERSOPTIONS_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QDataStream>
#else
#include "logic/utils/qvariantutils.h"
#endif

#include <QMetaType>

// An enum that tells how to display en entry

enum class EntryCharactersOptions : int
{
    ONLY_SIMPLIFIED,
    ONLY_TRADITIONAL,
    PREFER_SIMPLIFIED,
    PREFER_TRADITIONAL,
};

Q_DECLARE_METATYPE(EntryCharactersOptions);

#endif // ENTRYCHARACTERSOPTIONS_H
