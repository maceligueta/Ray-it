#ifndef __Ray_it_geometric_operations__
#define __Ray_it_geometric_operations__

#include "vector.h"

namespace GeometricOperations {

static Vec3 ComputeReflectionDirection(const Vec3& incident_dir, const Vec3& normal) {
    return incident_dir - 2.0 * Vec3::DotProduct(incident_dir, normal) * normal;
}

static Vec3 ComputeNormalFromIncidentAndReflected(const Vec3& incident_dir, const Vec3& reflected_dir) {
    Vec3 aux = 0.5 * (reflected_dir - incident_dir);
    return aux.Normalize();
}

}

#endif //__Ray_it_geometric_operations__