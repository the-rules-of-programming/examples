
#include "geom.h"

#include <algorithm>
#include <math.h>
#include "assert.h"

#pragma warning(disable: 26812)

namespace geom
{
	float getDistance(Point point, Point otherPoint)
	{
		return (point - otherPoint).getLength();
	}

	float getDistanceToLineSegment(Point point, Point segmentStart, Point segmentStop)
	{
		Vector segmentOffset = segmentStop - segmentStart;
		if (dotProduct(segmentOffset, segmentOffset) < 1e-4f)
		{
			return getDistance(point, segmentStart);
		}

		float u = dotProduct(point - segmentStart, segmentStop - segmentStart) /
				  dotProduct(segmentOffset, segmentOffset);

		u = std::max(0.0f, std::min(1.0f, u));

		Point closestPoint = segmentStart + u * segmentOffset;

		return getDistance(point, closestPoint);
	}

	float Vector::getLength() const
	{
		return sqrtf(dotProduct(*this, *this));
	}

	Matrix Invert(const Matrix & matrix)
	{
		assert(false);
		return Matrix();
	}

	Vector operator * (float value, Vector vector)
	{
		return { value * vector.m_x, value * vector.m_y, value * vector.m_z };
	}

	Point operator * (float value, Point point)
	{
		return { value * point.m_x, value * point.m_y, value * point.m_z };
	}

	Point & operator *= (Point & point, const Matrix & matrix)
	{
		return point;
	}

	Vector operator + (Vector vector, Vector otherVector)
	{
		return { vector.m_x + otherVector.m_x, vector.m_y + otherVector.m_y, vector.m_z + otherVector.m_z };
	}

	Point operator + (Point point, Vector vector)
	{
		return { point.m_x + vector.m_x, point.m_y + vector.m_y, point.m_z + vector.m_z };
	}

	Point operator + (Point point, Point otherPoint)
	{
		return { point.m_x + otherPoint.m_x, point.m_y + otherPoint.m_y, point.m_z + otherPoint.m_z };
	}

	Vector operator - (Point point, Vector vector)
	{
		return { point.m_x - vector.m_x, point.m_y - vector.m_y, point.m_z - vector.m_z };
	}

	Vector operator - (Point point, Point otherPoint)
	{
		return { point.m_x - otherPoint.m_x, point.m_y - otherPoint.m_y, point.m_z - otherPoint.m_z };
	}

	float dotProduct(Vector vector, Vector otherVector)
	{
		return vector.m_x * otherVector.m_x + vector.m_y * otherVector.m_y + vector.m_z * otherVector.m_z;
	}
};
