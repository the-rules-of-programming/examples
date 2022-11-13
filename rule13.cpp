
#include <vector>
#include <assert.h>
#include "geom.h"

using namespace std;

namespace rule13
{
	namespace chr
	{
		struct Character
		{
			const char * name() const
				{ return ""; }
			const char * sourceFile() const
				{ return ""; }
			int priority() const
				{ return 0;}
		};

		void trace(const char * format, ...)
		{
		}

		void showObjectDetails(const Character * character)
		{
			trace(
				"character %s [%s] %s",
				(character) ? character->name() : "",
				character,
				(character->sourceFile()) ? character->sourceFile() : "");
		}

		vector<Character *> g_allCharacters;

	};

	namespace x1
	{
		using namespace chr;

		int calculateHighestCharacterPriority()
		{
			Character * bestCharacter = nullptr;

			for (Character * character : g_allCharacters)
			{
				if (!bestCharacter ||
					character->priority() > bestCharacter->priority())
				{
					bestCharacter = character;
				}
			}

			return bestCharacter->priority();
		}
	};

	namespace x2
	{
		using namespace chr;

		int calculateHighestCharacterPriority()
		{
			Character * bestCharacter = nullptr;

			for (Character * character : g_allCharacters)
			{
				if (!bestCharacter ||
					character->priority() > bestCharacter->priority())
				{
					bestCharacter = character;
				}
			}

			return (bestCharacter) ? bestCharacter->priority() : 0;
		}
	};

	namespace x3
	{
		using namespace chr;

		int calculateHighestCharacterPriority()
		{
			int highestPriority = 0;

			for (Character * character : g_allCharacters)
			{
				highestPriority = max(
									highestPriority, 
									character->priority());
			}

			return highestPriority;
		}
	};

	namespace x4
	{
		int getFibonacci(int n)
		{
			static vector<int> values = { 0, 1, 1, 2, 3, 5, 8, 13, 23, 34, 55 };
			for (int i = values.size(); i <= n; ++i)
			{
				values.push_back(values[i - 2] + values[i - 1]);
			}
			return values[n];
		}
	};

	namespace arm
	{
		struct Armor
		{
			int getThreat() const
				{ return 0; }
		};

		struct Weapon
		{
			int getThreat() const
				{ return 0; }
		};

		int getThreatFromHitPoints(float hitPoints)
		{
			return 0;
		}

	};

	namespace x5
	{
		using namespace arm;

		struct Character
		{
			void setArmor(Armor * armor)
			{
				m_threat -= m_armor->getThreat();
				m_threat += armor->getThreat();
				m_armor = armor;
			}

			void setWeapon(Weapon * weapon)
			{
				m_threat -= weapon->getThreat();
				m_threat += weapon->getThreat();
				m_weapon = weapon;
			}

			void setHitPoint(float hitPoints)
			{
				m_threat -= getThreatFromHitPoints(m_hitPoints);
				m_threat += getThreatFromHitPoints(hitPoints);
				m_hitPoints = hitPoints;
			}

			int getThreat() const
			{ 
				return m_threat; 
			}

		protected:

			int m_threat;
			Armor * m_armor;
			Weapon * m_weapon;
			float m_hitPoints;
		};
	};

	namespace x6
	{
		using namespace x5;

		struct Character : public x5::Character
		{
			void setWeapon(Weapon * weapon)
			{
				m_threat -= weapon->getThreat();
				m_threat += weapon->getThreat();
				m_weapon = weapon;
				audit();
			}

			void audit() const
			{
				int expectedThreat = m_armor->getThreat() +
									 m_weapon->getThreat() +
									 getThreatFromHitPoints(m_hitPoints);

				assert(m_threat == expectedThreat);
			}
		};

	};

	namespace x7
	{
		using namespace arm;

		struct Character
		{
			void setArmor(Armor * armor)
			{
				m_armor = armor;
			}

			void setWeapon(Weapon * weapon)
			{
				m_weapon = weapon;
			}

			void setHitPoint(float hitPoints)
			{
				m_hitPoints = hitPoints;
			}

			int getThreat() const
			{ 
				return m_armor->getThreat() -
					   m_weapon->getThreat() +
					   getThreatFromHitPoints(m_hitPoints); 
			}

		protected:

			Armor * m_armor;
			Weapon * m_weapon;
			float m_hitPoints;
		};
	};

	namespace dmr
	{
		using namespace geom;

		struct Damage
		{
			bool isArrow() const
				{ return false; }
			Vector impactVelocity() const
				{ return Vector(); }
		};

		struct Reaction
		{
			bool isStumble() const
				{ return false; }
			Vector stumbleDirection() const
				{ return Vector(); }
		};
	};

	namespace x8
	{
		using namespace dmr;

		struct DamageArbiter
		{
			void getDamageReaction(
				const Damage * damage,
				Reaction * reaction) const;
		};

		void DamageArbiter::getDamageReaction(
			const Damage * damage, 
			Reaction * reaction) const
		{
			// All the actual logic for mapping damage to reactions goes here.
			//  There's a single function that does this in the Sucker Punch
			//  engine. That function is nearly 3000 lines long and is not the
			//  purest embodiment of the Rules, though to be fair it's solving
			//  a very complicated problem.

			if (damage->isArrow())
			{
				assert(reaction->isStumble());
				Vector arrowVelocity = damage->impactVelocity();
				Vector stumbleDirection = reaction->stumbleDirection();
				assert(dotProduct(arrowVelocity, stumbleDirection) > 0.0f);
			}
		}

	};

	namespace x9
	{
		using namespace dmr;

#define CHRISZ 1

		struct DamageArbiter
		{
			void getDamageReaction(
				const Damage * damage,
				Reaction * reaction) const;
		};

		void DamageArbiter::getDamageReaction(
			const Damage * damage, 
			Reaction * reaction) const
		{
			// All the actual logic for mapping damage to reactions goes here.
			//  There's a single function that does this in the Sucker Punch
			//  engine. That function is nearly 3000 lines long and is not the
			//  purest embodiment of the Rules, though to be fair it's solving
			//  a very complicated problem.

			if (damage->isArrow())
			{
				assert(reaction->isStumble());
				Vector arrowVelocity = damage->impactVelocity();
				Vector stumbleDirection = reaction->stumbleDirection();
				if (dotProduct(arrowVelocity, stumbleDirection) <= 0.0f)
				{
					assert(false);

					static bool s_debugProblem = CHRISZ;
					if (s_debugProblem)
					{
						getDamageReaction(damage, reaction);
					}
				}
			}
		}
	};
};
