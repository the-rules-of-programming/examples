#pragma once

namespace geom
{
	enum IdentityTag
	{
		Identity
	};

	struct Point
	{
		Point() :
			m_x(0.0f), m_y(0.0f), m_z(0.0f)
			{ ; }
		Point(float x, float y, float z) :
			m_x(x), m_y(y), m_z(z)
			{ ; }

		float m_x;
		float m_y;
		float m_z;
	};

	float getDistance(Point point, Point otherPoint);
	float getDistanceToLineSegment(Point point, Point segmentStart, Point segmentStop);

	struct Vector
	{
		Vector() :
			m_x(0.0f), m_y(0.0f), m_z(0.0f)
			{ ; }
		Vector(float x, float y, float z) :
			m_x(x), m_y(y), m_z(z)
			{ ; }

		float getLength() const;

		float m_x;
		float m_y;
		float m_z;
	};

	struct Matrix
	{
		Matrix()
			{ ; }
		Matrix(IdentityTag)
			{ ; }

		bool isZero() const
			{ return false; }
		bool isIdentity() const
			{ return false; }
	};

	struct Sphere
	{
	};

	Matrix Invert(const Matrix & matrix);

	Vector operator * (float value, Vector vector);
	Point operator * (float value, Point point);
	Point & operator *= (Point & point, const Matrix & matrix);
	Vector operator + (Vector vector, Vector otherVector);
	Point operator + (Point point, Vector vector);
	Point operator + (Point point, Point otherPoint);
	Vector operator - (Point point, Vector vector);
	Vector operator - (Point point, Point otherPoint);

	float dotProduct(Vector vector, Vector otherVector);
};
