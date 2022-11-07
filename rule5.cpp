
#include <vector>
#include <assert.h>
#include "tests.h"

using namespace std;

namespace rule5
{
	namespace x1
	{
		int stripNegativeValues(int count, int * data)
		{
			int * to = data;

			for (int * from = data, * end = data + count; 
					from < end; 
					++from)
			{
				if (*from >= 0)
					*to++ = *from;
			}

			return to - data;
		}
	};

	namespace x2
	{
		typedef struct
		{
			float x, y, z;
		} Vector;

		#define dotProduct(A, B) (A.x * B.x + A.y * B.y + A.z * B.z)
	};

	namespace rnd
	{
		int randomInRange(int first, int last)
		{
			return first + rand() % (1 + last - first);
		}
	};

	namespace x3
	{
		using namespace rnd;

		template <class T>
		T chooseRandomValue(int count, const int * weights, const T * values)
		{
			int totalWeight = 0;
			for (int index = 0; index < count; ++index)
			{
				totalWeight += weights[index];
			}

			int selectWeight = randomInRange(0, totalWeight - 1);
			for (int index = 0;; ++index)
			{
				selectWeight -= weights[index];
				if (selectWeight < 0)
					return values[index];
			}

			assert(false);
			return T();
		}
	};

	namespace x4
	{
		using namespace rnd;

		template <class T>
		T chooseRandomValue(int count, const int * weights, const T * values)
		{
			vector<int> weightSums = { 0 };
			for (int index = 0; index < count; ++index)
			{
				weightSums.push_back(weightSums.back() + weights[index]);
			}

			int weight = randomInRange(0, weightSums.back() - 1);

			int minIndex = 0;
			int maxIndex = count;

			while (minIndex + 1 < maxIndex)
			{
				int midIndex = (minIndex + maxIndex) / 2;
				if (weight >= weightSums[midIndex])
					minIndex = midIndex;
				else
					maxIndex = midIndex;
			}

			return values[minIndex];
		}
	};

	namespace x5
	{
		using namespace rnd;

		struct Distribution
		{
			Distribution(int count, int * weights, int * values);

			int chooseRandomValue() const;

			vector<int> m_weights;
			vector<int> m_values;
			int m_totalWeight;
		};

		Distribution::Distribution(int count, int * weights, int * values) :
			m_weights(),
			m_values(),
			m_totalWeight(0)
		{
			int totalWeight = 0;

			for (int index = 0; index < count; ++index)
			{
				m_weights.push_back(weights[index]);
				m_values.push_back(values[index]);

				totalWeight += weights[index];
			}

			m_totalWeight = totalWeight;
		}

		int Distribution::chooseRandomValue() const
		{
			int select = randomInRange(0, m_totalWeight - 1);

			for (int index = 0;; ++index)
			{
				select -= m_weights[index];
				if (select < 0)
					return m_values[index];
			}

			assert(false);
			return 0;
		}
	};

	namespace x6
	{
		using namespace rnd;

		struct Distribution
		{
			Distribution(int count, int * weights, int * values);

			int chooseRandomValue() const;

			vector<int> m_weights;
			vector<int> m_values;
			vector<int> m_weightSums;
		};

		Distribution::Distribution(int count, int * weights, int * values) :
			m_weights(),
			m_values(),
			m_weightSums()
		{
			int totalWeight = 0;

			for (int index = 0; index < count; ++index)
			{
				m_weights.push_back(weights[index]);
				m_values.push_back(values[index]);
				m_weightSums.push_back(totalWeight);

				totalWeight += weights[index];
			}

			m_weightSums.push_back(totalWeight);
		}

		int Distribution::chooseRandomValue() const
		{
			int select = randomInRange(0, m_weightSums.back() - 1);

			int minIndex = 0;
			int maxIndex = m_weights.size();

			while (minIndex + 1 < maxIndex)
			{
				int midIndex = (minIndex + maxIndex) / 2;
				if (select >= m_weightSums[midIndex])
					minIndex = midIndex;
				else
					maxIndex = midIndex;
			}

			return m_values[minIndex];
		}
	};

	int sum(int count, int * data)
	{
		int sum = 0;

		for (int index = 0; index < count; ++index)
			sum += data[index];

		return sum;
	}

    void test(bool showDetails)
    {
		{
			using namespace x1;

			int values0[] = { 1, 2, -1, 3 };
			int values1[] = { -1, -2, -3, -4 };
			int values2[] = { -1, -2, 3, -3 };

			assert(sum(stripNegativeValues(4, values0), values0) == 6);
			assert(sum(stripNegativeValues(4, values1), values1) == 0);
			assert(sum(stripNegativeValues(4, values2), values2) == 3);
		}

		{
			using namespace x3;

			int weights[] = { 1, 2, 3, 4 };
			int values[] = { 1, 2, 3, 4 };

			int counts[5] = {};

			for (int i = 0; i < 10000; ++i)
			{
				int value = chooseRandomValue(4, weights, values);
				assert(value >= 1 && value <= 4);
				++counts[value];
			}

			assert(counts[1] > 900 && counts[1] < 1100);
			assert(counts[2] > 1900 && counts[2] < 2100);
			assert(counts[3] > 2900 && counts[3] < 3100);
			assert(counts[4] > 3900 && counts[4] < 4100);
		}

		{
			using namespace x4;

			int weights[] = { 1, 2, 3, 4 };
			int values[] = { 1, 2, 3, 4 };

			int counts[5] = {};

			for (int i = 0; i < 10000; ++i)
			{
				int value = chooseRandomValue(4, weights, values);
				assert(value >= 1 && value <= 4);
				++counts[value];
			}

			assert(counts[1] > 900 && counts[1] < 1100);
			assert(counts[2] > 1900 && counts[2] < 2100);
			assert(counts[3] > 2900 && counts[3] < 3100);
			assert(counts[4] > 3900 && counts[4] < 4100);
		}

		{
			using namespace x5;

			int weights[] = { 1, 2, 3, 4 };
			int values[] = { 1, 2, 3, 4 };

			Distribution distribution(4, weights, values);

			int counts[5] = {};

			for (int i = 0; i < 10000; ++i)
			{
				int value = distribution.chooseRandomValue();
				assert(value >= 1 && value <= 4);
				++counts[value];
			}

			assert(counts[1] > 900 && counts[1] < 1100);
			assert(counts[2] > 1900 && counts[2] < 2100);
			assert(counts[3] > 2900 && counts[3] < 3100);
			assert(counts[4] > 3900 && counts[4] < 4100);
		}

		{
			using namespace x6;

			int weights[] = { 1, 2, 3, 4 };
			int values[] = { 1, 2, 3, 4 };

			Distribution distribution(4, weights, values);

			int counts[5] = {};

			for (int i = 0; i < 10000; ++i)
			{
				int value = distribution.chooseRandomValue();
				assert(value >= 1 && value <= 4);
				++counts[value];
			}

			assert(counts[1] > 900 && counts[1] < 1100);
			assert(counts[2] > 1900 && counts[2] < 2100);
			assert(counts[3] > 2900 && counts[3] < 3100);
			assert(counts[4] > 3900 && counts[4] < 4100);
		}
    }

	RegisterTest rt("Rule 5", test);
};
