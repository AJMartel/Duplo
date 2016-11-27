/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "StringUtil.h"

#include <sstream>

/**
 * Trim string
 *
 * @param input  string to trim
 * @return returns trimmed string
 */
std::string StringUtil::trim(const std::string& input){

    auto str = input;

    std::stringstream trimmer;
    trimmer << str;
    str.clear();
    trimmer >> str;

    // Return the new string
    return str;
}

/**
 * Split string
 *
 * @param input  string to split
 * @param delimiter  delimiter string to trim
 * @param results  results vector with substrings
 * @param trim  boolean to indicate trimming or not
 * @return returns number of substrings
 */
int StringUtil::split(const std::string& input, const std::string& delimiter, std::vector<std::string>& results, bool doTrim){
    int sizeDelim = (int)delimiter.size();

    int newPos = (int)input.find(delimiter, 0);

    if(newPos < 0){
        if(doTrim){
            results.push_back(trim(input));
        } else {
            results.push_back(input);
        }
        return 0;
    }

    int numFound = 0;

    std::vector<int> positions;

    // At the begin is always a marker
    positions.push_back(-sizeDelim);
    int pos = 0;
    while(pos != -1){
        numFound++;
        pos = (int)input.find(delimiter, pos + sizeDelim);
        if(pos != -1){
            positions.push_back(pos);
        }
    }

    // At the end is always a marker
    positions.push_back((int)input.size());

    for(int i=0;i<(int)positions.size()-1;i++){
        std::string s;

        int start = positions[i] + sizeDelim;
        int size = positions[i+1] - positions[i] - sizeDelim;

        if(size > 0){
            s = input.substr(start, size);   
        }

        if(doTrim){
            results.push_back(trim(s));
        } else {
            results.push_back(s);
        }

    }    

    return numFound;
}

std::string StringUtil::substitute(char s, char d, const std::string& str){
    std::string tmp = str;
    
    for(int i=0;i<(int)tmp.size();i++){
        if(tmp[i] == s){
            tmp[i]=d;
        }
    }

    return tmp;
}

// from:
//      http://www.codecomments.com/archive272-2005-4-473566.html
void StringUtil::StrSub(std::string& cp, const std::string& sub_this, const std::string& for_this, const int& num_times)
{
    int loc = 0;
    if (cp.empty())
    {
        cp = sub_this;
        return;
    }
    for (int i = 0; i != num_times; i++)
    {
        loc = (int)cp.find(for_this, loc);
        if (loc >= 0)
        {
            cp.replace(loc, for_this.length(), sub_this);
            loc += (int)for_this.length();
        }
        else
        {
            return;
        }
    }
}
