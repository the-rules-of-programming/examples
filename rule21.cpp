
#include <vector>
#include <string>
#include <assert.h>
#include "geom.h"

using namespace std;

#pragma warning (disable: 4018)

namespace rule21
{
	namespace chr
	{
		using namespace geom;

		struct Character
		{
			bool isAlliedWith(const Character * otherCharacter) const
				{ return false; }
			bool isIncapacitated() const
				{ return false; }
		};
	};

	namespace x1
	{
		using namespace chr;

		vector<Character *> findNearbyCharacters(
			const Point & point,
			float maxDistance);
	};

	namespace x2
	{
		using namespace chr;

		vector<Character *> findNearbyCharacters(
			const Point & point,
			float maxDistance,
			vector<Character *> excludeCharacters);
	};

	namespace x3
	{
		using namespace chr;

		vector<Character *> findNearbyCharacters(
			const Point & point,
			float maxDistance,
			vector<Character *> excludeCharacters = vector<
			 Character *>())
		{
			return vector<Character *>();
		}
	};

	namespace x4
	{
		using namespace chr;

		vector<Character *> findNearbyCharacters(
			const Point & point,
			float maxDistance);
		vector<Character *> findNearbyCharacters(
			const Point & point,
			float maxDistance,
			vector<Character *> excludeCharacters);
	};

	namespace x5
	{
		using namespace chr;

		struct CharacterFilter
		{
			virtual bool isCharacterAllowed(Character * character) const = 0;
		};

		vector<Character *> findNearbyCharacters(
			const Point & point,
			float maxDistance,
			CharacterFilter * filter);

		struct ThreatFilter : public CharacterFilter
		{
			ThreatFilter(const Character * character) :
				m_character(character)
				{ ; }

			bool isCharacterAllowed(Character * character) const override
			{
				return !character->isAlliedWith(m_character) &&
					   !character->isIncapacitated();
			}

			const Character * m_character;
		};

		struct ExcludeFilter : public CharacterFilter
		{
			ExcludeFilter(const vector<const Character *> & characters) :
				m_characters(characters)
				{ ;  }

			bool isCharacterAllowed(Character * character) const override
			{
				return m_characters.end() == find(
												m_characters.begin(),
												m_characters.end(),
												character);
			}

			vector<const Character *> m_characters;
		};
	};

	namespace hsh
	{
		int hashString(string s)
		{
			int hash = -1;
			for (char c : s)
			{
				hash = hash * 937 + c;
			}
			return hash;
		}

		int combineHashes()
		{
			return 0;
		}

		template <class T, class... TT>
		int combineHashes(T hash, TT... hashes)
		{
			return hash * 937 + combineHashes(hashes...);
		}
	};

	namespace x6
	{
		using namespace hsh;

		struct Unit
		{
			int m_id;
			string m_firstName;
			string m_lastName;
			string m_userName;
		};

		unsigned int hashUnit(const Unit & unit)
		{
			return combineHashes(
						hashString(unit.m_firstName),
						hashString(unit.m_lastName),
						hashString(unit.m_userName));
		}

		void squashAdjacentDups(
			vector<Unit> & units, 
			int (* hash)(const Unit &))
		{
			int nextIndex = 1;

			for (int index = 1; index < units.size(); ++index)
			{
				if (hash(units[index]) != hash(units[nextIndex - 1]))
				{
					units[nextIndex++] = units[index];
				}
			}

			while (units.size() > nextIndex)
			{
				units.pop_back();
			}
		}
	};
};
