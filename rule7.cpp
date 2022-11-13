
#include <vector>
#include <string>
#include <assert.h>
#include "geom.h"

using namespace std;

#pragma warning(disable: 4477 4018 4305 4244)

namespace rule7
{
	namespace x1
	{
		void showAuthorRoyalties(const char * authorName, double amount)
		{
			printf("%s is due $%.2f this quarter.\n", authorName, amount);
		}
	};

	namespace x2
	{
		void showAuthorRoyalties(const char * authorName, double amount)
		{
			printf("remit $%.2f to %s this quarter.\n", authorName, amount);
		}
	};

	namespace x3
	{
		enum class MessageID
		{
			RoyaltyFormat
		};

		const char * getLocalizedMessage(MessageID messageID)
		{
			return nullptr;
		}

		void showAuthorRoyalties(const char * authorName, double amount)
		{
			printf(
				getLocalizedMessage(MessageID::RoyaltyFormat), 
				authorName, 
				amount);
		}
	};

	namespace x4
	{
		void showAuthorRoyalties(
			const vector<string> & titles, 
			const vector<double> & royalties)
		{
			assert(titles.size() == royalties.size());

			for (int index = 0; index < titles.size(); ++index)
			{
				printf("%s,%f\n", titles[index].c_str(), royalties[index]);
			}
		}
	};

	namespace x5
	{
		using namespace geom;

		Point convertCoordinateSystem(
			const Point & point, 
			bool isFromIdentity, 
			const Matrix & fromMatrix, 
			bool isToIdentity,
			const Matrix & toMatrix)
		{
			assert(!isFromIdentity || fromMatrix.isZero());
			assert(!isToIdentity || toMatrix.isZero());

			Point convertedPoint = point;
			if (!isFromIdentity)
				convertedPoint *= fromMatrix;
			if (!isToIdentity)
				convertedPoint *= Invert(toMatrix);

			return convertedPoint;
		}

	};

	namespace x6
	{
		using namespace geom;

		struct TitleInfo
		{
			string m_title;
			float m_royalty;
		};

		void showAuthorRoyalties(const vector<TitleInfo> & titleInfos)
		{
			for (const TitleInfo & titleInfo : titleInfos)
			{
				printf("%s,%f\n", titleInfo.m_title.c_str(), titleInfo.m_royalty);
			}
		}

		Point convertCoordinateSystem(
			const Point & point, 
			const Matrix & fromMatrix, 
			const Matrix & toMatrix)
		{
			Point convertedPoint = point;
			if (!fromMatrix.isIdentity())
				convertedPoint *= fromMatrix;
			if (!toMatrix.isIdentity())
				convertedPoint *= Invert(toMatrix);

			return convertedPoint;
		}
	};

	namespace drw
	{
		enum class ViewKind
		{
			Invalid,
			World
		};

		enum class DrawStyle
		{
			Invalid,
			Wireframe,
			Solid
		};

		enum class TimeStyle
		{
			Invalid,
			Update
		};

		struct Time
		{
		};

		struct OffsetPolys
		{
		};

		struct LineWidth
		{
		};

		struct CustomView
		{
		};

		struct BufferStrategy
		{
		};

		struct XRay
		{
		};

		struct HitTestContext
		{
		};

		enum ColorTag
		{
			Red
		};

		struct Color
		{
			Color()
				{ ; }
			Color(ColorTag colorTag)
				{ ; }
		};
	};

	namespace chr
	{
		using namespace geom;

		enum class InvulnerabilityReason
		{
			CutScene,
			Potion
		};

		struct Character
		{
			Point getPosition() const
				{ return Point(); }
			void setInvulnerable(bool invulnerable)
				{ ; }
			bool isInvulnerable() const
				{ return false; }
			void setInvulnerable(InvulnerabilityReason reason, bool invulnerable)
				{ ; }
			void pushInvulnerability()
				{ ; }
			void popInvulnerability()
				{ ; }
		};

		Character * getPlayer()
			{ return nullptr; }
		void playCutScene(const char * cutSceneName)
			{ ; }

		float now()
		{
			return 0.0f;
		}

		void sleepUntil(float time)
		{
		}
	};

	namespace x7
	{
		using namespace geom;
		using namespace drw;
		using namespace chr;

		//

		struct Params
		{
			Params(
				const Matrix & matrix,
				const Sphere & sphereBounds,
				ViewKind viewKind,
				DrawStyle drawStyle,
				TimeStyle timeStyle,
				const Time & timeExpires,
				string tagName,
				const OffsetPolys & offsetPolys,
				const LineWidth & lineWidth,
				const CustomView & customView,
				const BufferStrategy & bufferStrategy,
				const XRay & xRay,
				const HitTestContext * hitTestContext,
				bool exclude,
				bool pulse,
				bool faceCamera);

			void drawSphere(Point point, float radius, Color color);
		};

		void markCharacterPosition(const Character * character)
		{
			Params params(
				Matrix(Identity),
				Sphere(),
				ViewKind::World,
				DrawStyle::Wireframe,
				TimeStyle::Update,
				Time(),
				string(),
				OffsetPolys(),
				LineWidth(),
				CustomView(),
				BufferStrategy(),
				XRay(),
				nullptr,
				false,
				false,
				false);

			params.drawSphere(
				character->getPosition() + Vector(0.0, 0.0, 2.0),
				0.015,
				Color(Red));
		}

		//

		Params::Params(
			const Matrix & matrix,
			const Sphere & sphereBounds,
			ViewKind viewKind,
			DrawStyle drawStyle,
			TimeStyle timeStyle,
			const Time & timeExpires,
			string tagName,
			const OffsetPolys & offsetPolys,
			const LineWidth & lineWidth,
			const CustomView & customView,
			const BufferStrategy & bufferStrategy,
			const XRay & xRay,
			const HitTestContext * hitTestContext,
			bool exclude,
			bool pulse,
			bool faceCamera)
		{
		}

		void Params::drawSphere(Point point, float radius, Color color)
		{
		}
	};

	namespace x8
	{
		using namespace geom;
		using namespace drw;
		using namespace chr;

		//

		struct Params
		{
			Params();

			void setXRay(float alpha);
			void commit();

			void drawSphere(Point point, float radius, Color color);
		};

		void markCharacterPosition(const Character * character)
		{
			Params params;
			params.setXRay(0.5);
			params.commit();

			params.drawSphere(
				character->getPosition() + Vector(0.0, 0.0, 2.0),
				0.015,
				Color(Red));
		}

		//

		Params::Params()
		{
		}

		void Params::setXRay(float alpha)
		{
		}

		void Params::commit()
		{
		}
		
		void Params::drawSphere(Point point, float radius, Color color)
		{
		}
	};

	namespace x9
	{
		using namespace geom;
		using namespace drw;
		using namespace chr;

		//

		struct Params
		{
			Params();

			void setXRay(float alpha);
			void setDrawStyle(DrawStyle drawStyle);
			void setPulse(bool pulse);
		};

		struct Draw
		{
			Draw(const Params & params);

			void drawSphere(Point point, float radius, Color color);
		};

		void markCharacterPosition(const Character * character)
		{
			Params params;
			params.setXRay(0.5);
			params.setDrawStyle(DrawStyle::Solid);
			params.setPulse(true);

			Draw draw(params);
			draw.drawSphere(
				character->getPosition() + Vector(0.0, 0.0, 2.0),
				0.015,
				Color(Red));
		}

		//

		Params::Params()
		{
		}

		void Params::setXRay(float alpha)
		{
		}

		void Params::setDrawStyle(DrawStyle drawStyle)
		{
		}

		void Params::setPulse(bool pulse)
		{
		}

		Draw::Draw(const Params & params)
		{
		}

		void Draw::drawSphere(Point point, float radius, Color color)
		{
		}
	};

	namespace x10
	{
		using namespace geom;
		using namespace drw;
		using namespace chr;

		//

		struct Params
		{
			Params();

			Params & setXRay(float alpha);
			Params & setDrawStyle(DrawStyle drawStyle);
			Params & setPulse(bool pulse);
		};

		struct Draw
		{
			Draw(const Params & params);

			void drawSphere(Point point, float radius, Color color);
		};

		void markCharacterPosition(const Character * character)
		{
			const Params params = Params()
								  .setXRay(0.5)
								  .setDrawStyle(DrawStyle::Solid)
								  .setPulse(true);

			Draw draw(params);
			draw.drawSphere(
				character->getPosition() + Vector(0.0, 0.0, 2.0),
				0.015,
				Color(Red));
		}

		//

		Params::Params()
		{
		}

		Params & Params::setXRay(float alpha)
		{
			return *this;
		}

		Params & Params::setDrawStyle(DrawStyle drawStyle)
		{
			return *this;
		}

		Params & Params::setPulse(bool pulse)
		{
			return *this;
		}


		Draw::Draw(const Params & params)
		{
		}

		void Draw::drawSphere(Point point, float radius, Color color)
		{
		}
	};

	namespace x11
	{
		using namespace geom;
		using namespace drw;
		using namespace chr;

		//

		struct Params
		{
			Params();

			Params & setXRay(float alpha);
			Params & setDrawStyle(DrawStyle drawStyle);
			Params & setPulse(bool pulse);
		};

		struct Draw
		{
			Draw(const Params & params);

			void drawSphere(Point point, float radius, Color color);
		};

		void markCharacterPosition(const Character * character)
		{
			Draw draw = Params()
						.setXRay(0.5)
						.setDrawStyle(DrawStyle::Solid)
						.setPulse(true);

			draw.drawSphere(
				character->getPosition() + Vector(0.0, 0.0, 2.0),
				0.015,
				Color(Red));
		}

		//

		Params::Params()
		{
		}

		Params & Params::setXRay(float alpha)
		{
			return *this;
		}

		Params & Params::setDrawStyle(DrawStyle drawStyle)
		{
			return *this;
		}

		Params & Params::setPulse(bool pulse)
		{
			return *this;
		}

		Draw::Draw(const Params & params)
		{
		}

		void Draw::drawSphere(Point point, float radius, Color color)
		{
		}
	};

	namespace x12
	{
		using namespace geom;
		using namespace drw;
		using namespace chr;

		struct Pulse
		{
		};

		struct XRay
		{
			XRay(float alpha)
				{ ; }
		};

		struct Draw
		{
			template <class... TT>
			Draw(TT... args)
				{ addArgs(args...); }

			void addArgs()
				{ ; }
			template <class T, class... TT>
			void addArgs(const T & arg, TT... remainingArgs)
				{
					addArg(arg);
					addArgs(remainingArgs...);
				}

			void addArg(const XRay & xRay)
				{ ; }
			void addArg(const Pulse & pulse)
				{ ;  }
			void addArg(const DrawStyle & drawStyle)
				{ ; }

			void drawSphere(
				const Point & point,
				double radius,
				Color color)
				{ ; }
		};

		void markCharacterPosition(const Character * character)
		{
			Draw draw(XRay(0.5), DrawStyle::Solid, Pulse());

			draw.drawSphere(
				character->getPosition() + Vector(0.0, 0.0, 2.0),
				0.015,
				Color(Red));
		}
	};

	namespace x13
	{
		using namespace chr;

		void playCelebrationCutScene()
		{
			Character * player = getPlayer();
			player->setInvulnerable(true);
			playCutScene("where's chewie's medal.cut");
			player->setInvulnerable(false);
		}

		void chugInvulnerabilityPotion()
		{
			Character * player = getPlayer();
			player->setInvulnerable(true);
			sleepUntil(now() + 5.0);
			player->setInvulnerable(false);
		}
	};

	namespace x14
	{
		using namespace chr;

		void playCelebrationCutScene()
		{
			Character * player = getPlayer();
			bool wasInvulnerable = player->isInvulnerable();
			player->setInvulnerable(true);
			playCutScene("where's chewie's medal.cut");
			player->setInvulnerable(wasInvulnerable);
		}

		void chugInvulnerabilityPotion()
		{
			Character * player = getPlayer();
			bool wasInvulnerable = player->isInvulnerable();
			player->setInvulnerable(true);
			sleepUntil(now() + 5.0);
			player->setInvulnerable(wasInvulnerable);
		}
	};

	namespace x15
	{
		using namespace chr;

		void playCelebrationCutScene()
		{
			Character * player = getPlayer();
			player->setInvulnerable(InvulnerabilityReason::CutScene, true);
			playCutScene("it's anti-fur bias, that's what it is.cut");
			player->setInvulnerable(InvulnerabilityReason::CutScene, false);
		}

		void chugInvulnerabilityPotion()
		{
			Character * player = getPlayer();
			player->setInvulnerable(InvulnerabilityReason::Potion, true);
			sleepUntil(now() + 5.0);
			player->setInvulnerable(InvulnerabilityReason::Potion, false);
		}
	};

	namespace x16
	{
		using namespace chr;

		void playCelebrationCutScene()
		{
			Character * player = getPlayer();
			player->pushInvulnerability();
			playCutScene("I'm getting my own ship.cut");
			player->popInvulnerability();
		}

		void chugInvulnerabilityPotion()
		{
			Character * player = getPlayer();
			player->pushInvulnerability();
			sleepUntil(now() + 5.0);
			player->popInvulnerability();
		}
	};

	namespace x17
	{
		using namespace chr;

		struct InvulnerableToken
		{
			InvulnerableToken(Character * character) :
				m_character(character)
				{ m_character->pushInvulnerability(); }
			~InvulnerableToken()
				{ m_character->popInvulnerability(); }

			Character * m_character;
		};

		void playCelebrationCutScene()
		{
			Character * player = getPlayer();
			InvulnerableToken invulnerable(player);

			playCutScene("see you later, losers.cut");
		}

		void chugInvulnerabilityPotion()
		{
			Character * player = getPlayer();
			InvulnerableToken invulnerable(player);

			sleepUntil(now() + 5.0);
		}
	};
};
