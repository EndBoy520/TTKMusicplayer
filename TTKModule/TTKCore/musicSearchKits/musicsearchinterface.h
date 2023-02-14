#ifndef MUSICSEARCHINTERFACE_H
#define MUSICSEARCHINTERFACE_H

/***************************************************************************
 * This file is part of the TTK Music Player project
 * Copyright (C) 2015 - 2023 Greedysky Studio

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include "ttkglobaldefine.h"

/*! @brief The class of the item search interface.
 * @author Greedysky <greedysky@163.com>
 */
template <typename T>
class TTK_MODULE_EXPORT MusicItemSearchInterface
{
public:
    using TTKIntListMap = QMap<int, TTKIntList>;
    using MusicItemSearchInterfaceClass = MusicItemSearchInterface<T>;

public:
    /*!
     * Object contsructor.
     */
    explicit MusicItemSearchInterface()
        : m_containerItems(),
          m_searchResultLevel(0),
          m_searchResultCache()
    {

    }

    /*!
     * Check current search result is empty or not.
     */
    inline bool hasSearchResult() const
    {
        return !m_searchResultCache.isEmpty();
    }

    /*!
     * Clear search result.
     */
    inline void clearSearchResult()
    {
        m_searchResultLevel = 0;
        m_searchResultCache.clear();
    }

    /*!
     * Mapped search row.
     */
    inline void mappedSearchRow(int pos, int &row) const
    {
        if(hasSearchResult())
        {
            const TTKIntList &v = m_searchResultCache.value(pos);
            row = !v.isEmpty() ? v[row] : row;
        }
    }

protected:
    T m_containerItems;
    int m_searchResultLevel;
    TTKIntListMap m_searchResultCache;

};

#endif // MUSICSEARCHINTERFACE_H
