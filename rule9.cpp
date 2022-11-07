
#include <vector>
#include <algorithm>
#include <assert.h>
#include "tests.h"

using namespace std;

#pragma warning(disable: 4018)

namespace rule9
{
	namespace x1
	{
		void example()
		{
			vector<int> values;

			int sum = 0;
			for (int value : values)
			{
				sum += value;
			}
		}
	};

	namespace x2
	{
		void example(vector<int> * finalResults)
		{
			vector<bool> flags(100, false);
			vector<int> results;

			for (int value = 2; value < flags.size(); ++value)
			{
				if (flags[value])
					continue;

				results.push_back(value);

				for (int multiple = value;
					 multiple < flags.size();
					 multiple += value)
				{
					flags[multiple] = true;
				}
			}

			*finalResults = results;
		}
	};

	namespace x3
	{
		void factorValue(
			int unfactoredValue, 
			vector<int> * factors)
		{
			// Clear flags marking multiples of primes

			vector<bool> isMultiple;
			for (int value = 0; value < 100; ++value)
				isMultiple.push_back(false);

			// Find primes by skipping multiples of primes

			vector<int> primes;

			for (int value = 2; value < isMultiple.size(); ++value)
			{
				if (isMultiple[value])
					continue;

				primes.push_back(value);

				for (int multiple = value; 
					 multiple < isMultiple.size(); 
					 multiple += value)
				{
					isMultiple[multiple] = true;
				}
			}

			// Find prime factors of value

			int remainder = unfactoredValue;

			for (int prime : primes)
			{
				while (remainder % prime == 0)
				{
					factors->push_back(prime);
					remainder /= prime;
				}
			}
		}
	};

	namespace x4
	{
		void clearFlags(
			int count, 
			vector<bool> * flags)
		{
			flags->clear();
			for (int value = 0; value < count; ++value)
				flags->push_back(false);
		}

		void getPrimes(
			vector<bool> & isMultiple, 
			vector<int> * primes)
		{
			for (int value = 2; value < isMultiple.size(); ++value)
			{
				if (isMultiple[value])
					continue;

				primes->push_back(value);

				for (int multiple = value; 
					 multiple < isMultiple.size(); 
					 multiple += value)
				{
					isMultiple[multiple] = true;
				}
			}
		}

		void getFactors(
			int unfactoredValue, 
			const vector<int> & primes, 
			vector<int> * factors)
		{
			int remainder = unfactoredValue;

			for (int prime : primes)
			{
				while (remainder % prime == 0)
				{
					factors->push_back(prime);
					remainder /= prime;
				}
			}
		}

		void factorValue(
			int unfactoredValue, 
			vector<int> * factors)
		{
			vector<bool> isMultiple;
			clearFlags(100, &isMultiple);

			vector<int> primes;
			getPrimes(isMultiple, &primes);

			getFactors(unfactoredValue, primes, factors);
		}
	};

	namespace x5
	{
		template <class T, class D, class F>
		D reduce(T begin, T end, D init, F func)
		{
			D accum = init;
			for (auto iter = begin; iter != end; ++iter)
			{
				accum = func(accum, *iter);
			}
			return accum;
		}

		template <class T, class D, class F>
		D reduce(T & t, D init, F func)
		{
			return reduce(t.begin(), t.end(), init, func);
		}

		int add(int a, int b)
		{
			return a + b;
		}

		int example()
		{
			vector<int> values = { 1, 2, 3, 4 };

			int sum = reduce(values, 0, add);

			return sum;
		}
	};

	namespace x6
	{
		template <class ITER, class COMP>
		void processVector(ITER begin, ITER end, COMP compare)
		{
			sort(begin, end, compare);
		}

		void example(vector<float> & values)
		{
			sort(
				values.begin(),
				values.end(),
				[](float a, float b) { return a < b; });

			processVector(
				values.begin(),
				values.end(),
				[](float a, float b) { return a < b; });
		}
	};

	namespace x7
	{
		class BitVector
		{
		public:

			BitVector(int size) :
				m_size(size),
				m_values()
			{
				m_values.resize((size + 31) / 32, 0);
			}

			int size() const
			{
				return m_size;
			}

			class Bit
			{
				friend class BitVector;

			public:

				operator bool () const
				{
					return (*m_value & m_bit) != 0; 
				}

				void operator = (bool value)
				{
					if (value) 
						*m_value |= m_bit; 
					else 
						*m_value *= ~m_bit;
				}

				unsigned int * m_value;
				unsigned int m_bit;
			};

			Bit operator [] (int index)
			{
				assert(index >= 0 && index < m_size);
				Bit bit = { &m_values[index / 32], 1U << (index % 32) };
				return bit;
			}

		protected:

			int m_size;
			vector<unsigned int> m_values;
		};

		void example(vector<int> * result)
		{
			vector<int> primes;
			BitVector isMultiple(100);

			for (int value = 2; value < isMultiple.size(); ++value)
			{
				if (isMultiple[value])
					continue;

				primes.push_back(value);

				for (int multiple = value; 
					 multiple < isMultiple.size(); 
					 multiple += value)
				{
					isMultiple[multiple] = true;
				}
			}

			*result = primes;
		}
	};

    void test(bool showDetails)
    {
		{
			using namespace x2;

			vector<int> results;
			example(&results);

			vector<int> expectedResults = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };
			assert(results == expectedResults);
		}

		{
			using namespace x3;

			vector<int> factors;
			vector<int> expectedFactors;

			factorValue(5, &factors);

			expectedFactors = { 5 };
			assert(factors == expectedFactors);

			factors.clear();
			factorValue(13 * 23 * 97, &factors);

			expectedFactors = { 13, 23, 97 };
			assert(factors == expectedFactors);

			factors.clear();
			factorValue(2 * 2 * 13 * 13 * 23 * 97, &factors);

			expectedFactors = { 2, 2, 13, 13, 23, 97 };
			assert(factors == expectedFactors);
		}

		{
			using namespace x4;

			vector<int> factors;
			vector<int> expectedFactors;

			factorValue(5, &factors);

			expectedFactors = { 5 };
			assert(factors == expectedFactors);

			factors.clear();
			factorValue(13 * 23 * 97, &factors);

			expectedFactors = { 13, 23, 97 };
			assert(factors == expectedFactors);

			factors.clear();
			factorValue(2 * 2 * 13 * 13 * 23 * 97, &factors);

			expectedFactors = { 2, 2, 13, 13, 23, 97 };
			assert(factors == expectedFactors);
		}

		{
			using namespace x5;

			assert(example() == 10);
		}

		{
			using namespace x6;

			vector<float> values = { 1.0f, 4.0f, 2.0f, 5.0f, 3.0f };
			example(values);

			vector<float> expectedValues = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
			assert(values == expectedValues);
		}

		{
			using namespace x7;

			vector<int> results;
			example(&results);

			vector<int> expectedResults = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };
			assert(results == expectedResults);
		}
    }

	RegisterTest rt("Rule 9", test);
};
