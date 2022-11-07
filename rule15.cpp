
#include <vector>
#include <assert.h>
#include "tests.h"

using namespace std;

namespace rule15
{
	namespace x1
	{
		// @brief Remove duplicate integers from a vector
		//
		// @param values Integer vector to compress

		template <class T>
		void compressVector(
			vector<T> & values, 
			bool (* is_equal)(const T &, const T &))
		{
			if (values.size() == 0)
				return;

			int iDst = 1;

			for (int iSrc = 1, c = values.size(); iSrc < c; ++iSrc) {
				// Check for unqiue values
				if (!is_equal(values[iDst - 1], values[iSrc]))
				{
					values[iDst++] = values[iSrc];
				}
			}

			values.resize(iDst);
		}
	};

	namespace x2
	{
		// @brief Compress sequences of equal values in a vector
		// 
		// For any sequence of values in the vector considered equal, keep only
		// the first value in that sequence, removing the duplicates.
		//
		// @param values Vector to compress
		// @param is_equal Comparison function to use

		template <class T>
		void compressVector(
			vector<T> & values, 
			bool (* is_equal)(const T &, const T &))
		{
			if (values.size() == 0)
				return;

			int iDst = 1;

			for (int iSrc = 1, c = values.size(); iSrc < c; ++iSrc) {
				// Check for unique values
				if (!is_equal(values[iDst - 1], values[iSrc]))
				{
					values[iDst++] = values[iSrc];
				}
			}

			values.resize(iDst);
		}
	};

	namespace x3
	{
		// @brief Compress sequences of equal values in a vector
		// 
		// For any sequence of values in the vector considered equal, keep only
		// the first value in that sequence, removing the duplicates.
		//
		// @param values Vector to compress
		// @param isEqual Comparison function to use

		template <class T>
		void compressVector(
			vector<T> & values, 
			bool (* isEqual)(const T &, const T &))
		{
			int count = values.size();
			if (count == 0)
				return;

			// Copy values that aren't equal to their predecessor

			int destIndex = 1;
			for (int sourceIndex = 1; sourceIndex < count; ++sourceIndex) 
			{
				if (!isEqual(values[destIndex - 1], values[sourceIndex]))
				{
					values[destIndex++] = values[sourceIndex];
				}
			}

			values.resize(destIndex);
		}
	};

	bool isEqual(const int & left, const int & right)
	{
		return left == right;
	}

	void test(bool showDetails)
	{
		{
			using namespace x1;

			vector<int> values;
			vector<int> expectedValues;

			values = { 1, 1, 2, 3, 3, 3, 4, 1, 1, 2, 5, 5, 3, 3, 3 };
			expectedValues = { 1, 2, 3, 4, 1, 2, 5, 3 };

			compressVector(values, isEqual);
			assert(values == expectedValues);

			values = { 0 };
			expectedValues = { 0 };

			compressVector(values, isEqual);
			assert(values == expectedValues);

			values = {};
			expectedValues = {};

			compressVector(values, isEqual);
			assert(values == expectedValues);
		}

		{
			using namespace x2;

			vector<int> values;
			vector<int> expectedValues;

			values = { 1, 1, 2, 3, 3, 3, 4, 1, 1, 2, 5, 5, 3, 3, 3 };
			expectedValues = { 1, 2, 3, 4, 1, 2, 5, 3 };

			compressVector(values, isEqual);
			assert(values == expectedValues);

			values = { 0 };
			expectedValues = { 0 };

			compressVector(values, isEqual);
			assert(values == expectedValues);

			values = {};
			expectedValues = {};

			compressVector(values, isEqual);
			assert(values == expectedValues);
		}

		{
			using namespace x3;

			vector<int> values;
			vector<int> expectedValues;

			values = { 1, 1, 2, 3, 3, 3, 4, 1, 1, 2, 5, 5, 3, 3, 3 };
			expectedValues = { 1, 2, 3, 4, 1, 2, 5, 3 };

			compressVector(values, isEqual);
			assert(values == expectedValues);

			values = { 0 };
			expectedValues = { 0 };

			compressVector(values, isEqual);
			assert(values == expectedValues);

			values = {};
			expectedValues = {};

			compressVector(values, isEqual);
			assert(values == expectedValues);
		}
	}

	RegisterTest rt("Rule 15", test);
};
