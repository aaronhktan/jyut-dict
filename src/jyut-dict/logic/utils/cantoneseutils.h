#ifndef CANTONESEUTILS_H
#define CANTONESEUTILS_H

#include <QString>

#include <string>

namespace CantoneseUtils {

std::string convertJyutpingToYale(const std::string &jyutping,
                                  bool useSpacesToSegment = false);
std::string convertJyutpingToIPA(const std::string &jyutping,
                                 bool useSpacesToSegment = false);

bool segmentJyutping(const QString &string,
                     std::vector<std::string> &out,
                     bool removeSpecialCharacters = true,
                     bool removeGlobCharacters = true,
                     bool removeRegexCharacters = true);

bool jyutpingAutocorrect(const QString &in,
                         QString &out,
                         bool unsafeSubstitutions = false);
bool jyutpingSoundChanges(std::vector<std::string> &inOut);

} // namespace CantoneseUtils

#endif // CANTONESEUTILS_H
