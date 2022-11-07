
#include <vector>
#include <string>
#include <assert.h>
#include "tests.h"

using namespace std;

namespace rule18
{
	namespace x1
	{
		//

		enum class Connect
		{
			Retry = 1,
			InternalServer = 2
		};

		int operator | (Connect connectLeft, Connect connectRight)
		{
			return int(connectLeft) | int(connectRight);
		}

		class ConnectionHandle
		{
		public:

			ConnectionHandle()
				{ ; }

			bool isValid() const
				{ return true;  }
		};

		class Blob
		{
		public:

			Blob()
				{ ; }
		};

		ConnectionHandle connectToStagingServer(const string, int connectFlags)
		{
			return ConnectionHandle();
		}

		void postBlob(ConnectionHandle handle, Blob * blob)
		{
		}

		//

		void postToStagingServer(string url, Blob * payload)
		{
			// Will always get a valid handle back due to Connect::Retry

			ConnectionHandle handle = connectToStagingServer(
										url,
										Connect::Retry | Connect::InternalServer);

			// Post data 

			postBlob(handle, payload);
		}

	};

	namespace x2
	{
		struct Blob
		{
		};

		/**
		* @brief Post payload to staging server
		* 
		* Attempt to post the given payload to the staging server at the
		* given address, returning @c true if the post is successful and
		* @c false if it fails for some reason.
		* 
		* @param url URL to server
		* @param payload Data to post
		* @returns true on success
		*/
		bool tryPostToStagingServer(string url, Blob * payload);

	};

	namespace x3
	{
		bool findPermutation(const string & p, const string & s)
		{
			int pl = p.length(), sl = s.length();
			if (sl < pl) return false;
			int pcs[CHAR_MAX] = {}, scs[CHAR_MAX] = {};
			for (unsigned char c : p)
			{ ++pcs[c]; }
			int si = 0;
			for (; si < pl; ++si)
			{ ++scs[static_cast<unsigned char>(s[si])]; }
			for (;; ++si)
			{
			for (int pi = 0;; ++pi)
			{
			if (pi >= pl) return true;
			unsigned char c = p[pi];
			if (pcs[c] != scs[c]) break;
			}
			if (si >= sl) break;
			--scs[static_cast<unsigned char>(s[si - pl])];
			++scs[static_cast<unsigned char>(s[si])];
			}
			return false;
		}
	};

	namespace x4
	{
		// Check whether any permutation of the permute string appears in the 
		//  search string 

		bool tryFindPermutation(const string & permute, const string & search)
		{
			// If the search string is shorter than the permute string, then there's 
			//  no way it can be a permutation. Exit now to simplify things.

			int permuteLength = permute.length();
			int searchLength = search.length();
			if (searchLength < permuteLength) 
				return false;

			// Count how many times each letter shows up in the permute string. We'll
			//  compare these counts to running counts we'll keep in the search string.

			int permuteCounts[UCHAR_MAX] = {};
			for (unsigned char c : permute)
			{ 
				++permuteCounts[c]; 
			}

			// Make the same counts for the first possible match in the search string

			int searchCounts[UCHAR_MAX] = {};
			int searchIndex = 0;

			for (; searchIndex < permuteLength; ++searchIndex)
			{ 
				unsigned char c = search[searchIndex];
				++searchCounts[c]; 
			}

			// Loop over the possible matching substrings in the search string

			for (;; ++searchIndex)
			{
				// Check whether the current substring matches the permute string

				for (int permuteIndex = 0;; ++permuteIndex)
				{
					// If we didn't find any character count mismatches after we've
					//  checked all the characters in the permute string, then we've
					//  found a permutation. Return true to mark that success.

					if (permuteIndex >= permuteLength) 
						return true;

					// If the count of this character in the permute string doesn't 
					//  match the count in this substring of the search string, then 
					//  the substring isn't a permutation. Move on to the next one.

					unsigned char c = permute[permuteIndex];
					if (permuteCounts[c] != searchCounts[c]) 
						break;
				}

				// Stop once we've checked all possible substrings in the search string

				if (searchIndex >= searchLength)
					break;

				// Update the running character counts to match the next possible match

				unsigned char drop = search[searchIndex - permuteLength];
				unsigned char add = search[searchIndex];

				--searchCounts[drop]; 
				++searchCounts[add];
			}

			// If we make it here, then we're out of substrings and didn't find any
			//  matching permutations, since we return immediately once they're found.

			return false;
		}
	};

	void test(bool showDetails)
	{
		{
			using namespace x4;

            assert(tryFindPermutation("abc", "cabbage"));
            assert(tryFindPermutation("abc", "abacus"));
            assert(!tryFindPermutation("abc", "scramble"));
            assert(!tryFindPermutation("abc", "brackish"));
            assert(tryFindPermutation("aabb", "cabbage"));
            assert(!tryFindPermutation("aaabb", "cabbage"));
		}
	}

	RegisterTest rt("Rule 18", test);
};
