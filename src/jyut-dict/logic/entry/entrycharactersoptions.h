#ifndef ENTRYCHARACTERSOPTIONS_H
#define ENTRYCHARACTERSOPTIONS_H

#include <QDataStream>
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
