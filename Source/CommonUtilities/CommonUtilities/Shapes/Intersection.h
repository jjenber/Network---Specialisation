#pragma once
#include "Plane.h"
#include "Ray.h"
#include "AABB3D.h"
#include "Sphere.h"
#include <Math/Vector3.h>
#include <math.h>

namespace CommonUtilities
{
#pragma region Declarations
	template <typename T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint);

	template <typename T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay);

	template <typename T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay);
#pragma endregion Declarations

	template<typename T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vector3<T>& aOutIntersectionPoint)
	{
		const T denominator = aPlane.GetNormal().Dot(aRay.GetDirection());
		if (denominator < 0.0000001 && denominator > -0.0000001)
		{
			return false;
		}
		T t = Vector3<T>{ aRay.GetOrigin() - aPlane.GetPoint() }.Dot(aPlane.GetNormal());
		aOutIntersectionPoint = t < 0 ? aRay.GetOrigin() : aRay.GetOrigin() + (aRay.GetDirection() * t);
		return true;
	}

	template <typename T>
	bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay)
	{
		Plane<T> plane;
		Vector3<T> intersectionPoint;
		// front
		plane.InitWithPointAndNormal(aAABB.myMin, { 0, 0, T(-1) });
		if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
		{
			if (aAABB.IsInside(intersectionPoint))
			{
				return true;
			}
		}
		// back
		plane.InitWithPointAndNormal(aAABB.myMax, { 0, 0, T(1) });
		if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
		{
			if (aAABB.IsInside(intersectionPoint))
			{
				return true;
			}
		}
		// left
		plane.InitWithPointAndNormal(aAABB.myMin, { T(-1), 0, 0 });
		if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
		{
			if (aAABB.IsInside(intersectionPoint))
			{
				return true;
			}
		}
		// right
		plane.InitWithPointAndNormal(aAABB.myMax, { T(1), 0, 0 });
		if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
		{
			if (aAABB.IsInside(intersectionPoint))
			{
				return true;
			}
		}
		// top
		plane.InitWithPointAndNormal(aAABB.myMax, { 0, T(1), 0 });
		if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
		{
			if (aAABB.IsInside(intersectionPoint))
			{
				return true;
			}
		}
		// bottom
		plane.InitWithPointAndNormal(aAABB.myMin, { 0, T(-1), 0 });
		if (IntersectionPlaneRay(plane, aRay, intersectionPoint))
		{
			if (aAABB.IsInside(intersectionPoint))
			{
				return true;
			}
		}
		return false;
	}

	template <typename T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay)
	{
		Vector3<T> fromRayToSphere = { aSphere.GetCenter() - aRay.GetOrigin() };
		T projectOntoRayDir = fromRayToSphere.Dot(aRay.GetDirection());
		
		const T radiusSqrd = aSphere.GetRadius() * aSphere.GetRadius();
		const T aSqrd = projectOntoRayDir * projectOntoRayDir;
		
		const T intersectionSideSqrd = radiusSqrd - fromRayToSphere.LengthSqr() + aSqrd;
		return (intersectionSideSqrd >= 0);
	}
}