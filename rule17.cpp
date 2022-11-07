
#include <vector>
#include <assert.h>
#include "geom.h"
#include "tests.h"

using namespace std;

namespace rule17
{
	namespace x1
	{
        struct ObjectiveFunction
        {
            virtual float evaluate(float value) const = 0;
        };

        float optimizeViaGoldenSection(
            const ObjectiveFunction & objectiveFunction, 
            float initialGuess, 
            float step,
            float tolerance)
        {
            // Track a domain + range pair for the objective function

            struct Sample
            {
                Sample(float x, const ObjectiveFunction & objectiveFunction) :
                    m_x(x),
                    m_y(objectiveFunction.evaluate(x))
                    { ; }
                Sample(
                    const Sample & a, 
                    const Sample & b, 
                    float r, 
                    const ObjectiveFunction & objectiveFunction) :
                    m_x(a.m_x + (b.m_x - a.m_x) * r),
                    m_y(objectiveFunction.evaluate(m_x))
                    { ;  }

                float m_x;
                float m_y;
            };

            // Get an initial triplet of samples around the initial guess

            Sample a(initialGuess - step, objectiveFunction);
            Sample mid(initialGuess, objectiveFunction);
            Sample b(initialGuess + step, objectiveFunction);

            // Make sure the "a" side has a smaller range value than the "b" 
            //  side. If we haven't lucked into an initial range bracketing a
            //  minimum, we'll travel towards "a" until we find one.

            if (a.m_y > b.m_y)
            {
                swap(a, b);
            }

            // Find a point where the "mid" range value is smaller than the 
            //  "a" and "b" range values. That guarantees a local minimum
            //  somewhere between a and b.

            while (a.m_y < mid.m_y)
            {
                b = mid;
                mid = a;
                a = Sample(b, mid, 2.61034f, objectiveFunction);
            }

            // Loop until we've got a tight enough bracket on the domain

            while (abs(a.m_x - b.m_x) > tolerance)
            {
                // Makes sure the "a" side brackets a bigger domain than the 
                //  "b" side, so that the golden section is taken out of the
                //  bigger side.

                if (abs(mid.m_x - a.m_x) < abs(mid.m_x - b.m_x))
                    swap(a, b);

                // Test a point between the "mid" sample (our best guess so 
                //  far) and the "a" sample. If it's better than the mid
                //  sample, it becomes the new mid sample and the old mid
                //  sample is the new "b" side. Otherwise, the new sample
                //  becomes the new "a" side.

                Sample test(mid, a, 0.381966f, objectiveFunction);

                if (test.m_y < mid.m_y)
                {
                    b = mid;
                    mid = test;
                }
                else
                {
                    a = test;
                }
            }

            // Return the best domain value we found

            return mid.m_x;
        }
	};

    namespace bez
    {
        using namespace geom;

		struct Bezier
		{
			Point evaluate(float u) const
			{
				float u1 = 1.0f - u;

				return u1 * u1 * u1 * m_p0 +
					   3.0f * u * u1 * u1 * m_p1 +
					   3.0f * u * u * u1 * m_p2 +
					   u * u * u * m_p3;
			}

			Point m_p0;
			Point m_p1;
			Point m_p2;
			Point m_p3;
		};
    };

    namespace quad
    {
		struct QuadraticSolution
		{
            int m_solutionCount;
			float m_solutions[2];
		};

        QuadraticSolution solveQuadratic(float a, float b, float c)
        {
            assert(fabsf(a) > (fabsf(b) + fabsf(c)) * 1e-6f);

            float r = b * b - 4.0f * a * c;

            int count = 0;
            float x0 = 0.0f;
            float x1 = 0.0f;

            if (fabsf(r / a) < 1e-6f)
            {
                count = 1;
                x0 = -0.5f * b / a;
            }
            else if (r > 0.0f)
            {
                float sign = (b >= 0.0f) ? 1.0f : -1.0f;
                float q = -0.5f * (b + sign * sqrtf(r));

                count = 2;
                x0 = q / a;
                x1 = c / q;

                if (x0 > x1)
                    swap(x0, x1);
            }

            return { count, { x0, x1 } };
		}
    };

	namespace x2
	{
        using namespace x1;
        using namespace bez;
        using namespace quad;

        struct BezierCostFunction : public ObjectiveFunction
        {
            BezierCostFunction(
                const Bezier & bezier, 
                const Point & currentPosition, 
                const Vector & currentVelocity,
                float gravity) :
                m_bezier(bezier),
                m_currentPosition(currentPosition),
                m_currentVelocity(currentVelocity),
                m_gravity(gravity)
            {
            }

            float evaluate(float u) const override;

            Bezier m_bezier;
            Point m_currentPosition;
            Vector m_currentVelocity;
            float m_gravity;
        };

        float BezierCostFunction::evaluate(float u) const
        {
            // Get point along curve

            Point point = m_bezier.evaluate(u);

            // Calculate how much time it will take to fall to the height
            //  of that point

            QuadraticSolution result;
            result = solveQuadratic(
                        0.5f * m_gravity,
                        m_currentVelocity.m_z,
                        m_currentPosition.m_z - point.m_z);

            float t = result.m_solutions[1];

            // Assume we scrub off all horizontal velocity before landing

            Vector finalVelocity =
                {
                    0.0f,
                    0.0f,
                    m_currentVelocity.m_z + t * m_gravity
                };

            // Get immediate and final accelerations... since we're
            //  following a cubic curve one of these will be the maximum
            //  acceleration

            Vector a0 = (6.0f / (t * t)) * (point - m_currentPosition) +
                        -4.0f / t * m_currentVelocity +
                        -2.0f / t * finalVelocity;

            Vector a1 = (-6.0f / (t * t)) * (point - m_currentPosition) +
                        2.0f / t * m_currentVelocity +
                        4.0f / t * finalVelocity;

            // Ignore acceleration in Z, since we know that's gravity

            a0.m_z = 0.0f;
            a1.m_z = 0.0f;

            // Calculate cost function

            return t * max(a0.getLength(), a1.getLength());
        }
    };

	namespace x3
	{
        using namespace geom;

        class PathManager
        {
        public:

            float clipEdge(
                const Point & start,
                const Point & end) const;
            vector<Point> findPath(
                const Point & startPoint,
                float heading,
                float idealDistance) const;
        };
	};

	namespace x4
	{
        using namespace geom;

        struct GridPoint
        {
            int m_x;
            int m_y;
        };

        struct PathExtension
        {
            virtual bool isCellClear(const GridPoint & gridPoint) const = 0;
        };

        class PathManager
        {
        public:

            float clipEdge(
                const Point & start,
                const Point & end,
                const PathExtension * pathExtension = nullptr) const;
            vector<Point> findPath(
                const Point & startPoint,
                float heading,
                float idealDistance,
                const PathExtension * pathExtension = nullptr) const;
        };

        Point getPointFromGridPoint(GridPoint gridPoint)
        {
            return Point(0.0f, 0.0f, 0.0f);
        }

        struct AvoidLozenges : public PathExtension
        {
            struct Lozenge
            {
                Point m_points[2];
                float m_radius;
            };

            bool isCellClear(const GridPoint & gridPoint) const override
            {
                Point point = getPointFromGridPoint(gridPoint);

                for (const Lozenge & lozenge : m_lozenges)
                {
                    float distance = getDistanceToLineSegment(
                                        point, 
                                        lozenge.m_points[0], 
                                        lozenge.m_points[1]);

                    if (distance < lozenge.m_radius)
                        return false;
                }

                return true;
            }

            vector<Lozenge> m_lozenges;
        };
	};

    void test(bool showDetails)
    {
        {
            using namespace quad;

            {
                QuadraticSolution qs = solveQuadratic(1.0f, 0.0f, -1.0f);
                assert(qs.m_solutionCount == 2);
                assert(qs.m_solutions[0] == -1.0f);
                assert(qs.m_solutions[1] == 1.0f);
            }

            {
                QuadraticSolution qs = solveQuadratic(1.0f, 0.0f, 0.0f);
                assert(qs.m_solutionCount == 1);
                assert(qs.m_solutions[0] == 0.0f);
            }

            {
                QuadraticSolution qs = solveQuadratic(1.0f, 0.0f, 1.0f);
                assert(qs.m_solutionCount == 0);
            }
        }

        {
            using namespace x1;

            struct PolynomialFunction : ObjectiveFunction
            {
                PolynomialFunction(initializer_list<float> il) :
                    m_coefficients(il)
                    { ; }

                float evaluate(float value) const
                {
                    float nth = 1.0f;
                    float result = 0.0f;
                    for (float coefficient : m_coefficients)
                    {
                        result += coefficient * nth;
                        nth *= value;
                    }
                    return result;
                }

                vector<float> m_coefficients;
            };

            {
                PolynomialFunction pf({ 1.0f, 0.0f, 1.0f });
                float min = optimizeViaGoldenSection(pf, 0.0f, 0.5f, 1e-4f);
                assert(fabsf(min - 0.0f) < 1e-4f);
            }

            {
                PolynomialFunction pf({ 1.0f, 2.0f, 3.0f });
                float min = optimizeViaGoldenSection(pf, 0.0f, 0.5f, 1e-4f);
                assert(fabsf(min - -0.3333333f) < 1e-4f);
            }
        }

        {
            using namespace x2;

            Bezier bez =
                {
                    { 0.0f, 0.0f, 0.0f },
                    { 100.0f, 0.0f, 0.0f },
                    { 100.0f, 200.0f, 0.0f },
                    { 200.0f, 200.0f, 0.0f },
                };

            {
                BezierCostFunction bcf(
                    bez,
                    { 100.0f, 100.0f, 200.0f },
                    { 0.0f, 0.0f, 0.0f },
                    -980.0f);

                float u = optimizeViaGoldenSection(bcf, 0.0f, 0.1f, 1e-4f);
                Point landing = bez.evaluate(u);

                assert(fabsf(u - 0.5f) < 1e-4f);
                assert(getDistance(landing, Point(100.0f, 100.0f, 0.0f)) < 1e-2f);
            }

            {
                BezierCostFunction bcf(
                    bez,
                    { 100.0f, 100.0f, 200.0f },
                    { 100.0f, 100.0f, 0.0f },
                    -980.0f);

                float u = optimizeViaGoldenSection(bcf, 0.0f, 0.1f, 1e-4f);
                assert(u > 0.5f);
            }

            {
                BezierCostFunction bcf(
                    bez,
                    { 100.0f, 100.0f, 200.0f },
                    { -200.0f, 0.0f, 0.0f },
                    -980.0f);

                float u = optimizeViaGoldenSection(bcf, 0.0f, 0.1f, 1e-4f);
                assert(u < 0.5f);
            }
        }
    }

    RegisterTest rt("Rule 17", test);
};
