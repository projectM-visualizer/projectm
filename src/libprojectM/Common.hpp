/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id$
 *
 * $Log$
 */

#ifndef COMMON_HPP
#define COMMON_HPP
#include <cassert>
#include <cstdarg>
#include <locale>
#include <typeinfo>
#include <vector>

#include <algorithm>
#include <string>

//CPP17: std::filesystem::path::preferred_separator
/** Per-platform path separators */
#ifdef WIN32
char constexpr pathSeparator{'\\'};
#else
char constexpr pathSeparator{'/'};
#endif /** WIN32 */

size_t constexpr maxTokenSize{512};
size_t constexpr maxPathSize{4096};

//CPP23: uz suffix (https://en.cppreference.com/w/cpp/language/integer_literal)
size_t constexpr stringBufferSize{static_cast<size_t>(1024 * 150)};
size_t constexpr stringLineSize{1024};

double constexpr maxDoubleSize{10000000.0};
double constexpr minDoubleSize{-maxDoubleSize};

int constexpr maxIntSize{10000000};
int constexpr minIntSize{-maxIntSize};

/* default float initial value */
double constexpr defaultDoubleIv{0.0};

/* default float lower bound */
double constexpr defaultDoubleLb{minDoubleSize};

/* default float upper bound */
double constexpr defaultDoubleUb{maxDoubleSize};

unsigned int const numQVariables(32);
std::string const projectmFileExtension("prjm");
std::string const milkdropFileExtension("milk");
std::string const projectmModuleExtension("so");


//CPP17: std::filesystem::path::extension
inline auto ParseExtension(const std::string& filename) -> std::string
{

    const std::size_t start = filename.find_last_of('.');

    if (start == std::string::npos || start >= (filename.length() - 1)) {
        return "";
    }
    std::string ext = filename.substr(start + 1, filename.length());
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    return ext;
}

//CPP17: std::filesystem::path::filename
inline auto ParseFilename(const std::string& filename) -> std::string
{

    const std::size_t start = filename.find_last_of('/');

    if (start == std::string::npos || start >= (filename.length() - 1)) {
        return "";
    }

    return filename.substr(start + 1, filename.length());
}

inline auto MeanSquaredError(const double& x, const double& y) -> double
{
    return (x - y) * (x - y);
}

inline auto CaseInsensitiveSubstringFind(std::string const& haystack, std::string const& needle) -> size_t
{
    auto const it = std::search(
        haystack.cbegin(),
        haystack.cend(),
        needle.cbegin(),
        needle.cend(),
        [](char ch1, char ch2) {
            return std::toupper(ch1) == std::toupper(ch2);
        });

    if (it != haystack.end())
    {
        return std::distance(haystack.begin(), it);
    }

    return std::string::npos;
}

enum PresetRatingType
{
    FIRST_RATING_TYPE = 0,
    HARD_CUT_RATING_TYPE = FIRST_RATING_TYPE,
    SOFT_CUT_RATING_TYPE,
    LAST_RATING_TYPE = SOFT_CUT_RATING_TYPE,
    TOTAL_RATING_TYPES = SOFT_CUT_RATING_TYPE + 1
};


using RatingList = std::vector<int>;

#endif
