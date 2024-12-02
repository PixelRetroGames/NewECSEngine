#pragma once

#include "collision_detection_system.h"
#include "bullet_collision_system.h"
#include "collider_debug_system.h"
#include "health_renderer.h"
#include "physics_system.h"
#include "collision_resolution_system.h"
#include "physics_transform_update_system.h"
#include "player_movement_system.h"

#include "shared.h"

namespace SpaceShooter {

StatelessSystem(VelocityMovementSystem, GameContext);
StatelessSystem(TextureRendererSystem, GameContext);
StatelessSystem(BulletShooterSystem, GameContext);

}
