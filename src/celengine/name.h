//
// C++ Interface: name
//
// Description:
//
//
// Author: Toti <root@totibox>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _NAME_H_
#define _NAME_H_

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <celutil/debug.h>
#include <celutil/util.h>
#include <celutil/utf8.h>

// TODO: this can be "detemplatized" by creating e.g. a global-scope enum InvalidCatalogNumber since there
// lies the one and only need for type genericity.
template <class OBJ> class NameDatabase
{
 public:
    typedef std::map<std::string, uint32_t, CompareIgnoringCasePredicate> NameIndex;
    typedef std::multimap<uint32_t, std::string> NumberIndex;

 public:
    NameDatabase() {};


    uint32_t getNameCount() const;

    void add(const uint32_t, const std::string&);

    // delete all names associated with the specified catalog number
    void erase(const uint32_t);

    uint32_t      getCatalogNumberByName(const std::string&) const;
    std::string getNameByCatalogNumber(const uint32_t)       const;

    NumberIndex::const_iterator getFirstNameIter(const uint32_t catalogNumber) const;
    NumberIndex::const_iterator getFinalNameIter() const;

    std::vector<std::string> getCompletion(const std::string& name) const;

 protected:
    NameIndex   nameIndex;
    NumberIndex numberIndex;
};



template <class OBJ>
uint32_t NameDatabase<OBJ>::getNameCount() const
{
    return nameIndex.size();
}


template <class OBJ>
void NameDatabase<OBJ>::add(const uint32_t catalogNumber, const std::string& name)
{
    if (name.length() != 0)
    {
#ifdef DEBUG
        uint32_t tmp;
        if ((tmp = getCatalogNumberByName(name)) != OBJ::InvalidCatalogNumber)
            DPRINTF(2,"Duplicated name '%s' on object with catalog numbers: %d and %d\n", name.c_str(), tmp, catalogNumber);
#endif
        // Add the new name
        //nameIndex.insert(NameIndex::value_type(name, catalogNumber));

        nameIndex[name]   = catalogNumber;
        numberIndex.insert(NumberIndex::value_type(catalogNumber, name));
    }
}


template <class OBJ>
void NameDatabase<OBJ>::erase(const uint32_t catalogNumber)
{
    numberIndex.erase(catalogNumber);
}


template <class OBJ>
uint32_t NameDatabase<OBJ>::getCatalogNumberByName(const std::string& name) const
{
    NameIndex::const_iterator iter = nameIndex.find(name);

    if (iter == nameIndex.end())
        return OBJ::InvalidCatalogNumber;
    else
        return iter->second;
}


// Return the first name matching the catalog number or end()
// if there are no matching names.  The first name *should* be the
// proper name of the OBJ, if one exists. This requires the
// OBJ name database file to have the proper names listed before
// other designations.  Also, the STL implementation must
// preserve this order when inserting the names into the multimap
// (not certain whether or not this behavior is in the STL spec.
// but it works on the implementations I've tried so far.)
template <class OBJ>
std::string NameDatabase<OBJ>::getNameByCatalogNumber(const uint32_t catalogNumber) const
{
    if (catalogNumber == OBJ::InvalidCatalogNumber)
        return "";

    NumberIndex::const_iterator iter = numberIndex.lower_bound(catalogNumber);

    if (iter != numberIndex.end() && iter->first == catalogNumber)
        return iter->second;

    return "";
}


// Return the first name matching the catalog number or end()
// if there are no matching names.  The first name *should* be the
// proper name of the OBJ, if one exists. This requires the
// OBJ name database file to have the proper names listed before
// other designations.  Also, the STL implementation must
// preserve this order when inserting the names into the multimap
// (not certain whether or not this behavior is in the STL spec.
// but it works on the implementations I've tried so far.)
template <class OBJ>
NameDatabase<OBJ>::NumberIndex::const_iterator NameDatabase<OBJ>::getFirstNameIter(const uint32_t catalogNumber) const
{
    NumberIndex::const_iterator iter = numberIndex.lower_bound(catalogNumber);

    if (iter == numberIndex.end() || iter->first != catalogNumber)
        return getFinalNameIter();
    else
        return iter;
}


template <class OBJ>
NameDatabase<OBJ>::NumberIndex::const_iterator NameDatabase<OBJ>::getFinalNameIter() const
{
    return numberIndex.end();
}


template <class OBJ>
std::vector<std::string> NameDatabase<OBJ>::getCompletion(const std::string& name) const
{
    std::vector<std::string> completion;
    int name_length = UTF8Length(name);

    for (NameIndex::const_iterator iter = nameIndex.begin(); iter != nameIndex.end(); ++iter)
    {
        if (!UTF8StringCompare(iter->first, name, name_length))
        {
            completion.push_back(iter->first);
        }
    }
    return completion;
}

#endif  // _NAME_H_
