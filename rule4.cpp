
#include <vector>
#include <regex>
#include <assert.h>

using namespace std;

#pragma warning (disable: 4018 26451)

namespace rule4
{
	namespace xSign
	{
		enum class Color
		{
			Invalid,
			Red,
			Green,
			Blue
		};

		bool isColorValid(Color color)
		{
			return color != Color::Invalid;
		}

		bool matchColors(
			const vector<Color> & colors, 
			Color colorMatch)
		{
			if (colors.empty())
				return true;

			for (Color color : colors)
				if (color == colorMatch)
					return true;

			return false;
		}

		struct Location
		{
			bool operator != (Location otherLocation)
			{
				return true;
			}

			static Location Invalid;
		};

		Location Location::Invalid;

		bool isLocationValid(Location location)
		{
			return location != Location::Invalid;
		}

		float getDistance(Location location, Location otherLocation)
		{
			return 0.0f;
		}
		
		bool matchLocation(
			Location location, 
			float distanceMax, 
			Location locationMatch)
		{
			float distance = getDistance(location, locationMatch);
			return distance < distanceMax;
		}

		Location getClosestLocationOnSegment(
			Location locationStart,
			Location locationEnd,
			Location location)
		{
			return locationStart;
		}

		struct Sign
		{
			Color color() const
				{ return m_color; }
			Location location() const
				{ return m_location; }
			string text() const
				{ return m_text; }

			Color m_color;
			Location m_location;
			string m_text;
		};
	};

	namespace x1
	{
		using namespace xSign;

		Sign * findRedSign(const vector<Sign *> & signs)
		{
			for (Sign * sign : signs)
				if (sign->color() == Color::Red)
					return sign;

			return nullptr;
		}
	};

	namespace x2
	{
		using namespace xSign;

		Sign * findSignByColor(const vector<Sign *> & signs, Color color)
		{
			for (Sign * sign : signs)
				if (sign->color() == color)
					return sign;

			return nullptr;
		}
	};

	namespace x3
	{
		using namespace xSign;

		struct SignQuery
		{
			SignQuery() :
				m_colors(),
				m_location(),
				m_distance(FLT_MAX),
				m_textExpression(".*")
			{
				; 
			}

			bool matchSign(const Sign * sign) const
			{
				return matchColors(m_colors, sign->color()) &&
					   matchLocation(m_location, m_distance, sign->location()) &&
					   regex_match(sign->text(), m_textExpression);
			}

			vector<Color> m_colors;
			Location m_location;
			float m_distance;
			regex m_textExpression;
		};

		Sign * findSign(const SignQuery & query, const vector<Sign *> & signs)
		{
			for (Sign * sign : signs)
				if (query.matchSign(sign))
					return sign;

			return nullptr;
		}

		Sign * findRedSign(const vector<Sign *> & signs)
		{
			SignQuery query;
			query.m_colors = { Color::Red };
			return findSign(query, signs);
		}
	};

	namespace x4
	{
		using namespace xSign;

		Sign * findSignWithColorNearLocation(
			const vector<Sign *> & signs,
			Color color = Color::Invalid,
			Location location = Location::Invalid,
			float distance = 0.0f)
		{
			for (Sign * sign : signs)
			{
				if (isColorValid(color) &&
					sign->color() != color)
				{
					continue;
				}

				if (isLocationValid(location) &&
					getDistance(sign->location(), location) > distance)
				{
					continue;
				}

				return sign;
			}

			return nullptr;
		}

		Sign * findRedSign(const vector<Sign *> & signs)
		{
			for (Sign * sign : signs)
				if (sign->color() == Color::Red)
					return sign;

			return nullptr;
		}
	};

	namespace x5
	{
		using namespace xSign;

		Sign * findSignNearLocation(
			const vector<Sign *> & signs, 
			Location location,
			float distance)
		{
			for (Sign * sign : signs)
			{
				if (getDistance(sign->location(), location) <= distance)
				{
					return sign;
				}
			}

			return nullptr;
		}

		Sign * findSignWithColorNearLocation(
			const vector<Sign *> & signs,
			Color color,
			Location location,
			float distance)
		{
			for (Sign * sign : signs)
			{
				if (sign->color() == color &&
					getDistance(sign->location(), location) >= distance)
				{
					return sign;
				}
			}

			return nullptr;
		}
	};

	namespace x6
	{
		using namespace xSign;

		Sign * findSignWithColorNearLocation(
			const vector<Sign *> & signs, 
			Color color = Color::Invalid, 
			Location location = Location::Invalid,
			float distance = 0.0f)
		{
			for (Sign * sign : signs)
			{
				if (isColorValid(color) && 
					sign->color() != color)
				{
					continue;
				}

				if (isLocationValid(location) &&
					getDistance(sign->location(), location) > distance)
				{
					continue;
				}

				return sign;
			}

			return nullptr;
		}
	};

	namespace x7
	{
		using namespace xSign;
		using namespace x3;

		vector<Sign *> findSigns(
			const SignQuery & query,
			const vector<Sign *> & signs)
		{
			vector<Sign *> matchedSigns;

			for (Sign * sign : signs)
			{
				if (query.matchSign(sign))
					matchedSigns.push_back(sign);
			}

			return matchedSigns;
		}
	};

	namespace xArea
	{
		using namespace xSign;

		struct Area
		{
			enum class Kind
			{
				Invalid,
				Point,
				Street,
			};

			Kind m_kind;
			vector<Location> m_locations;
			float m_maxDistance;
		};

		static bool matchArea(const Area & area, Location matchLocation)
		{
			switch (area.m_kind)
			{
			case Area::Kind::Invalid:
				return true;

			case Area::Kind::Point:
				{
					float distance = getDistance(
										area.m_locations[0], 
										matchLocation);
					return distance <= area.m_maxDistance;
				}
				break;

			case Area::Kind::Street:
				{
					for (int index = 0; 
						 index < area.m_locations.size() - 1; 
						 ++index)
					{
						Location location = getClosestLocationOnSegment(
												area.m_locations[index + 0],
												area.m_locations[index + 1],
												matchLocation);

						float distance = getDistance(location, matchLocation);
						if (distance <= area.m_maxDistance)
							return true;
					}

					return false;
				}
				break;
			}

			return false;
		}
	};

	namespace x8
	{
		using namespace xArea;

		struct SignQuery
		{
			SignQuery() :
				m_colors(),
				m_area(),
				m_textExpression(".*")
			{
				; 
			}

			bool matchSign(const Sign * sign) const
			{
				return matchColors(m_colors, sign->color()) &&
					   matchArea(m_area, sign->location()) &&
					   regex_match(sign->m_text, m_textExpression);
			}

			vector<Color> m_colors;
			Area m_area;
			regex m_textExpression;
		};
	};

	namespace xAreas
	{
		using namespace xArea;

		bool matchAreas(const vector<Area> & areas, Location matchLocation)
		{
			if (areas.empty())
				return true;

			for (const Area & area : areas)
				if (matchArea(area, matchLocation))
					return true;

			return false;
		}
	};

	namespace x9
	{
		using namespace xAreas;

		struct SignQuery
		{
			SignQuery() :
				m_colors(),
				m_areas(),
				m_textExpression(".*")
			{
				; 
			}

			bool matchSign(const Sign * sign) const
			{
				return matchColors(m_colors, sign->color()) &&
					   matchAreas(m_areas, sign->location()) &&
					   regex_match(sign->m_text, m_textExpression);
			}

			vector<Color> m_colors;
			vector<Area> m_areas;
			regex m_textExpression;
		};
	};

	namespace x10
	{
		using namespace xAreas;

		struct SignQuery
		{
			SignQuery() :
				m_colors(),
				m_areas(),
				m_textExpression(".*"),
				m_boolean(Boolean::None),
				m_queries()
			{
				; 
			}

			~SignQuery()
			{
				for (SignQuery * query : m_queries)
					delete query;
			}

			enum class Boolean
			{
				None,
				And,
				Or,
				Not
			};

			static bool matchBoolean(
				Boolean boolean, 
				const vector<SignQuery *> & queries, 
				const Sign * sign)
			{
				switch (boolean)
				{
				case Boolean::Not:
					return !queries[0]->matchSign(sign);

				case Boolean::Or:
					{
						for (const SignQuery * query : queries)
							if (query->matchSign(sign))
								return true;

						return false;
					}
					break;

				case Boolean::And:
					{
						for (const SignQuery * query : queries)
							if (!query->matchSign(sign))
								return false;

						return true;
					}
					break;
				}

				return true;
			}

			bool matchSign(const Sign * sign) const
			{
				return matchColors(m_colors, sign->color()) &&
						matchAreas(m_areas, sign->location()) &&
						regex_match(sign->m_text, m_textExpression) &&
						matchBoolean(m_boolean, m_queries, sign);
			}

			vector<Color> m_colors;
			vector<Area> m_areas;
			regex m_textExpression;
			Boolean m_boolean;
			vector<SignQuery *> m_queries;
		};

		vector<Sign *> findSigns(
			const SignQuery & query,
			const vector<Sign *> & signs)
		{
			vector<Sign *> matchedSigns;

			for (Sign * sign : signs)
			{
				if (query.matchSign(sign))
					matchedSigns.push_back(sign);
			}

			return matchedSigns;
		}
	};

	namespace x11
	{
		using namespace x10;

		Area mainStreet;
		vector<Sign *> signs;

		void example()
		{
			SignQuery * blueQuery = new SignQuery;
			blueQuery->m_colors = { Color::Blue };

			SignQuery * locationQuery = new SignQuery;
			locationQuery->m_areas = { mainStreet };

			SignQuery query;
			query.m_boolean = SignQuery::Boolean::Or;
			query.m_queries = { blueQuery, locationQuery };

			vector<Sign *> locationSigns = findSigns(query, signs);
		}

	};

	namespace x12
	{
		using namespace xArea;

		Area mainStreet;
		vector<Sign *> signs;

		void example()
		{
			vector<Sign *> locationSigns;
			for (Sign * sign : signs)
			{
				if (sign->color() == Color::Blue ||
					matchArea(mainStreet, sign->location()))
				{
					locationSigns.push_back(sign);
				}
			}
		}
	};
};

