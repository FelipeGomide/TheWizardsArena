#include "../../Math.h"

struct SteeringBehavior {
    static Vector2 Seek(Vector2 pos, Vector2 target, Vector2 vel, float maxSpeed) {
        Vector2 desired = (target - pos);
        desired.Normalize();
        desired *= maxSpeed;
        return desired - vel;
    }

    static Vector2 Arrive(Vector2 pos, Vector2 target, Vector2 vel, float slowingRadius, float maxSpeed){
        Vector2 toTarget = target - pos;
        float dist = toTarget.Length();
        if (Math::NearZero(dist)) return Vector2::Zero;

        float desiredSpeed = (dist < slowingRadius)
            ? maxSpeed * (dist / slowingRadius)
            : maxSpeed;

        toTarget.Normalize();
        Vector2 desired = toTarget * desiredSpeed;
        return desired - vel;
    }

    static Vector2 Flee(Vector2 pos, Vector2 threat, Vector2 vel, float maxSpeed) {
        return Seek(pos, pos + (pos - threat), vel, maxSpeed);
    }

    static Vector2 Wander(Vector2, float) {
        return Vector2::Zero;
    }
};