#pragma once

namespace Components {

// Tag component — no data.
// Marks an entity as the player-controlled entity.
// Systems can query for this to identify which entity to apply input to.
struct PlayerComponent {};

} // namespace Components