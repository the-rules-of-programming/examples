
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <assert.h>
#include "tests.h"

using namespace std;

#pragma warning(disable: 4018)

namespace rule14
{
	namespace x1
	{
		struct Bounds
		{
			Bounds(int minValue, int maxValue)
			: m_minValue(minValue), m_maxValue(maxValue)
				{ ; }

			int m_minValue;
			int m_maxValue;
		};

		Bounds findBounds(const vector<int> & values)
		{
			int minValue = INT_MAX;
			int maxValue = INT_MIN;

			for (int value : values)
			{
				minValue = min(minValue, value);
				maxValue = max(maxValue, value);
			}

			return Bounds(minValue, maxValue);
		}
	};

	namespace x2
	{
        enum EmptyTag
        {
            kEmpty
        };

        template <typename T> T MinValue() { return 0; }
        template <typename T> T MaxValue() { return 0; }

        template <> int MinValue<int>() { return INT_MIN; }
        template <> int MaxValue<int>() { return INT_MAX; }

        template <class T>
        struct Bounds
        {
            Bounds(const T & value)
            : m_minValue(value), m_maxValue(value)
                { ; }
            Bounds(const T & minValue, const T & maxValue)
            : m_minValue(minValue), m_maxValue(maxValue)
                { ; }
            Bounds(EmptyTag)
            : m_minValue(MaxValue<T>()), m_maxValue(MinValue<T>())
                { ; }

            Bounds & operator |= (const T & value)
            {
                m_minValue = min(m_minValue, value);
                m_maxValue = max(m_maxValue, value);

                return *this;
            }

            T m_minValue;
            T m_maxValue;
        };

        template <class T>
        struct Range
        {
            Range(const T & begin, const T & end)
            : m_begin(begin), m_end(end)
                { ; }

            const T & begin() const
                { return m_begin; }
            const T & end() const
                { return m_end; }

            T m_begin;
            T m_end;
        };

        template <class T>
        Range<typename vector<T>::const_iterator> getVectorRange(
            const vector<T> & values, 
            int beginIndex, 
            int endIndex)
        {
            return Range<vector<T>::const_iterator>(
                        values.begin() + beginIndex, 
                        values.begin() + endIndex);
        }

        template <class T, class I>
        T iterateAndMerge(const T & init, const I & iterable)
        {
            T merge(init);

            for (const auto & value : iterable)
            {
                merge |= value;
            }

            return merge;
        }

        void findBounds(const vector<int> & values, Bounds<int> * bounds)
        {
            *bounds = iterateAndMerge(
                            Bounds<int>(kEmpty), 
                            getVectorRange(values, 0, values.size()));
        }
	};

	namespace x3
	{
        struct Bounds
        {
            Bounds(int minValue, int maxValue)
            : m_minValue(minValue), m_maxValue(maxValue)
                { ; }

            int m_minValue;
            int m_maxValue;
        };

        template <class COMPARE>
        int findNth(const vector<int> & values, int n)
        {
            priority_queue<int, vector<int>, COMPARE> queue;
            COMPARE compare;

            bool f0 = compare(0, 2);
            bool f1 = compare(2, 0);
            bool f2 = compare(0, 0);

            int q = 0;
            for (int value : values)
            {
                if (queue.size() < n)
                {
                    queue.push(value);
                }
                else if (compare(value, queue.top()))
                {
                    queue.pop();
                    queue.push(value);
                }
              
                q = queue.top();
            }

            return queue.top();
        }

        void findBounds(const vector<int> & values, Bounds * bounds)
        {
            bounds->m_minValue = findNth<less<int>>(values, 1);
            bounds->m_maxValue = findNth<greater<int>> (values, 1);
        }
	};

	namespace x4
	{
        struct Bounds
        {
            Bounds(int minValue, int maxValue)
            : m_minValue(minValue), m_maxValue(maxValue)
                { ; }

            int m_minValue;
            int m_maxValue;
        };

        int findExtreme(const vector<int> & values, int sign)
        {
            for (int index = 0; index < values.size(); ++index)
            {
                for (int otherIndex = 0;; ++otherIndex)
                {
                    if (otherIndex >= values.size())
                        return values[index];

                    if (sign * values[index] < sign * values[otherIndex])
                        break;
                }
            }

            assert(false);
            return 0;
        }

        void findBounds(const vector<int> & values, Bounds * bounds)
        {
            bounds->m_minValue = findExtreme(values, -1);
            bounds->m_maxValue = findExtreme(values, +1);
        }
	};

	namespace x5
	{
        vector<string> generatePermutations(const string & permute)
        {
            vector<string> permutations;

            if (permute.length() == 1)
            {
                permutations.push_back(permute);
            }
            else
            {
                for (int index = 0; index < permute.length(); ++index)
                {
                    string single = permute.substr(index, 1);
                    string rest = permute.substr(0, index) + 
                                  permute.substr(
                                      index + 1, 
                                      permute.length() - index - 1);

                    for (string permutation : generatePermutations(rest))
                    {
                        permutations.push_back(single + permutation);
                    }
                }
            }

            return permutations;
        }

        bool findPermutation(const string & permute, const string & search)
        {
            vector<string> permutations = generatePermutations(permute);
            for (const string & permutation : permutations)
            {
                if (search.find(permutation) != string::npos)
                    return true;
            }

            return false;
        }
	};

	namespace x6
	{
        vector<string> generatePermutations(const string & permute)
        {
            vector<string> permutations;

            if (permute.length() == 1)
            {
                permutations.push_back(permute);
            }
            else
            {
                for (int index = 0; index < permute.length(); ++index)
                {
                    string single = permute.substr(index, 1);
                    string rest = permute.substr(0, index) + 
                                  permute.substr(
                                      index + 1, 
                                      permute.length() - index - 1);

                    for (string permutation : generatePermutations(rest))
                    {
                        permutations.push_back(single + permutation);
                    }
                }
            }

            sort(
                permutations.begin(), 
                permutations.end());
            permutations.erase(
                unique(permutations.begin(), permutations.end()), 
                permutations.end());

            return permutations;
        }

        bool findPermutation(const string & permute, const string & search)
        {
            vector<string> permutations = generatePermutations(permute);
            for (const string & permutation : permutations)
            {
                if (search.find(permutation) != string::npos)
                    return true;
            }

            return false;
        }
	};

	namespace x7
	{
        bool findPermutation(const string & permute, const string & search)
        {
            int permuteLength = permute.length();
            int searchLength = search.length();

            vector<bool> found(permuteLength, false);

            for (int lastIndex = permuteLength; 
                 lastIndex < searchLength; 
                 ++lastIndex)
            {
                bool foundPermutation = true;

                for (int searchIndex = lastIndex - permuteLength; 
                     searchIndex < lastIndex; 
                     ++searchIndex)
                {
                    bool foundMatch = false;

                    for (int permuteIndex = 0; 
                         permuteIndex < permuteLength; 
                         ++permuteIndex)
                    {
                        if (search[searchIndex] == permute[permuteIndex] &&
                            !found[permuteIndex])
                        {
                            foundMatch = true;
                            found[permuteIndex] = true;
                            break;
                        }
                    }

                    if (!foundMatch)
                    {
                        foundPermutation = false;
                        break;
                    }
                }

                if (foundPermutation)
                    return true;

                fill(found.begin(), found.end(), false);
            }

            return false;
        }
	};

	namespace x8
	{
        #define LARGE_PRIME 104729

        bool findPermutation(const string & permute, const string & search)
        {
            int permuteCounts[UCHAR_MAX] = {};
            int currentCounts[UCHAR_MAX] = {};

            int permuteHash = 0;
            int currentHash = 0;

            for (unsigned char character : permute)
            {
                ++permuteCounts[character];
                permuteHash += character * (character + LARGE_PRIME);
            }

            int permuteLength = permute.length();
            int searchLength = search.length();

            if (searchLength < permuteLength)
                return false;

            for (int searchIndex = 0; searchIndex < permuteLength; ++searchIndex)
            {
                unsigned char character = search[searchIndex];

                ++currentCounts[character];
                currentHash += character * (character + LARGE_PRIME);
            }

            for (int searchIndex = permuteLength;; ++searchIndex)
            {
                if (currentHash == permuteHash)
                {
                    bool match = true;

                    for (char character : permute)
                    {
                        if (permuteCounts[character] != currentCounts[character])
                            match = false;
                    }

                    if (match)
                        return true;
                }

                if (searchIndex >= searchLength)
                    break;

                unsigned char removeCharacter = search[searchIndex - permuteLength];
                unsigned char addCharacter = search[searchIndex];

                --currentCounts[removeCharacter];
                currentHash -= removeCharacter * (removeCharacter + LARGE_PRIME);

                ++currentCounts[addCharacter];
                currentHash += addCharacter * (addCharacter + LARGE_PRIME);
            }

            return false;
        }
	};

    namespace x9
    {
        bool findPermutation(const string & permute, const string & search)
        {
            int permuteLength = permute.length();

            string sortedPermute = permute;
            sort(sortedPermute.begin(), sortedPermute.end());

            for (int index = permuteLength; index < search.length(); ++index)
            {
                string sortedSubstring = search.substr(
                                            index - permuteLength, 
                                            permuteLength);
                sort(sortedSubstring.begin(), sortedSubstring.end());

                if (sortedPermute == sortedSubstring)
                    return true;
            }

            return false;
        }
    };

    void test(bool showDetails)
    {
        {
            using namespace x1;

            vector<int> values = { 5, -8, 13, 25, 19, 19 };

            Bounds bounds = findBounds(values);

            assert(bounds.m_minValue == -8);
            assert(bounds.m_maxValue == 25);
        }

        {
            using namespace x2;

            vector<int> values = { 5, -8, 13, 25, 19, 19 };

            Bounds<int> bounds(kEmpty);
            findBounds(values, &bounds);
        
            assert(bounds.m_minValue == -8);
            assert(bounds.m_maxValue == 25);
        }

        {
            using namespace x3;

            vector<int> values = { 5, -8, 13, 25, 19, 19, 21, -5, 0, 3, 2  };

            Bounds bounds(0, 0);
            findBounds(values, &bounds);
        
            assert(bounds.m_minValue == -8);
            assert(bounds.m_maxValue == 25);
        }

        {
            using namespace x4;

            vector<int> values = { 5, -8, 13, 25, 19, 19, 21, -5, 0, 3, 2  };

            Bounds bounds(0, 0);
            findBounds(values, &bounds);
        
            assert(bounds.m_minValue == -8);
            assert(bounds.m_maxValue == 25);
        }

        {
            using namespace x5;

            assert(findPermutation("abc", "cabbage"));
            assert(findPermutation("abc", "abacus"));
            assert(!findPermutation("abc", "scramble"));
            assert(!findPermutation("abc", "brackish"));
            assert(findPermutation("aabb", "cabbage"));
            assert(!findPermutation("aaabb", "cabbage"));
        }

        {
            using namespace x6;

            assert(findPermutation("abc", "cabbage"));
            assert(findPermutation("abc", "abacus"));
            assert(!findPermutation("abc", "scramble"));
            assert(!findPermutation("abc", "brackish"));
            assert(findPermutation("aabb", "cabbage"));
            assert(!findPermutation("aaabb", "cabbage"));
        }

        {
            using namespace x7;

            assert(findPermutation("abc", "cabbage"));
            assert(findPermutation("abc", "abacus"));
            assert(!findPermutation("abc", "scramble"));
            assert(!findPermutation("abc", "brackish"));
            assert(findPermutation("aabb", "cabbage"));
            assert(!findPermutation("aaabb", "cabbage"));
        }

        {
            using namespace x8;

            assert(findPermutation("abc", "cabbage"));
            assert(findPermutation("abc", "abacus"));
            assert(!findPermutation("abc", "scramble"));
            assert(!findPermutation("abc", "brackish"));
            assert(findPermutation("aabb", "cabbage"));
            assert(!findPermutation("aaabb", "cabbage"));
        }

        {
            using namespace x9;

            assert(findPermutation("abc", "cabbage"));
            assert(findPermutation("abc", "abacus"));
            assert(!findPermutation("abc", "scramble"));
            assert(!findPermutation("abc", "brackish"));
            assert(findPermutation("aabb", "cabbage"));
            assert(!findPermutation("aaabb", "cabbage"));
        }
    }

    RegisterTest rt("Rule 14", test);
};
